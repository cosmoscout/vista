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


#include "VistaLocatable.h"

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

IVistaLocatable::IVistaLocatable()
{
}

IVistaLocatable::~IVistaLocatable()
{
}

/*============================================================================*/
/* For convenience: Implementation Prototype based on MATHTYPES               */
/* Just copy this blob, and reimplement the topmost empty functions           */
/* Then search-andreplace ClassName by (surprise) the implementation class    */
/*============================================================================*/
/*
bool VistaNode::GetTranslation( VistaVector3D& v3Translation ) const
{
	//IMPLEMENT ME
	return false;
}
bool VistaNode::GetWorldPosition( VistaVector3D& v3Position ) const
{
	//IMPLEMENT ME
	return false;
}
bool VistaNode::GetRotation( VistaQuaternion& qRotation ) const
{
	//IMPLEMENT ME
	return false;
}
bool VistaNode::GetWorldOrientation( VistaQuaternion& qOrientation ) const
{
	//IMPLEMENT ME
	return false;
}
bool VistaNode::GetScale( VistaVector3D& v3Scale ) const
{
	//IMPLEMENT ME
	return false;
}
bool VistaNode::GetWorldScale( VistaVector3D& v3Scale ) const
{
	//IMPLEMENT ME
	return false;
}
bool VistaNode::GetTransform( VistaTransformMatrix& matTransform ) const
{
	//IMPLEMENT ME
	return false;
}
bool VistaNode::GetWorldTransform( VistaTransformMatrix& matTransform ) const
{
	//IMPLEMENT ME
	return false;
}
bool VistaNode::GetParentWorldTransform( VistaTransformMatrix& matTransform ) const
{
	//IMPLEMENT ME
	return false;
}


//GENERIC IMPLEMENTATION

bool ClassName::GetTranslation( float& fX, float& fY, float& fZ ) const
{
	VistaVector3D v3Result;
	if( GetTranslation( v3Result ) == false )
		return false;
	fX = v3Result[Vista::X];
	fY = v3Result[Vista::Y];
	fZ = v3Result[Vista::Z];
	return false;
}
bool ClassName::GetTranslation( float a3fTranslation[3] ) const
{
	VistaVector3D v3Result;
	if( GetTranslation( v3Result ) == false )
		return false;
	v3Result.GetValues( a3fTranslation );
	return true;
}
bool ClassName::GetTranslation( double a3dTranslation[3] ) const
{
	VistaVector3D v3Result;
	if( GetTranslation( v3Result ) == false )
		return false;
	v3Result.GetValues( a3dTranslation );
	return true;
}
bool ClassName::GetWorldPosition( float& fX, float& fY, float& fZ ) const
{
	VistaVector3D v3Result;
	if( GetWorldPosition( v3Result ) == false )
		return false;
	fX = v3Result[Vista::X];
	fY = v3Result[Vista::Y];
	fZ = v3Result[Vista::Z];
	return true;
}
bool ClassName::GetWorldPosition( float a3fPosition[3] ) const
{
	VistaVector3D v3Result;
	if( GetWorldPosition( v3Result ) == false )
		return false;
	v3Result.GetValues( a3fPosition );
	return true;
}
bool ClassName::GetWorldPosition( double a3dPosition[3] ) const
{
	VistaVector3D v3Result;
	if( GetWorldPosition( v3Result ) == false )
		return false;
	v3Result.GetValues( a3dPosition );
	return true;
}
bool ClassName::GetRotation( float& fX, float& fY, float& fZ, float& fW ) const
{
	VistaQuaternion qResult;
	if( GetRotation( qResult ) == false)
		return false;
	fX = qResult[Vista::X];
	fY = qResult[Vista::Y];
	fZ = qResult[Vista::Z];
	fW = qResult[Vista::W];
	return true;
}
bool ClassName::GetRotation( float a4fRotation[4] ) const
{
	VistaQuaternion qResult;
	if( GetRotation( qResult ) == false)
		return false;
	qResult.GetValues( a4fRotation );
	return true;
}
bool ClassName::GetRotation( double a4dRotation[4] ) const
{
	VistaQuaternion qResult;
	if( GetRotation( qResult ) == false)
		return false;
	qResult.GetValues( a4dRotation );
	return true;
}
bool ClassName::GetWorldOrientation( float& fX, float& fY, float& fZ, float& fW ) const
{
	VistaQuaternion qResult;
	if( GetWorldOrientation( qResult ) == false)
		return false;
	fX = qResult[Vista::X];
	fY = qResult[Vista::Y];
	fZ = qResult[Vista::Z];
	fW = qResult[Vista::W];
	return true;
}
bool ClassName::GetWorldOrientation( float a4fOrientation[4] ) const
{
	VistaQuaternion qResult;
	if( GetWorldOrientation( qResult ) == false)
		return false;
	qResult.GetValues( a4fOrientation );
	return true;
}
bool ClassName::GetWorldOrientation( double a4dOrientation[4] ) const
{
	VistaQuaternion qResult;
	if( GetWorldOrientation( qResult ) == false)
		return false;
	qResult.GetValues( a4dOrientation );
	return true;
}
bool ClassName::GetScale( float& fX, float& fY, float& fZ ) const
{
	VistaVector3D v3Result;
	if( GetScale( v3Result ) == false)
		return false;
	fX = v3Result[Vista::X];
	fY = v3Result[Vista::Y];
	fZ = v3Result[Vista::Z];
	return true;
}
bool ClassName::GetScale( float a3fScale[3] ) const
{
	VistaVector3D v3Result;
	if( GetScale( v3Result ) == false)
		return false;
	v3Result.GetValues( a3fScale );
	return true;
}
bool ClassName::GetScale( double a3dScale[4] ) const
{
	VistaVector3D v3Result;
	if( GetScale( v3Result ) == false)
		return false;
	v3Result.GetValues( a3dScale );
	return true;
}
bool ClassName::GetWorldScale( float& fX, float& fY, float& fZ ) const
{
	VistaVector3D v3Result;
	if( GetWorldScale( v3Result ) == false)
		return false;
	fX = v3Result[Vista::X];
	fY = v3Result[Vista::Y];
	fZ = v3Result[Vista::Z];
	return true;
}
bool ClassName::GetWorldScale( float a3fScale[3] ) const
{
	VistaVector3D v3Result;
	if( GetWorldScale( v3Result ) == false)
		return false;
	v3Result.GetValues( a3fScale );
	return true;
}
bool ClassName::GetWorldScale( double a3dScale[4] ) const
{
	VistaVector3D v3Result;
	if( GetWorldScale( v3Result ) == false)
		return false;
	v3Result.GetValues( a3dScale );
	return true;
}
bool ClassName::GetTransform( float a16fTransform[16], const bool bColumnMajor ) const
{
	VistaTransformMatrix matTransform;
	if( GetTransform( matTransform ) == false )
		return false;
	if( bColumnMajor )
		matTransform.GetTransposedValues( a16fTransform );
	else
		matTransform.GetValues( a16fTransform );
	return true;
}
bool ClassName::GetTransform( double a16dTransform[16], const bool bColumnMajor ) const
{
	VistaTransformMatrix matTransform;
	if( GetTransform( matTransform ) == false )
		return false;
	if( bColumnMajor )
		matTransform.GetTransposedValues( a16dTransform );
	else
		matTransform.GetValues( a16dTransform );
	return true;
}
bool ClassName::GetWorldTransform( float a16fTransform[16], const bool bColumnMajor ) const
{
	VistaTransformMatrix matTransform;
	if( GetWorldTransform( matTransform ) == false )
		return false;
	if( bColumnMajor )
		matTransform.GetTransposedValues( a16fTransform );
	else
		matTransform.GetValues( a16fTransform );
	return true;
}
bool ClassName::GetWorldTransform( double a16dTransform[16], const bool bColumnMajor ) const
{
	VistaTransformMatrix matTransform;
	if( GetWorldTransform( matTransform ) == false )
		return false;
	if( bColumnMajor )
		matTransform.GetTransposedValues( a16dTransform );
	else
		matTransform.GetValues( a16dTransform );
	return true;
}
bool ClassName::GetParentWorldTransform( float a16fTransform[16], const bool bColumnMajor ) const
{
	VistaTransformMatrix matTransform;
	if( GetParentWorldTransform( matTransform ) == false )
		return false;
	if( bColumnMajor )
		matTransform.GetTransposedValues( a16fTransform );
	else
		matTransform.GetValues( a16fTransform );
	return true;
}
bool ClassName::GetParentWorldTransform( double a16dTransform[16], const bool bColumnMajor ) const
{
	VistaTransformMatrix matTransform;
	if( GetParentWorldTransform( matTransform ) == false )
		return false;
	if( bColumnMajor )
		matTransform.GetTransposedValues( a16dTransform );
	else
		matTransform.GetValues( a16dTransform );
	return true;
}

*/


