/**
 * This file is part of PaintingRegistration.
 *
 * Created by Jonathan Hook (jonathan.hook@ncl.ac.uk)
 * Copyright (c) 2012 Jonathan Hook. All rights reserved.
 *
 * PaintingRegistration is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PaintingRegistration is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PaintingRegistration.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "SlideBrowserControls.h"
#include "SlidePaintingRenderer.h"
#include "SlideBrowser.h"

namespace PaintingRegistration
{
    /* Public */
    SlideBrowser::SlideBrowser(const std::string textureFilenameFormat, const Point2i &position, const Point2i &dims, const Point2i &frameDims, const Point2i &textureDims, const Point2i &targetDims) :
        Browser(position, dims, textureDims, textureDims, targetDims)
    {
        int px = position.getX();
        int py = position.getY() + dimensions.getY() - CONTROL_BAR_HEIGHT;
        int dx = dimensions.getX();
        int dy = CONTROL_BAR_HEIGHT;
        
        controls = new SlideBrowserControls(Point2i(px, py), Point2i(dx, dy));
        controls->setClickedCallback(MakeDelegate(this, &SlideBrowser::controls_Clicked));
        ((SlideBrowserControls *)controls)->setPositionChangedCallback(MakeDelegate(this, &SlideBrowser::controls_PositionChanged));
        registerEventHandler(controls);
        
        painting = new SlidePaintingRenderer(textureFilenameFormat, position, dims,frameDims, textureDims, targetDims);
    }
    
    SlideBrowser::~SlideBrowser(void)
    {
        
    }
    
    void SlideBrowser::reset(void)
    {
        painting->setPosition(((SlideBrowserControls *)controls)->getSliderPosition());
    }
    
    /* Private */
    void SlideBrowser::controls_PositionChanged(float position)
    {
        painting->setPosition(position);
    }
}