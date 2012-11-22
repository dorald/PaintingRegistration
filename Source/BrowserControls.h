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
#pragma once

#include "UIElement.h"

namespace PaintingRegistration
{
    class ExitButton;
    class Slider;
    
    class BrowserControls :
        public UIElement
    {
    public:
        BrowserControls(const Point2i &position, const Point2i &dimensions);
        ~BrowserControls(void);
        
        void render(void) const;
        
    private:
        ExitButton *exit;
        Slider *slider;
        
        void exit_Clicked(UIElement *e);
        void slider_ValueChanged(float value);
    };
}