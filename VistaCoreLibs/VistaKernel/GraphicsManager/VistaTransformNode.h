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


#ifndef _VISTATRANSFORMNODE_H
#define _VISTATRANSFORMNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaGroupNode.h"
#include <VistaAspects/VistaTransformable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
#ifdef WIN32
	// this warning says that we use multiple inheritance and have a diamond
	// we know this, and constructed it to work correctly. This warning just
	// tells us that everything works as expected, so we deactivate it locally
	#pragma warning(push)
	#pragma warning(disable: 4250)
#endif
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaTransformNode : public VistaGroupNode, public IVistaTransformable
{
	friend class VistaSceneGraph;
	friend class IVistaNodeBridge;
public:
	virtual ~VistaTransformNode();

	// Transformable implementation
	virtual bool SetTranslation( const VistaVector3D& v3Translation );
	virtual bool SetTranslation( const float fX, const float fY, const float fZ );
	virtual bool SetTranslation( const float a3fTranslation[3] );
	virtual bool SetTranslation( const double a3dTranslation[3] );
	
	virtual bool Translate( const VistaVector3D& v3Translation );
	virtual bool Translate( const float fX, const float fY, const float fZ );
	virtual bool Translate( const float a3fTranslation[3] );
	virtual bool Translate( const double a3dTranslation[3] );
	
	virtual bool SetRotation( const VistaQuaternion& qRotation );
	virtual bool SetRotation( const float fX, const float fY, const float fZ, const float fW );
	virtual bool SetRotation( const float a4fRotation[4] );
	virtual bool SetRotation( const double a4dRotation[4] );
	
	virtual bool Rotate( const VistaQuaternion& qRotation );
	virtual bool Rotate( const float fX, const float fY, const float fZ, const float fW );
	virtual bool Rotate( const float a4fRotation[4] );
	virtual bool Rotate( const double a4dRotation[4] );

	virtual bool SetScale( const VistaVector3D& v3Scale );
	virtual bool SetScale( const float fX, const float fY, const float fZ );
	virtual bool SetScale( const float a3fScale[3] );
	virtual bool SetScale( const double a3dScale[3] );

	virtual bool Scale( const VistaVector3D& v3Scale );
	virtual bool Scale( const float fX, const float fY, const float fZ );
	virtual bool Scale( const float a3fScale[3] );
	virtual bool Scale( const double a3dScale[3] );

	virtual bool SetTransform( const VistaTransformMatrix& matTransform );
	virtual bool SetTransform( const float a16fTransform[16], const bool bColumnMajor = false );
	virtual bool SetTransform( const double a16dTransform[16], const bool bColumnMajor = false );

	virtual bool Transform( const VistaTransformMatrix& matTransform );
	virtual bool Transform( const float a16fTransform[16], const bool bColumnMajor = false );
	virtual bool Transform( const double a16dTransform[16], const bool bColumnMajor = false );

	//we need to re-implement the bathing (mathtype) locatable api
	virtual bool GetTranslation( VistaVector3D& v3Translation ) const;
	virtual bool GetRotation( VistaQuaternion& qRotation ) const;
	virtual bool GetTransform( VistaTransformMatrix& matTransform ) const;
	//since the re-implementation hides the other types, we have to specify them
	using VistaNode::GetTranslation;
	using VistaNode::GetRotation;
	using VistaNode::GetTransform;

	// convenience interface
	VistaVector3D GetTranslation() const;
	VistaQuaternion GetRotation() const;
	VistaTransformMatrix GetTransform() const;
	

protected:
	VistaTransformNode();
	VistaTransformNode(VistaGroupNode* pParent,
						IVistaNodeBridge* pBridge,
						IVistaNodeData* pData,
						std::string	 strName = "");
};

#ifdef WIN32
	#pragma warning(pop)
	#pragma warning(disable: 4250)
#endif

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

