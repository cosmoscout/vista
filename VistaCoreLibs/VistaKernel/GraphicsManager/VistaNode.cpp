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


#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaNodeBridge.h>


#include <VistaAspects/VistaConversion.h>
#include <VistaMath/VistaBoundingBox.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cassert>
//#include <iostream.h>
/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaNode::VistaNode()
{
	m_nType = VISTA_NODE;
	//m_sName = "";
	m_pParent = NULL;
	m_pData = NULL;
	m_pBridge = NULL;
	m_nLocalTransScore = 0;
}

VistaNode::VistaNode(VistaGroupNode*				pParent,
					   IVistaNodeBridge*			pBridge,
					   IVistaNodeData*				pData,
					   const std::string	&strName)
{
	m_nType		=	VISTA_NODE;
	m_pParent	=	pParent;
	//m_sName		=	strName;
	m_pBridge	=	pBridge;
	m_pData		=	pData;

	SetName(strName);
	m_nLocalTransScore = 0;

}

VistaNode::VistaNode(const VistaNode&)
{
	assert(false);
}

VistaNode& VistaNode::operator=(const VistaNode&)
{
	assert(false);
	return *this;
}

VistaNode::~VistaNode()
{
  //  cout << "VistaNode::~VistaNode() @ " << this << "[" << GetName() << "]" << std::endl;
	if(m_pParent)
		m_pParent->DisconnectChild(this); // remove me
	delete m_pData;
}

// ============================================================================
// ============================================================================
std::string VistaNode::GetName() const
{
	if(m_pBridge)
	{
		std::string sTmp;
		if(m_pBridge->GetName(sTmp, m_pData))
			return sTmp;
	}

	return "";
}
// ============================================================================
// ============================================================================
bool VistaNode::SetName(const std::string &strName)
{
	//m_sName = strName;
	return m_pBridge->SetName(strName, m_pData);
}
// ============================================================================
// ============================================================================
VISTA_NODETYPE VistaNode::GetType() const
{
	return m_nType;
}
// ============================================================================
// ============================================================================
bool VistaNode::CanHaveChildren() const
{
	return false;
}

// ============================================================================
// ============================================================================
VistaGroupNode* VistaNode::GetParent() const
{
	return m_pParent;
}


// ============================================================================
// ============================================================================
bool VistaNode::GetIsEnabled() const
{
	return m_pBridge->GetIsEnabled(m_pData);
}
// ============================================================================
// ============================================================================
void VistaNode::SetIsEnabled(bool bEnable)
{
	m_pBridge->SetIsEnabled(bEnable, m_pData);
}

