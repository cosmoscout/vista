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


#ifndef _VDFNREADWORKSPACENODE_H
#define _VDFNREADWORKSPACENODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include <string>

#include "VdfnSerializer.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverMap;
class VistaDriverWorkspaceAspect;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a node for working with VistaDriverWorkspaceAspect.
 * This node only has outports, reflecting the current state of the workspace
 * aspect, which was given to the node from a special device.
 * @ingroup VdfnNodes
 * @outport{min_x,float,the min_x value of the workspace}
 * @outport{min_y,float,the min_y value of the workspace}
 * @outport{min_z,float,the min_z value of the workspace}
 * @outport{max_x,float,the max_x value of the workspace}
 * @outport{max_y,float,the max_y value of the workspace}
 * @outport{max_z,float,the max_z value of the workspace}
 * @outport{bounds,VistaBoundingBox,the AABB as VistaBoundingBox}
 * @outport{width,float,the width value of the workspace}
 * @outport{height,float,the height value of the workspace}
 * @outport{depth,float,the depth value of the workspace}
 *
 */
class VISTADFNAPI VdfnReadWorkspaceNode : public IVdfnNode
{
public:
	VdfnReadWorkspaceNode();
	~VdfnReadWorkspaceNode();

	/**
	 * @return true when a workspace token was set (non-empty) and
	           a workspace aspect was set.
	 */
	bool GetIsValid() const;

    // ############### WORKSPACE API ################

	/**
	 * @return the name of the workspace that was given to this node
	 */
	std::string GetWorkspaceToken() const;

	/**
	 * the workspace aspect can represent more than one workspace, so
	 * this API is used to define the workspace aspect to select.
	 * Setting the workspace token will cause the node to re-evaluate
	 * his outports.
	 * @param strWorkspaceToken the name of the workspace that
	          is represented by this node
	 */
	void SetWorkspaceToken( const std::string & strWorkspaceToken);

	/**
	 * @return the workspace aspect that was assigned to this node
	 */
	VistaDriverWorkspaceAspect *GetWorkspaceAspect() const;

	/**
	 * setting the workspace will cause the node to re-evaluate his
	 * outports. Setting NULL is ok.
	 * @param pWS the workspace aspect to represent
	 */
	void SetWorkspaceAspect( VistaDriverWorkspaceAspect *pWS );

protected:
	bool DoEvalNode();
	virtual unsigned int    CalcUpdateNeededScore() const;
private:
	TVdfnPort<float> *m_pMinX,
	                 *m_pMaxX,
	                 *m_pMinY,
	                 *m_pMaxY,
	                 *m_pMinZ,
	                 *m_pMaxZ,
                     *m_pWidth,
                     *m_pHeight,
                     *m_pDepth;
	TVdfnPort<VistaBoundingBox> *m_pBounds;

	VistaDriverWorkspaceAspect *m_pWorkspace;
	std::string m_strWorkspaceToken;


    mutable bool m_bDirty;
    mutable unsigned int m_nCnt;
};

/**
 * creates a VdfnReadWorkspaceNode
 */
class VISTADFNAPI VdfnReadWorkspaceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	/**
	 * @param pMap a pointer to the driver map to query for devices.
	          The pointer must outlive this creator.
	 */
	VdfnReadWorkspaceNodeCreate( VistaDriverMap *pMap );

	/**
	 * creates a VdfnReadWorkspaceNode. accepts
	 * - device_id: the <i>name</i> of the driver to look for in the driver map
	 * - workspace: the <i>name</i> of the workspace to assign to the node
	                (some drivers may offer more than one workspace)
	 * @return a proper node having its workspace set
	 */
	virtual IVdfnNode *CreateNode(const VistaPropertyList &oParams ) const;
private:
	VistaDriverMap *m_pMap;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNREADWORKSPACENODE_H

