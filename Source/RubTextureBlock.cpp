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
#include <assert.h>
#include <math.h>
#include "JDHUtility/FileLocationUtility.h"
#include "JDHUtility/Ndelete.h"
#include "RubTextureBlock.h"

namespace PaintingRegistration
{
    /* Static */
#ifdef TRANSAMERICA
    const float RubTextureBlock::RUB_DECREMENT = 0.038f;
#elif FLATIRON
    const float RubTextureBlock::RUB_DECREMENT = 0.01f;
#endif
    
    const float RubTextureBlock::SPEED_MULTIPLIER = 10.0f;
    const float RubTextureBlock::MAX_SPEED = 2.0f;
    
    /* Public */
    RubTextureBlock::RubTextureBlock(std::string format, unsigned int start, unsigned int end) :
    TextureBlock(format, start, end)
    {        
        mask = new float[dimensions.getX() * dimensions.getY()];
        initMask();
    }
    
    RubTextureBlock::~RubTextureBlock(void)
    {        
        NDELETE_ARRAY(mask);
    }
    
    void RubTextureBlock::reset(void)
    {
        initMask();
        updatePixels(0, 0, dimensions.getX(), dimensions.getY());
    }
    
    void RubTextureBlock::update(float fx, float fy, float fcs, bool mode, float fingerSpeed)
    {
        int x = (int)(fx * (float)dimensions.getX());
        int y = (int)(fy * (float)dimensions.getY());
        
        if(x > 0 && x < dimensions.getX() && y > 0 && y < dimensions.getY())
        {
            float speed = fingerSpeed * SPEED_MULTIPLIER;
            if(speed > MAX_SPEED)
            {
                speed = MAX_SPEED;
            }

            int cursorSize = (int)(fcs * (float)dimensions.getX());
            int limit = cursorSize / 2;
            
            int stride = dimensions.getX();
            for(int i = y - limit; i < y + limit; i++)
            {
                float fd = (float)limit - (float)abs(i - y);
                fd = (fd / (float)limit) * (M_PI / 2.0f);
                fd = sin(fd);
                fd *= limit;
                
                int d = (int)fd;
                for(int j = x - d; j < x + d; j++)
                {
                    if(isWithinPainting(j, i))
                    {
                        int index = i * stride + j;
                        
                        float v = 0.0f;
                        if(mode)
                        {
                            v = mask[index] - (RUB_DECREMENT * speed);
                        }
                        else
                        {
                            v = mask[index] + (RUB_DECREMENT * speed);
                        }
                        
                        if(v < 0.0f)
                        {
                            v = 0.0f;
                        }
                        else if(v > 1.0f)
                        {
                            v = 1.0f;
                        }
                        
                        mask[index] = v;
                    }
                }
            }
            
            updatePixels(x - limit, y - limit, cursorSize, cursorSize);
        }
    }
    
    /* Private */
    void RubTextureBlock::initMask(void)
    {
        unsigned int size = dimensions.getX() * dimensions.getY();
        for(unsigned int i = 0; i < size; i++)
        {
            mask[i] = 1.0f;
        }
    }
    
    bool RubTextureBlock::isWithinPainting(int x, int y) const
    {
        return x >= 0 &&
        x < dimensions.getX() &&
        y >= 0 &&
        y < dimensions.getY();
    }
    
    void RubTextureBlock::updatePixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
    {
        unsigned int imageWidth = dimensions.getX();
        unsigned int imageHeight = dimensions.getY();
        unsigned int cursorSize = width * height * bpp;
        unsigned char *data = new unsigned char[cursorSize];
        unsigned int ptr = 0;
        
        for(unsigned int i = y; i < y + height; i++)
        {
            for(unsigned int j = x; j < x + width; j++)
            {
                if(i < imageHeight && j < imageWidth)
                {
#define LERP
#ifndef LERP
                    float m = mask[j + (i * imageWidth)];
                    unsigned int tId = (unsigned int)(m * (float)(textures.size() - 1));
                    unsigned char *t = textures[tId];
                    
                    unsigned int index = (j * bpp) + (i * imageWidth * bpp);
                    unsigned char r = t[index];
                    unsigned char g = t[index + 1];
                    unsigned char b = t[index + 2];
                    
                    data[ptr] = r;
                    data[ptr + 1] = g;
                    data[ptr + 2] = b;
                    ptr += 3;
#else
                    float m = mask[j + (i * imageWidth)];
                    unsigned int tSize = textures.size();
                    
                    float mIndex = m  *(float)(tSize - 1);
                    float floorM = floor(mIndex);
                    float ceilM = ceil(mIndex);
                    
                    unsigned int f_tId = (unsigned int)floorM;
                    unsigned char *f_t = textures[f_tId];                    
                    unsigned int index = (j * bpp) + (i * imageWidth * bpp);
                    
                    if(ceilM == floorM)
                    {
                        unsigned char r = f_t[index];
                        unsigned char g = f_t[index + 1];
                        unsigned char b = f_t[index + 2];
                        
                        data[ptr] = r;
                        data[ptr + 1] = g;
                        data[ptr + 2] = b;
                        ptr += 3;
                    }
                    else
                    {
                        unsigned int c_tId = (unsigned int)ceilM;
                        unsigned char *c_t = textures[c_tId];
                        
                        float f_r = (float)f_t[index];
                        float f_g = (float)f_t[index + 1];
                        float f_b = (float)f_t[index + 2];
                    
                        float c_r = (float)c_t[index];
                        float c_g = (float)c_t[index + 1];
                        float c_b = (float)c_t[index + 2];
                    
                        float fd = 1.0f - (mIndex - floorM);
                        float cd = 1.0f - (ceilM - mIndex);
                        
                        float fr = f_r * fd;
                        float fg = f_g * fd;
                        float fb = f_b * fd;

                        float cr = c_r * cd;
                        float cg = c_g * cd;
                        float cb = c_b * cd;
                        
                        unsigned char r = (unsigned char)(fr + cr);
                        unsigned char g = (unsigned char)(fg + cg);
                        unsigned char b = (unsigned char)(fb + cb);

                        data[ptr] = r;
                        data[ptr + 1] = g;
                        data[ptr + 2] = b;
                        ptr += 3;
                    }
#endif
                }
            }
        }
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, bpp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, NULL);
        glDisable(GL_TEXTURE_2D);
        
        NDELETE_ARRAY(data);
    }
}