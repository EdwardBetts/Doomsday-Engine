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
#include <QRegExp>

using namespace de;

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
        "DeHackEd patch"
    };
}

DENG2_PIMPL(DataBundle)
{
    SafePtr<File> source;
    Format format;
    String packageId; // linked under /sys/bundles/
    std::unique_ptr<res::LumpDirectory> lumpDir;
    SafePtr<LinkFile> pkgLink;

    Instance(Public *i, Format fmt) : Base(i), format(fmt)
    {}

    ~Instance()
    {
        delete pkgLink.get();
    }

    Folder &bundleFolder()
    {
        return App::rootFolder().locate<Folder>("/sys/bundles");
    }

    void identify()
    {
        DENG2_ASSERT(packageId.isEmpty()); // should be only called once
        if(!packageId.isEmpty()) return;

        // Load the lump directory of WAD files.
        if(format == Wad || format == Pwad || format == Iwad)
        {
            lumpDir.reset(new res::LumpDirectory(source->as<ByteArrayFile>()));
            if(!lumpDir->isValid())
            {
                throw FormatError("DataBundle::identify",
                                  dynamic_cast<File *>(thisPublic)->description() +
                                  ": WAD file lump directory not found");
            }

            format = (lumpDir->type() == res::LumpDirectory::Pwad? Pwad : Iwad);

            /*qDebug() << "format:" << (lumpDir->type()==res::LumpDirectory::Pwad? "PWAD" : "IWAD")
                     << "\nfileName:" << source->name()
                     << "\nfileSize:" << source->size()
                     << "\nlumpDirCRC32:" << QString::number(lumpDir->crc32(), 16).toLatin1();*/
        }
        else if(self.isNested())
        {
            /*qDebug() << "[DataBundle]" << source->description().toLatin1().constData()
                     << "is nested, no package will be generated";*/
            return;
        }

        // Search for known data files in the bundle registry.
        res::Bundles::MatchResult matched = DoomsdayApp::bundles().match(self);

        if(matched)
        {
            packageId = matched.packageId;
        }
        else
        {
            // Generate an identifier based on the information we have.
            static String const formatDomains[] = {
                "file.local",
                "file.pk3",
                "file.wad",
                "file.iwad",
                "file.pwad",
                "file.lump",
                "file.defs",
                "file.dehacked"
            };
            String cleanName = source->name().fileNameWithoutExtension().toLower();
            cleanName.replace('.', "-"); // periods have meaning for packages
            packageId = formatDomains[format] + "." + cleanName;
        }

        qDebug() << "[DataBundle] Identified:" << packageId
                 << matched.packageVersion.asText()
                 << ::internal::formatDescriptions[format]
                 << "score:" << matched.bestScore;
        /*
         * Set up the package metadata according to the best matched known
         * information or autogenerated entries.
         */
        File &dataFile = self.asFile();

        // Finally, make a link that represents the package.
        for(int attempt = 0; attempt < 3; ++attempt)
        {
            String linkPath = packageId;
            String version = (matched.packageVersion.isValid()? matched.packageVersion.asText() : "");

            // Try a few different ways to generate a locally unique version number.
            switch(attempt)
            {
            case 0: // unmodified
                break;

            case 1: // parent folder as version label
                if(dataFile.path().fileNamePath() != "/local/wads")
                {
                    if(version.isEmpty()) version = "0";
                    Path const filePath(dataFile.path());
                    if(filePath.segmentCount() >= 2)
                    {
                        version += "-" + filePath.segment(filePath.segmentCount() - 2).toString().toLower();
                    }
                }
                break;

            case 2: // status
                version = version.concatenateMember(dataFile.status().modifiedAt
                                                    .asDateTime().toString("yyMMdd.hhmmss"));
                break;
            }

            if(!version.isEmpty())
            {
                linkPath += QString("_%1.pack").arg(version);
            }
            else
            {
                linkPath += QStringLiteral(".pack");
            }

            qDebug() << "[DataBundle] Trying" << linkPath;

            // Each link must have a unique name.
            if(!bundleFolder().has(linkPath))
            {
                qDebug() << "[DataBundle] Linking" << dataFile.path() << "as" << linkPath;

                pkgLink.reset(&bundleFolder().add(LinkFile::newLinkToFile(dataFile, linkPath)));

                // Set up package metadata in the link.
                Record &metadata = Package::initializeMetadata(*pkgLink, packageId);
                metadata.set("path", dataFile.path());
                metadata.set("version", !version.isEmpty()? version : "0.0");
                if(lumpDir)
                {
                    metadata.set("lumpDirCRC32", lumpDir->crc32())
                            .value().as<NumberValue>().setSemanticHints(NumberValue::Hex);
                }
                metadata.set("bundleScore", matched.bestScore);

                // Get the rest of the metadata.
                if(matched)
                {
                    metadata.set("title",  matched.bestMatch->keyValue("info:title"));
                    metadata.set("tags",   matched.bestMatch->keyValue("info:tags"));
                    metadata.set("author", matched.bestMatch->keyValue("info:author"));

                    String license = matched.bestMatch->keyValue("info:license");
                    if(license.isEmpty()) license = "Unknown";
                    metadata.set("license", license);
                }
                else
                {
                    metadata.set("title", dataFile.name());
                    metadata.set("author", "Unknown");
                    metadata.set("license", "Unknown");
                    metadata.set("tags", "generated");
                }

                qDebug() << "[DataBundle] Generated package:";
                qDebug() << metadata.asText().toLatin1();

                App::fileSystem().index(*pkgLink);

                break; // No further attempts needed.
            }
        }
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
    if(!d->source)
    {
        return "invalid data bundle";
    }
    return QString("%1 \"%2\"")
            .arg(::internal::formatDescriptions[d->format])
            .arg(d->source->name().fileNameWithoutExtension());
}

File &DataBundle::asFile()
{
    return *dynamic_cast<File *>(this);
}

File const &DataBundle::asFile() const
{
    return *dynamic_cast<File const *>(this);
}

IByteArray::Size DataBundle::size() const
{
    if(d->source)
    {
        return d->source->size();
    }
    return 0;
}

void DataBundle::get(Offset at, Byte *values, Size count) const
{
    if(!d->source)
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
    return dynamic_cast<File *>(this)->objectNamespace().subrecord(QStringLiteral("package"));
}

Record const &DataBundle::objectNamespace() const
{
    DENG2_ASSERT(dynamic_cast<File const *>(this) != nullptr);
    return dynamic_cast<File const *>(this)->objectNamespace().subrecord(QStringLiteral("package"));
}

void DataBundle::setFormat(Format format)
{
    d->format = format;
}

void DataBundle::identifyPackages() const
{
    d->identify();
}

bool DataBundle::isNested() const
{
    return containerBundle() != nullptr || !containerPackageId().isEmpty();
}

DataBundle const *DataBundle::containerBundle() const
{
    auto const *file = dynamic_cast<File const *>(this);
    DENG2_ASSERT(file != nullptr);

    for(Folder const *folder = file->parent(); folder; folder = folder->parent())
    {
        if(auto const *data = folder->maybeAs<DataFolder>())
            return data;
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
        { ".wad", Wad /* I/P checked later */ },
        { ".lmp", Lump },
        { ".ded", Ded },
        { ".deh", Dehacked }
    };
    String const ext = sourceData->name().fileNameExtension();
    for(auto const &fmt : formats)
    {
        if(!ext.compareWithoutCase(fmt.str))
        {
            LOG_RES_VERBOSE("Interpreted ") << sourceData->description()
                                            << " as "
                                            << ::internal::formatDescriptions[fmt.format];

            switch(fmt.format)
            {
            case Pk3:
                return new DataFolder(fmt.format, *sourceData);

            default:
                return new DataFile(fmt.format, *sourceData);
            }
        }
    }
    // Was not interpreted.
    return nullptr;
}