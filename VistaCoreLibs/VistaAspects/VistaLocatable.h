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


#ifndef _VISTALOCATABLE_H
#define _VISTALOCATABLE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaAspectsConfig.h"

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAASPECTSAPI IVistaLocatable
{
public:
	virtual ~IVistaLocatable();
	
	virtual bool GetTranslation( VistaVector3D& v3Translation ) const = 0;
	virtual bool GetTranslation( float& fX, float& fY,	float& fZ ) const = 0;
	virtual bool GetTranslation( float a3fTranslation[3] ) const = 0;
	virtual bool GetTranslation( double a3dTranslation[3] ) const = 0;

	virtual bool GetWorldPosition( VistaVector3D& v3Position ) const = 0;
	virtual bool GetWorldPosition( float& fX, float& fY, float& fZ ) const = 0;
	virtual bool GetWorldPosition( float a3fPosition[3] ) const = 0;	
	virtual bool GetWorldPosition( double a3dPosition[3] ) const = 0;	
	
	virtual bool GetRotation( VistaQuaternion& qRotation ) const = 0;
	virtual bool GetRotation( float& fX, float& fY, float& fZ, float& fW ) const = 0;
	virtual bool GetRotation( float a4fRotation[4] ) const = 0;
	virtual bool GetRotation( double a4dRotation[4] ) const = 0;
	
	virtual bool GetWorldOrientation( VistaQuaternion& qOrientation ) const = 0;
	virtual bool GetWorldOrientation( float& fX, float& fY, float& fZ, float& fW ) const = 0;
	virtual bool GetWorldOrientation( float a4fOrientation[4] ) const = 0;
	virtual bool GetWorldOrientation( double a4dOrientation[4] ) const = 0;

	virtual bool GetScale( VistaVector3D& v3Scale ) const = 0;
	virtual bool GetScale( float& fX, float& fY, float& fZ ) const = 0;
	virtual bool GetScale( float a3fScale[3] ) const = 0;
	virtual bool GetScale( double a3dScale[3] ) const = 0;

	virtual bool GetWorldScale( VistaVector3D& v3Scale ) const = 0;
	virtual bool GetWorldScale( float& fX, float& fY, float& fZ ) const = 0;
	virtual bool GetWorldScale( float a3fScale[3] ) const = 0;
	virtual bool GetWorldScale( double a3dScale[3] ) const = 0;

	virtual bool GetTransform( VistaTransformMatrix& matTransform ) const = 0;
	virtual bool GetTransform( float a16fTransform[16], const bool bColumnMajor = false ) const = 0;
	virtual bool GetTransform( double a16dTransform[16], const bool bColumnMajor = false ) const = 0;
	
	virtual bool GetWorldTransform( VistaTransformMatrix& matTransform ) const = 0;
	virtual bool GetWorldTransform( float a16fTransform[16], const bool bColumnMajor = false ) const = 0;
	virtual bool GetWorldTransform( double a16dTransform[16], const bool bColumnMajor = false ) const = 0;	

	/**
	 *  returns true and gets the WorldTransform of the parent if the locatable is in
	 *  a hierarchy (even if it does not have a parent, e.g. if it is a rootnode, in which
	 *  case a unit matrix is returned).
	 *  returns false if the locatable is not in a hierarchy.
	 */
	virtual bool GetParentWorldTransform( VistaTransformMatrix& matTransform ) const = 0;
	virtual bool GetParentWorldTransform( float a16fTransform[16], const bool bColumnMajor = false ) const = 0;	
	virtual bool GetParentWorldTransform( double a16dTransform[16], const bool bColumnMajor = false ) const = 0;	

protected:
	IVistaLocatable();
private:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTALOCATABLE_H

