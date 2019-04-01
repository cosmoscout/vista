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


#ifndef _VISTAPROXIMITYSIGN_H
#define _VISTAPROXIMITYSIGN_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include "VistaProximityWarningBase.h"

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaGraphicsManager;
class VistaTransformNode;
class VistaGeometry;
class VistaGroupNode;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaProximitySign : public IVistaProximityWarningBase
{
public:
	VistaProximitySign( VistaEventManager* pManager,
								const float nBeginWarningDistance,
								const float nMaxWarningDistance,
								const bool bDisableOcclusion,
								VistaGraphicsManager* pGraphicsManager );
	virtual ~VistaProximitySign();

	void SetScale( const float nXScale, const float nYScale );
	bool SetTexture( const std::string sTexture ) const;
	bool SetDefaultTexture() const;
	void SetParentNode( VistaGroupNode* pNode );

	virtual bool DoUpdate( const float nMinDistance,
							const float nWarningLevel,
							const VistaVector3D& v3PointOnBounds,
							const VistaVector3D& v3UserPosition,
							const VistaQuaternion& qUserOrientation );

	virtual bool GetIsEnabled() const;
	virtual bool SetIsEnabled( const bool bSet );

	virtual bool DoTimeUpdate( VistaType::systemtime nTime, const float nOpacityScale, const bool bFlashState );


private:
	bool m_bEnabled;
	bool m_bCurrentFlashState;
	float m_nWarningLevel;
	VistaTransformNode* m_pPositionNode;
	VistaTransformNode* m_pScaleNode;
	VistaGeometry* m_pGeometry;
};

#endif //_VISTAPROXIMITYSIGN_H

