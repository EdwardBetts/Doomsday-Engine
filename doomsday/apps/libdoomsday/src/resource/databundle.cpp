/** @file databundle.cpp  Classic data files: PK3, WAD, LMP, DED, DEH.
 *
 * @authors Copyright (c) 2016 Jaakko Keränen <jaakko.keranen@iki.fi>
 *
 * @par License
 * GPL: http://www.gnu.org/licenses/gpl.html
 *
 * <small>This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version. This program is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details. You should have received a copy of the GNU
 * General Public License along with this program; if not, see:
 * http://www.gnu.org/licenses</small>
 */

#include "doomsday/resource/databundle.h"
#include "doomsday/filesys/datafolder.h"
#include "doomsday/filesys/datafile.h"
#include "doomsday/resource/bundles.h"
#include "doomsday/resource/resources.h"
#include "doomsday/resource/lumpdirectory.h"
#include "doomsday/doomsdayapp.h"

#include <de/App>
#include <de/ArchiveFeed>
#include <de/Info>
#include <de/LinkFile>
#include <de/Package>
#include <de/Path>
#include <QRegExp>

using namespace de;

static String const VAR_TAGS("tags");

namespace internal
{
    static char const *formatDescriptions[] =
    {
        "unknown",
        "PK3 archive",
        "WAD file",
        "IWAD file",
        "PWAD file",
        "data lump",
        "Doomsday Engine definitions",
        "DeHackEd patch",
        "collection"
    };
}

DENG2_PIMPL(DataBundle)
{
    SafePtr<File> source;
    Format format;
    String packageId; // linked under /sys/bundles/
    String versionedPackageId;
    std::unique_ptr<res::LumpDirectory> lumpDir;
    SafePtr<LinkFile> pkgLink;

    Instance(Public *i, Format fmt) : Base(i), format(fmt)
    {}

    ~Instance()
    {
        delete pkgLink.get();
    }

    static Folder &bundleFolder()
    {
        return App::rootFolder().locate<Folder>("/sys/bundles");
    }

    static String cleanIdentifier(String const &text)
    {
        String cleaned = text.toLower();
        cleaned.replace(QRegExp("[._]"), "-"); // periods and underscores have special meaning in packages IDs
        return cleaned;
    }

    static String stripVersion(String const &text, Version *version = nullptr)
    {
        QRegExp re(".*([-_. ][0-9._-]+)$");
        if (re.exactMatch(text))
        {
            if (version)
            {
                String str = re.cap(1).mid(1);
                str.replace("_", ".");
                version->parseVersionString(str);
            }
            return text.mid(0, text.size() - re.cap(1).size());
        }
        return text;
    }

    /**
     * Identifies the data bundle and sets up a package link under "/sys/bundles" with
     * the appropriate metadata.
     *
     * Sets up the package metadata according to the best matched known information or
     * autogenerated entries.
     */
    void identify()
    {
        // It is sufficient to identify each bundle only once.
        if (!packageId.isEmpty()) return;

        // Load the lump directory of WAD files.
        if (format == Wad || format == Pwad || format == Iwad)
        {
            // The lump directory needs to be loaded before matching against known
            // bundles because it can be used for identification.
            lumpDir.reset(new res::LumpDirectory(source->as<ByteArrayFile>()));
            if (!lumpDir->isValid())
            {
                throw FormatError("DataBundle::identify",
                                  dynamic_cast<File *>(thisPublic)->description() +
                                  ": WAD file lump directory not found");
            }

            // Determine the WAD type, if unspecified.
            format = (lumpDir->type() == res::LumpDirectory::Pwad? Pwad : Iwad);

            /*qDebug() << "format:" << (lumpDir->type()==res::LumpDirectory::Pwad? "PWAD" : "IWAD")
                     << "\nfileName:" << source->name()
                     << "\nfileSize:" << source->size()
                     << "\nlumpDirCRC32:" << QString::number(lumpDir->crc32(), 16).toLatin1();*/
        }
        else if (!self.containerPackageId().isEmpty())
        {
            // This file is inside a package, so the package will take care of it.
            /*qDebug() << "[DataBundle]" << source->description().toLatin1().constData()
                     << "is nested, no package will be generated";*/
            return;
        }

        // Search for known data files in the bundle registry.
        res::Bundles::MatchResult matched = DoomsdayApp::bundles().match(self);
        File &dataFile = self.asFile();
        String const dataFilePath = dataFile.path();

        // Metadata for the package will be collected into this record.
        Record meta;
        meta.set("path",        dataFilePath);
        meta.set("bundleScore", matched.bestScore);

        DataBundle const *container = self.containerBundle();

        // At least two criteria must match -- otherwise simply having the correct
        // type would be accepted.
        if (matched)
        {
            // Package metadata has been defined for this file (databundles.dei).
            packageId = matched.packageId;

            if (lumpDir)
            {
                meta.set("lumpDirCRC32", lumpDir->crc32())
                        .value().as<NumberValue>().setSemanticHints(NumberValue::Hex);
            }

            meta.set("version", matched.packageVersion.asText());
            meta.set("title",   matched.bestMatch->keyValue("info:title"));
            meta.set("author",  matched.bestMatch->keyValue("info:author"));
            meta.set("license", matched.bestMatch->keyValue("info:license", "Unknown"));
            meta.set(VAR_TAGS,  matched.bestMatch->keyValue("info:tags"));
        }
        else
        {
            meta.set("version", "0.0");
            meta.set("title",   dataFile.name());
            meta.set("author",  "Unknown");
            meta.set("license", "Unknown");
            meta.set(VAR_TAGS,  dataFile.extension());

            // Generate a default identifier based on the information we have.
            static String const formatDomains[] = {
                "file.local",
                "file.pk3",
                "file.wad",
                "file.iwad",
                "file.pwad",
                "file.lump",
                "file.defs",
                "file.dehacked",
                "file.collection"
            };

            // Containers become part of the identifier.
            for (DataBundle const *i = container; i; i = i->containerBundle())
            {
                packageId = cleanIdentifier(stripVersion(i->sourceFile().name().fileNameWithoutExtension()))
                            .concatenateMember(packageId);
            }

            // The file name may contain a version number.
            Version parsedVersion("");
            String strippedName = stripVersion(source->name().fileNameWithoutExtension(),
                                               &parsedVersion);
            if (strippedName != source->name())
            {
                meta.set("version", parsedVersion.asText());
            }

            packageId = formatDomains[format]
                    .concatenateMember(packageId)
                    .concatenateMember(cleanIdentifier(strippedName));

            /*
             *  There may be non-native metadata available, though:
             * - Info entry inside root folder
             * - .manifest companion
             */
            File const *sbInfo = App::rootFolder().tryLocate<File const>(
                        dataFilePath.fileNamePath() / dataFilePath.fileNameWithoutExtension() +
                        ".manifest");
            if (!sbInfo)
            {
                sbInfo = App::rootFolder().tryLocate<File const>(dataFilePath/"Info");
            }
            if (!sbInfo)
            {
                sbInfo = App::rootFolder().tryLocate<File const>(dataFilePath/"Contents/Info");
            }
            if (sbInfo)
            {
                parseSnowberryInfo(*sbInfo, meta);
            }
        }

        LOG_RES_VERBOSE("Identified \"%s\" %s %s score: %i")
                << packageId
                << meta.gets("version")
                << ::internal::formatDescriptions[format]
                << matched.bestScore;

        versionedPackageId = packageId;

        // Finally, make a link that represents the package.
        if (auto chosen = chooseUniqueLinkPathAndVersion(dataFile, packageId,
                                                         meta.gets("version"),
                                                         matched.bestScore))
        {
            LOGDEV_RES_VERBOSE("Linking %s as %s") << dataFile.path() << chosen.path;

            pkgLink.reset(&bundleFolder().add(LinkFile::newLinkToFile(dataFile, chosen.path)));

            // Set up package metadata in the link.
            Record &metadata = Package::initializeMetadata(*pkgLink, packageId);
            metadata.copyMembersFrom(meta);
            metadata.set("version", !chosen.version.isEmpty()? chosen.version : "0.0");

            // Compose a versioned ID.
            if (!chosen.version.isEmpty())
            {
                versionedPackageId += "_" + chosen.version;
            }

            LOG_RES_VERBOSE("Generated package:\n%s") << metadata.asText();

            App::fileSystem().index(*pkgLink);

            // Make this a required package in the container bundle.
            if (container && isAutoLoaded())
            {
                // Autoloaded data files are hidden.
                meta.set(VAR_TAGS, meta.gets(VAR_TAGS) + " hidden");

                // Make sure that the container has been fully identified.
                container->identifyPackages();

                if (container->d->pkgLink)
                {
                    qDebug() << container->d->versionedPackageId << "loads" << versionedPackageId
                             << "from" << dataFilePath;
                    Package::addRequiredPackage(*container->d->pkgLink, versionedPackageId);
                }
            }
        }
    }

    /**
     * Determines if the data bundle is intended to be automatically loaded by Doomsday
     * according to the v1.x autoload rules.
     */
    bool isAutoLoaded() const
    {
        Path const path(self.asFile().path());

        if (path.segmentCount() >= 3)
        {
            String const parent      = path.reverseSegment(1).toString().toLower();
            String const grandParent = path.reverseSegment(2).toString().toLower();

            if (parent.fileNameExtension() == ".pk3" ||
                parent.fileNameExtension() == ".box")
            {
                // Data files in the root of a PK3/box are all automatically loaded.
                return true;
            }
            if (parent.fileNameExtension().isEmpty() &&
                (parent == "auto" || parent.beginsWith("#") || parent.beginsWith("@")))
            {
                return true;
            }

            if (grandParent.fileNameExtension() == ".box")
            {
                if (parent == "required")
                {
                    return true;
                }
                /// @todo What about "Extra"?
            }
        }
        return false;
    }

    /**
     * Reads a Snowberry-style Info file and extracts the relevant parts into the
     * Doomsday 2 package metadata record.
     *
     * @param infoFile  Snowberry Info file.
     * @param meta      Package metadata.
     */
    void parseSnowberryInfo(File const &infoFile, Record &meta)
    {
        Info info(String::fromUtf8(Block(infoFile)));
        auto const &rootBlock = info.root();

        // Tag it as a Snowberry package.
        meta.set(VAR_TAGS, meta.gets(VAR_TAGS) + " legacy");

        if (rootBlock.contains("name"))
        {
            meta.set("title", rootBlock.keyValue("name"));
        }

        String component = rootBlock.keyValue("component");
        if (!component.isEmpty())
        {
            String gameTags;
            if (!component.compareWithoutCase("game-jdoom"))
            {
                gameTags = " doom doom2";
            }
            else if (!component.compareWithoutCase("game-jheretic"))
            {
                gameTags = " heretic";
            }
            else if (!component.compareWithoutCase("game-jhexen"))
            {
                gameTags = " hexen";
            }
            if (!gameTags.isEmpty())
            {
                meta.set(VAR_TAGS, meta.gets(VAR_TAGS) + gameTags);
            }
        }

        if (Info::BlockElement const *english = rootBlock.findAs<Info::BlockElement>("english"))
        {
            if(english->blockType() == "language")
            {
                // Doomsday must understand the version number.
                meta.set("version", Version(english->keyValue("version")).asText());
                meta.set("author",  english->keyValue("author"));
                meta.set("license", english->keyValue("license"));
                meta.set("contact", english->keyValue("contact"));
                meta.set("notes",   english->keyValue("readme"));
            }
        }
    }

    struct PathAndVersion {
        String path;
        String version;
        PathAndVersion(String const &path = String(), String const &version = String())
            : path(path), version(version) {}
        operator bool() { return !path.isEmpty(); }
    };

    PathAndVersion chooseUniqueLinkPathAndVersion(File const &dataFile,
                                                  String const &packageId,
                                                  Version const &packageVersion,
                                                  dint bundleScore)
    {
        for (int attempt = 0; attempt < 4; ++attempt)
        {
            String linkPath = packageId;
            String version = (packageVersion.isValid()? packageVersion.asText() : "");

            // Try a few different ways to generate a locally unique version number.
            switch (attempt)
            {
            case 0: // unmodified
                break;

            case 1: // parse version from parent folder
            case 2: // parent folder as version label
                if (dataFile.path().fileNamePath() != "/local/wads")
                {
                    Path const filePath(dataFile.path());
                    if (filePath.segmentCount() >= 2)
                    {
                        auto const &parentName = filePath.reverseSegment(1)
                                .toString().fileNameWithoutExtension();
                        if (attempt == 1)
                        {
                            Version parsed("");
                            stripVersion(parentName, &parsed);
                            if (parsed.isValid())
                            {
                                version = parsed.asText();
                            }
                        }
                        else
                        {
                            version = "0-" + filePath.reverseSegment(1)
                                    .toString().fileNameWithoutExtension().toLower();
                        }
                    }
                }
                break;

            case 3: // status
                version = version.concatenateMember(dataFile.status().modifiedAt
                                                    .asDateTime().toString("yyyyMMdd.hhmmss"));
                break;
            }

            if (!version.isEmpty())
            {
                linkPath += QString("_%1.pack").arg(version);
            }
            else
            {
                linkPath += QStringLiteral(".pack");
            }

            // Each link must have a unique name.
            if (!bundleFolder().has(linkPath))
            {
                return PathAndVersion(linkPath, version);
            }
            else
            {
                // This could still be a better scored match.
                Record const &pkgInfo = bundleFolder().locate<File const>(linkPath).objectNamespace();
                if (bundleScore > pkgInfo.geti("package.bundleScore"))
                {
                    // Forget about the previous link.
                    bundleFolder().removeFile(linkPath);
                    return PathAndVersion(linkPath, version);
                }
            }
        }

        // Unique path & version not available. This version of the package is probably
        // already available.
        return PathAndVersion();
    }
};

