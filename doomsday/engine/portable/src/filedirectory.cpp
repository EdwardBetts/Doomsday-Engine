/**
 * @file filedirectory.cpp
 * FileDirectory implementation. @ingroup fs
 *
 * @authors Copyright &copy; 2003-2012 Jaakko Keränen <jaakko.keranen@iki.fi>
 * @authors Copyright &copy; 2006-2012 Daniel Swanson <danij@dengine.net>
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

#include "de_base.h"
#include "de_console.h"
#include "de_filesys.h"

#include "timer.h"
#include "filedirectory.h"
#include <de/memory.h>

struct FileDirectoryNodeInfo
{
    bool processed;
    FileDirectoryNodeInfo() : processed(false) {}
};

static de::PathTree::Node* attachMissingNodeInfo(de::PathTree::Node* node)
{
    if(!node) return NULL;
    // Has this already been processed?
    FileDirectoryNodeInfo* info = reinterpret_cast<FileDirectoryNodeInfo*>(node->userData());
    if(!info)
    {
        // Clearly not. Attach new node info.
        node->setUserData(new FileDirectoryNodeInfo());
    }
    return node;
}

de::FileDirectory::FileDirectory(const char* _basePath)
    : PathTree(), basePath(0), baseNode(0)
{
    if(_basePath && _basePath[0])
    {
        basePath = Str_Set(Str_NewStd(), _basePath);
        // Ensure path is correctly formed.
        F_AppendMissingSlash(basePath);
    }
}

de::FileDirectory::~FileDirectory()
{
    clearNodeInfo();
    if(basePath) Str_Delete(basePath);
}

void de::FileDirectory::clearNodeInfo()
{
    DENG2_FOR_EACH_CONST(Nodes, i, leafNodes())
    {
        FileDirectoryNodeInfo* info = reinterpret_cast<FileDirectoryNodeInfo*>((*i)->userData());
        if(info)
        {
            // Detach our user data from this node.
            (*i)->setUserData(0);
            delete info;
        }
    }

    DENG2_FOR_EACH_CONST(Nodes, i, branchNodes())
    {
        FileDirectoryNodeInfo* info = reinterpret_cast<FileDirectoryNodeInfo*>((*i)->userData());
        if(info)
        {
            // Detach our user data from this node.
            (*i)->setUserData(0);
            delete info;
        }
    }
}

void de::FileDirectory::clear()
{
    clearNodeInfo();
    PathTree::clear();
    baseNode = NULL;
}

bool de::FileDirectory::find(PathTreeNodeType nodeType,
    const char* _searchPath, char searchDelimiter, ddstring_t* foundPath,
    char foundDelimiter)
{
    if(foundPath)
    {
        Str_Clear(foundPath);
    }

    if(!_searchPath || !_searchPath[0]) return false;

    // Convert the raw path into one we can process.
    AutoStr* searchPath = AutoStr_FromTextStd(_searchPath);
    F_FixSlashes(searchPath, searchPath);

    // Try to make it a relative path?
    if(basePath && !F_IsAbsolute(searchPath))
    {
        F_RemoveBasePath2(searchPath, searchPath, basePath);
    }

    // Perform the search.
    try
    {
        int const findFlags = (nodeType == PT_LEAF? PCF_NO_BRANCH : PCF_NO_LEAF) | PCF_MATCH_FULL;
        de::PathTree::Node const& foundNode = PathTree::find(findFlags, Str_Text(searchPath), searchDelimiter);
        // Does caller want to know the full path?
        if(foundPath)
        {
            foundNode.composePath(foundPath, NULL, foundDelimiter);
        }
        return true;
    }
    catch(NotFoundError const&)
    {} // Ignore this error.
    return false;
}

de::PathTree::Node* de::FileDirectory::addPathNodes(ddstring_t const* rawPath)
{
    if(!rawPath || Str_IsEmpty(rawPath)) return NULL;

    ddstring_t const* path;
    ddstring_t buf;
    if(basePath)
    {
        // Try to make it a relative path?
        if(F_IsAbsolute(rawPath))
        {
            F_RemoveBasePath2(Str_InitStd(&buf), rawPath, basePath);
            path = &buf;
        }
        else
        {
            path = rawPath;
        }

        // If this is equal to the base path, return that node.
        if(Str_IsEmpty(path))
        {
            // Time to construct the relative base node?
            // This node is purely symbolic, its only necessary for our internal use.
            if(!baseNode)
            {
                baseNode = insert("./", '/');
                attachMissingNodeInfo(baseNode);
            }

            if(path == &buf) Str_Free(&buf);
            return baseNode;
        }
    }
    else
    {
        // Do not add relative paths.
        path = rawPath;
        if(!F_IsAbsolute(path)) return NULL;
    }

    de::PathTree::Node* node = insert(Str_Text(path), '/');
    attachMissingNodeInfo(node);

    if(path == &buf) Str_Free(&buf);

    return node;
}

int de::FileDirectory::addChildNodes(de::PathTree::Node& node, int flags,
    int (*callback) (de::PathTree::Node& node, void* parameters), void* parameters)
{
    int result = 0; // Continue iteration.

    if(PT_BRANCH == node.type())
    {
        // Compose the search pattern.
        ddstring_t searchPattern; Str_InitStd(&searchPattern);
        node.composePath(&searchPattern, NULL, '/');
        // We're interested in *everything*.
        Str_AppendChar(&searchPattern, '*');

        // Process this search.
        FS1::PathList found;
        if(App_FileSystem()->findAllPaths(Str_Text(&searchPattern), flags, found))
        {
            DENG2_FOR_EACH_CONST(FS1::PathList, i, found)
            {
                QByteArray foundPathUtf8 = i->path.toUtf8();
                ddstring_t foundPath; Str_InitStatic(&foundPath, foundPathUtf8.constData());
                PathTreeNodeType nodeType = (i->attrib & A_SUBDIR)? PT_BRANCH : PT_LEAF;

                result = addPathNodesAndMaybeDescendBranch(!(flags & SPF_NO_DESCEND), &foundPath, nodeType,
                                                           flags, callback, parameters);
                if(result) break;
            }
        }

        Str_Free(&searchPattern);
    }

    return result;
}

int de::FileDirectory::addPathNodesAndMaybeDescendBranch(bool descendBranches,
    ddstring_t const* filePath, PathTreeNodeType nodeType,
    int flags, int (*callback) (de::PathTree::Node& node, void* parameters),
    void* parameters)
{
    DENG2_ASSERT(VALID_PATHTREENODE_TYPE(nodeType));
    DENG2_UNUSED(nodeType);

    int result = 0; // Continue iteration.

    // Add this path to the directory.
    de::PathTree::Node* node = addPathNodes(filePath);
    if(node)
    {
        FileDirectoryNodeInfo* info = reinterpret_cast<FileDirectoryNodeInfo*>(node->userData());

        if(PT_BRANCH == node->type())
        {
            // Descend into this subdirectory?
            if(descendBranches)
            {
                if(info->processed)
                {
                    // Does caller want to process it again?
                    if(callback)
                    {
                        DENG2_FOR_EACH_CONST(Nodes, i, leafNodes())
                        {
                            if(node == (*i)->parent())
                            {
                                result = callback(**i, parameters);
                                if(result) break;
                            }
                        }

                        if(!result)
                        {
                            DENG2_FOR_EACH_CONST(Nodes, i, branchNodes())
                            {
                                if(node == (*i)->parent())
                                {
                                    result = callback(**i, parameters);
                                    if(result) break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    result = addChildNodes(*node, flags, callback, parameters);

                    // This node is now considered processed.
                    info->processed = true;
                }
            }
        }
        // Node is a leaf.
        else if(callback)
        {
            result = callback(*node, parameters);

            // This node is now considered processed (if it wasn't already).
            info->processed = true;
        }
    }

    return result;
}

DENG_DEBUG_ONLY(
static void printUriList(Uri const* const* pathList, size_t pathCount, int indent)
{
    if(!pathList) return;

    Uri const* const* pathsIt = pathList;
    for(size_t i = 0; i < pathCount && (*pathsIt); ++i, pathsIt++)
    {
        Uri_Print(*pathsIt, indent);
    }
})

void de::FileDirectory::addPaths(int flags,
    Uri const* const* searchPaths, uint searchPathsCount,
    int (*callback) (de::PathTree::Node& node, void* parameters), void* parameters)
{
    if(!searchPaths || searchPathsCount == 0)
    {
        DEBUG_Message(("Warning: FileDirectory::AddPaths: Attempt to add zero-sized path list, ignoring.\n"));
        return;
    }

#if _DEBUG
    VERBOSE2( Con_Message("Adding paths to FileDirectory...\n");
              printUriList(searchPaths, searchPathsCount, 2/*indent*/) )
