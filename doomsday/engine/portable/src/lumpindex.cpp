/**
 * @file lumpindex.cpp
 *
 * @ingroup fs
 *
 * @author Copyright &copy; 2003-2012 Jaakko Keränen <jaakko.keranen@iki.fi>
 * @author Copyright &copy; 2006-2012 Daniel Swanson <danij@dengine.net>
 * @author Copyright &copy; 2006 Jamie Jones <jamie_jones_au@yahoo.com.au>
 * @author Copyright &copy; 1999-2006 by Colin Phipps, Florian Schulze, Neil Stevens, Andrey Budko (PrBoom 2.2.6)
 * @author Copyright &copy; 1999-2001 by Jess Haas, Nicolas Kalkhof (PrBoom 2.2.6)
 * @author Copyright &copy; 1999 Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman (PrBoom 2.2.6)
 * @author Copyright &copy; 1993-1996 by id Software, Inc.
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
#include "de_filesys.h"

#include "lumpindex.h"

#include <QBitArray>
#include <QList>
#include <QVector>

#include <de/Log>

using namespace de;
using de::AbstractFile;

/**
 * @ingroup lumpIndexFlags
 */
///@{
#define LIF_INTERNAL_MASK               0xff000000
#define LIF_NEED_REBUILD_HASH           0x80000000 ///< Path hash map must be rebuilt.
#define LIF_NEED_PRUNE_DUPLICATES       0x40000000 ///< Path duplicate records must be pruned.
///@}

/// Stores indexes into LumpIndex::Instance::records forming a chain of
/// PathDirectoryNode fragment hashes. For ultra-fast name lookups.
struct LumpIndexHashRecord
{
    lumpnum_t head, next;
};

struct LumpIndex::Instance
{
    typedef QVector<LumpIndexHashRecord> HashMap;

    LumpIndex* self;
    int flags; /// @see lumpIndexFlags
    LumpIndex::Lumps lumpInfos;
    HashMap* hashMap;

    Instance(LumpIndex* d, int _flags)
        : self(d), flags(_flags & ~LIF_INTERNAL_MASK), lumpInfos(), hashMap(0)
    {}

    ~Instance()
    {
        if(hashMap) delete hashMap;
    }

    void buildHashMap()
    {
        if(!(flags & LIF_NEED_REBUILD_HASH)) return;

        int numRecords = lumpInfos.size();
        if(!hashMap)    hashMap = new HashMap(numRecords);
        else            hashMap->resize(numRecords);

        // Clear the chains.
        DENG2_FOR_EACH(i, *hashMap, HashMap::iterator)
        {
            i->head = -1;
        }

        // Prepend nodes to each chain, in first-to-last load order, so that
        // the last lump with a given name appears first in the chain.
        for(int i = 0; i < numRecords; ++i)
        {
            LumpInfo const* lumpInfo = lumpInfos[i];
            AbstractFile* container = reinterpret_cast<AbstractFile*>(lumpInfo->container);
            PathDirectoryNode const& node = container->lumpDirectoryNode(lumpInfo->lumpIdx);
            ushort j = node.hash() % (unsigned)numRecords;

            (*hashMap)[i].next = (*hashMap)[j].head; // Prepend to the chain.
            (*hashMap)[j].head = i;
        }

        flags &= ~LIF_NEED_REBUILD_HASH;

        LOG_DEBUG("Rebuilt hashMap for LumpIndex %p.") << self;
    }

    /**
     * @param pruneFlags  Passed by reference to avoid deep copy on value-write.
     * @return Number of lumps newly flagged during this op.
     */
    int flagFileLumps(QBitArray& pruneFlags, AbstractFile& file)
    {
        DENG_ASSERT(pruneFlags.size() == lumpInfos.size());

        int const numRecords = lumpInfos.size();
        int numFlagged = 0;
        for(int i = 0; i < numRecords; ++i)
        {
            if(pruneFlags.testBit(i)) continue;
            if(reinterpret_cast<AbstractFile*>(lumpInfos[i]->container) != &file) continue;
            pruneFlags.setBit(i, true);
            numFlagged += 1;
        }
        return numFlagged;
    }

    struct LumpSortInfo
    {
        LumpInfo const* lumpInfo;
        AutoStr* path;
        int origIndex;
    };
    static int lumpSorter(void const* a, void const* b)
    {
        LumpSortInfo const* infoA = (LumpSortInfo const*)a;
        LumpSortInfo const* infoB = (LumpSortInfo const*)b;
        int result = Str_CompareIgnoreCase(infoA->path, Str_Text(infoB->path));
        if(0 != result) return result;

        // Still matched; try the file load order indexes.
        result = (reinterpret_cast<AbstractFile*>(infoA->lumpInfo->container)->loadOrderIndex() -
                  reinterpret_cast<AbstractFile*>(infoB->lumpInfo->container)->loadOrderIndex());
        if(0 != result) return result;

        // Still matched (i.e., present in the same package); use the original indexes.
        return (infoB->origIndex - infoA->origIndex);
    }

