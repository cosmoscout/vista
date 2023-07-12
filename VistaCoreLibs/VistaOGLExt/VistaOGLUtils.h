/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2016 RWTH Aachen University               */
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

#ifndef _VISTAOGLUTILS_H
#define _VISTAOGLUTILS_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/*  INCLUDES                                                                  */
/*============================================================================*/
#include <GL/glew.h>

#include "VistaOGLExtConfig.h"

#include <string>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaTexture;

/*============================================================================*/
/* PROTOTYPES                                                                 */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaOGLUtils some helper functions for graphics and (even remotely)
 * OpenGL related functionality.
 */
namespace VistaOGLUtils {
/**
 * Compute the dimensions for a texture containing the given number
 * of data elements. The return value contains the total number of
 * elements in a texture of the given size.
 */
VISTAOGLEXTAPI int ComputeTextureDimensions(int iCount, int& iWidth, int& iHeight);

/**
 * Load an image from file. The filename extension is used to
 * determine, which format the file should be in. For now, only TGA
 * files are supported.
 * NOTE: Use Delete() to free the memory allocated by this method!
 */
VISTAOGLEXTAPI unsigned char* LoadImageFromFile(
    const std::string& strFilename, int& iWidth, int& iHeight, int& iChannels);

/**
 * Load a TGA image from file.
 * NOTE: Use Delete() to free the memory allocated by this method!
 */
VISTAOGLEXTAPI unsigned char* LoadImageFromTga(
    const std::string& strFilename, int& iWidth, int& iHeight, int& iChannels);

VISTAOGLEXTAPI VistaTexture* LoadTextureFromTga(
    const std::string& strFilename, bool bGenerateMipMaps = true);

/**
 * Create a floating point array to be used as texture data for a
 * fake illumination texture of a sphere. The resulting data is of
 * format GL_LUMINANCE_ALPHA.
 * NOTE: Use Delete() to free the memory allocated by this method!
 */
VISTAOGLEXTAPI float* CreateSphereIlluminationTextureData(int iWidth, int iHeight);

/**
 * Create a floating point array to be used as texture data for a normal
 * map of a sphere. The resulting data is of format GL_RGBA.
 * Setting bRemap to true, remaps the texture values from [-1, 1] to [0, 1],
 * which is needed for using this data with non-floating point textures.
 * NOTE: Use Delete() to free the memory allocated by this method!
 */
VISTAOGLEXTAPI float* CreateSphereNormalsTextureData(int iWidth, int iHeight, bool bRemap);

/**
 * Create a floating point array to be used as texture data for a normal
 * map of a tube. The alpha channel still contains a circular mask.
 * The resulting data is of format GL_RGBA.
 * Setting bRemap to true, remaps the texture values from [-1, 1] to [0, 1],
 * which is needed for using this data with non-floating point textures.
 * NOTE: Use Delete() to free the memory allocated by this method!
 */
VISTAOGLEXTAPI float* CreateTubeNormalsTextureData(int iWidth, int iHeight, bool bRemap);

/**
 * Create a floating point array to be used as texture data for a normal
 * map of a cone. The resulting data is of format GL_RGBA.
 * Setting bRemap to true, remaps the texture values from [-1, 1] to [0, 1],
 * which is needed for using this data with non-floating point textures.
 * NOTE: Use Delete() to free the memory allocated by this method!
 */
VISTAOGLEXTAPI float* CreateConeNormalsTextureData(int iWidth, int iHeight, bool bRemap);

VISTAOGLEXTAPI float* CreateConeDeapthTextureData(int iWidth, int iHeight);

/**
 * Create a floating point array to be used as texture data for a
 * blending function based on the gaussian normal distribution, rescaled
 * to reach 1.0 at the center of the texture.
 * The resulting data is of format GL_ALPHA.
 * NOTE: Use Delete() to free the memory allocated by this method!
 */
VISTAOGLEXTAPI float* CreateGaussianBlendingTextureData(int iWidth, int iHeight);

/**
 * Free the given memory. This method is to be used in conjunction with
 * other methods of this class, which allocate memory.
 */
VISTAOGLEXTAPI void Delete(unsigned char* pData);
VISTAOGLEXTAPI void Delete(float* pData);

VISTAOGLEXTAPI void BeginOrtho(
    double dLeft, double dRight, double dBottom, double dTop, double dNear, double dFar);
VISTAOGLEXTAPI void BeginOrtho2D(double dLeft, double dRight, double dBottom, double dTop);
VISTAOGLEXTAPI void EndOrtho();

/**
 * Checks whether an OpenGL error occured and prints its error code and a
 * human-readable string to the ViSTA error stream.
 * NOTE: Calling this function consumes the last OpenGL error, so that
 *		 subsequent calls to glGetError() will yield a GL_NO_ERROR.
 *
 * @param strFilename Optional parameter to indicate the file in which
 *		  the check occured (empty string --> ignore filename).
 * @param nFileNumber Optional paramater to indicate the line number in
 *		  which the check occured (negative value --> ignore line #).
 * @return Returns true if an error occured, false if none occured.
 */
VISTAOGLEXTAPI bool CheckForOGLError(const std::string& strFilename = "", int nLineNumber = -1);
}; // namespace VistaOGLUtils

/*============================================================================*/
/* INLINED METHODS                                                            */
/*============================================================================*/

#ifndef _NDEBUG
// check error and display where it came from...that's why we need it to be a macro :-(
#define VistaOGLUtilsAssertGLOK()                                                                  \
  {                                                                                                \
    const int   line     = __LINE__;                                                               \
    const char* filename = __FILE__;                                                               \
    assert(VistaOGLUtils::CheckGLOK(true, line, filename));                                        \
  }
#else
// do nothing in release mode
#define VistaOGLUtilsAssertGLOK()
#endif

#endif // _VISTAOGLUTILS_H

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
