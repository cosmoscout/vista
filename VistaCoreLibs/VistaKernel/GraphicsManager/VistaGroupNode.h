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


#ifndef _VISTAGROUPNODE_H
#define _VISTAGROUPNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/GraphicsManager/VistaNode.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaNodeData;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaGroupNode : public VistaNode
{
	friend class VistaSceneGraph;
	friend class IVistaNodeBridge;
public:
	//**additional SG-navigation routines
	/** Retrieve numbered child
	  * @param int nChildIndex : specifies number of child (in [0..NumChildren-1])
	  * @return IVistaNode* : Pointer to requested child / NULL if not existant
	*/
	virtual IVistaNode* GetChild(unsigned int nChildindex) const;
	/** Retrieve child named strName
	  * @param string strName : name of requested child
	  * @ return IVistaNode* : pointer to requested child if found / else NULL
	*/
	virtual IVistaNode* GetChild(const std::string &strName) const;
	/** Check if number of children is greater than zero
	  * @return bool true/false
	*/
	virtual bool		HasChildren() const;

	virtual bool        GetIsChild( const IVistaNode * ) const;

	/** Retrieve number of children currently attached to this node
	  * @return int : Number of children
	*/
	virtual unsigned int			GetNumChildren() const;

	/** Test whether the node pNode can be added to this group node without creating a circle
	  * @param IVistaNode* pNode : Node to be attached
	  * @return bool true/false
	*/
	virtual bool			CanAddChild(IVistaNode* pNode) const;
	/** Add cild pChild to group node. CanAddChild(..) is performed implicitly
	  * @param IVistaNode* pChild : Node to be attached
	  * @return bool true/false
	*/
	virtual bool			AddChild(IVistaNode* pChild);
	/** Insert child pChild after index
	  * @param IVistaNode pChild : Node to be inserted
	  * @param int index : Index of child after which pChild will be inserted
	  * @return bool true/false
	*/
	virtual bool			InsertChild(IVistaNode* pChild, unsigned int index);
	/** Disconnect child from group node
	  * @param int nChildIndex : Index of child to be disconnected (from [0..NumChildren-1])
	  * @return IVistaNode* : Pointer to disconnected child if sucessful / else NULL
	*/
	virtual IVistaNode*		DisconnectChild(unsigned int nChildindex);
	/** Disconnect child from group node
	  * @param string strName : Name of child to be disconnected
	  * @return IVistaNode* : Pointer to disconnected child if sucessful / else NULL
	*/
	virtual IVistaNode*		DisconnectChild(const std::string &strName);
	/** Disconnect child from group node
	  * @param IVistaNode* pChild : pointer to child to be disconnected
	  * @return int : Index of disconnected child if successful / else -1
	*/
	virtual int				DisconnectChild(IVistaNode* pChild);
	//virtual void			Debug(std::ostream &out, int nLevel=0);
	virtual bool CanHaveChildren() const;

	virtual ~VistaGroupNode();
protected:
	VistaGroupNode();
	VistaGroupNode(	VistaGroupNode*			pParent,
						IVistaNodeBridge*			pBridge,
						IVistaNodeData*				pData,
						std::string					strName = "");


	VistaNodeVector	m_vecChildren;
	//int m_nNumChildren;

	bool IsValidIndex(unsigned int iIndex) const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAGROUPNODE_H

