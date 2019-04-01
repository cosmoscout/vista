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



#include "VistaOGLUtils.h"
#include "VistaTexture.h"

#include <VistaBase/VistaMathBasics.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaAspects/VistaConversion.h>

#include <iostream>
#include <fstream>
#include <cassert>

#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <cmath>

using namespace std;


namespace
{
	void PrintError( GLenum eErrorCode, string strHumanReadableName,
		const string& strFilename, int nLineNumber )
	{
		vstr::errp() << "OpenGL error " << eErrorCode << " ( "
			<< strHumanReadableName << " )" << flush;

		if( !strFilename.empty() )
		{
			vstr::err() << " in file " << strFilename << flush;
		}

		if( nLineNumber > -1 )
		{
			vstr::err() << " in line " << nLineNumber << flush;
		}

		vstr::err() << " detected." << endl;
	}
}

// @todo: Mind the cast.
#define TO_UPPER(strString) \
{ \
	for (size_t _i=0; _i<strString.size(); ++_i) \
		strString[_i] = static_cast<char>(toupper(strString[_i])); \
}

/*============================================================================*/
/*  FORWARD DECLARATIONS                                                      */
/*============================================================================*/
static unsigned char *LoadImageFromTgaStream(istream &is, int &iWidth, int &iHeight, int &iChannels);

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ComputeTextureDimensions                                    */
/*                                                                            */
/*============================================================================*/
int VistaOGLUtils::ComputeTextureDimensions(int iCount, int &iWidth, int &iHeight)
{
	if (iCount < 0)
		return 0;

	iWidth = int(ceil(sqrt(float(iCount))));
	iHeight = int(floor(sqrt(float(iCount))));
	while (iWidth * iHeight < iCount)
	{
		++iHeight;
		if (iWidth * iHeight < iCount)
			++iWidth;
	}
	return iWidth * iHeight;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   LoadImageFromFile                                           */
/*                                                                            */
/*============================================================================*/
unsigned char *VistaOGLUtils::LoadImageFromFile(const std::string &strFilename,
												 int &iWidth,
												 int &iHeight,
												 int &iChannels)
{
	// try to determine the file extension
	if (strFilename.size() < 4)
	{
		vstr::errp() << " [VistaOGLUtils::LoadImage] - invalid filename..." << endl;
		return NULL;
	}

	string strExtension = strFilename.substr(strFilename.size()-4, 4);
	VistaConversion::StringToUpper(strExtension);

	if (strExtension == ".TGA")
		return LoadImageFromTga(strFilename, iWidth, iHeight, iChannels);
	else
	{
		vstr::errp() << " [VistaOGLUtils::LoadImage] - unknown file extension '"
			<< strExtension << "'" << endl;
	}

	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   LoadImageFromTga                                            */
/*                                                                            */
/*============================================================================*/
unsigned char *VistaOGLUtils::LoadImageFromTga(const std::string &strFilename,
												int &iWidth, 
												int &iHeight, 
												int &iChannels)
{
	ifstream iFile(strFilename.c_str(), ios::binary);

	if (iFile.is_open())
	{

		vstr::debugi() << " [VistaOGLUtils::LoadImageFromTga] - Successfully opened TGA file..." << endl;
		vstr::debugi() << "        file name: " << strFilename << endl;

		return LoadImageFromTgaStream(iFile, iWidth, iHeight, iChannels);
	}

	vstr::errp() << " [VistaOGLUtils::LoadImageFromTga] - Unable to open file..." << endl;
	vstr::erri() << "                                             filename: " << strFilename << endl;

	return NULL;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   LoadTextureFromTga                                            */
/*                                                                            */
/*============================================================================*/
VistaTexture* VistaOGLUtils::LoadTextureFromTga(const std::string &strFilename,
												bool bGenerateMipMaps /* = true */)
{
	int iWidth = -1, iHeight = -1, iComponents = -1;
	unsigned char* pData = LoadImageFromTga( strFilename,
		iWidth, iHeight, iComponents );

	if( pData == NULL )
		return NULL;

	assert( iComponents == 3 || iComponents == 4 ); // cannot handle anything else, yet
	VistaTexture* pTexture = new VistaTexture( GL_TEXTURE_2D );
	pTexture->UploadTexture( iWidth, iHeight, pData, bGenerateMipMaps,
		iComponents == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE );

	delete [] pData;

	return pTexture;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateSphereIlluminationTextureData                         */
/*                                                                            */
/*============================================================================*/
float *VistaOGLUtils::CreateSphereIlluminationTextureData(int iWidth, int iHeight)
{
	if (iWidth <= 0 || iHeight <= 0)
		return NULL;

	float *pResult = new float[iWidth * iHeight * 2];
	float *pPos = pResult;

	float du = 2.0f / iWidth;
	float dv = 2.0f / iHeight;

	float v = 0.5f * dv - 1.0f;

	for (int j=0; j<iHeight; ++j, v+=dv)
	{
		float u = 0.5f * du - 1.0f;
		for (int i=0; i<iWidth; ++i, u+=du)
		{
			pPos[0] = 1.0f - u*u - v*v;
			if (pPos[0] > 0)
				pPos[1] = 1.0f;
			else
				pPos[0] = pPos[1] = 0;

			pPos += 2;
		}
	}
	return pResult;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateSphereNormalsTextureData                              */
/*                                                                            */
/*============================================================================*/
float *VistaOGLUtils::CreateSphereNormalsTextureData(int iWidth, 
													  int iHeight,
													  bool bRemap)
{
	if (iWidth <= 0 || iHeight <= 0)
		return NULL;

	float *pResult = new float[iWidth * iHeight * 4];
	float *pPos = pResult;

	float du = 2.0f / iWidth;
	float dv = 2.0f / iHeight;

	float v = 0.5f * dv - 1.0f;

	for (int j=0; j<iHeight; ++j, v+=dv)
	{
		float u = 0.5f * du - 1.0f;
		for (int i=0; i<iWidth; ++i, u+=du)
		{
			float z = 1.0f - u*u - v*v;
			if (z<0)
				z=0;
			else
				z=sqrt(z);

			float fOneByLength = 1.0f / sqrt(u*u+v*v+z*z);

			pPos[0] = fOneByLength * u;
			pPos[1] = fOneByLength * v;
			pPos[2] = fOneByLength * z;
			pPos[3] = (z>0 ? 1.0f : 0.0f);

			if (bRemap)
			{
				pPos[0] = pPos[0]*0.5f + 0.5f;
				pPos[1] = pPos[1]*0.5f + 0.5f;
				pPos[2] = pPos[2]*0.5f + 0.5f;
			}

			pPos += 4;
		}
	}
	return pResult;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateTubeNormalsTextureData                                */
/*                                                                            */
/*============================================================================*/
float *VistaOGLUtils::CreateTubeNormalsTextureData(int iWidth, 
													  int iHeight,
													  bool bRemap)
{
	if (iWidth <= 0 || iHeight <= 0)
		return NULL;

	float *pResult = new float[iWidth * iHeight * 4];
	float *pPos = pResult;

	float du = 2.0f / iWidth;
	float dv = 2.0f / iHeight;

	float v = 0.5f * dv - 1.0f;

	for (int j=0; j<iHeight; ++j, v+=dv)
	{
		float u = 0.5f * du - 1.0f;
		for (int i=0; i<iWidth; ++i, u+=du)
		{
			float z = 1.0f - v*v;
			if (z<0)
				z=0;
			else
				z=sqrt(z);

			float fOneByLength = 1.0f / sqrt(v*v+z*z);

			pPos[0] = 0;
			pPos[1] = fOneByLength * v;
			pPos[2] = fOneByLength * z;
			pPos[3] = (1.0f - u*u - v*v > 0 ? 1.0f : 0.0f);

			if (bRemap)
			{
				pPos[0] = pPos[0]*0.5f + 0.5f;
				pPos[1] = pPos[1]*0.5f + 0.5f;
				pPos[2] = pPos[2]*0.5f + 0.5f;
			}

			pPos += 4;
		}
	}
	return pResult;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateConeNormalsTextureData                                */
/*                                                                            */
/*============================================================================*/
float *VistaOGLUtils::CreateConeNormalsTextureData(
	int iWidth, int iHeight,  bool bRemap )
{
	if (iWidth <= 0 || iHeight <= 0 )
		return NULL;

	float *pResult = new float[iWidth * iHeight * 4];
	float *pPos = pResult;

	float du = 2.0f / iWidth;
	float dv = 2.0f / iHeight;

	float v = 0.5f * dv - 1.0f;
	for (int j=0; j<iHeight; ++j, v+=dv)
	{
		float u = 0.5f * du - 1.0f;
		for (int i=0; i<iWidth; ++i, u+=du)
		{
			float r = -0.5f*v + 0.5f;
			float x = -u;
			float y = 0.5f*r;
			float z = r*r - x*x;

			if (z<0)
				z=0;
			else
				z=sqrt(z);

			float fOneByLength = 1.0f / sqrt(x*x + y*y + z*z);

			pPos[0] = fOneByLength * x;
			pPos[1] = fOneByLength * y;
			pPos[2] = fOneByLength * z;
			pPos[3] = (z>0 ? 1.0f : 0.0f);

			if (bRemap)
			{
				pPos[0] = pPos[0]*0.5f + 0.5f;
				pPos[1] = pPos[1]*0.5f + 0.5f;
				pPos[2] = pPos[2]*0.5f + 0.5f;
			}

			pPos += 4;
		}
	}
	return pResult;
}
float *VistaOGLUtils::CreateConeDeapthTextureData( int iWidth, int iHeight )
{
	if (iWidth <= 0 || iHeight <= 0)
		return NULL;

	float *pResult = new float[iWidth * iHeight];
	float *pPos = pResult;

	float du = 2.0f / iWidth;
	float dv = 2.0f / iHeight;

	float v = 0.5f * dv - 1.0f;
	for (int j=0; j<iHeight; ++j, v+=dv)
	{
		float u = 0.5f * du - 1.0f;
		for (int i=0; i<iWidth; ++i, u+=du)
		{
			float r = -0.5f*v + 0.5f;
			float z =  r*r - u*u;

			if (z<0)
				z=0;
			else
				z=sqrt(z);

			pPos[0] = z;
			++pPos;
		}
	}
	return pResult;
}
/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateGaussianBlendingTextureData                           */
/*                                                                            */
/*============================================================================*/
float *VistaOGLUtils::CreateGaussianBlendingTextureData(int iWidth, int iHeight)
{
	if (iWidth <= 0 || iHeight <= 0)
		return NULL;

	float *pResult = new float[iWidth * iHeight];
	float *pPos = pResult;

	float du = 2.0f / iWidth;
	float dv = 2.0f / iHeight;
	float fFactor = 2.5f / sqrt(2.0f*float(Vista::Pi));

	float v = 0.5f * dv - 1.0f;

	for (int j=0; j<iHeight; ++j, v+=dv)
	{
		float u = 0.5f * du - 1.0f;
		for (int i=0; i<iWidth; ++i, u+=du)
		{
			float x = sqrt(u*u + v*v) * 4.0f;
			*(pPos++) = fFactor * exp(-0.5f*x*x);
		}
	}
	return pResult;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Delete                                                      */
/*                                                                            */
/*============================================================================*/
void VistaOGLUtils::Delete(unsigned char *pData)
{
	delete [] pData;
}

void VistaOGLUtils::Delete(float *pData)
{
	delete [] pData;
}


/*============================================================================*/
/*  LOKAL VARS / FUNCTIONS                                                    */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   LoadImageFromTgaStream                                      */
/*                                                                            */
/*============================================================================*/
static unsigned char *LoadImageFromTgaStream(istream &is, int &iWidth, int &iHeight, int &iChannels)
{
	// read in TGA header data - bug out, if we encounter an error
	if (is.get() != 0)	// id length
		return NULL;
	if (is.get() != 0)	// color map type
		return NULL;
	if (is.get() != 2)	// data type (=2)
		return NULL;
	for (int i=0; i<5; ++i)		// some stuff...
	{
		if (is.get() != 0)
			return NULL;
	}
	for (int i=0; i<4; ++i)		// ignore image origin
		is.get();

	// retrieve image dimensions
	iWidth = is.get();
	iWidth += ((int)is.get()) << 8;

	iHeight = is.get();
	iHeight += ((int)is.get()) << 8;

	iChannels = is.get() / 8;	// color depth
	if (iChannels != 3 && iChannels != 4)
		return NULL;

	is.get();		// image descriptor

	if (is.eof())			
		return NULL;


	vstr::debugi() << " [VistaOGLUtils::LoadImageFromTgaStream] - successfully read TGA header..." << endl;
	vstr::debugi() << "        image size ( width x height x channels ) : " << iWidth 
		<< "x" << iHeight << "x" << iChannels << endl;


	// allright, let's allocate some data...
	unsigned char *pData = new unsigned char[iWidth * iHeight * iChannels];

	if (!pData)
	{
		vstr::errp()<< " [VistaOGLUtils::LoadImageFromTgaStream] - Unable to allocate memory..." << endl;
		vstr::erri() << "        required memory in bytes: " << (iWidth * iHeight * iChannels) << endl;
		return NULL;
	}

	// open mouth, close eyes...
	is.read((char *)pData, iWidth * iHeight * iChannels);

	if (is.eof())
	{
		vstr::errp() << " [VistaOGLUtils::LoadImageFromTgaStream] - Unexpected end of file..." << endl;
		delete [] pData;
		return NULL;
	}

	// as data was in BGR(A) format, swizzle it around to get RGB(A)...
	int iPixelCount = iWidth * iHeight;
	unsigned char *pPos = pData;
	for (int i=0; i<iPixelCount; ++i)
	{
#ifndef SUNOS	// thanks, Sun... :-(
		swap(pPos[0], pPos[2]);
#else
		unsigned char cTemp = pPos[0];
		pPos[0] = pPos[2];
		pPos[2] = cTemp;
#endif
		pPos += iChannels;
	}

	// that's all, folks
	return pData;
}

void VistaOGLUtils::BeginOrtho( double dLeft, double dRight, double dBottom,
	double dTop, double dNear, double dFar )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( dLeft, dRight, dBottom, dTop, dNear, dFar );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
}

void VistaOGLUtils::BeginOrtho2D( double dLeft, double dRight,
	double dBottom, double dTop )
{
	BeginOrtho( dLeft, dRight, dBottom, dTop, -1.0, 1.0 );
}

void VistaOGLUtils::EndOrtho()
{
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
}


bool VistaOGLUtils::CheckForOGLError( const std::string& strFilename /* = "" */,
	int nLineNumber /* = -1 */ )
{
	const GLenum eErrorCode = glGetError();

	if( GL_NO_ERROR != eErrorCode )
	{
		switch( eErrorCode )
		{
		case GL_INVALID_ENUM:
			PrintError( eErrorCode, "invalid enum", strFilename, nLineNumber );
			break;
		case GL_INVALID_VALUE:
			PrintError( eErrorCode, "invalid value", strFilename, nLineNumber );
			break;
		case GL_INVALID_OPERATION:
			PrintError( eErrorCode, "invalid operation", strFilename, nLineNumber );
			break;
		case GL_STACK_OVERFLOW:
			PrintError( eErrorCode, "stack overflow", strFilename, nLineNumber );
			break;
		case GL_STACK_UNDERFLOW:
			PrintError( eErrorCode, "stack underflow", strFilename, nLineNumber );
			break;
		default:
			PrintError( eErrorCode, "unknown error!!", strFilename, nLineNumber );
			vstr::warn() << "Please add this error to CheckForOpenGLError() in "
				<< __FILE__ << " of the VistaVisExt." << endl;	
			break;
		};

		return true;
	}

	return false;
}


/*============================================================================*/
/*  END OF FILE "VistaOGLUtils.cpp"                                           */
/*============================================================================*/
