/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/


#include <GL/glew.h>

#include "VistaOpenGLDebug.h"

#include <VistaBase/VistaStreamUtils.h>

#if defined(WIN32)
	#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif 

#if defined(SUNOS)
#include <sunmath.h>
#endif

#include <cmath>

/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

VistaOpenGLDebug::VistaOpenGLDebug()
{
	m_pOutputStream = &(vstr::out());
}

VistaOpenGLDebug::~VistaOpenGLDebug()
{

}

// ============================================================================
// ============================================================================
bool VistaOpenGLDebug::DebugMsg (bool bVal)
{
	if (bVal)
		*(m_pOutputStream) << "TRUE";
	else
		*(m_pOutputStream) << "FALSE";
	return true;
}

bool VistaOpenGLDebug::DebugMsg (int nVal)
{
	*(m_pOutputStream) << nVal;
	return true;
}

bool VistaOpenGLDebug::DebugMsg (double nVal)
{
	*(m_pOutputStream) << nVal;
	return true;
}

bool VistaOpenGLDebug::DebugMsg (const char * charVal)
{
	std::string  strVal = charVal;
	return DebugMsg (strVal);
}

bool VistaOpenGLDebug::DebugMsg (std::string strVal)
{
	*(m_pOutputStream) << strVal.c_str();
	return true;
}

// ============================================================================
// ============================================================================
void VistaOpenGLDebug::DebugOglError ()
{
	GLenum curErr = glGetError();
	if (curErr != GL_NO_ERROR)
	{
		DebugMsg ("!! ==> OPENGL ERROR OCCURED: ");
		while (curErr != GL_NO_ERROR)
		{
			switch (curErr)
			{
				case GL_INVALID_ENUM:
					DebugMsg ("GL_INVALID_ENUM ");
				break;
				case GL_INVALID_VALUE:
					DebugMsg ("GL_INVALID_VALUE ");
				break;
				case GL_INVALID_OPERATION:
					DebugMsg ("GL_INVALID_OPERATION ");
				break;
				case GL_STACK_OVERFLOW:
					DebugMsg ("GL_STACK_OVERFLOW ");
				break;
				case GL_STACK_UNDERFLOW:
					DebugMsg ("GL_STACK_UNDERFLOW ");
				break;
				case GL_OUT_OF_MEMORY:
					DebugMsg ("GL_OUT_OF_MEMORY ");
				break;
				default:
					DebugMsg ("UNKNOWN ");
				break;
			}
			curErr = glGetError();
		}
		DebugMsg ("<== !!");
		DebugMsg ("\n");
	}
}

// ============================================================================
// ============================================================================
void VistaOpenGLDebug::DebugProjection ()
{
	DebugMsg ("========\nPROJECTION\n========\n");

	int    viewport[4];
	glGetIntegerv (GL_VIEWPORT, viewport);
	DebugMsg (" - Viewport:\n");
	DebugMsg (" -- Win Origin: ");
	DebugMsg (viewport[0]);
	DebugMsg (", ");
	DebugMsg (viewport[1]);
	DebugMsg ("\n -- Win Width/Height: ");
	DebugMsg (viewport[2]);
	DebugMsg (", ");
	DebugMsg (viewport[3]);
	DebugMsg ("\n");

	DebugOglError ();

	// retrieve projection matrix
	DebugProjectionMatrix();

	// retrieve the model matrix
	DebugModelMatrix();

	DebugMsg (" -- Clipping Planes\n");
	int maxUserClips = 0;
	glGetIntegerv (GL_MAX_CLIP_PLANES, &maxUserClips);
	int clipNum = 0;
	for (clipNum = 0; clipNum < maxUserClips; clipNum++)
	{
		int oglClipNum = 0;

		switch (clipNum)
		{
			case 0:
				oglClipNum = GL_CLIP_PLANE0;
			break;
			case 1:
				oglClipNum = GL_CLIP_PLANE1;
			break;
			case 2:
				oglClipNum = GL_CLIP_PLANE2;
			break;
			case 3:
				oglClipNum = GL_CLIP_PLANE3;
			break;
			case 4:
				oglClipNum = GL_CLIP_PLANE4;
			break;
			case 5:
				oglClipNum = GL_CLIP_PLANE5;
			break;
			default:
				oglClipNum = GL_CLIP_PLANE0;
			break;
		}

		bool bPlaneOn = glIsEnabled (oglClipNum) ? true : false;
		DebugMsg (" --- Clipping Plane ");
		DebugMsg (clipNum);
		DebugMsg (" ON: ");
		DebugMsg (bPlaneOn);
		DebugMsg ("\n");
		if (bPlaneOn)
		{
			GLdouble equation[4];
			glGetClipPlane (oglClipNum, equation);
			DebugMsg (" ---- Equation: ");
			DebugMsg (equation[0]);
			DebugMsg (", ");
			DebugMsg (equation[1]);
			DebugMsg (", ");
			DebugMsg (equation[2]);
			DebugMsg (", ");
			DebugMsg (equation[3]);
			DebugMsg ("\n");
		}
	}

	DebugOglError ();
}

// ============================================================================
// ============================================================================
void VistaOpenGLDebug::DebugProjectionMatrix ()
{
	GLfloat  curProjMatrix[16];
	glGetFloatv   (GL_PROJECTION_MATRIX, curProjMatrix);
	DebugMsg (" -- Projection Matrix: \n");
	DebugMsg (" ---  [ ");
	DebugMsg (curProjMatrix[0]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[4]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[8]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[12]);
	DebugMsg (" ]\n");
	DebugMsg (" ---  [ ");
	DebugMsg (curProjMatrix[1]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[5]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[9]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[13]);
	DebugMsg (" ]\n");
	DebugMsg (" ---  [ ");
	DebugMsg (curProjMatrix[2]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[6]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[10]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[14]);
	DebugMsg (" ]\n");
	DebugMsg (" ---  [ ");
	DebugMsg (curProjMatrix[3]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[7]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[11]);
	DebugMsg (", ");
	DebugMsg (curProjMatrix[15]);
	DebugMsg (" ]\n");
	// if the perspective is on
	if (curProjMatrix[11] != 0.0f)
	{
		// check other entries on zero
		if ( (curProjMatrix[1] != 0.0f) ||
			 (curProjMatrix[2] != 0.0f) ||
			 (curProjMatrix[3] != 0.0f) ||
			 (curProjMatrix[4] != 0.0f) ||
			 (curProjMatrix[6] != 0.0f) ||
			 (curProjMatrix[7] != 0.0f) ||
			 (curProjMatrix[12] != 0.0f) ||
			 (curProjMatrix[13] != 0.0f) ||
			 (curProjMatrix[15] != 0.0f) )
		{
			DebugMsg (" ---- NO STANDARD PERSPECTIVE PROJECTION !!\n");
		}
		else
		{
			bool bError = false;
			DebugMsg (" ---- STANDARD PERSPECTIVE FRUSTUM PROJECTION !!\n");
			if ((curProjMatrix[10] == 1.0f) || (curProjMatrix[10] == -1.0f))
			{
				DebugMsg (" ---- ERROR IN CALCULATE THE NEAR/FAR DISTANCES !!");
				bError = true;
			}
			if (curProjMatrix[0] == 0.0f)
			{
				DebugMsg (" ---- ERROR IN CALCULATE THE LEFT/RIGHT !!");
				bError = true;
			}
			if (curProjMatrix[5] == 0.0f)
			{
				DebugMsg (" ---- ERROR IN CALCULATE THE BOTTOM/TOP !!");
				bError = true;
			}
			if (bError == false)
			{
				double nFar    = curProjMatrix[14] / (curProjMatrix[10] + 1.0f);
				double nNear   = curProjMatrix[14] / (curProjMatrix[10] - 1.0f);
				double nRight  = nNear * (curProjMatrix[8] + 1.0f) / curProjMatrix[0];
				double nLeft   = nNear * (curProjMatrix[8] - 1.0f) / curProjMatrix[0];
				double nTop    = nNear * (curProjMatrix[9] + 1.0f) / curProjMatrix[5];
				double nBottom = nNear * (curProjMatrix[9] - 1.0f) / curProjMatrix[5];
				DebugMsg (" ---- Clipping Plane Distances (Near/Far): ");
				DebugMsg (nNear);
				DebugMsg (", ");
				DebugMsg (nFar);
				DebugMsg ("\n");
				DebugMsg (" ---- Near Clipping Plane Window:\n");
				DebugMsg (" ----- Left-Bottom-Corner: [");
				DebugMsg (nLeft);
				DebugMsg (", ");
				DebugMsg (nBottom);
				DebugMsg (", ");
				DebugMsg (-nNear);
				DebugMsg ("]\n ----- Right-Up-Corner: [");
				DebugMsg (nRight);
				DebugMsg (", ");
				DebugMsg (nTop);
				DebugMsg (", ");
				DebugMsg (-nNear);
				DebugMsg ("]\n");
			}
		}
	}
	// it is a orthogonal projection
	else
	{
		// check other entries on zero
		if ( (curProjMatrix[1] != 0.0f) ||
			 (curProjMatrix[2] != 0.0f) ||
			 (curProjMatrix[3] != 0.0f) ||
			 (curProjMatrix[4] != 0.0f) ||
			 (curProjMatrix[6] != 0.0f) ||
			 (curProjMatrix[7] != 0.0f) ||
			 (curProjMatrix[8] != 0.0f) ||
			 (curProjMatrix[9] != 0.0f) ||
			 (curProjMatrix[11] != 0.0f) )
		{
			DebugMsg (" ---- NO STANDARD ORTHOGONAL PROJECTION !!\n");
		}
		else
		{
			DebugMsg (" ---- STANDARD ORTHOGONAL PROJECTION !!\n");
		}
	}

	DebugOglError ();
}