#endif

    for(uint i = 0; i < searchPathsCount; ++i)
    {
        ddstring_t const* searchPath = Uri_ResolvedConst(searchPaths[i]);
        if(!searchPath) continue;

        // Add new nodes on this path and/or re-process previously seen nodes.
        addPathNodesAndMaybeDescendBranch(true/*do descend*/, searchPath, PT_BRANCH,
                                          flags, callback, parameters);
    }

/*#if _DEBUG
    debugPrint(fd);
#endif*/
}

void de::FileDirectory::addPathList(int flags, char const* pathList,
    int (*callback) (de::PathTree::Node& node, void* parameters), void* parameters)
{
    Uri** paths = NULL;
    int pathsCount = 0;

    if(pathList && pathList[0])
        paths = F_CreateUriList2(RC_UNKNOWN, pathList, &pathsCount);

    addPaths(flags, (Uri const* const*)paths, (uint)pathsCount, callback, parameters);
    if(paths) F_DestroyUriList(paths);
}

ddstring_t* de::FileDirectory::collectPaths(size_t* count, int flags, char delimiter)
{
    return PathTree::collectPaths(count, flags, delimiter);
}

#if _DEBUG
/// @return Lexicographical delta between the two ddstring_ts @a and @a b.
static int comparePaths(void const* a, void const* b)
{
    return stricmp(Str_Text((ddstring_t*)a), Str_Text((ddstring_t*)b));
}

