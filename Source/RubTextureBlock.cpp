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
#include "JDHUtility/stb_image.h"
#include "RubTextureBlock.h"

namespace PaintingRegistration
{
    /* Static */
    const float RubTextureBlock::RUB_DECREMENT = 0.001f;
    
    /* Public */
    RubTextureBlock::RubTextureBlock(std::string format, unsigned int start, unsigned int end) :
    TextureBlock(format, start, end)
    {
        loadAll();
        initTexture();
        
        mask = new float[dimensions.getX() * dimensions.getY()];
        initMask();
    }
    
    RubTextureBlock::~RubTextureBlock(void)
    {
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            free(textures[i]);
        }
        textures.clear();
        
        NDELETE_ARRAY(mask);
    }
    
    void RubTextureBlock::bind(void) const
    {
        glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }
    
    void RubTextureBlock::unbind(void) const
    {
        glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, NULL);
    }
    
    void RubTextureBlock::reset(void)
    {
        initMask();
        updatePixels(0, 0, dimensions.getX(), dimensions.getY());
    }
    
    void RubTextureBlock::update(float fx, float fy, float fcs, bool mode)
    {
        int x = (int)(fx * (float)dimensions.getX());
        int y = (int)(fy * (float)dimensions.getY());
        
        if(x > 0 && x < dimensions.getX() && y > 0 && y < dimensions.getY())
        {
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
                            v = mask[index] - RUB_DECREMENT;
                        }
                        else
                        {
                            v = mask[index] + RUB_DECREMENT;
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
    
    void RubTextureBlock::initTexture(void)
    {
        unsigned char *initialData = textures[textures.size() - 1];
        
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, bpp == 4 ? GL_RGBA : GL_RGB, dimensions.getX(), dimensions.getY(), 0, bpp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, initialData);
        glDisable(GL_TEXTURE_2D);
    }
    
    void RubTextureBlock::loadTexture(unsigned int i)
    {
        char buffer[1024];
        sprintf(buffer, format.c_str(), i, i);
        std::string resPath = FileLocationUtility::getFileInResourcePath(buffer);
        
#ifdef IOS_WINDOWING
        stbi_convert_iphone_png_to_rgb(1);
#endif
        int x, y, n;
        unsigned char *data = stbi_load(resPath.c_str(), &x, &y, &n, 0);
        textures.push_back(data);
        
        dimensions.setX(x);
        dimensions.setY(y);
        bpp = n;
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
//#define LERP
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
                    float mIndex = m  *(float)(textures.size() - 1);
                    float floorM = floor(mIndex);
                    float ceilM = ceil(mIndex);
                    
                    unsigned int f_tId = (unsigned int)floorM;
                    unsigned char *f_t = textures[f_tId];
                    
                    unsigned int c_tId = (unsigned int)ceilM;
                    unsigned char *c_t = textures[c_tId];
                    
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
                        unsigned short f_r = f_t[index];
                        unsigned short f_g = f_t[index + 1];
                        unsigned short f_b = f_t[index + 2];
                    
                        unsigned short c_r = c_t[index];
                        unsigned short c_g = c_t[index + 1];
                        unsigned short c_b = c_t[index + 2];
                    
                        float fd = 1.0f -(mIndex - floorM);
                        float cd = 1.0f - (ceilM - mIndex);
                        
                        float fr = (float)f_r * fd;
                        float fg = (float)f_g * fd;
                        float fb = (float)f_b * fd;

                        float cr = (float)c_r * cd;
                        float cg = (float)c_g * cd;
                        float cb = (float)c_b * cd;
                        
                        unsigned short r = (unsigned char)(fr + cr);
                        unsigned short g = (unsigned char)(fg + cg);
                        unsigned short b = (unsigned char)(fb + cb);
                    
                        data[ptr] = (unsigned char)r;
                        data[ptr + 1] = (unsigned char)g;
                        data[ptr + 2] = (unsigned char)b;
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