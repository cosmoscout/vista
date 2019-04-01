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


#include "VistaTransformable.h"

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/


IVistaTransformable::IVistaTransformable()
{
}

IVistaTransformable::~IVistaTransformable()
{
}


/*============================================================================*/
/* For convenience: Implementation Prototype based on MATHTYPES               */
/* Just copy this blob, and reimplement the topmost empty functions           */
/* Then search-andreplace ClassName by (surprise) the implementation class    */
/*============================================================================*/
/*

bool ClassName::SetTranslation( const VistaVector3D& v3Translation )
{
	//IMPLEMENT ME
	return false;
}
bool ClassName::Translate( const VistaVector3D& v3Translation )
{
	//IMPLEMENT ME
	return false;
}
bool ClassName::SetRotation( const VistaQuaternion& qRotation )
{
	//IMPLEMENT ME
	return false;
}
bool ClassName::Rotate( const VistaQuaternion& qRotation )
{
	//IMPLEMENT ME
	return false;
}
bool ClassName::SetScale( const VistaVector3D& v3Scale )
{
	//IMPLEMENT ME
	return false;
}
bool ClassName::Scale( const VistaVector3D& v3Scale )
{
	//IMPLEMENT ME
	return false;
}
bool ClassName::SetTransform( const VistaTransformMatrix& matTransform )
{
	//IMPLEMENT ME
	return false;
}
bool ClassName::Transform( const VistaTransformMatrix& matTransform )
{
	//IMPLEMENT ME
	return false;
}

//GENERIC IMPLEMENTATION

bool ClassName::SetTranslation( const float fX, const float fY, const float fZ )
{
	return SetTranslation( VistaVector3D( fX, fY, fZ ) );
}
bool ClassName::SetTranslation( const float a3fTranslation[3] )
{
	return SetTranslation( VistaVector3D( a3fTranslation ) );
}
bool ClassName::SetTranslation( const double a3dTranslation[3] )
{
	return SetTranslation( VistaVector3D( a3dTranslation ) );
}
bool ClassName::Translate( const float fX, const float fY, const float fZ )
{
	return Translate( VistaVector3D( fX, fY, fZ ) );
}
bool ClassName::Translate( const float a3fTranslation[3] )
{
	return Translate( VistaVector3D( a3fTranslation ) );
}
bool ClassName::Translate( const double a3dTranslation[3] )
{
	return Translate( VistaVector3D( a3dTranslation ) );
}
bool ClassName::SetRotation( const float fX, const float fY, const float fZ, const float fW )
{
	return SetRotation( VistaQuaternion( fX, fY, fZ, fW ) );
}
bool ClassName::SetRotation( const float a4fRotation[4] )
{
	return SetRotation( VistaQuaternion( a4fRotation ) );
}
bool ClassName::SetRotation( const double a4dRotation[4] )
{
	return SetRotation( VistaQuaternion( a4dRotation ) );
}
bool ClassName::Rotate( const float fX, const float fY, const float fZ, const float fW )
{
	return Rotate( VistaQuaternion( fX, fY, fZ, fW ) );
}
bool ClassName::Rotate( const float a4fRotation[4] )
{
	return Rotate( VistaQuaternion( a4fRotation ) );
}
bool ClassName::Rotate( const double a4dRotation[4] )
{
	return Rotate( VistaQuaternion( a4dRotation ) );
}
bool ClassName::SetTransform( const float a16fTransform[16], const bool bColumnMajor )
{
	return SetTransform( VistaTransformMatrix( a16fTransform, bColumnMajor ) );
}
bool ClassName::SetTransform( const double a16dTransform[16], const bool bColumnMajor )
{
	return SetTransform( VistaTransformMatrix( a16dTransform, bColumnMajor ) );
}
bool ClassName::Transform( const float a16fTransform[16], const bool bColumnMajor )
{
	return Transform( VistaTransformMatrix( a16fTransform, bColumnMajor ) );
}
bool ClassName::Transform( const double a16dTransform[16], const bool bColumnMajor )
{
	return Transform( VistaTransformMatrix( a16dTransform, bColumnMajor ) );
}
bool ClassName::SetScale( const float fX, const float fY, const float fZ )
{	
	return SetScale( VistaVector3D( fX, fY, fZ ) );
}
bool ClassName::SetScale( const float a3fScale[3] )
{
	return SetScale( VistaVector3D( a3fScale ) );
}
bool ClassName::SetScale( const double a3dScale[3] )
{
	return SetScale( VistaVector3D( a3dScale ) );
}
bool ClassName::Scale( const float fX, const float fY, const float fZ )
{	
	return Scale( VistaVector3D( fX, fY, fZ ) );
}
bool ClassName::Scale( const float a3fScale[3] )
{
	return Scale( VistaVector3D( a3fScale ) );
}
bool ClassName::Scale( const double a3dScale[3] )
{
	return Scale( VistaVector3D( a3dScale ) );
}

*/
