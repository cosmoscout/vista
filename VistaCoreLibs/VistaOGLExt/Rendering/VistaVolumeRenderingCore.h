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



#ifndef __VistaVolumeRenderingCore_h
#define __VistaVolumeRenderingCore_h

#include "../VistaOGLExtConfig.h"

class VistaTexture;


class VISTAOGLEXTAPI IVistaVolumeRenderingCore
{
public:
	virtual ~IVistaVolumeRenderingCore();

	// *** Data specification ***
	virtual bool SetVolumeTexture( VistaTexture* pVolumeTexture, int iVolumeId ) = 0;
	virtual VistaTexture* GetVolumeTexture( int iVolumeId ) const = 0;

	virtual bool SetExtents( float aDataExtents[3] ) = 0;
	virtual void GetExtents( float aDataExtents[3] ) = 0;

	// *** Lookup table specification ***
	virtual bool SetLookupTexture( VistaTexture* pLookupTexture, int iLookupId ) = 0;
	virtual VistaTexture* GetLookupTexture( int iLookupId ) const = 0;

	virtual bool SetLookupRange( float* pLookupRange, int iRangeId,
			unsigned int uiNumComponents, unsigned int uiNumVectors ) = 0;
	virtual bool GetLookupRange( float* pLookupRange, int iRangeId,
			unsigned int uiNumComponents, unsigned int uiNumVectors ) const = 0;

	virtual void Draw() = 0;

protected:
	IVistaVolumeRenderingCore();
};

#endif // Include guard.
