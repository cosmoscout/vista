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


#ifndef _COLORCHANGERDFNNODE_H
#define _COLORCHANGERDFNNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
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
 * This class defines a node for the DataFlowNet. 
 * This node has three inports where new values are fed into the node,
 * and an outport that delivers the final 
 *
 * @inport{change_red,float,the value to change the red value}
 * @inport{change_green,float,the value to change the green value}
 * @inport{change_blue,float,the value to change the blue value}
 * @outport{color,VistaVector3D}
 */
class ColorChangerDfnNode : public IVdfnNode
{
public:
	ColorChangerDfnNode();

	bool GetIsValid() const;
	bool PrepareEvaluationRun();

protected:
	bool DoEvalNode();

protected:
	// storage for our inports
	TVdfnPort<float>*			m_pChangeRPort;
	TVdfnPort<float>*			m_pChangeGPort;
	TVdfnPort<float>*			m_pChangeBPort;

	//storage for the outport
	TVdfnPort<VistaVector3D>*	m_pColorPort;

	VistaVector3D				m_v3CurrentColor;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_COLORCHANGERDFNNODE_H

