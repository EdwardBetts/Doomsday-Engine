/** @file directoryfeed.h  Directory Feed.
 *
 * @author Copyright © 2009-2017 Jaakko Keränen <jaakko.keranen@iki.fi>
 *
 * @par License
 * LGPL: http://www.gnu.org/licenses/lgpl.html
 *
 * <small>This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version. This program is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details. You should have received a copy of
 * the GNU Lesser General Public License along with this program; if not, see:
 * http://www.gnu.org/licenses</small>
 */

#ifndef LIBDENG2_DIRECTORYFEED_H
#define LIBDENG2_DIRECTORYFEED_H

#include "../File"
#include "../Feed"
#include "../NativePath"

#include <QFlags>

namespace de {

/**
 * Reads from and writes to directories in the native file system.
 *
 * @ingroup fs
 */
class DENG2_PUBLIC DirectoryFeed : public Feed
{
public:
    /// The native directory was not found. @ingroup errors
    DENG2_ERROR(NotFoundError);

    /// Failed attempt to find out the status of a file. @ingroup errors
    DENG2_ERROR(StatusError);

    /// An error occurred changing the working directory. @ingroup errors
    DENG2_ERROR(WorkingDirError);

    enum Flag
    {
        /// Opens all files and folders in write mode.
        AllowWrite = 0x1,

        /// Creates the native directory if not does not exist.
        CreateIfMissing = 0x2,

        /// When populating the contents of the folder, descend to native
        /// subfolders.
        PopulateNativeSubfolders = 0x4,

        OnlyThisFolder = 0,

        DefaultFlags = PopulateNativeSubfolders
    };
    Q_DECLARE_FLAGS(Flags, Flag)

public:
    /**
     * Constructs a DirectoryFeed that accesses a directory in the native file system.
     *
     * @param nativePath  Path of the native directory.
     * @param mode        Feed mode.
     */
    DirectoryFeed(NativePath const &nativePath, Flags const &mode = DefaultFlags);

    virtual ~DirectoryFeed();

    String description() const;

    /**
     * Returns the native path of the source folder.
     */
    NativePath const &nativePath() const;

    PopulatedFiles populate(Folder const &folder);
    bool prune(File &file) const;
    File *createFile(String const &name);
    void destroyFile(String const &name);
    Feed *newSubFeed(String const &name);

public:
    /**
     * Changes the native working directory.
     *
     * @param nativePath  New path to use as the working directory.
     */
    static void changeWorkingDir(NativePath const &nativePath);

    /**
     * Determines the status of a file in the directory.
     * StatusError is thrown if the file status cannot be determined
     * due to any reason.
     *
     * @param nativePath  Path of the file.
     *
     * @return  Status of the file.
     */
    static File::Status fileStatus(NativePath const &nativePath);

    /**
     * Creates and interprets a single native file and adds it to a folder. The created
     * file is not governed by any feed and will not be pruned during folder pruning.
     *
     * @param nativePath    Native path of the source file.
     * @param parentFolder  Folder where to place the interpreted file.
     *
     * @return Reference to the interpreted file.
     */
    static File &manuallyPopulateSingleFile(NativePath const &nativePath, Folder &parentFolder);

protected:
    void populateSubFolder(Folder const &folder, String const &entryName);
    void populateFile(Folder const &folder, String const &entryName, PopulatedFiles &populated);

private:
    NativePath const _nativePath;
    Flags _mode;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DirectoryFeed::Flags)

} // namespace de

#endif /* LIBDENG2_DIRECTORYFEED_H */
