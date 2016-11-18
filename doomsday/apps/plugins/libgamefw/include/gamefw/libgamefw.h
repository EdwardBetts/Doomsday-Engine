/** @file libgamefw.h  Common framework for games.
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

#ifndef LIBGAMEFW_H
#define LIBGAMEFW_H

/*
 * The LIBGAMEFW_PUBLIC macro is used for declaring exported symbols. It must be
 * applied in all exported classes and functions. DEF files are not used for
 * exporting symbols on Windows.
 */
#if defined(_WIN32) && defined(_MSC_VER)
#  ifdef __LIBGAMEFW__
// This is defined when compiling the library.
#    define LIBGAMEFW_PUBLIC __declspec(dllexport)
#  else
#    define LIBGAMEFW_PUBLIC __declspec(dllimport)
#  endif
#else
// No need to use any special declarators.
#  define LIBGAMEFW_PUBLIC
#endif

#ifdef __cplusplus

/// libgamefw uses the @c gfw namespace for all its C++ symbols.
namespace gfw {



} // namespace gfw

#endif // __cplusplus

#endif // LIBGAMEFW_H
