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


#ifndef _VISTASWITCHNODE_H
#define _VISTASWITCHNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include "VistaKernel/GraphicsManager/VistaGroupNode.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNodeBridge;
class IVistaNodeData;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaSwitchNode
 * per default no child (index == -1) is selected!
 *
 */
class VISTAKERNELAPI VistaSwitchNode : public VistaGroupNode
{
	friend class VistaSceneGraph;
	friend class IVistaNodeBridge;
public:
	/** Set child to be rendered
	  * @param int i : Index of child to be rendered. -1 means no child (default).
	  * @return bool: forwarding to toolkit node succeeded
	  */
	virtual bool	SetActiveChild(int i);
	/** Retrieve index of currently active child
	  * @return int : Index of currently rendered child
	*/
	virtual int		GetActiveChild() const;
	//virtual void	Debug(std::ostream &out, int nLevel=0);

	virtual ~VistaSwitchNode();
protected:
	VistaSwitchNode();
	VistaSwitchNode(	VistaGroupNode*				pParent,
						IVistaNodeBridge*				pBridge,
						IVistaNodeData*					pData,
						std::string						strName="");

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASWITCHNODE_H
