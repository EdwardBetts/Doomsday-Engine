/** @file pathtreenode.cpp PathTree::Node implementation.
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

#include "de/PathTree"
#include "de/Log"

namespace de {

struct PathTree::Node::Instance
{
    /// @c true = this is a leaf node.
    bool isLeaf;

    /// PathTree which owns this node.
    PathTree &tree;

    /// Unique identifier for the path fragment this node represents,
    /// in the owning PathTree.
    PathTree::SegmentId segmentId;

    /// Parent node in the user's logical hierarchy.
    PathTree::Node *parent;

    Instance(PathTree &_tree, bool _isLeaf, PathTree::SegmentId _segmentId,
             PathTree::Node *_parent)
        : isLeaf(_isLeaf), tree(_tree), segmentId(_segmentId), parent(_parent)
    {}
};

PathTree::Node::Node(PathTree::NodeArgs const &args)
{
    d = new Instance(args.tree, args.type == PathTree::Leaf, args.segmentId, args.parent);
}

PathTree::Node::~Node()
{
    delete d;
}

bool PathTree::Node::isLeaf() const
{
    return d->isLeaf;
}

PathTree &PathTree::Node::tree() const
{
    return d->tree;
}

PathTree::Node *PathTree::Node::parent() const
{
    return d->parent;
}

PathTree::SegmentId PathTree::Node::segmentId() const
{
    return d->segmentId;
}

String const &PathTree::Node::name() const
{
    return tree().segmentName(d->segmentId);
}

Path::hash_type PathTree::Node::hash() const
{
    return tree().segmentHash(d->segmentId);
}

/// @todo This logic should be encapsulated in de::Path or de::Path::Segment; use QChar.
static int matchName(char const *string, char const *pattern)
{
    char const *in = string, *st = pattern;

    while(*in)
    {
        if(*st == '*')
        {
            st++;
            continue;
        }

        if(*st != '?' && (tolower((unsigned char) *st) != tolower((unsigned char) *in)))
        {
            // A mismatch. Hmm. Go back to a previous '*'.
            while(st >= pattern && *st != '*')
            { st--; }

            // No match?
            if(st < pattern) return false;

            // The asterisk lets us continue.
        }

        // This character of the pattern is OK.
        st++;
        in++;
    }

    // Skip remaining asterisks.
    while(*st == '*')
    { st++; }

    // Match is good if the end of the pattern was reached.
    return *st == 0;
}

int PathTree::Node::comparePath(de::Path const &searchPattern, ComparisonFlags flags) const
{
    if(((flags & PathTree::NoLeaf)   && isLeaf()) ||
       ((flags & PathTree::NoBranch) && !isLeaf()))
        return 1;

    try
    {
        de::Path::Segment const *snode = &searchPattern.lastSegment();

        // In reverse order, compare each path node in the search term.
        int pathNodeCount = searchPattern.segmentCount();

        PathTree::Node const *node = this;
        for(int i = 0; i < pathNodeCount; ++i)
        {
            bool const snameIsWild = !snode->toString().compare("*");
            if(!snameIsWild)
            {
                // If the hashes don't match it can't possibly be this.
                if(snode->hash() != node->hash())
                {
                    return 1;
                }

                // Compare the names.
                /// @todo Optimize: conversion to string is unnecessary.
                QByteArray name  = node->name().toUtf8();
                QByteArray sname = snode->toString().toUtf8();

                if(!matchName(name.constData(), sname.constData()))
                {
                    return 1;
                }
            }

            // Have we arrived at the search target?
            if(i == pathNodeCount - 1)
            {
                return !(!(flags & MatchFull) || !node->parent());
            }

            // Is the hierarchy too shallow?
            if(!node->parent())
            {
                return 1;
            }

            // So far so good. Move one level up the hierarchy.
            node  = node->parent();
            snode = &searchPattern.reverseSegment(i + 1);
        }
    }
    catch(de::Path::OutOfBoundsError const &)
    {} // Ignore this error.

    return 1;
}

#ifdef LIBDENG_STACK_MONITOR
static void *stackStart;
static size_t maxStackDepth;
#endif

namespace internal {
    struct PathConstructorParams {
        size_t length;
        String composedPath;
        QChar separator;
    };
}

/**
 * Recursive path constructor. First finds the root and the full length of the
 * path (when descending), then allocates memory for the string, and finally
 * copies each segment with the separators (on the way out).
 */
static void pathConstructor(internal::PathConstructorParams &parm, PathTree::Node const &trav)
{
    String const &fragment = trav.name();

#ifdef LIBDENG_STACK_MONITOR
    maxStackDepth = MAX_OF(maxStackDepth, stackStart - (void *)&fragment);
#endif

    parm.length += fragment.length();

    if(trav.parent())
    {
        if(!parm.separator.isNull())
        {
            // There also needs to be a separator (a single character).
            parm.length += 1;
        }

        // Descend to parent level.
        pathConstructor(parm, *trav.parent());

        // Append the separator.
        if(!parm.separator.isNull())
            parm.composedPath.append(parm.separator);
    }
    // We've arrived at the deepest level. The full length is now known.
    // Ensure there's enough memory for the string.
    else if(parm.composedPath)
    {
        parm.composedPath.reserve(parm.length);
    }

    // Assemble the path by appending the fragment.
    parm.composedPath.append(fragment);
}

/**
 * @todo This is a good candidate for result caching: the constructed path
 * could be saved and returned on subsequent calls. Are there any circumstances
 * in which the cached result becomes obsolete? -jk
 *
 * The only times the result becomes obsolete is when the delimiter is changed
 * or when the directory itself is rebuilt (in which case the nodes themselves
 * will be free'd). Note that any caching mechanism should not counteract one
 * of the primary goals of this class, i.e., optimal memory usage for the whole
 * directory. Caching constructed paths for every leaf node in the directory
 * would completely negate the benefits of the design of this class.
 *
 * Perhaps a fixed size MRU cache? -ds
 */
Path PathTree::Node::composePath(QChar sep) const
{
    internal::PathConstructorParams parm = { 0, String(), sep };
#ifdef LIBDENG_STACK_MONITOR
    stackStart = &parm;
#endif

    // Include a terminating path delimiter for branches.
    if(!sep.isNull() && !isLeaf())
    {
        parm.length++; // A single character.
    }

    // Recursively construct the path from fragments and delimiters.
    pathConstructor(parm, *this);

    // Terminating delimiter for branches.
    if(!sep.isNull() && !isLeaf())
    {
        parm.composedPath += sep;
    }

    DENG2_ASSERT(parm.composedPath.length() == (int)parm.length);

#ifdef LIBDENG_STACK_MONITOR
    LOG_AS("pathConstructor");
    LOG_INFO("Max stack depth: %1 bytes") << maxStackDepth;
#endif

    return Path(parm.composedPath, sep);
}

UserDataNode::UserDataNode(PathTree::NodeArgs const &args, void *userPointer, int userValue)
    : PathTree::Node(args),
      _pointer(userPointer),
      _value(userValue)
{}

void *UserDataNode::userPointer() const
{
    return _pointer;
}

int UserDataNode::userValue() const
{
    return _value;
}

UserDataNode &UserDataNode::setUserPointer(void *ptr)
{
    _pointer = ptr;
    return *this;
}

UserDataNode &UserDataNode::setUserValue(int value)
{
    _value = value;
    return *this;
}

} // namespace de
