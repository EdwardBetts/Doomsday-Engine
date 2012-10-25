/**
 * @file filedirectory.h
 *
 * FileDirectory. Core system component representing a hierarchical file path
 * structure.
 *
 * A specialization of de::PathTree which implements automatic population
 * of the directory itself from the virtual file system.
 *
 * @note Paths are resolved prior to pushing them into the directory.
 *
 * @ingroup fs
 *
 * @authors Copyright &copy; 2003-2012 Jaakko Keränen <jaakko.keranen@iki.fi>
 * @authors Copyright &copy; 2009-2012 Daniel Swanson <danij@dengine.net>
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
 * General Public License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA</small>
 */

#ifndef LIBDENG_FILEDIRECTORY_H
#define LIBDENG_FILEDIRECTORY_H

#include "uri.h"
#include "pathtree.h"

#ifdef __cplusplus

namespace de {

class FileDirectory : private PathTree
{
public:
    /**
     * @param basePath  Used with directories which represent the internal paths
     *                  natively as relative paths. @c NULL means only absolute
     *                  paths will be added to the directory (attempts to add
     *                  relative paths will fail silently).
     */
    explicit FileDirectory(char const* basePath = 0);
    ~FileDirectory();

    /**
     * Clear this file directory's contents.
     */
    void clear();

    /**
     * Find a path in this directory.
     *
     * @param type              Only consider nodes of this type.
     * @param searchPath        Relative or absolute path.
     * @param searchDelimiter   Fragments of @a searchPath are delimited by this.
     * @param foundPath         If not @c NULL, the fully composed path to the found
     *                          directory node is written back here.
     * @param foundDelimiter    Delimiter to be use when composing @a foundPath.
     *
     * @return  @c true iff successful.
     */
    bool find(PathTreeNodeType type, char const* searchPath, char searchDelimiter = '/',
              ddstring_t* foundPath = 0, char foundDelimiter = '/');

    /**
     * Add a new set of paths. Duplicates are automatically pruned.
     *
     * @param flags         @ref searchPathFlags
     * @param paths         One or more paths.
     * @param pathsCount    Number of elements in @a paths.
     * @param callback      Callback to make for each path added to this directory.
     * @param parameters    Passed to the callback.
     */
    void addPaths(int flags, Uri const* const* searchPaths, uint searchPathsCount,
                  int (*callback) (PathTree::Node& node, void* parameters) = 0,
                  void* parameters = 0);

    /**
     * Add a new set of paths from a path list. Duplicates are automatically pruned.
     *
     * @param flags         @ref searchPathFlags
     * @param pathList      One or more paths separated by semicolons.
     * @param callback      Callback to make for each path added to this directory.
     * @param parameters    Passed to the callback.
     */
    void addPathList(int flags, char const* pathList,
                     int (*callback) (PathTree::Node& node, void* parameters) = 0, void* parameters = 0);

    /**
     * Collate all paths in the directory into a list.
     *
     * @param count         Total number of collated paths is written back here.
     * @param flags         @ref pathComparisonFlags
     * @param delimiter     Fragments of the path will be delimited by this character.
     *
     * @return  The allocated list; it is the responsibility of the caller to Str_Free()
     *          each string in the list and free() the list itself.
     */
    ddstring_t* collectPaths(size_t* count, int flags, char delimiter='/');

#if _DEBUG
    static void debugPrint(FileDirectory& inst);
    static void debugPrintHashDistribution(FileDirectory& inst);
#endif

private:
    void clearNodeInfo();

    PathTree::Node* addPathNodes(ddstring_t const* rawPath);

    int addChildNodes(PathTree::Node& node, int flags,
                      int (*callback) (PathTree::Node& node, void* parameters),
                      void* parameters);

    /**
     * @param filePath      Possibly-relative path to an element in the virtual file system.
     * @param flags         @ref searchPathFlags
     * @param callback      If not @c NULL the callback's logic dictates whether iteration continues.
     * @param parameters    Passed to the callback.
     * @param nodeType      Type of element, either a branch (directory) or a leaf (file).
     *
     * @return  Non-zero if the current iteration should stop else @c 0.
     */
    int addPathNodesAndMaybeDescendBranch(bool descendBranches, ddstring_t const* filePath,
                                          PathTreeNodeType nodeType, int flags,
                                          int (*callback) (PathTree::Node& node, void* parameters),
                                          void* parameters);

private:
    /// Used with relative path directories.
    ddstring_t* basePath;

    /// Used with relative path directories.
    PathTree::Node* baseNode;
};

} // namespace de

extern "C" {
#endif

struct filedirectory_s; // The filedirectory instance (opaque).
typedef struct filedirectory_s FileDirectory;

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* LIBDENG_FILEDIRECTORY_H */
