#ifndef BEST_FIT_OBB_H

#define BEST_FIT_OBB_H

#include <vector>
#include "float_math.h"
#include "algebra3.h"
/*----------------------------------------------------------------------
		Copyright (c) 2004 Open Dynamics Framework Group
					www.physicstools.org
		All rights reserved.

		Redistribution and use in source and binary forms, with or without modification, are permitted provided
		that the following conditions are met:

		Redistributions of source code must retain the above copyright notice, this list of conditions
		and the following disclaimer.

		Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

		Neither the name of the Open Dynamics Framework Group nor the names of its contributors may
		be used to endorse or promote products derived from this software without specific prior written permission.

		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,
		INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
		DISCLAIMED. IN NO EVENT SHALL THE INTEL OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
		EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
		IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
		THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------*/

// http://codesuppository.blogspot.com
//
// mailto: jratcliff@infiniplex.net
//
// http://www.amillionpixels.us
//

//	The inputs are:
//
//	vcount : number of input vertices in the point cloud.
//	points : a pointer to the first vertex.
//	pstride : The stride between each point measured in bytes.
//
//	The outputs are:
//
//	sides : The length of the sides of the OBB as X, Y, Z distance.
//	matrix : A pointer to a 4x4 matrix. This will contain the 3x3 rotation and the translation component.
//	pos : The center of the OBB
//	quat : The orientation of the OBB expressed as quaternion in the form of X,Y,Z,W

void computeSimpleBB(unsigned int vcount, const std::vector<float> *points, vec3 &bound_size, vec3 &bounding_max, vec3 &bounding_min, vec3 &bound_center);
void computeOBB(unsigned int vcount, const std::vector<float> *points, float *sides, const mat4 &matrix);
void computeOBB(unsigned int vcount, const float *points, unsigned int pstride, float *sides, const float *matrix);
void computeBestFitOBB(unsigned int vcount, const std::vector<float> *points, vec3 &obb_size, vec3 &obb_max, vec3 &obb_min, vec3 &obb_angle, mat4 &matrix);
void computeBestFitOBB(unsigned int vcount,const float *points,unsigned int pstride,float *sides,float *matrix);

#endif
