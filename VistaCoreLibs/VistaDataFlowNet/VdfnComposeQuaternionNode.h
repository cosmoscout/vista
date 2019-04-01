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


#ifndef _VDFNCOMPOSEQUATERNIONNODE
#define _VDFNCOMPOSEQUATERNIONNODE

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
 * Assembles a Quaternion from four floats
 *
 *
 * @ingroup VdfnNodes
 * @inport{x,float,mandatory, x-component of the vector }
 * @inport{y,float,mandatory, y-component of the vector }
 * @inport{z,float,mandatory, z-component of the vector }
 * @inport{w,float,mandatory, w-component of the quaternion }
 *
 * @outport{out,VistaVector3D, assembled quaternion (normalized)}
 */
class VISTADFNAPI VdfnComposeQuaternionNode : public IVdfnNode
{
public:
	VdfnComposeQuaternionNode();
	virtual ~VdfnComposeQuaternionNode();
	
	bool PrepareEvaluationRun();

protected:
	virtual bool DoEvalNode();

private:
	TVdfnPort<float>*				m_pInX;
	TVdfnPort<float>*				m_pInY;
	TVdfnPort<float>*				m_pInZ;
	TVdfnPort<float>*				m_pInW;
	TVdfnPort<VistaQuaternion>*		m_pOut;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNCOMPOSEQUATERNIONNODE
