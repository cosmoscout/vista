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


#ifndef _VISTADFNFADEOUTNODE_H
#define _VISTADFNFADEOUTNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaBase/VistaExceptionBase.h>

#include <VistaKernel/VistaKernelConfig.h>

#include <map>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnUtil.h>


#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <VistaAspects/VistaPropertyAwareable.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaViewport;
class VistaDisplayManager;
class VistaClusterMode;
class VistaColorOverlay;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This node adds an overlay onto given viewports and fades them in or out, i.e.,
 * sets the overlay opacity on the viewport to the value specified at the inports.
 *
 * This node creates an animation on opacity, it is evalued unconditional on ever frame.
 *
 * @ingroup VdfnNodes
 * @inport{opacity, float, optional\, when state is given, the transparency value of the overlay}
 * @inport{state, bool, optional\, when opacity is given, application state: on or off} 
 */
class VISTAKERNELAPI VistaDfnFadeoutNode : public IVdfnNode
{
public:
	VistaDfnFadeoutNode( VistaClusterMode* pClusterMode );
	~VistaDfnFadeoutNode();

	bool GetIsValid() const;
	bool PrepareEvaluationRun();

	void AddToViewport( VistaViewport* pViewport );
	void AddToAllViewports( VistaDisplayManager* pManager );

	VistaColor GetColor() const;
	void SetColor( const VistaColor& oValue );

	VistaType::microtime GetFadeinTime() const;
	void SetFadeinTime( const VistaType::microtime oValue );
	VistaType::microtime GetFadeoutTime() const;
	void SetFadeoutTime( const VistaType::microtime oValue );


protected:
	bool DoEvalNode();
	void SetModeOpacity();

private:
	std::vector<VistaColorOverlay*> m_vecOverlays;

	VistaClusterMode* m_pClusterMode;

	TVdfnPort<bool>*	m_pStatePort;
	TVdfnPort<float>*	m_pOpacityPort;

	VistaType::microtime m_nFadeinTime;
	VistaType::microtime m_nFadeoutTime;
	VistaColor m_oColor;

	int m_nState;
	VistaType::microtime m_nStartTime;
	float m_nOpacity;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFOVERLAYTEXTLABELNODE_H

