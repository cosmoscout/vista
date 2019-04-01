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


#ifndef _VDFNSHALLOWNODE_H
#define _VDFNSHALLOWNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnNode.h"
#include "VdfnConfig.h"
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
 * the shallow node is in itself meaningless. It does nothing in itself, it is
 * meant to be a base class for nodes that accept any types of inports and
 * creates outports according to a certain strategy.
 * It overloads the SetInPort() and SetOutPorts() method accepting all ports
 * as in and outports.
 * @ingroup VdfnNodes
 */
class VISTADFNAPI VdfnShallowNode : public IVdfnNode
{
public:
	VdfnShallowNode();
	~VdfnShallowNode();

	virtual bool SetInPort(const std::string &sName,
						   IVdfnPort *pPort );

	void SetOutPort( const std::string &sName, IVdfnPort *pPort );
protected:
	bool DoEvalNode() { return true; }
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNSHALLOWNODE_H