    /**
     * @param pruneFlags  Passed by reference to avoid deep copy on value-write.
     * @return Number of lumps newly flagged during this op.
     */
    int flagDuplicateLumps(QBitArray& pruneFlags)
    {
        DENG_ASSERT(pruneFlags.size() == lumpInfos.size());

        // Any work to do?
        if(!(flags & LIF_UNIQUE_PATHS)) return 0;
        if(!(flags & LIF_NEED_PRUNE_DUPLICATES)) return 0;

        int const numRecords = lumpInfos.size();
        if(numRecords <= 1) return 0;

        // Sort in descending load order for pruning.
        LumpSortInfo* sortInfos = new LumpSortInfo[numRecords];
        for(int i = 0; i < numRecords; ++i)
        {
            LumpSortInfo& sortInfo   = sortInfos[i];
            LumpInfo const* lumpInfo = lumpInfos[i];

            sortInfo.lumpInfo = lumpInfo;
            sortInfo.origIndex = i;
            sortInfo.path = reinterpret_cast<AbstractFile*>(lumpInfo->container)->composeLumpPath(lumpInfo->lumpIdx, '/' /*delimiter is irrelevant*/);
        }
        qsort(sortInfos, numRecords, sizeof(*sortInfos), lumpSorter);

        // Flag the lumps we'll be pruning.
        int numFlagged = 0;
        for(int i = 1; i < numRecords; ++i)
        {
            if(pruneFlags.testBit(i)) continue;
            if(Str_CompareIgnoreCase(sortInfos[i - 1].path, Str_Text(sortInfos[i].path))) continue;
            pruneFlags.setBit(sortInfos[i].origIndex, true);
            numFlagged += 1;
        }

        // Free temporary sort data.
        delete[] sortInfos;

        return numFlagged;
    }

    /// @return Number of pruned lumps.
    int pruneFlaggedLumps(QBitArray flaggedLumps)
    {
        DENG_ASSERT(flaggedLumps.size() == lumpInfos.size());

        // Have we lumps to prune?
        int const numFlaggedForPrune = flaggedLumps.count(true);
        if(numFlaggedForPrune)
        {
            // We'll need to rebuild the hash after this.
            flags |= LIF_NEED_REBUILD_HASH;

            int numRecords = lumpInfos.size();
            if(numRecords == numFlaggedForPrune)
            {
                lumpInfos.clear();
            }
            else
            {
                // Do this one lump at a time, respecting the possibly-sorted order.
                for(int i = 0, newIdx = 0; i < numRecords; ++i)
                {
                    if(!flaggedLumps.testBit(i))
                    {
                        ++newIdx;
                        continue;
                    }

                    // Move the info for the lump to be pruned to the end.
                    lumpInfos.move(newIdx, lumpInfos.size() - 1);
                }

                // Erase the pruned lumps from the end of the list.
                int firstPruned = lumpInfos.size() - numFlaggedForPrune;
                lumpInfos.erase(lumpInfos.begin() + firstPruned, lumpInfos.end());
            }
        }
        return numFlaggedForPrune;
    }

    void pruneDuplicates()
    {
        int const numRecords = lumpInfos.size();
        if(numRecords <= 1) return;

        QBitArray pruneFlags(numRecords);
        flagDuplicateLumps(pruneFlags);
        pruneFlaggedLumps(pruneFlags);

        flags &= ~LIF_NEED_PRUNE_DUPLICATES;
    }
};

LumpIndex::LumpIndex(int flags)
{
    d = new Instance(this, flags);
}

LumpIndex::~LumpIndex()
{
    clear();
    delete d;
}

bool LumpIndex::isValidIndex(lumpnum_t lumpNum)
{
    // We may need to prune path-duplicate lumps.
    d->pruneDuplicates();
    return (lumpNum >= 0 && lumpNum < d->lumpInfos.size());
}

static QString invalidIndexMessage(int invalidIdx, int lastValidIdx)
{
    QString msg = QString("Invalid lump index %1 ").arg(invalidIdx);
    if(lastValidIdx < 0) msg += "(file is empty)";
    else                 msg += QString("(valid range: [0..%2])").arg(lastValidIdx);
    return msg;
}