// ============================================================================
// ============================================================================
void VistaOpenGLDebug::DebugModelMatrix ()
{
	double nPI  = 3.1415926535897932384626433832795;
	double nEps = 0.00001;//0.000000001;

	GLfloat  curModelMatrix[16];
	glGetFloatv   (GL_MODELVIEW_MATRIX, curModelMatrix);
	DebugMsg (" -- Model Matrix: \n");
	DebugMsg (" ---  [ ");
	DebugMsg (curModelMatrix[0]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[4]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[8]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[12]);
	DebugMsg (" ]\n");
	DebugMsg (" ---  [ ");
	DebugMsg (curModelMatrix[1]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[5]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[9]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[13]);
	DebugMsg (" ]\n");
	DebugMsg (" ---  [ ");
	DebugMsg (curModelMatrix[2]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[6]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[10]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[14]);
	DebugMsg (" ]\n");
	DebugMsg (" ---  [ ");
	DebugMsg (curModelMatrix[3]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[7]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[11]);
	DebugMsg (", ");
	DebugMsg (curModelMatrix[15]);
	DebugMsg (" ]\n");
	{
		double matRDet =
				curModelMatrix[0]*curModelMatrix[5]*curModelMatrix[10] -
				curModelMatrix[8]*curModelMatrix[5]*curModelMatrix[2]  +
				curModelMatrix[4]*curModelMatrix[9]*curModelMatrix[2]  -
				curModelMatrix[0]*curModelMatrix[9]*curModelMatrix[6]  +
				curModelMatrix[8]*curModelMatrix[1]*curModelMatrix[6]  -
				curModelMatrix[4]*curModelMatrix[1]*curModelMatrix[10];
		DebugMsg (" -----  Rotation Submatrix Determinante: ");
		DebugMsg (matRDet);
		DebugMsg ("\n");
	}
	{
		double scalprodR12 =
			curModelMatrix[0]*curModelMatrix[4] +
			curModelMatrix[1]*curModelMatrix[5] +
			curModelMatrix[2]*curModelMatrix[6];
		double scalprodR13 =
			curModelMatrix[0]*curModelMatrix[8] +
			curModelMatrix[1]*curModelMatrix[9] +
			curModelMatrix[2]*curModelMatrix[10];
		double scalprodR23 =
			curModelMatrix[4]*curModelMatrix[8] +
			curModelMatrix[5]*curModelMatrix[9] +
			curModelMatrix[6]*curModelMatrix[10];
		double lenR1 = sqrt (
			curModelMatrix[0]*curModelMatrix[0] +
			curModelMatrix[1]*curModelMatrix[1] +
			curModelMatrix[2]*curModelMatrix[2]  );
		double lenR2 = sqrt (
			curModelMatrix[4]*curModelMatrix[4] +
			curModelMatrix[5]*curModelMatrix[5] +
			curModelMatrix[6]*curModelMatrix[6]  );
		double lenR3 = sqrt (
			curModelMatrix[8]*curModelMatrix[8] +
			curModelMatrix[9]*curModelMatrix[9] +
			curModelMatrix[10]*curModelMatrix[10]  );
		double angR12 = std::acos (scalprodR12 / (lenR1 * lenR2));
		double angR13 = std::acos (scalprodR13 / (lenR1 * lenR3));
		double angR23 = std::acos (scalprodR23 / (lenR2 * lenR3));
		DebugMsg (" -----  Rotation Submatrix Orthogonality: ");
		DebugMsg (angR12 * 180.0 / nPI);
		DebugMsg (", ");
		DebugMsg (angR13 * 180.0 / nPI);
		DebugMsg (", ");
		DebugMsg (angR23 * 180.0 / nPI);
		DebugMsg ("\n");
	}
	// if it seems to be a standard model matrix
	if ( (curModelMatrix[3]  == 0.0f) ||
		 (curModelMatrix[7]  == 0.0f) ||
		 (curModelMatrix[11] == 0.0f) ||
		 (curModelMatrix[15] == 1.0f) )
	{
		DebugMsg (" ---- COULD BE A STANDARD EYE TRANSFORMATION MATRIX (MM=RMx*RMy*RMz*TM) !!\n");
		DebugMsg (" ---- Viewpoint: (");
		DebugMsg (-curModelMatrix[12]);
		DebugMsg (", ");
		DebugMsg (-curModelMatrix[13]);
		DebugMsg (", ");
		DebugMsg (-curModelMatrix[14]);
		DebugMsg (")\n");
		// this calc returns a value between 90 and -90 degrees	
		double rotY = std::asin (curModelMatrix[8]);
		double rotX = 0.0;
		double rotZ = 0.0;	
		// attention: std::cos(+/-90)=0.0!!
		if ((rotY != 1.0) && (rotY != -1.0))
		{
			rotX = std::asin (-curModelMatrix[9]/std::cos(rotY));
			rotZ = std::asin (-curModelMatrix[4]/std::cos(rotY));

			// test x rot result
			double m9  = -std::sin(rotX)*std::cos(rotY);
			double m10 =  std::cos(rotX)*std::cos(rotY);
			// test z rot result
			double m0 =  std::cos(rotY)*std::cos(rotZ);
			double m4 = -std::cos(rotY)*std::sin(rotZ);

			// CHECK Y ROT
			// if mat[0] and mat[10] are wrong
			if (  ( (m0  > (curModelMatrix[0]  + nEps) ) ||
					(m0  < (curModelMatrix[0]  - nEps) )  )
				  &&
				  ( (m10 > (curModelMatrix[10] + nEps) ) ||
					(m10 < (curModelMatrix[10] - nEps) ) )
			   )
			{
				// sine switch for y rot
				rotY = nPI - rotY;
				// recalc other rotations
				rotX = std::asin (-curModelMatrix[9]/std::cos(rotY));
				rotZ = std::asin (-curModelMatrix[4]/std::cos(rotY));
				// test x rot result
				m9  = -std::sin(rotX)*std::cos(rotY);
				m10 =  std::cos(rotX)*std::cos(rotY);
				// test z rot result
				m0 =  std::cos(rotY)*std::cos(rotZ);
				m4 = -std::cos(rotY)*std::sin(rotZ);
				// I hope rot Y is ok, now...
			}
				
			// CHECK X ROT
			if ( (m10  > (curModelMatrix[10]  + nEps) ) ||
				 (m10  < (curModelMatrix[10]  - nEps) ) )
			{
				// sine switch for x rot
				rotX = nPI - rotX;
				// recalc x rot specific terms
				m9  = -std::sin(rotX)*std::cos(rotY);
				m10 =  std::cos(rotX)*std::cos(rotY);
			}
			if ( (m9  > (curModelMatrix[9]  + nEps) ) ||
				 (m9  < (curModelMatrix[9]  - nEps) ) )
			{
				// cosine switch for x rot
				rotX = - rotX;
			}

			// CHECK Z ROT
			if ( (m0  > (curModelMatrix[0]  + nEps) ) ||
				 (m0  < (curModelMatrix[0]  - nEps) ) )
			{
				// sine switch for z rot
				rotZ = nPI - rotZ;
				// recalc x rot specific terms
				m0 =  std::cos(rotY)*std::cos(rotZ);
				m4 = -std::cos(rotY)*std::sin(rotZ);
			}
			if ( (m4  > (curModelMatrix[4]  + nEps) ) ||
				 (m4  < (curModelMatrix[4]  - nEps) ) )
			{
				// cosine switch for z rot
				rotZ = - rotZ;
			}

			// it is nice to check again
			// test x rot result
			m9  = -std::sin(rotX)*std::cos(rotY);
			m10 =  std::cos(rotX)*std::cos(rotY);
			// test z rot result
			m0 =  std::cos(rotY)*std::cos(rotZ);
			m4 = -std::cos(rotY)*std::sin(rotZ);
		}
		else
		{
			rotX = - std::asin (curModelMatrix[9]);
			rotZ = - std::asin (curModelMatrix[4]);
			// now check here, too
			// ...
			DebugMsg ("TO CHECK: ONLY ROT X AND ROT Z\n");
		}
		// check the epsilon threshold
		if (std::abs(rotX) < nEps)
			rotX = 0.0;
		if (std::abs(rotY) < nEps)
			rotY = 0.0;
		if (std::abs(rotZ) < nEps)
			rotZ = 0.0;
		// output
		DebugMsg (" ---- Rotation: (");
		DebugMsg (rotX * 180 / nPI);
		DebugMsg (", ");
		DebugMsg (rotY * 180 / nPI);
		DebugMsg (", ");
		DebugMsg (rotZ * 180 / nPI);
		DebugMsg (")\n");
		{
			// check the calculated angles
			float mat[16];
			mat[0]  = (float)( std::cos(rotY)*std::cos(rotZ) );
			mat[1]  = (float)( std::sin(rotX)*std::sin(rotY)*std::cos(rotZ) + std::cos(rotX)*std::sin(rotZ) );
			mat[2]  = (float)( -std::cos(rotX)*std::sin(rotY)*std::cos(rotZ) + std::sin(rotX)*std::sin(rotZ) );
			mat[3]  = 0.0f;
			mat[4]  = (float)( -std::cos(rotY)*std::sin(rotZ) );
			mat[5]  = (float)( -std::sin(rotX)*std::sin(rotY)*std::sin(rotZ) + std::cos(rotX)*std::cos(rotZ) );
			mat[6]  = (float)( std::cos(rotX)*std::sin(rotY)*std::sin(rotZ) + std::sin(rotX)*std::cos(rotZ) );
			mat[7]  = 0.0f;
			mat[8]  = (float)( std::sin(rotY) );
			mat[9]  = (float)( -std::sin(rotX)*std::cos(rotY) );
			mat[10] = (float)( std::cos(rotX)*std::cos(rotY) );
			mat[11]  = 0.0f;
			mat[12]  = curModelMatrix[12];
			mat[13]  = curModelMatrix[13];
			mat[14]  = curModelMatrix[14];
			mat[15]  = 1.0f;
			// check the epsilon threshold
			if (std::abs(mat[0]) < nEps)
				mat[0] = 0.0f;
			if (std::abs(mat[1]) < nEps)
				mat[1] = 0.0f;
			if (std::abs(mat[2]) < nEps)
				mat[2] = 0.0f;
			if (std::abs(mat[4]) < nEps)
				mat[4] = 0.0f;
			if (std::abs(mat[5]) < nEps)
				mat[5] = 0.0f;
			if (std::abs(mat[6]) < nEps)
				mat[6] = 0.0f;
			if (std::abs(mat[8]) < nEps)
				mat[8] = 0.0f;
			if (std::abs(mat[9]) < nEps)
				mat[9] = 0.0f;
			if (std::abs(mat[10]) < nEps)
				mat[10] = 0.0f;
			double matRDet =
				mat[0]*mat[5]*mat[10] - mat[8]*mat[5]*mat[2] +
				mat[4]*mat[9]*mat[2]  - mat[0]*mat[9]*mat[6] +
				mat[8]*mat[1]*mat[6]  - mat[4]*mat[1]*mat[10];

			// output
			DebugMsg (" ---- Check Recalculated Rotation Submatrix: \n");
			DebugMsg (" -----  [ ");
			DebugMsg (mat[0]);
			DebugMsg (", ");
			DebugMsg (mat[4]);
			DebugMsg (", ");
			DebugMsg (mat[8]);
			DebugMsg (" ]\n");
			DebugMsg (" -----  [ ");
			DebugMsg (mat[1]);
			DebugMsg (", ");
			DebugMsg (mat[5]);
			DebugMsg (", ");
			DebugMsg (mat[9]);
			DebugMsg (" ]\n");
			DebugMsg (" -----  [ ");
			DebugMsg (mat[2]);
			DebugMsg (", ");
			DebugMsg (mat[6]);
			DebugMsg (", ");
			DebugMsg (mat[10]);
			DebugMsg (" ]\n");
			DebugMsg (" -----  Rotation Submatrix Determinante: ");
			DebugMsg (matRDet);
			DebugMsg ("\n");
			{
				double scalprodR12 =
					mat[0]*mat[4] + mat[1]*mat[5] + mat[2]*mat[6];
				double scalprodR13 =
					mat[0]*mat[8] + mat[1]*mat[9] + mat[2]*mat[10];
				double scalprodR23 =
					mat[4]*mat[8] + mat[5]*mat[9] + mat[6]*mat[10];
				double lenR1 = sqrt ( mat[0]*mat[0] + mat[1]*mat[1] + mat[2]*mat[2]  );
				double lenR2 = sqrt ( mat[4]*mat[4] + mat[5]*mat[5] + mat[6]*mat[6]  );
				double lenR3 = sqrt ( mat[8]*mat[8] + mat[9]*mat[9] + mat[10]*mat[10] );
				double angR12 = std::acos (scalprodR12 / (lenR1 * lenR2));
				double angR13 = std::acos (scalprodR13 / (lenR1 * lenR3));
				double angR23 = std::acos (scalprodR23 / (lenR2 * lenR3));
				DebugMsg (" -----  Rotation Submatrix Orthogonality: ");
				DebugMsg (angR12 * 180.0 / nPI);
				DebugMsg (", ");
				DebugMsg (angR13 * 180.0 / nPI);
				DebugMsg (", ");
				DebugMsg (angR23 * 180.0 / nPI);
				DebugMsg ("\n");
			}
			/////#####################/////
			//glLoadMatrixf(mat);
		}
	}

	DebugOglError ();
}

