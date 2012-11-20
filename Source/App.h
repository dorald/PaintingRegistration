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
#include <string>
#include "MultiTouchEvents/MultiTouchEventManager.h"

using namespace MultiTouchEvents;

namespace JDHUtility
{
    class GLTexture;
}
using namespace JDHUtility;

namespace PaintingRegistration
{
    class Browser;
    class Camera;
    class PaintingTracker;
    class UIElement;
    
    class App :
        public MultiTouchEventManager
    {
    public:
        enum UIMode { CAMERA, BROWSER };
        
        App(unsigned int width, unsigned int height, std::string resourcePath);
        ~App(void);
        
        void render(void) const;
        void update(void);
        
    private:
        Browser *browser;
        Camera *camera;
        PaintingTracker *tracker;
        GLTexture *texture;
        UIMode uiMode;
        
        void camera_Clicked(UIElement *e);
        void initScene(unsigned int width, unsigned int height);
        void initUI(unsigned int width, unsigned int height);
    };
}