LumpInfo const& LumpIndex::lumpInfo(lumpnum_t lumpNum)
{
    if(!isValidIndex(lumpNum)) throw de::Error("LumpIndex::lumpInfo", invalidIndexMessage(lumpNum, size() - 1));
    return *d->lumpInfos[lumpNum];
}

LumpIndex::Lumps const& LumpIndex::lumps()
{
    // We may need to prune path-duplicate lumps.
    d->pruneDuplicates();

    return d->lumpInfos;
}

int LumpIndex::size()
{
    // We may need to prune path-duplicate lumps.
    d->pruneDuplicates();

    return d->lumpInfos.size();
}

int LumpIndex::pruneByFile(AbstractFile& file)
{
    if(d->lumpInfos.empty()) return 0;

    int const numRecords = d->lumpInfos.size();
    QBitArray pruneFlags(numRecords);

    // We may need to prune path-duplicate lumps. We'll fold those into this
    // op as pruning may result in reallocations.
    d->flagDuplicateLumps(pruneFlags);

    // Flag the lumps we'll be pruning.
    int numFlaggedForFile = d->flagFileLumps(pruneFlags, file);

    // Perform the prune.
    d->pruneFlaggedLumps(pruneFlags);

    d->flags &= ~LIF_NEED_PRUNE_DUPLICATES;

    return numFlaggedForFile;
}

bool LumpIndex::pruneLump(LumpInfo& lumpInfo)
{
    if(d->lumpInfos.empty()) return 0;

    // We may need to prune path-duplicate lumps.
    d->pruneDuplicates();

    // Prune this lump.
    if(!d->lumpInfos.removeOne(&lumpInfo)) return false;

    // We'll need to rebuild the path hash chains.
    d->flags |= LIF_NEED_REBUILD_HASH;
    return true;
}

void LumpIndex::catalogLumps(AbstractFile& file, int lumpIdxBase, int numLumps)
{
    if(numLumps <= 0) return;

#ifdef DENG2_QT_4_7_OR_NEWER
    // Allocate more memory for the new records.
    d->lumpInfos.reserve(d->lumpInfos.size() + numLumps);
#endif

    for(int i = 0; i < numLumps; ++i)
    {
        LumpInfo const* info = &file.lumpInfo(lumpIdxBase + i);
        d->lumpInfos.push_back(info);
    }

    // We'll need to rebuild the name hash chains.
    d->flags |= LIF_NEED_REBUILD_HASH;

    if(d->flags & LIF_UNIQUE_PATHS)
    {
        // We may need to prune duplicate paths.
        d->flags |= LIF_NEED_PRUNE_DUPLICATES;
    }
}

void LumpIndex::clear()
{
    d->lumpInfos.clear();
    d->flags &= ~(LIF_NEED_REBUILD_HASH | LIF_NEED_PRUNE_DUPLICATES);
}

bool LumpIndex::catalogues(AbstractFile& file)
{
    // We may need to prune path-duplicate lumps.
    d->pruneDuplicates();

    DENG2_FOR_EACH(i, d->lumpInfos, Lumps::iterator)
    {
        LumpInfo const* lumpInfo = *i;
        if(reinterpret_cast<AbstractFile*>(lumpInfo->container) == &file) return true;
    }
    return false;
}

lumpnum_t LumpIndex::indexForPath(char const* path)
{
    if(!path || !path[0] || d->lumpInfos.empty()) return -1;

    // We may need to prune path-duplicate lumps.
    d->pruneDuplicates();

    // We may need to rebuild the path hash map.
    d->buildHashMap();
    DENG_ASSERT(d->hashMap);

    // Perform the search.
    bool builtSearchPattern = false;
    PathMap searchPattern;

    int hash = PathDirectory::hashPathFragment(path, strlen(path)) % d->hashMap->size();
    int idx;
    for(idx = (*d->hashMap)[hash].head; idx != -1; idx = (*d->hashMap)[idx].next)
    {
        LumpInfo const* lumpInfo = d->lumpInfos[idx];
        AbstractFile* container = reinterpret_cast<AbstractFile*>(lumpInfo->container);
        PathDirectoryNode const& node = container->lumpDirectoryNode(lumpInfo->lumpIdx);

        // Time to build the pattern?
        if(!builtSearchPattern)
        {
            PathMap_Initialize(&searchPattern, PathDirectory::hashPathFragment, path);
            builtSearchPattern = true;
        }

        if(node.matchDirectory(0, &searchPattern))
        {
            // This is the lump we are looking for.
            break;
        }
    }

    if(builtSearchPattern)
        PathMap_Destroy(&searchPattern);

    return idx;
}
