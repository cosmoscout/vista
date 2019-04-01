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


#ifndef _VISTAEXTENSIONNODE_H
#define _VISTAEXTENSIONNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>


#include "VistaKernel/GraphicsManager/VistaLeafNode.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGroupNode;
class IVistaNodeBridge;
class IVistaNodeData;

class IVistaExplicitCallbackInterface;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaExtensionNode : public VistaLeafNode
{
	friend class VistaSceneGraph;
public:
	virtual ~VistaExtensionNode();

	/** Call drawing routine specified within current NodeExtension
	  * @return bool true/false
	*/
	virtual bool Update();
	virtual bool CanHaveChildren() const;

	virtual bool Init();

	VistaExtensionNode(	VistaGroupNode*				pParent,
							IVistaExplicitCallbackInterface* pExt,
							IVistaNodeBridge*				pBridge,
							IVistaNodeData*					pData,
							const std::string &name = "");


	IVistaExplicitCallbackInterface *GetExtension() const;
	virtual bool SetExtension(IVistaExplicitCallbackInterface *pExt);
protected:
	VistaExtensionNode();

	IVistaExplicitCallbackInterface* m_pExtension;

	bool m_bValid;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAEXTENSIONNODE_H


