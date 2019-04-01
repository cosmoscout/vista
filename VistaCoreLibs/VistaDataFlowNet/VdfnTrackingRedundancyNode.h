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


#ifndef _VDFNTRACKINGREDUNDANCYNODE
#define _VDFNTRACKINGREDUNDANCYNODE

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnNode.h"
#include "VdfnPort.h"

#include "VdfnSerializer.h"

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

/**
 * VdfnTrackingRedundancyNode
 * Realizes redundant tracking using an additional tracking target and computing
 * the data of the original one if that one is not tracked.
 * If data from the original are available, the data is just forwarded to the
 * output. If not, it is calculated from the secondary data.
 *
 * @ingroup VdfnNodes
 * @inport{position_main,VistaVector3D,mandatory, position of the main tracking target }
 * @inport{orientation_main,VistaQuaternion,mandatory, orientation of the main tracking target }
 * @inport{position_secondary,VistaVector3D,mandatory, position of the redundant tracking target }
 * @inport{orientation_secondary,VistaQuaternion,mandatory, orientation of the redundant tracking target }
 *
 * @outport{position,VistaVector3D, position of the main tracking target (forwarded or computed)}
 * @outport{orientation,VistaQuaternion, orientation of the main tracking target (forwarded or computed)}
 * @outport{valid,bool, true iff the data that is output is valid and up to date }
 * @outport{computed,bool, true iff the output data is computed from secondary tracking data}
 */
class VISTADFNAPI VdfnTrackingRedundancyNode : public IVdfnNode
{
public:
	VdfnTrackingRedundancyNode();
	virtual ~VdfnTrackingRedundancyNode();

	bool GetIsValid() const;

	bool PrepareEvaluationRun();

protected:
	virtual bool DoEvalNode();

private:

	TVdfnPort<VistaVector3D>*		m_pInPosMain;
	TVdfnPort<VistaQuaternion>*		m_pInOriMain;
	TVdfnPort<VistaVector3D>*		m_pInPosSecondary;
	TVdfnPort<VistaQuaternion>*		m_pInOriSecondary;

	TVdfnPort<VistaVector3D>*		m_pOutPos;
	TVdfnPort<VistaQuaternion>*		m_pOutOri;
	TVdfnPort<bool>*				m_pOutValid;
	TVdfnPort<bool>*				m_pOutComputed;


	VistaVector3D					m_v3LastMain;
	VistaQuaternion					m_qLastMain;
	VistaVector3D					m_v3LastSecondary;
	VistaQuaternion					m_qLastSecondary;
	unsigned int					m_uiUpdateCounterMain;
	unsigned int					m_uiUpdateCounterSecondary;


	VistaTransformMatrix			m_matLastTransform;
	bool							m_bLastTransformValid;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VdfnTrackingRedundancyNode
