/** @file modeldrawable.cpp  Drawable specialized for 3D models.
 *
 * @authors Copyright (c) 2014 Jaakko Keränen <jaakko.keranen@iki.fi>
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

#include "de/ModelDrawable"

namespace de {

DENG2_PIMPL(ModelDrawable)
{
    Asset modelAsset;

    Instance(Public *i) : Base(i)
    {}

    /// Release all loaded model data.
    void clear()
    {
        modelAsset.setState(NotReady);
    }
};

ModelDrawable::ModelDrawable() : d(new Instance(this))
{
    *this += d->modelAsset;
}

void ModelDrawable::load(File const &file)
{
    // Get rid of all existing data.
    clear();


    // Yay, we seem to have a model loaded.
    d->modelAsset.setState(Ready);
}

void ModelDrawable::clear()
{
    d->clear();
    Drawable::clear();
}

void ModelDrawable::glInit()
{

}

void ModelDrawable::glDeinit()
{
    clear();
}

} // namespace de
