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


#ifndef _VISTADFNKEYSTATENODE_H
#define _VISTADFNKEYSTATENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <map>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>

#include "VistaDfnKeyCallbackNode.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

class IVistaExplicitCallbackInterface;
class VistaKeyboardSystemControl;
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/**
 * A node that registers with the VistaKeyboardSystemControl and outputs the pressed
 * state of a registered key.
 *
 * The output thereby notes the pressed / release state.
 *
 * @ingroup VdfnNodes
 * @outport{value, bool, pressed or released}
 */
class VISTAKERNELAPI VistaDfnKeyStateNode : public VistaDfnKeyCallbackNode
{
public:
	VistaDfnKeyStateNode();
	~VistaDfnKeyStateNode();
	
	virtual void SetupKeyboardCallback(  VistaKeyboardSystemControl* pKeyboard
										, const int nKeyCode
										, const int nModCode
										, const std::string& sDescription
										, const bool bForce );
	
	bool PrepareEvaluationRun();
	bool GetIsValid() const;

	IVistaExplicitCallbackInterface* GetCallback() const;
protected:
	bool DoEvalNode();
	virtual unsigned int CalcUpdateNeededScore() const;

	virtual void Callback( const CounterCallback* pCallback );
private:	
	VistaDfnKeyCallbackNode::CounterCallback* m_pCallbackKeyUp;
	TVdfnPort<bool>* m_pKeyState;
	bool m_bKeyState;
	unsigned int m_uiUpdateScore;
};

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNKEYSTATENODE_H

