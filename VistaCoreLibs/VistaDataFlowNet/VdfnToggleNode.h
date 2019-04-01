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


#ifndef _VDFNTOGGLENODE_H
#define _VDFNTOGGLENODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnNode.h"
#include "VdfnConfig.h"
#include "VdfnPort.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVdfnPort;
class IVdfnPortTypeCompare;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * the toggle node holds an internal bool state, and toggles it depending on
 * the state of the toggle inport, depending on the mode
 * @ingroup VdfnNodes
 * @outport{toggle, bool, signal to toggle the internal state}
 * @outport{out, bool, intrnal state}
 */
class VISTADFNAPI VdfnToggleNode : public IVdfnNode
{
public:
	enum ToggleMode
	{
		TM_ON_ANY_UPDATE, // whenever a new value arives
		TM_ON_UPDATE_IF_TRUE, // whenever the inport is updated and the value is true
		TM_ON_UPDATE_IF_FALSE, // whenever the inport is updated and the value is false
		TM_ON_ANY_CHANGE, // whenever the value on the inport changes (to true or false)
		TM_ON_CHANGE_TO_TRUE, // whenever the value changes to true
		TM_ON_CHANGE_TO_FALSE, // whenever the value changes to false		
	};

	VdfnToggleNode( ToggleMode eMode, const bool bInitialState );
	~VdfnToggleNode();

	virtual bool PrepareEvaluationRun();

protected:
	bool DoEvalNode();
	bool CheckForInportChange();
	void Toggle();

	ToggleMode m_eToggleMode;
	TVdfnPort<bool>* m_pTogglePort;
	TVdfnPort<bool>* m_pOutPort;
	bool m_bLastValue;
	bool m_bState;

};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNTOGGLENODE_H