// ============================================================================
void VistaNode::Debug(std::ostream& out, int nLevel) const
{
	//Indent line
	out << vstr::indent;
	for(int j=0; j<nLevel; j++)
		out << "  ";
	out << "|--";
	out << "Level " << nLevel << ": " ;
	//Write type
	VISTA_NODETYPE typ = m_nType;
	switch(typ)
	{
		case VISTA_NODE:
			out << "VistaNode::";
		break;
		case VISTA_GROUPNODE:
			out << "VistaGroupNode::";
		break;
		case VISTA_SWITCHNODE:
			out << "VistaSwitchNode::";
		break;
		case VISTA_LEAFNODE:
			out << "VistaLeafNode::";
		break;
		case VISTA_LIGHTNODE:
			out << "VistaLightNode::";
		break;
		case VISTA_GEOMNODE:
			out << "VistaGeomNode::";
		break;
		case VISTA_LODNODE:
			out << "VistaLODNode::";
			break;
		case VISTA_TRANSFORMNODE:
			out << "VistaTransformNode::";
			break;
		case VISTA_AMBIENTLIGHTNODE:
			out << "VistaAmbientLightNode::";
			break;
		case VISTA_DIRECTIONALLIGHTNODE:
			out << "VistaDirectionalLightNode::";
			break;
		case VISTA_POINTLIGHTNODE:
			out << "VistaPointlightNode::";
			break;
		case VISTA_SPOTLIGHTNODE:
			out << "VistaSpotlightNode::";
			break;
		case VISTA_EXTENSIONNODE:
			out << "VistaExtensionNode::";
			break;
		case VISTA_OPENGLNODE:
			out << "VistaOpenGLNode::";
			break;
		case VISTA_TEXTNODE:
			out << "VistaTextNode::";
			break;
		default:
			out << "<###>--[" << VistaConversion::ToString(typ)
				 << "]::";
			break;
		break;
	}
	//Write Name
	std::string sTmp;

	m_pBridge->GetName(sTmp, m_pData);
	if(!sTmp.empty())
		out << sTmp;
	else
		out << "NONAME";

	if( GetIsEnabled() )
		out << " [ENABLED]";
	else
		out << " [DISABLED]";

	out << std::endl;	
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetBoundingBox                                                */
/*                                                                            */
/*============================================================================*/
bool VistaNode::GetBoundingBox(VistaVector3D &pMin, VistaVector3D &pMax) const
{
	return m_pBridge->GetBoundingBox(pMin, pMax, m_pData);
}

bool VistaNode::GetBoundingBox( VistaBoundingBox& oBox ) const
{
	return GetBoundingBox( oBox.m_v3Min, oBox.m_v3Max );
}

VistaBoundingBox VistaNode::GetBoundingBox() const
{
	VistaBoundingBox oBox;
	if( GetBoundingBox( oBox.m_v3Min, oBox.m_v3Max ) == false )
		return VistaBoundingBox();
	return oBox;
}


bool VistaNode::GetWorldBoundingBox( VistaVector3D& v3Min, VistaVector3D& v3Max ) const
{
	VistaBoundingBox oBox;
	if( GetWorldBoundingBox( oBox ) == false )
		return false;
	v3Min = oBox.m_v3Min;
	v3Max = oBox.m_v3Max;
	return true;
}

bool VistaNode::GetWorldBoundingBox( VistaBoundingBox& oBox ) const
{
	VistaVector3D v3Min, v3Max;
	if( GetBoundingBox( v3Min, v3Max ) == false )
		return false;
	VistaTransformMatrix matToWorldTransform;
	if( GetParentWorldTransform( matToWorldTransform ) == false )
		return false;
	return VistaBoundingBox::ComputeAABB( v3Min, v3Max,
									matToWorldTransform, oBox );
}

VistaBoundingBox VistaNode::GetWorldBoundingBox() const
{
	VistaBoundingBox oBox;
	if( GetWorldBoundingBox( oBox ) == false )
		return VistaBoundingBox();
	return oBox;
}



/*============================================================================*/
/*                                                                            */
/*  NAME    :   GetData                                                       */
/*                                                                            */
/*============================================================================*/
IVistaNodeData* VistaNode::GetData() const
{
	return m_pData;
}

VistaType::uint64 VistaNode::GetTransformScore() const
{
	if(m_pParent)
		return m_pParent->GetTransformScore() + m_nLocalTransScore;
	return m_nLocalTransScore;
}

/*============================================================================*/
/* IVistaLocatable Implementation                                             */
/*============================================================================*/
bool VistaNode::GetTranslation( VistaVector3D& v3Translation ) const
{
	return GetWorldPosition( v3Translation );
}
bool VistaNode::GetWorldPosition( VistaVector3D& v3Position ) const
{
	return m_pBridge->GetWorldPosition( v3Position, m_pData );
}

bool VistaNode::GetRotation( VistaQuaternion& qRotation ) const
{
	return GetWorldOrientation( qRotation );
}

bool VistaNode::GetWorldOrientation( VistaQuaternion& qOrientation ) const
{
	return m_pBridge->GetWorldOrientation( qOrientation, m_pData );
}

bool VistaNode::GetScale( VistaVector3D& v3Scale ) const
{
	VistaTransformMatrix matTransform;
	if( GetTransform( matTransform ) == false )
		return false;

	VistaVector3D v3Translation;
	VistaQuaternion qRot;
	// this will fail if the matrix contains a shearing component!
	if( matTransform.Decompose( v3Translation, qRot, v3Scale ) == false )
	{
		vstr::warnp() << "[VistaNode::GetScale]: Trying to retrieve non-uniform, sheared scale!" << std::endl;
		return false;
	}
	return true;
}
bool VistaNode::GetWorldScale( VistaVector3D& v3Scale ) const
{
	VistaTransformMatrix matTransform;
	if( GetWorldTransform( matTransform ) == false )
		return false;

	VistaVector3D v3Translation;
	VistaQuaternion qRot;
	// this will fail if the matrix contains a shearing component!
	if( matTransform.Decompose( v3Translation, qRot, v3Scale ) == false )
	{
		vstr::warnp() << "[VistaNode::GetWorldScale]: Trying to retrieve non-uniform, sheared scale!" << std::endl;
		return false;
	}
	return true;
}

bool VistaNode::GetTransform( VistaTransformMatrix& matTransform ) const
{
	return GetWorldTransform( matTransform );
}

bool VistaNode::GetWorldTransform( VistaTransformMatrix& matTransform ) const
{
	return m_pBridge->GetWorldTransform( matTransform, m_pData );
}

bool VistaNode::GetParentWorldTransform( VistaTransformMatrix& matTransform ) const
{
	if( m_pParent != NULL )
		return m_pParent->GetWorldTransform( matTransform );
	else
		matTransform.SetToIdentity();
	return true;
}


bool VistaNode::GetTranslation( float& fX, float& fY, float& fZ ) const
{
	VistaVector3D v3Result;
	if( GetTranslation( v3Result ) == false )
		return false;
	fX = v3Result[Vista::X];
	fY = v3Result[Vista::Y];
	fZ = v3Result[Vista::Z];
	return false;
}
bool VistaNode::GetTranslation( float a3fTranslation[3] ) const
{
	VistaVector3D v3Result;
	if( GetTranslation( v3Result ) == false )
		return false;
	v3Result.GetValues( a3fTranslation );
	return true;
}
bool VistaNode::GetTranslation( double a3dTranslation[3] ) const
{
	VistaVector3D v3Result;
	if( GetTranslation( v3Result ) == false )
		return false;
	v3Result.GetValues( a3dTranslation );
	return true;
}
bool VistaNode::GetWorldPosition( float& fX, float& fY, float& fZ ) const
{
	VistaVector3D v3Result;
	if( GetWorldPosition( v3Result ) == false )
		return false;
	fX = v3Result[Vista::X];
	fY = v3Result[Vista::Y];
	fZ = v3Result[Vista::Z];
	return true;
}
bool VistaNode::GetWorldPosition( float a3fPosition[3] ) const
{
	VistaVector3D v3Result;
	if( GetWorldPosition( v3Result ) == false )
		return false;
	v3Result.GetValues( a3fPosition );
	return true;
}
bool VistaNode::GetWorldPosition( double a3dPosition[3] ) const
{
	VistaVector3D v3Result;
	if( GetWorldPosition( v3Result ) == false )
		return false;
	v3Result.GetValues( a3dPosition );
	return true;
}
bool VistaNode::GetRotation( float& fX, float& fY, float& fZ, float& fW ) const
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
bool VistaNode::GetRotation( float a4fRotation[4] ) const
{
	VistaQuaternion qResult;
	if( GetRotation( qResult ) == false)
		return false;
	qResult.GetValues( a4fRotation );
	return true;
}
bool VistaNode::GetRotation( double a4dRotation[4] ) const
{
	VistaQuaternion qResult;
	if( GetRotation( qResult ) == false)
		return false;
	qResult.GetValues( a4dRotation );
	return true;
}
bool VistaNode::GetWorldOrientation( float& fX, float& fY, float& fZ, float& fW ) const
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
bool VistaNode::GetWorldOrientation( float a4fOrientation[4] ) const
{
	VistaQuaternion qResult;
	if( GetWorldOrientation( qResult ) == false)
		return false;
	qResult.GetValues( a4fOrientation );
	return true;
}
bool VistaNode::GetWorldOrientation( double a4dOrientation[4] ) const
{
	VistaQuaternion qResult;
	if( GetWorldOrientation( qResult ) == false)
		return false;
	qResult.GetValues( a4dOrientation );
	return true;
}
bool VistaNode::GetScale( float& fX, float& fY, float& fZ ) const
{
	VistaVector3D v3Result;
	if( GetScale( v3Result ) == false)
		return false;
	fX = v3Result[Vista::X];
	fY = v3Result[Vista::Y];
	fZ = v3Result[Vista::Z];
	return true;
}
bool VistaNode::GetScale( float a3fScale[3] ) const
{
	VistaVector3D v3Result;
	if( GetScale( v3Result ) == false)
		return false;
	v3Result.GetValues( a3fScale );
	return true;
}
bool VistaNode::GetScale( double a3dScale[4] ) const
{
	VistaVector3D v3Result;
	if( GetScale( v3Result ) == false)
		return false;
	v3Result.GetValues( a3dScale );
	return true;
}
bool VistaNode::GetWorldScale( float& fX, float& fY, float& fZ ) const
{
	VistaVector3D v3Result;
	if( GetWorldScale( v3Result ) == false)
		return false;
	fX = v3Result[Vista::X];
	fY = v3Result[Vista::Y];
	fZ = v3Result[Vista::Z];
	return true;
}
bool VistaNode::GetWorldScale( float a3fScale[3] ) const
{
	VistaVector3D v3Result;
	if( GetWorldScale( v3Result ) == false)
		return false;
	v3Result.GetValues( a3fScale );
	return true;
}
bool VistaNode::GetWorldScale( double a3dScale[4] ) const
{
	VistaVector3D v3Result;
	if( GetWorldScale( v3Result ) == false)
		return false;
	v3Result.GetValues( a3dScale );
	return true;
}
bool VistaNode::GetTransform( float a16fTransform[16], const bool bColumnMajor ) const
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
bool VistaNode::GetTransform( double a16dTransform[16], const bool bColumnMajor ) const
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
bool VistaNode::GetWorldTransform( float a16fTransform[16], const bool bColumnMajor ) const
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
bool VistaNode::GetWorldTransform( double a16dTransform[16], const bool bColumnMajor ) const
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
bool VistaNode::GetParentWorldTransform( float a16fTransform[16], const bool bColumnMajor ) const
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
bool VistaNode::GetParentWorldTransform( double a16dTransform[16], const bool bColumnMajor ) const
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