DataBundle::DataBundle(Format format, File &source)
    : d(new Instance(this, format))
{
    d->source.reset(&source);
}

DataBundle::~DataBundle()
{}

DataBundle::Format DataBundle::format() const
{
    return d->format;
}

String DataBundle::description() const
{
    if (!d->source)
    {
        return "invalid data bundle";
    }
    return QString("%1 \"%2\"")
            .arg(::internal::formatDescriptions[d->format])
            .arg(d->source->name());
}

File &DataBundle::asFile()
{
    return *dynamic_cast<File *>(this);
}

File const &DataBundle::asFile() const
{
    return *dynamic_cast<File const *>(this);
}

File const &DataBundle::sourceFile() const
{
    return *asFile().source();
}

String DataBundle::packageId() const
{
    if (d->packageId.isEmpty())
    {
        identifyPackages();
    }
    return d->packageId;
}

IByteArray::Size DataBundle::size() const
{
    if (d->source)
    {
        return d->source->size();
    }
    return 0;
}

void DataBundle::get(Offset at, Byte *values, Size count) const
{
    if (!d->source)
    {
        throw File::InputError("DataBundle::get", "Source file has been destroyed");
    }
    d->source->as<ByteArrayFile>().get(at, values, count);
}

void DataBundle::set(Offset, Byte const *, Size)
{
    throw File::OutputError("DataBundle::set", "Classic data formats are read-only");
}

