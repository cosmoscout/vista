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


#include "VistaTransformNode.h"
#include "VistaNodeBridge.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaTransformNode::VistaTransformNode()
: VistaGroupNode(),
  IVistaTransformable()
{
	m_nType = VISTA_TRANSFORMNODE;
}

VistaTransformNode::VistaTransformNode(VistaGroupNode*			pParent,
						IVistaNodeBridge*			pBridge,
						IVistaNodeData*				pData,
						std::string					strName)
: VistaGroupNode(pParent, pBridge, pData, strName),
  IVistaTransformable()
{
	m_nType = VISTA_TRANSFORMNODE;
}

VistaTransformNode::~VistaTransformNode()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaTransformNode::SetTranslation( const VistaVector3D& v3Translation )
{
	if( m_pBridge->SetTranslation( v3Translation, m_pData ) == false )
		return false;
	IncTransScore();
	return true;
}
bool VistaTransformNode::Translate( const VistaVector3D& v3Translation )
{
	if( m_pBridge->Translate( v3Translation, m_pData ) == false )
		return false;
	IncTransScore();
	return true;
}
bool VistaTransformNode::SetRotation( const VistaQuaternion& qRotation )
{
	if( m_pBridge->SetRotation( qRotation, m_pData ) == false )
		return false;
	IncTransScore();
	return true;
}
bool VistaTransformNode::Rotate( const VistaQuaternion& qRotation )
{
	if( m_pBridge->Rotate( qRotation, m_pData ) == false )
		return false;
	IncTransScore();
	return true;
}
bool VistaTransformNode::SetScale( const VistaVector3D& v3Scale )
{
	VistaTransformMatrix matTransform;
	if( m_pBridge->GetTransform( matTransform, m_pData ) == false )
		return false;
	VistaVector3D v3Translation, v3DecScale;
	VistaQuaternion qRotation;

	// The Decomposition will fail if the matrix contains a shearing component!
	//if( matTransform.Decompose( v3Translation, qRotation, v3DecScale ) == false )
	//	return false;

	// even if it is sheared, SetScale should overwrite, so it's okay...
	VistaQuaternion qDummy;
	matTransform.Decompose( v3Translation, qRotation, v3DecScale, qDummy );
	matTransform.Compose( v3Translation, qRotation, v3Scale );
	m_pBridge->SetTransform( matTransform, m_pData );

	IncTransScore();
	return true;
}
bool VistaTransformNode::Scale( const VistaVector3D& v3Scale )
{
	VistaTransformMatrix matTransform;
	if( m_pBridge->GetTransform( matTransform, m_pData ) == false )
		return false;
	for( int i = 0; i < 3; ++i )
	{
		for( int j = 0; j < 4; ++j )
		{
			matTransform[i][j] *= v3Scale[i];
		}
	}
	if( m_pBridge->SetTransform( matTransform, m_pData ) == false )
		return false;
	IncTransScore();
	return true;
}
bool VistaTransformNode::SetTransform( const VistaTransformMatrix& matTransform )
{
	if( m_pBridge->SetTransform( matTransform, m_pData ) == false )
		return false;
	IncTransScore();
	return true;
}
bool VistaTransformNode::Transform( const VistaTransformMatrix& matTransform )
{
	VistaTransformMatrix matOrigTransform;
	if( m_pBridge->GetTransform( matOrigTransform, m_pData ) == false )
		return false;
	matOrigTransform = matTransform * matOrigTransform;
	if( m_pBridge->SetTransform( matTransform, m_pData ) == false )
		return false;
	IncTransScore();
	return true;
}

// transformable implementation based on mathtypes

