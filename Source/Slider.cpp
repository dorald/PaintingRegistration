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
#include "JDHUtility/GLPrimitives.h"
#include "JDHUtility/GLTexture.h"
#include "JDHUtility/Ndelete.h"
#include "Slider.h"

namespace PaintingRegistration
{
    /* Static */
    const unsigned int Slider::BAR_WIDTH = 30;

    /* Public */
    Slider::Slider(float value, const Point2i &position, const Point2i &dimensions) :
        UIElement(position, dimensions)
    {
        this->value = value;
        
        valueChanged = NULL;
        
        texture = new GLTexture("slider.png");
        barTexture = new GLTexture("bar.png");
    }
    
    Slider::~Slider(void)
    {
        NDELETE(texture);
    }
    
    void Slider::fingerAdded(const FingerEventArgs &e)
    {
        UIElement::fingerAdded(e);
        updateSliderValue(e.getX());
    }
    
    void Slider::fingerUpdated(const FingerEventArgs &e)
    {
        UIElement::fingerUpdated(e);
        updateSliderValue(e.getX());
    }
    
    float Slider::getValue(void) const
    {
        return value;
    }
    
    void Slider::render(void) const
    {
        float x = getSizef(position.getX());
		float y	= getSizef(position.getY());
		float h	= getSizef(dimensions.getY());
		float w	= getSizef(dimensions.getX());
        
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		glScalef(w, h, 1.0f);
        
        if(selected)
        {
            BLUE.use();
        }
        else
        {
            GREY.use();
        }
        
        GLPrimitives::getInstance()->renderSquare();
        
        glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        texture->bind(GL_REPLACE);
        GLPrimitives::getInstance()->renderSquare();
        texture->unbind();
        
        glPopMatrix();

        float b = getSizef(BAR_WIDTH);
        float bx = x + (value * (w - b));

        glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(bx, y, 0.0f);
		glScalef(b, h, 1.0f);
        
        barTexture->bind();
        GLPrimitives::getInstance()->renderSquare();
        barTexture->unbind();
        
        glDisable(GL_BLEND);
        glPopMatrix();
    }
    
    void Slider::setValue(float value)
    {
        this->value = value;
    }
    
    void Slider::setValueChangedCallback(ValueChangedCallback valueChanged)
    {
        this->valueChanged = valueChanged;
    }
    
    /* Private */
    void Slider::updateSliderValue(float x)
    {
        x = x - getSizef(position.getX()) - getSizef(BAR_WIDTH / 2);
        value = x / getSizef(dimensions.getX() - (BAR_WIDTH / 2));
        
        if(value < 0.0f)
        {
            value = 0.0f;
        }
        else if(value > 1.0f)
        {
            value = 1.0f;
        }
        
        if(valueChanged != NULL)
        {
            valueChanged(value);
        }
    }
}