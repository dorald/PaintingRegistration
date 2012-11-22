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
#include <OpenGL.h>

namespace JDHUtility
{
    class GLVbo;
    
	class GLPrimitives
	{
	public:
        static const GLsizei CIRCLE_VERTICES;
        
		static GLPrimitives *instance;
		static GLPrimitives *getInstance(void);

        void renderCircle           (void) const;
        void renderCircleOutline    (void) const;
		void renderSquare           (void) const;
        void renderSquareOutline    (void) const;

	private:
		GLPrimitives(void);
		~GLPrimitives(void);

		void initPrimitives(void);
       
        GLVbo *circleVbo;
        GLVbo *circleOutlineVbo;
        GLVbo *squareVbo;
        GLVbo *squareOutlineVbo;
        
	};
}