// ============================================================================
// ============================================================================
void VistaOpenGLDebug::DebugBlending ()
{
/*	float		queryFloat;
	float		fogColor[4];
*/

	DebugMsg ("========\nBLENDING\n========\n");

	bool bFogOn = glIsEnabled(GL_FOG) ? true : false;
	DebugMsg (" - FogEnabled:");
	DebugMsg (bFogOn);
	DebugMsg ("\n");
	
/*	glGetFloatv (GL_FOG_DENSITY, &(queryFloat));
	double nFogDensity = (double)queryFloat;

	glGetFloatv (GL_FOG_START, &(queryFloat));
	double nFogStart = (double)queryFloat;

	glGetFloatv (GL_FOG_END, &(queryFloat));
	double nFogEnd = (double)queryFloat;

	glGetFloatv (GL_FOG_COLOR, fogColor);
	double nFogRed   = fogColor[0];
	double nFogGreen = fogColor[1];
	double nFogBlue  = fogColor[2];
	double nFogAlpha = fogColor[3];

	bool bBlendOn       = (bool) glIsEnabled (GL_BLEND);
	bool bDepthTest     = (bool) glIsEnabled (GL_DEPTH_TEST);
	bool bSmoothPoint   = (bool) glIsEnabled (GL_POINT_SMOOTH);
	bool bSmoothLine    = (bool) glIsEnabled (GL_LINE_SMOOTH);
	bool bSmoothPolygon = (bool) glIsEnabled (GL_POLYGON_SMOOTH);
*/
	DebugOglError ();
}