static void printPathList(ddstring_t const* pathList, size_t numPaths, int indent)
{
    if(!pathList) return;
    for(size_t n = 0; n < numPaths; ++n)
    {
        ddstring_t const* path = pathList + n;
        Con_Printf("%*s\n", indent, Str_Text(path));
    }
}

static void deletePathList(ddstring_t* pathList, size_t numPaths)
{
    if(!pathList) return;
    for(size_t n = 0; n < numPaths; ++n)
    {
        ddstring_t* path = pathList + n;
        Str_Free(path);
    }
    M_Free(pathList);
}

void de::FileDirectory::debugPrint(FileDirectory& inst)
{
    Con_Printf("FileDirectory [%p]:\n", (void*)&inst);

    size_t numFiles;
    ddstring_t* pathList = inst.collectPaths(&numFiles, 0, DIR_SEP_CHAR);
    if(pathList)
    {
        qsort(pathList, numFiles, sizeof *pathList, comparePaths);

        printPathList(pathList, numFiles, 2/*indent*/);

        deletePathList(pathList, numFiles);
    }
    Con_Printf("  %lu %s in directory.\n", (unsigned long)numFiles, (numFiles==1? "path":"paths"));
}

void de::FileDirectory::debugPrintHashDistribution(FileDirectory& inst)
{
    PathTree::debugPrintHashDistribution(inst);
}
#endif