Record &DataBundle::objectNamespace()
{
    DENG2_ASSERT(dynamic_cast<File *>(this) != nullptr);
    return asFile().objectNamespace().subrecord(QStringLiteral("package"));
}

Record const &DataBundle::objectNamespace() const
{
    DENG2_ASSERT(dynamic_cast<File const *>(this) != nullptr);
    return asFile().objectNamespace().subrecord(QStringLiteral("package"));
}

void DataBundle::setFormat(Format format)
{
    d->format = format;
}

void DataBundle::identifyPackages() const
{
    LOG_AS("DataBundle");
    try
    {
        d->identify();
    }
    catch (Error const &er)
    {
        LOG_RES_WARNING("Failed to identify %s: %s") << description() << er.asText();
    }
}

bool DataBundle::isNested() const
{
    return containerBundle() != nullptr || !containerPackageId().isEmpty();
}

DataBundle const *DataBundle::containerBundle() const
{
    auto const *file = dynamic_cast<File const *>(this);
    DENG2_ASSERT(file != nullptr);

    for (Folder const *folder = file->parent(); folder; folder = folder->parent())
    {
        if (auto const *data = folder->maybeAs<DataFolder>())
        {
            return data;
        }
    }
    return nullptr;
}

String DataBundle::containerPackageId() const
{
    auto const *file = dynamic_cast<File const *>(this);
    DENG2_ASSERT(file != nullptr);

    return Package::identifierForContainerOfFile(*file);
}

res::LumpDirectory const *DataBundle::lumpDirectory() const
{
    return d->lumpDir.get();
}

File *DataBundle::Interpreter::interpretFile(File *sourceData) const
{
    // Naive check using the file extension.
    static struct { String str; Format format; } formats[] = {
        { ".pk3", Pk3 },
        { ".wad", Wad /* type (I or P) checked later */ },
        { ".lmp", Lump },
        { ".ded", Ded },
        { ".deh", Dehacked },
        { ".box", Collection },
    };
    String const ext = sourceData->extension();
    for (auto const &fmt : formats)
    {
        if (!ext.compareWithoutCase(fmt.str))
        {
            LOG_RES_VERBOSE("Interpreted ") << sourceData->description()
                                            << " as "
                                            << ::internal::formatDescriptions[fmt.format];

            switch (fmt.format)
            {
            case Pk3:
            case Collection:
                return new DataFolder(fmt.format, *sourceData);

            default:
                return new DataFile(fmt.format, *sourceData);
            }
        }
    }
    // Was not interpreted.
    return nullptr;
}
