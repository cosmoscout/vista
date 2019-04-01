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


#ifndef _VISTADFNMOUSEWHEELCONVERERNODE_H
#define _VISTADFNMOUSEWHEELCONVERERNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>


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
 * Node that is tailored to checking for a mouse wheel state change.
 *
 * Otherwise it is a bit tedious to read off the mouse wheel state of the history,
 * as the mouse history also changes when the mouse was moved, without a wheel change.
 * 
 * @ingroup VdfnNodes
 * @inport{wheel_state, int, mandatory, the wheel state value projected from the mouse history.}  
 * @outport{wheel_change, int, the difference of the wheel state since the last update.}
 */
class VISTAKERNELAPI VistaDfnMouseWheelChangeDetectNode : public IVdfnNode
{
public:
	VistaDfnMouseWheelChangeDetectNode();
	~VistaDfnMouseWheelChangeDetectNode();

	bool PrepareEvaluationRun();
protected:
	virtual bool DoEvalNode();
private:
	TVdfnPort<int>*				m_pWheelState;
	TVdfnPort<int>*				m_pWheelChange;

	int							m_iLastState;
};

class VISTAKERNELAPI VistaDfnMouseWheelChangeDetectNodeCreate
								: public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VistaDfnMouseWheelChangeDetectNodeCreate();

	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNMOUSEWHEELCONVERERNODE_H

