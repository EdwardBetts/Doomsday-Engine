/** @file sound.h  Logical Sound model for the audio::System.
 * @ingroup audio
 *
 * @authors Copyright © 2003-2013 Jaakko Keränen <jaakko.keranen@iki.fi>
 * @authors Copyright © 2007-2015 Daniel Swanson <danij@dengine.net>
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

#ifndef CLIENT_AUDIO_SOUND_H
#define CLIENT_AUDIO_SOUND_H

#include "dd_share.h"  // SoundEmitter
#include <de/Vector>
#include <QFlags>

namespace audio {

/**
 * Sound behavior flags.
 *
 * @ingroup audio
 */
enum SoundFlag
{
    Repeat              = 0x1,
    NoOrigin            = 0x2,  ///< Originator is some mystical emitter.
    NoVolumeAttenuation = 0x4,  ///< Distance based volume attenuation is disabled.

    DefaultSoundFlags = NoOrigin
};
Q_DECLARE_FLAGS(SoundFlags, SoundFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(SoundFlags)

/**
 * POD description of a sound to be played (used with audio::Stage).
 *
 * @ingroup audio
 */
struct SoundParams
{
    SoundFlags flags    { DefaultSoundFlags };
    de::dint effectId   = 0;
    de::dfloat volume   = 1;
    de::Vector3d origin = de::Vector3d(0, 0, 0);
};

/**
 * Model of a logical sound, playing somewhere in the soundstage.
 *
 * Sounds track audio events on a purely logical level (irrespective of whether or not an
 * audio::Channel is available to play it, or if the associated effect will actually be
 * audible to anyone).
 *
 * @ingroup audio
 */
class Sound
{
public:
    Sound();
    Sound(SoundFlags flags, de::dint effectId, de::dfloat volume, de::Vector3d const &origin,
          de::duint endTime, SoundEmitter *emitter = nullptr);

    Sound(Sound const &other);

    inline Sound &operator = (Sound other) {
        d.swap(other.d); return *this;
    }

    inline void swap(Sound &other) {
        d.swap(other.d);
    }

    /**
     * Returns @c true if the sound is currently playing relative to @a nowTime.
     */
    bool isPlaying(de::duint nowTime) const;

    /**
     * Returns the sound effect behavior flags.
     */
    SoundFlags flags() const;

    /**
     * Returns the attributed sound effect (i.e., Sound definition) identifier.
     */
    de::dint effectId() const;

    /**
     * Returns the origin of the sound effect in the soundstage.
     */
    de::Vector3d origin() const;

    /**
     * Returns the velocity of the sound effect in the soundstage.
     */
    de::Vector3d velocity() const;

    /**
     * Returns the @em initial volume of the sound effect.
     */
    de::dfloat volume() const;

    /**
     * Returns @c true if this is a moveable sound (i.e., the emitter is a map-object).
     */
    bool emitterIsMoving() const;

    /**
     * Returns the attributed soundstage emitter, if any (may return @c nullptr).
     */
    SoundEmitter *emitter() const;

    /**
     * If attributed to a soundstage emitter - update the current origin coordinates of
     * the sound with the latest coordinates from the emitter.
     *
     * @todo Handle this internally. -ds
     */
    void updateOriginFromEmitter();

private:
    DENG2_PRIVATE(d)
};

}  // namespace audio

namespace std {
    // std::swap specialization for audio::Sound
    template <>
    inline void swap<::audio::Sound>(::audio::Sound &a, ::audio::Sound &b) {
        a.swap(b);
    }
}

#endif  // CLIENT_AUDIO_SOUND_H