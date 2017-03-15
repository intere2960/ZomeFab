
#include "bestfitobb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

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

void computeSimpleBB(unsigned int vcount, const std::vector<float> *points, vec3 &bound_size, vec3 &bounding_max, vec3 &bounding_min, vec3 &bound_center)
{	
	float bmin[3];
	float bmax[3];
	fm_getAABB(vcount, points, bmin, bmax);

	bound_center[0] = (bmax[0] - bmin[0])*0.5f + bmin[0];
	bound_center[1] = (bmax[1] - bmin[1])*0.5f + bmin[1];
	bound_center[2] = (bmax[2] - bmin[2])*0.5f + bmin[2];

	bounding_max[0] = bmax[0];
	bounding_max[1] = bmax[1];
	bounding_max[2] = bmax[2];

	bounding_min[0] = bmin[0];
	bounding_min[1] = bmin[1];
	bounding_min[2] = bmin[2];

	bound_size[0] = bounding_max[0] - bounding_min[0];
	bound_size[1] = bounding_max[1] - bounding_min[1];
	bound_size[2] = bounding_max[2] - bounding_min[2];
}

// computes the OBB for this set of points relative to this transform matrix.
void computeOBB(unsigned int vcount, const std::vector<float> *points, float *sides, const mat4 &matrix)
{
	float bmin[3] = { 1e9, 1e9, 1e9 };
	float bmax[3] = { -1e9, -1e9, -1e9 };

	for (unsigned int i = 1; i <= vcount; i++)
	{
		const float p[3] = { points->at(3 * i + 0), points->at(3 * i + 1), points->at(3 * i + 2) };
		
		float t[3];

		fm_inverseRT(matrix, p, t); // inverse rotate translate

		if (t[0] < bmin[0]) bmin[0] = t[0];
		if (t[1] < bmin[1]) bmin[1] = t[1];
		if (t[2] < bmin[2]) bmin[2] = t[2];

		if (t[0] > bmax[0]) bmax[0] = t[0];
		if (t[1] > bmax[1]) bmax[1] = t[1];
		if (t[2] > bmax[2]) bmax[2] = t[2];
	}

	sides[0] = bmax[0];
	sides[1] = bmax[1];
	sides[2] = bmax[2];

	if (fabsf(bmin[0]) > sides[0]) sides[0] = fabsf(bmin[0]);
	if (fabsf(bmin[1]) > sides[1]) sides[1] = fabsf(bmin[1]);
	if (fabsf(bmin[2]) > sides[2]) sides[2] = fabsf(bmin[2]);

	sides[0] *= 2.0f;
	sides[1] *= 2.0f;
	sides[2] *= 2.0f;

	/*printf("%f %f %f\n", bmax[0], bmax[1], bmax[2]);
	printf("%f %f %f\n", bmin[0], bmin[1], bmin[2]);
	printf("%f %f %f\n", sides[0], sides[1], sides[2]);*/
}

// computes the OBB for this set of points relative to this transform matrix.
void computeOBB(unsigned int vcount,const float *points,unsigned int pstride,float *sides,const float *matrix)
{
  const float *src = points;

  float bmin[3] = { 1e9, 1e9, 1e9 };
  float bmax[3] = { -1e9, -1e9, -1e9 };

  for (unsigned int i=0; i<vcount; i++)
  {
    const float *p = (const float *) src;
    float t[3];

    fm_inverseRT(matrix, p, t ); // inverse rotate translate

    if ( t[0] < bmin[0] ) bmin[0] = t[0];
    if ( t[1] < bmin[1] ) bmin[1] = t[1];
    if ( t[2] < bmin[2] ) bmin[2] = t[2];

    if ( t[0] > bmax[0] ) bmax[0] = t[0];
    if ( t[1] > bmax[1] ) bmax[1] = t[1];
    if ( t[2] > bmax[2] ) bmax[2] = t[2];

    src+=pstride;
  }


  sides[0] = bmax[0];
  sides[1] = bmax[1];
  sides[2] = bmax[2];

  if ( fabsf(bmin[0]) > sides[0] ) sides[0] = fabsf(bmin[0]);
  if ( fabsf(bmin[1]) > sides[1] ) sides[1] = fabsf(bmin[1]);
  if ( fabsf(bmin[2]) > sides[2] ) sides[2] = fabsf(bmin[2]);

  sides[0]*=2.0f;
  sides[1]*=2.0f;
  sides[2]*=2.0f;

}

void computeBestFitOBB(unsigned int vcount, const std::vector<float> *points, vec3 &obb_size, vec3 &obb_max, vec3 &obb_min, vec3 &obb_center, vec3 &obb_angle, mat4 matrix)
{

	float bmin[3];
	float bmax[3];

	fm_getAABB(vcount, points, bmin, bmax);

	float center[3];

	center[0] = (bmax[0] - bmin[0])*0.5f + bmin[0];
	center[1] = (bmax[1] - bmin[1])*0.5f + bmin[1];
	center[2] = (bmax[2] - bmin[2])*0.5f + bmin[2];

	float ax = 0;
	float ay = 0;
	float az = 0;

	float sweep = 45.0f; // 180 degree sweep on all three axes.
	float steps = 8.0f; // 16 steps on each axis.

	float bestVolume = 1e9;
	float t_angle[3] = { 0.f, 0.f, 0.f };

	while (sweep >= 1)
	{

		bool found = false;

		float stepsize = sweep / steps;

		for (float x = ax - sweep; x <= ax + sweep; x += stepsize)
		{
			for (float y = ay - sweep; y <= ay + sweep; y += stepsize)
			{
				for (float z = az - sweep; z <= az + sweep; z += stepsize)
				{
					mat4 pmatrix;

					fm_eulerMatrix(x*FM_DEG_TO_RAD, y*FM_DEG_TO_RAD, z*FM_DEG_TO_RAD, pmatrix);

					pmatrix[3][0] = center[0];
					pmatrix[3][1] = center[1];
					pmatrix[3][2] = center[2];
					
					float psides[3];

					computeOBB(vcount, points, psides, pmatrix);

					float volume = psides[0] * psides[1] * psides[2]; // the volume of the cube

					if (volume <= bestVolume)
					{
						//printf("ya\n");
						bestVolume = volume;

						obb_size[0] = psides[0];
						obb_size[1] = psides[1];
						obb_size[2] = psides[2];

						t_angle[0] = x;
						t_angle[1] = y;
						t_angle[2] = z;

						/*printf("a: %f %f %f\n", ax, ay, az);
						printf("%f %f %f\n", angle[0], angle[1], angle[2]);*/

						matrix = pmatrix;
						found = true; // yes, we found an improvement.
					}
				}
			}
		}

		if (found)
		{

			ax = t_angle[0];
			ay = t_angle[1];
			az = t_angle[2];

			sweep *= 0.5f; // sweep 1/2 the distance as the last time.
		}
		else
		{
			break; // no improvement, so just
		}

	}

	obb_max[0] = bmax[0];
	obb_max[1] = bmax[1];
	obb_max[2] = bmax[2];

	obb_min[0] = bmin[0];
	obb_min[1] = bmin[1];
	obb_min[2] = bmin[2];

	obb_angle[0] = ax;
	obb_angle[1] = ay;
	obb_angle[2] = az;

	obb_center[0] = center[0];
	obb_center[1] = center[1];
	obb_center[2] = center[2];

	/*printf("%f %f %f\n", bmax[0], bmax[1], bmax[2]);
	printf("%f %f %f\n", bmin[0], bmin[1], bmin[2]);
	printf("%f %f %f\n", center[0], center[1], center[2]);
	printf("%f %f %f\n", obb_size[0], obb_size[1], obb_size[2]);
	printf("%f %f %f\n", ax, ay, az);*/
}

void computeBestFitOBB(unsigned int vcount,const float *points,unsigned int pstride,float *sides,float *matrix)
{

  float bmin[3];
  float bmax[3];

  fm_getAABB(vcount,points,pstride,bmin,bmax);

  float center[3];

  center[0] = (bmax[0]-bmin[0])*0.5f + bmin[0];
  center[1] = (bmax[1]-bmin[1])*0.5f + bmin[1];
  center[2] = (bmax[2]-bmin[2])*0.5f + bmin[2];

  float ax = 0;
  float ay = 0;
  float az = 0;

  float sweep =  45.0f; // 180 degree sweep on all three axes.
  float steps =   8.0f; // 16 steps on each axis.

  float bestVolume = 1e9;
  float angle[3]={0.f,0.f,0.f};

  while ( sweep >= 1 )
  {

    bool found = false;

    float stepsize = sweep / steps;

    for (float x=ax-sweep; x<=ax+sweep; x+=stepsize)
    {
      for (float y=ay-sweep; y<=ay+sweep; y+=stepsize)
      {
        for (float z=az-sweep; z<=az+sweep; z+=stepsize)
        {
          float pmatrix[16];

          fm_eulerMatrix( x*FM_DEG_TO_RAD, y*FM_DEG_TO_RAD, z*FM_DEG_TO_RAD, pmatrix );

          pmatrix[3*4+0] = center[0];
          pmatrix[3*4+1] = center[1];
          pmatrix[3*4+2] = center[2];

          float psides[3];

          computeOBB( vcount, points, pstride, psides, pmatrix );

          float volume = psides[0]*psides[1]*psides[2]; // the volume of the cube

          if ( volume <= bestVolume )
          {
            bestVolume = volume;

            sides[0] = psides[0];
            sides[1] = psides[1];
            sides[2] = psides[2];

            angle[0] = x;
            angle[1] = y;
            angle[2] = z;

            memcpy(matrix,pmatrix,sizeof(float)*16);
            found = true; // yes, we found an improvement.
          }
        }
      }
    }

    if ( found )
    {

      ax = angle[0];
      ay = angle[1];
      az = angle[2];

      sweep*=0.5f; // sweep 1/2 the distance as the last time.
    }
    else
    {
      break; // no improvement, so just
    }

  }

}
