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
// $Id: VistaAutoBuffer.h 31862 2012-08-31 22:54:08Z ingoassenmacher $

#ifndef _VISTAIMAGE_H_
#define _VISTAIMAGE_H_

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaAtomicCounter.h>

#include <VistaAspects/VistaSharedCore.h>
#include <VistaAspects/VistaSharedCoreOwner.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaImageCore;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaImage : public IVistaSharedCoreOwner
{
public:
	typedef unsigned int GLenum; // so that we don't have to include gl.h in header

	static unsigned int GetNumberOfComponentsForPixelFormat( const GLenum eFormat );
	static std::string GetNameForPixelFormat( const GLenum eFormat );
	static bool GetPixelFormatIsCompressed( const GLenum eFormat );
	static unsigned int GetNumberOfBytesForPixelDataType( const GLenum eDataType );
	static std::string GetNameForPixelDataType( const GLenum eDataType );
	static unsigned int GetNumberOfBytesPerPixel( const GLenum eFormat, const GLenum eDataType );
	static unsigned int GetPixelFormatHasAlpha( const GLenum eFormat );

	// @IMGTODO compression

	VistaImage( DataHandling eDataHandlingMode = DH_COPY_ON_WRITE );
	VistaImage( const VistaImage& oOther );
	virtual ~VistaImage();
	VistaImage& operator= ( const VistaImage& oOther );

	const IVistaImageCore* GetCore() const;
	IVistaImageCore* GetCoreForWriting();
	
	bool GetIsValid() const;

	bool LoadFromFile( const std::string& sFilename, const bool bUseCaching = true );
	bool WriteToFile( const std::string& sFilename ) const;

	bool Set1DData( const int nWidth,
						VistaType::byte* pData,
						const GLenum ePixelFormat,
						const GLenum ePixelDataType,
						const bool bCopyData = true );
	bool Set2DData( const int nWidth, const int nHeight,
						VistaType::byte* pData,
						const GLenum ePixelFormat,
						const GLenum ePixelDataType,
						const bool bCopyData = true );
	bool Set3DData( const int nWidth, const int nHeight, const int nDepth,
						VistaType::byte* pData,
						const GLenum ePixelFormat,
						const GLenum ePixelDataType,
						const bool bCopyData = true );

	unsigned int GetDimension() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetDepth() const;
	unsigned int GetBytesPerPixel() const;
	unsigned int GetComponentsPerPixel() const;
	unsigned int GetBytesPerComponent() const;

	bool GetHasAlphaChannel() const;
	bool GetHasTransparency() const;
	bool GetHasBinaryAlphaChannel() const;
	
	bool Rescale( const unsigned int nNewWidth, const unsigned int nNewHeight );
	bool Crop( const unsigned int nBottomLeftX, const unsigned int nBottomLeftY,
				const unsigned int nNewWidth, const unsigned int nNewHeight );
	
	GLenum GetPixelFormat() const;
	GLenum GetPixelDataType() const;
	bool ReformatImage( const GLenum eNewPixelFormat, const GLenum eNewPixelDataType );

	unsigned int GetDataSize() const;
	const VistaType::byte* GetData() const;
	VistaType::byte* GetDataWrite();
};


class VISTAKERNELAPI IVistaImageCore : public IVistaSharedCore
{
public:
	typedef unsigned int GLenum; // so that we don't have to include gl.h in header

	virtual bool GetIsValid() const = 0;

	virtual bool LoadFromFile( const std::string& sFilename ) = 0;
	virtual bool WriteToFile( const std::string& sFilename ) const = 0;

	virtual bool Set1DData( const int nWidth,
						VistaType::byte* pData,
						const GLenum ePixelFormat,
						const GLenum ePixelDataType,
						const bool bCopyData ) = 0;
	virtual bool Set2DData( const int nWidth, const int nHeight,
						VistaType::byte* pData,
						const GLenum ePixelFormat,
						const GLenum ePixelDataType,
						const bool bCopyData ) = 0;
	virtual bool Set3DData( const int nWidth, const int nHeight, const int nDepth,
						VistaType::byte* pData,
						const GLenum ePixelFormat,
						const GLenum ePixelDataType,
						const bool bCopyData ) = 0;

	virtual unsigned int GetDimension() const = 0;
	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;
	virtual unsigned int GetDepth() const = 0;
	virtual unsigned int GetBytesPerPixel() const = 0;
	virtual unsigned int GetComponentsPerPixel() const = 0;
	virtual unsigned int GetBytesPerComponent() const = 0;

	virtual bool GetHasAlphaChannel() const = 0;
	virtual bool GetHasTransparency() const = 0;
	virtual bool GetHasBinaryAlphaChannel() const = 0;
	
	virtual bool Rescale( const unsigned int nNewWidth, const unsigned int nNewHeight ) = 0;
	virtual bool Crop( const unsigned int nBottomLeftX, const unsigned int nBottomLeftY,
				const unsigned int nNewWidth, const unsigned int nNewHeight ) = 0;
	
	virtual GLenum GetPixelFormat() const = 0;
	virtual GLenum GetPixelDataType() const = 0;
	virtual bool ReformatImage( const GLenum eNewPixelFormat, const GLenum eNewPixelDataType ) = 0;

	virtual unsigned int GetDataSize() const = 0;
	virtual const VistaType::byte* GetData() const = 0;
	virtual VistaType::byte* GetDataWrite() = 0;

protected:
	IVistaImageCore() {};
	virtual ~IVistaImageCore() {};

	virtual IVistaImageCore* Clone() const = 0;
};


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif /* _VISTAIMAGE_H_ */
