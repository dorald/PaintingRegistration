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
#include "JDHUtility/GLVbo.h"
#include "JDHUtility/Ndelete.h"
#include "CameraControls.h"
#include "Camera.h"

namespace PaintingRegistration
{
    /* Public */
    Camera::Camera(const Point2i &position, const Point2i &dimensions) :
        UIElement(position, dimensions)
    {
        int px = position.getX();
        int py = position.getY() + dimensions.getY() - CONTROL_BAR_HEIGHT;
        int dx = dimensions.getX();
        int dy = CONTROL_BAR_HEIGHT;
        
        controls = new CameraControls(Point2i(px, py), Point2i(dx, dy));
        controls->setClickedCallback(MakeDelegate(this, &Camera::controls_Clicked));
        registerEventHandler(controls);
        
        
        GLfloat data[12] =
		{
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f
		};
        
        GLfloat textureData[8] =
		{
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
		};
        
        vbo = new GLVbo(GL_TRIANGLE_STRIP, GL_STATIC_DRAW, data, 4, textureData);
    }
    
    Camera::~Camera(void)
    {
        NDELETE(controls);
    }
    
    bool Camera::contains(const FingerEventArgs &e) const
    {
        return false;
    }
    
    void Camera::render(void) const
    {
        float x = getSizef(position.getX());
		float y	= getSizef(position.getY());
		float h	= getSizef(dimensions.getY());
		float w	= getSizef(dimensions.getX());
        
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		glScalef(w, h, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cameraTexture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        vbo->render();
        
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
        
        controls->render();
    }
    
    void Camera::setCameraTexture(GLuint cameraTexture)
    {
        this->cameraTexture = cameraTexture;
    }
    
    /* Private */
    void Camera::controls_Clicked(UIElement *e)
    {
        if (clicked != NULL)
        {
            clicked(this);
        }
    }
}