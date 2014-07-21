/** @file thinker.h  Base for all thinkers.
 *
 * @authors Copyright (c) 2014 Jaakko Keränen <jaakko.keranen@iki.fi>
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

#ifndef LIBDOOMSDAY_THINKER_H
#define LIBDOOMSDAY_THINKER_H

#include "../libdoomsday.h"
#include <de/types.h>

struct thinker_s;

/// Function pointer to a function to handle an actor's thinking.
/// The argument is a pointer to the object doing the thinking.
typedef void (*thinkfunc_t) (void *);

// Thinker flags:
#define THINKF_STD_MALLOC  0x1     // allocated using M_Malloc rather than the zone
#define THINKF_DISABLED    0x2     // thinker is disabled (in stasis)

/**
 * Base for all thinker objects.
 */
typedef struct thinker_s {
    struct thinker_s *prev, *next;
    thinkfunc_t function;
    uint32_t _flags;
    thid_t id;              ///< Only used for mobjs (zero is not an ID).
    void *d;                ///< Private data (owned).
} thinker_t;

#ifdef __cplusplus
extern "C" {
#endif

LIBDOOMSDAY_PUBLIC dd_bool Thinker_InStasis(thinker_t const *thinker);

/**
 * Change the 'in stasis' state of a thinker (stop it from thinking).
 *
 * @param thinker  The thinker to change.
 * @param on  @c true, put into stasis.
 */
LIBDOOMSDAY_PUBLIC void Thinker_SetStasis(thinker_t *thinker, dd_bool on);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

#include <de/libcore.h>

/**
 * C++ wrapper for a POD thinker.
 *
 * Copying or assigning the thinker via this class ensures that the entire allocated
 * thinker size is copied, and a duplicate of the private data instance is made.
 *
 * Thinker::~Thinker() will delete the entire thinker, including its private data.
 * One can use Thinker::take() to acquire ownership of the POD thinker to prevent it
 * from being destroyed.
 *
 * Ultimately, thinkers should become a C++ class hierarchy, with the private data being
 * a normal de::IPrivate.
 */
class LIBDOOMSDAY_PUBLIC Thinker
{
public:
    /**
     * Transparently accesses a member of the internal POD thinker struct via a member
     * that behaves like a regular member variable. Allows old code that deals with
     * thinker_s to work on a Thinker instance.
     */
    template <typename T>
    class LIBDOOMSDAY_PUBLIC MemberDelegate {
    public:
        MemberDelegate(Thinker &thinker, int offset) : _thinker(thinker), _offset(offset) {}
        inline T &ref() { return *reinterpret_cast<T *>((char *)&_thinker.base() + _offset); }
        inline T const &ref() const { return *reinterpret_cast<T const *>((char const *)&_thinker.base() + _offset); }
        inline T &operator -> () { return ref(); } // pointer type T expected
        inline T const &operator -> () const { return ref(); } // pointer type T expected
        inline operator T & () { return ref(); }
        inline operator T const & () const { return ref(); }
        inline T &operator = (T const &other) {
            ref() = other;
            return ref();
        }
    private:
        Thinker &_thinker;
        int _offset;
    };

    /// Base class for the private data of a thinker.
    class LIBDOOMSDAY_PUBLIC IData
    {
    public:
        virtual ~IData() {}
        virtual IData *duplicate() const = 0;

        DENG2_AS_IS_METHODS()
    };

    enum AllocMethod { AllocateStandard, AllocateMemoryZone };

public:
    /**
     * Allocates a thinker using standard malloc.
     *
     * @param sizeInBytes  Size of the thinker. At least sizeof(thinker_s).
     * @param data         Optional private instance data.
     */
    Thinker(de::dsize sizeInBytes = 0, IData *data = 0);

    Thinker(AllocMethod alloc, de::dsize sizeInBytes = 0, IData *data = 0);

    Thinker(Thinker const &other);

    /**
     * Constructs a copy of a POD thinker. A duplicate of the private data is made
     * if one is present in @a podThinker.
     *
     * @param podThinker   Existing thinker.
     * @param sizeInBytes  Size of the thinker struct.
     */
    Thinker(thinker_s const &podThinker, de::dsize sizeInBytes,
            AllocMethod alloc = AllocateStandard);

    Thinker &operator = (Thinker const &other);

    operator thinker_s * () { return &base(); }
    operator thinker_s const * () { return &base(); }

    struct thinker_s &base();
    struct thinker_s const &base() const;

    void enable(bool yes = true);
    inline void disable(bool yes = true) { enable(!yes); }

    bool isDisabled() const;
    bool hasData() const;

    /**
     * Determines the size of the thinker in bytes.
     */
    de::dsize sizeInBytes() const;

    /**
     * Gives ownership of the contained POD thinker to the caller. The caller also
     * gets ownership of the private data owned by the thinker (a C++ instance).
     * You should use destroy() to delete the returned memory.
     *
     * @return POD thinker. The size of this struct is actually sizeInBytes(), not
     * sizeof(thinker_s).
     */
    struct thinker_s *take();

    IData &data();
    IData const &data() const;

public:
    /**
     * Destroys a POD thinker that has been acquired using take(). All the memory owned
     * by the thinker is released.
     *
     * @param thinkerBase  POD thinker base.
     */
    static void destroy(struct thinker_s *thinkerBase);

protected:
    /**
     * Sets the private data for the thinker.
     *
     * @param data  Private data object. Ownership taken.
     */
    void setData(IData *data);

private:
    DENG2_PRIVATE(d)

public:
    // Value accessors (POD thinker_s compatibility for old code):
    MemberDelegate<thinker_s *> prev;
    MemberDelegate<thinker_s *> next;
    MemberDelegate<thinkfunc_t> function;
    MemberDelegate<uint32_t> flags;
    MemberDelegate<thid_t> id;
};

#endif // __cplusplus

#endif // LIBDOOMSDAY_THINKER_H