bool VistaTransformNode::SetTranslation( const float fX, const float fY, const float fZ )
{
	return SetTranslation( VistaVector3D( fX, fY, fZ ) );
}
bool VistaTransformNode::SetTranslation( const float a3fTranslation[3] )
{
	return SetTranslation( VistaVector3D( a3fTranslation ) );
}
bool VistaTransformNode::SetTranslation( const double a3dTranslation[3] )
{
	return SetTranslation( VistaVector3D( a3dTranslation ) );
}
bool VistaTransformNode::Translate( const float fX, const float fY, const float fZ )
{
	return Translate( VistaVector3D( fX, fY, fZ ) );
}
bool VistaTransformNode::Translate( const float a3fTranslation[3] )
{
	return Translate( VistaVector3D( a3fTranslation ) );
}
bool VistaTransformNode::Translate( const double a3dTranslation[3] )
{
	return Translate( VistaVector3D( a3dTranslation ) );
}
bool VistaTransformNode::SetRotation( const float fX, const float fY, const float fZ, const float fW )
{
	return SetRotation( VistaQuaternion( fX, fY, fZ, fW ) );
}
bool VistaTransformNode::SetRotation( const float a4fRotation[4] )
{
	return SetRotation( VistaQuaternion( a4fRotation ) );
}
bool VistaTransformNode::SetRotation( const double a4dRotation[4] )
{
	return SetRotation( VistaQuaternion( a4dRotation ) );
}
bool VistaTransformNode::Rotate( const float fX, const float fY, const float fZ, const float fW )
{
	return Rotate( VistaQuaternion( fX, fY, fZ, fW ) );
}
bool VistaTransformNode::Rotate( const float a4fRotation[4] )
{
	return Rotate( VistaQuaternion( a4fRotation ) );
}
bool VistaTransformNode::Rotate( const double a4dRotation[4] )
{
	return Rotate( VistaQuaternion( a4dRotation ) );
}
bool VistaTransformNode::SetTransform( const float a16fTransform[16], const bool bColumnMajor )
{
	return SetTransform( VistaTransformMatrix( a16fTransform, bColumnMajor ) );
}
bool VistaTransformNode::SetTransform( const double a16dTransform[16], const bool bColumnMajor )
{
	return SetTransform( VistaTransformMatrix( a16dTransform, bColumnMajor ) );
}
bool VistaTransformNode::Transform( const float a16fTransform[16], const bool bColumnMajor )
{
	return Transform( VistaTransformMatrix( a16fTransform, bColumnMajor ) );
}
bool VistaTransformNode::Transform( const double a16dTransform[16], const bool bColumnMajor )
{
	return Transform( VistaTransformMatrix( a16dTransform, bColumnMajor ) );
}
bool VistaTransformNode::SetScale( const float fX, const float fY, const float fZ )
{	
	return SetScale( VistaVector3D( fX, fY, fZ ) );
}
bool VistaTransformNode::SetScale( const float a3fScale[3] )
{
	return SetScale( VistaVector3D( a3fScale ) );
}
bool VistaTransformNode::SetScale( const double a3dScale[3] )
{
	return SetScale( VistaVector3D( a3dScale ) );
}
bool VistaTransformNode::Scale( const float fX, const float fY, const float fZ )
{	
	return Scale( VistaVector3D( fX, fY, fZ ) );
}
bool VistaTransformNode::Scale( const float a3fScale[3] )
{
	return Scale( VistaVector3D( a3fScale ) );
}
bool VistaTransformNode::Scale( const double a3dScale[3] )
{
	return Scale( VistaVector3D( a3dScale ) );
}

//locatable mathtypes re-implementation

bool VistaTransformNode::GetTranslation( VistaVector3D& v3Translation ) const
{
	return m_pBridge->GetTranslation( v3Translation, m_pData );
}

VistaVector3D VistaTransformNode::GetTranslation() const
{
	VistaVector3D v3Translation;
	this->GetTranslation(v3Translation);
	return v3Translation;
}

bool VistaTransformNode::GetRotation(VistaQuaternion& qRotation) const
{
	return m_pBridge->GetRotation( qRotation, m_pData );
}

VistaQuaternion VistaTransformNode::GetRotation() const
{
	VistaQuaternion qRotation;
	this->GetRotation(qRotation);
	return qRotation;
}

bool VistaTransformNode::GetTransform(VistaTransformMatrix& matTransform) const
{
	return m_pBridge->GetTransform( matTransform, m_pData );
}

VistaTransformMatrix VistaTransformNode::GetTransform() const
{
	VistaTransformMatrix matTransform;
	this->GetTransform(matTransform);
	return matTransform;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


