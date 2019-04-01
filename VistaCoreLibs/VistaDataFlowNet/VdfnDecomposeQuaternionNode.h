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


#ifndef _VDFNDECOMPOSEQUATERNIONNODE_H
#define _VDFNDECOMPOSEQUATERNIONNODE_H

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
 * VdfnDecomposeQuaternionNode
 * Disassembles a Quaternion and outputs its four components
 * to the outports
 *
 * @ingroup VdfnNodes
 * @inport{in,VistaQuaternion,mandatory, quaternion to be decomposed }
 *
 * @outport{x,float, x-component of the input quaternion}
 * @outport{y,float, y-component of the input quaternion}
 * @outport{z,float, z-component of the input quaternion}
 * @outport{w,float, w-component of the input quaternion}
 */
class VISTADFNAPI VdfnDecomposeQuaternionNode : public IVdfnNode
{
public:
	VdfnDecomposeQuaternionNode();
	virtual ~VdfnDecomposeQuaternionNode();
	
	bool PrepareEvaluationRun();

protected:
	virtual bool DoEvalNode();

private:
	TVdfnPort<float>*				m_pOutX;
	TVdfnPort<float>*				m_pOutY;
	TVdfnPort<float>*				m_pOutZ;
	TVdfnPort<float>*				m_pOutW;
	TVdfnPort<VistaQuaternion>*		m_pIn;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNDECOMPOSEQUATERNIONNODE_H