// ============================================================================
// ============================================================================
void VistaOpenGLDebug::DebugLighting ()
{
	int         queryInt;

	DebugMsg ("========\nLIGHTING\n========\n");

	bool bLightingOn = glIsEnabled (GL_LIGHTING) ? true : false;
	DebugMsg (" - LightingOn:");
	DebugMsg ((bool)bLightingOn);
	DebugMsg ("\n");

	if (bLightingOn)
	{
		// the light model
		glGetIntegerv (GL_LIGHT_MODEL_LOCAL_VIEWER, &queryInt);
		int localViewer = queryInt;
		DebugMsg (" - LocalViewer: ");
		DebugMsg (localViewer ? true : false);
		DebugMsg ("\n");
		glGetIntegerv (GL_LIGHT_MODEL_TWO_SIDE, &queryInt);
		int twoSide = queryInt;
		DebugMsg (" - TwoSide: ");
		DebugMsg (twoSide ? true : false);
		DebugMsg ("\n");
		float		lmodel_ambient[4];
		glGetFloatv (GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
		double nGlobalRed   = (double)lmodel_ambient[0];
		double nGlobalGreen = (double)lmodel_ambient[1];
		double nGlobalBlue  = (double)lmodel_ambient[2];
		double nGlobalAlpha = (double)lmodel_ambient[3];
		DebugMsg (" - AmbientLight (RGB): ");
		DebugMsg (nGlobalRed);
		DebugMsg (", ");
		DebugMsg (nGlobalGreen);
		DebugMsg (", ");
		DebugMsg (nGlobalBlue);
		DebugMsg (", ");
		DebugMsg (nGlobalAlpha);
		DebugMsg ("\n");

		for (int lightNum = 0; lightNum < 8; lightNum++)
		{
			DebugLight (lightNum);
		}
	}

	DebugOglError ();
}

// ============================================================================
// ============================================================================
void VistaOpenGLDebug::DebugLight (int lightNum)
{
	float		queryFloat;

	DebugMsg (" -- Light ");
	DebugMsg (lightNum);
	DebugMsg ("\n");

	int oglLightNum = 0;

	switch (lightNum)
	{
		case 0:
			oglLightNum = GL_LIGHT0;
		break;
		case 1:
			oglLightNum = GL_LIGHT1;
		break;
		case 2:
			oglLightNum = GL_LIGHT2;
		break;
		case 3:
			oglLightNum = GL_LIGHT3;
		break;
		case 4:
			oglLightNum = GL_LIGHT4;
		break;
		case 5:
			oglLightNum = GL_LIGHT5;
		break;
		case 6:
			oglLightNum = GL_LIGHT6;
		break;
		case 7:
			oglLightNum = GL_LIGHT7;
		break;
		default:
			oglLightNum = GL_LIGHT0;
		break;
	}

	bool bLightOn    = glIsEnabled (oglLightNum) ? true : false;
	DebugMsg (" --- LightEnabled:");
	DebugMsg (bLightOn);
	DebugMsg ("\n");

	if (bLightOn)
	{
		// light position
		float		light_pos[4];
		glGetLightfv (oglLightNum, GL_POSITION, light_pos);
		double nLightPosX = /*m_LightPos[0];*/(double)light_pos[0];
		double nLightPosY = /*m_LightPos[1];*/(double)light_pos[1];
		double nLightPosZ = /*m_LightPos[2];*/(double)light_pos[2];
		// if the light is a directional one
		double nLightPosW = (double)light_pos[3];
		//if (m_LightPos[3] == 0.0) dlgLight.m_nLightPosW = FALSE;
		// else it is a positional one
		//else dlgLight.m_nLightPosW = TRUE;
		DebugMsg (" --- LightPosition (x,y,z,w): ");
		DebugMsg (nLightPosX);
		DebugMsg (", ");
		DebugMsg (nLightPosY);
		DebugMsg (", ");
		DebugMsg (nLightPosZ);
		DebugMsg (", ");
		DebugMsg (nLightPosW);
		DebugMsg ("\n");

		float		spot_dir[4];
		glGetLightfv (oglLightNum, GL_SPOT_DIRECTION, spot_dir);
		double nSpotDirX = /*m_LightDir[0];*/(double)spot_dir[0];
		double nSpotDirY = /*m_LightDir[1];*/(double)spot_dir[1];
		double nSpotDirZ = /*m_LightDir[2];*/(double)spot_dir[2];
		DebugMsg (" --- LightDirection: ");
		DebugMsg (nSpotDirX);
		DebugMsg (", ");
		DebugMsg (nSpotDirY);
		DebugMsg (", ");
		DebugMsg (nSpotDirZ);
		DebugMsg ("\n");

		// light source color attributes
		float		light_amb[4];
		glGetLightfv (oglLightNum, GL_AMBIENT, light_amb);
		double nColAmbRed   = (double)light_amb[0];
		double nColAmbGreen = (double)light_amb[1];
		double nColAmbBlue  = (double)light_amb[2];
		double nColAmbAlpha = (double)light_amb[3];
		DebugMsg (" --- LightAmbient: ");
		DebugMsg (nColAmbRed);
		DebugMsg (", ");
		DebugMsg (nColAmbGreen);
		DebugMsg (", ");
		DebugMsg (nColAmbBlue);
		DebugMsg (", ");
		DebugMsg (nColAmbAlpha);
		DebugMsg ("\n");
		float		light_dif[4];
		glGetLightfv (oglLightNum, GL_DIFFUSE, light_dif);
		double nColDifRed   = (double)light_dif[0];
		double nColDifGreen = (double)light_dif[1];
		double nColDifBlue  = (double)light_dif[2];
		double nColDifAlpha = (double)light_dif[3];
		DebugMsg (" --- LightDiffuse: ");
		DebugMsg (nColDifRed);
		DebugMsg (", ");
		DebugMsg (nColDifGreen);
		DebugMsg (", ");
		DebugMsg (nColDifBlue);
		DebugMsg (", ");
		DebugMsg (nColDifAlpha);
		DebugMsg ("\n");
		float		light_spec[4];
		glGetLightfv (oglLightNum, GL_SPECULAR, light_spec);
		double nColSpecRed   = (double)light_spec[0];
		double nColSpecGreen = (double)light_spec[1];
		double nColSpecBlue  = (double)light_spec[2];
		double nColSpecAlpha = (double)light_spec[3];
		DebugMsg (" --- LightSpecular: ");
		DebugMsg (nColSpecRed);
		DebugMsg (", ");
		DebugMsg (nColSpecGreen);
		DebugMsg (", ");
		DebugMsg (nColSpecBlue);
		DebugMsg (", ");
		DebugMsg (nColSpecAlpha);
		DebugMsg ("\n");

		// light 0 attenuation
		glGetLightfv (oglLightNum, GL_CONSTANT_ATTENUATION, &(queryFloat));
		double nAttenConst = (double)queryFloat;
		DebugMsg (" --- Const Attenuation: ");
		DebugMsg (nAttenConst);
		DebugMsg ("\n");
		glGetLightfv (oglLightNum, GL_LINEAR_ATTENUATION, &(queryFloat));
		double nAttenLin = (double)queryFloat;
		DebugMsg (" --- Linear Attenuation: ");
		DebugMsg (nAttenLin);
		DebugMsg ("\n");
		glGetLightfv (oglLightNum, GL_QUADRATIC_ATTENUATION, &(queryFloat));
		double nAttenQuad = (double)queryFloat;
		DebugMsg (" --- Quad Attenuation: ");
		DebugMsg (nAttenQuad);
		DebugMsg ("\n");

		// light 0 spot props
		glGetLightfv (oglLightNum, GL_SPOT_CUTOFF, &(queryFloat));
		double nSpotCutOff = (double)queryFloat;
		DebugMsg (" --- Spot CutOff (0-90,180): ");
		DebugMsg (nSpotCutOff);
		DebugMsg ("\n");
		glGetLightfv (oglLightNum, GL_SPOT_EXPONENT, &(queryFloat));
		double nSpotExpo = (double)queryFloat;
		DebugMsg (" --- Spot Exponent: ");
		DebugMsg (nSpotExpo);
		DebugMsg ("\n");
	}

	DebugOglError ();
}

// ============================================================================
// ============================================================================
void VistaOpenGLDebug::DebugShading ()
{
	DebugMsg ("========\nSHADING\n========\n");

	DebugMsg (" - ShadeModel: ");
	GLint nShadeModel;
	glGetIntegerv (GL_SHADE_MODEL, &nShadeModel);
	if (nShadeModel == GL_SMOOTH)
		DebugMsg ("GL_SMOOTH");
	if (nShadeModel == GL_FLAT)
		DebugMsg ("GL_FLAT");
	DebugMsg ("\n");
	
	// some more tests
	DebugMsg (" - EdgeFlag: ");
	GLboolean bEdgeFlag;
	glGetBooleanv (GL_EDGE_FLAG, &bEdgeFlag);
	DebugMsg (bEdgeFlag ? true : false);
	DebugMsg ("\n");

	DebugOglError ();
}
