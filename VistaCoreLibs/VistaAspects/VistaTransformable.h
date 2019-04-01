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


#ifndef _VISTATRANSFORMABLE_H
#define _VISTATRANSFORMABLE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaAspectsConfig.h"
#include "VistaLocatable.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAASPECTSAPI IVistaTransformable : virtual public IVistaLocatable
{
public:
	virtual ~IVistaTransformable();

	virtual bool SetTranslation( const VistaVector3D& v3Translation ) = 0;
	virtual bool SetTranslation( const float fX, const float fY, const float fZ ) = 0;
	virtual bool SetTranslation( const float a3fTranslation[3] ) = 0;
	virtual bool SetTranslation( const double a3dTranslation[3] ) = 0;
	
	virtual bool Translate( const VistaVector3D& v3Translation ) = 0;
	virtual bool Translate( const float fX, const float fY, const float fZ ) = 0;
	virtual bool Translate( const float a3fTranslation[3] ) = 0;
	virtual bool Translate( const double a3dTranslation[3] ) = 0;
	
	virtual bool SetRotation( const VistaQuaternion& qRotation ) = 0;
	virtual bool SetRotation( const float fX, const float fY, const float fZ, const float fW ) = 0;
	virtual bool SetRotation( const float a4fRotation[4] ) = 0;
	virtual bool SetRotation( const double a4dRotation[4] ) = 0;
	
	virtual bool Rotate( const VistaQuaternion& qRotation ) = 0;
	virtual bool Rotate( const float fX, const float fY, const float fZ, const float fW ) = 0;
	virtual bool Rotate( const float a4fRotation[4] ) = 0;
	virtual bool Rotate( const double a4dRotation[4] ) = 0;

	virtual bool SetScale( const VistaVector3D& v3Scale ) = 0;
	virtual bool SetScale( const float fX, const float fY, const float fZ ) = 0;
	virtual bool SetScale( const float a3fScale[3] ) = 0;
	virtual bool SetScale( const double a3dScale[3] ) = 0;

	virtual bool Scale( const VistaVector3D& v3Scale ) = 0;
	virtual bool Scale( const float fX, const float fY, const float fZ ) = 0;
	virtual bool Scale( const float a3fScale[3] ) = 0;
	virtual bool Scale( const double a3dScale[3] ) = 0;

	virtual bool SetTransform( const VistaTransformMatrix& matTransform ) = 0;
	virtual bool SetTransform( const float a16fTransform[16], const bool bColumnMajor = false ) = 0;
	virtual bool SetTransform( const double a16dTransform[16], const bool bColumnMajor = false ) = 0;

	virtual bool Transform( const VistaTransformMatrix& matTransform ) = 0;
	virtual bool Transform( const float a16fTransform[16], const bool bColumnMajor = false ) = 0;
	virtual bool Transform( const double a16dTransform[16], const bool bColumnMajor = false ) = 0;

protected:
	IVistaTransformable();
private:
};



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTATRANSFORMABLE_H
