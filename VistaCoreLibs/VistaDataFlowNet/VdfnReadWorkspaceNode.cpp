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


#include "VdfnReadWorkspaceNode.h"

#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>
#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
VdfnReadWorkspaceNode::VdfnReadWorkspaceNode()
	: IVdfnNode(),
	  m_pMinX(new TVdfnPort<float>),
	  m_pMaxX(new TVdfnPort<float>),
	  m_pMinY(new TVdfnPort<float>),
	  m_pMaxY(new TVdfnPort<float>),
	  m_pMinZ(new TVdfnPort<float>),
	  m_pMaxZ(new TVdfnPort<float>),
      m_pWidth(new TVdfnPort<float>),
      m_pHeight(new TVdfnPort<float>),
      m_pDepth(new TVdfnPort<float>),
	  m_pBounds( new TVdfnPort<VistaBoundingBox>),
	  m_pWorkspace(NULL),
	  m_bDirty(false),
	  m_nCnt(0)
	{
		RegisterOutPort("min_x", m_pMinX);
		RegisterOutPort("max_x", m_pMaxX);
		RegisterOutPort("min_y", m_pMinY);
		RegisterOutPort("max_y", m_pMaxY);
		RegisterOutPort("min_z", m_pMinZ);
		RegisterOutPort("max_z", m_pMaxZ);
		RegisterOutPort("bounds", m_pBounds);
        RegisterOutPort("width", m_pWidth);
        RegisterOutPort("height", m_pHeight);
        RegisterOutPort("depth", m_pDepth);
	}

VdfnReadWorkspaceNode::~VdfnReadWorkspaceNode()
{
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

bool VdfnReadWorkspaceNode::GetIsValid() const
{
	return (m_pWorkspace != NULL) && !m_strWorkspaceToken.empty();
}

std::string VdfnReadWorkspaceNode::GetWorkspaceToken() const
{
	return m_strWorkspaceToken;
}

void VdfnReadWorkspaceNode::SetWorkspaceToken( const std::string & strWorkspaceToken)
{
	m_strWorkspaceToken = strWorkspaceToken;
    m_bDirty = true;
}

VistaDriverWorkspaceAspect *VdfnReadWorkspaceNode::GetWorkspaceAspect() const
{
	return m_pWorkspace;
}

void VdfnReadWorkspaceNode::SetWorkspaceAspect( VistaDriverWorkspaceAspect *pWS )
{
	m_pWorkspace = pWS;
    m_bDirty = true;
    if(!m_pWorkspace)
    {
        // clear fields
	    VistaBoundingBox bb;
	    m_pBounds->SetValue(bb, GetUpdateTimeStamp());
	    m_pMinX->SetValue(bb.m_v3Min[0], GetUpdateTimeStamp());
	    m_pMaxX->SetValue(bb.m_v3Max[0], GetUpdateTimeStamp());
	    m_pMinY->SetValue(bb.m_v3Min[1], GetUpdateTimeStamp());
	    m_pMaxY->SetValue(bb.m_v3Max[1], GetUpdateTimeStamp());
	    m_pMinZ->SetValue(bb.m_v3Min[2], GetUpdateTimeStamp());
	    m_pMaxZ->SetValue(bb.m_v3Max[2], GetUpdateTimeStamp());
        m_pWidth->SetValue(bb.m_v3Max[0] - bb.m_v3Min[0], GetUpdateTimeStamp() );
        m_pHeight->SetValue(bb.m_v3Max[1] - bb.m_v3Min[1], GetUpdateTimeStamp() );
        m_pDepth->SetValue(bb.m_v3Max[2] - bb.m_v3Min[2], GetUpdateTimeStamp() );
    }
}

bool VdfnReadWorkspaceNode::DoEvalNode()
{
	VistaBoundingBox bb;
	m_pWorkspace->GetWorkspace(m_strWorkspaceToken, bb);
	m_pBounds->SetValue(bb, GetUpdateTimeStamp());
	m_pMinX->SetValue(bb.m_v3Min[0], GetUpdateTimeStamp());
	m_pMaxX->SetValue(bb.m_v3Max[0], GetUpdateTimeStamp());
	m_pMinY->SetValue(bb.m_v3Min[1], GetUpdateTimeStamp());
	m_pMaxY->SetValue(bb.m_v3Max[1], GetUpdateTimeStamp());
	m_pMinZ->SetValue(bb.m_v3Min[2], GetUpdateTimeStamp());
	m_pMaxZ->SetValue(bb.m_v3Max[2], GetUpdateTimeStamp());
    m_pWidth->SetValue(bb.m_v3Max[0] - bb.m_v3Min[0], GetUpdateTimeStamp() );
    m_pHeight->SetValue(bb.m_v3Max[1] - bb.m_v3Min[1], GetUpdateTimeStamp() );
    m_pDepth->SetValue(bb.m_v3Max[2] - bb.m_v3Min[2], GetUpdateTimeStamp() );

    return true;
}

unsigned int VdfnReadWorkspaceNode::CalcUpdateNeededScore() const
{
    if(m_bDirty)
    {
        m_bDirty = false;
        return ++m_nCnt;
    }
    return m_nCnt;
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VdfnReadWorkspaceNodeCreate::VdfnReadWorkspaceNodeCreate( VistaDriverMap *pMap )
: m_pMap(pMap)
{

}

IVdfnNode *VdfnReadWorkspaceNodeCreate::CreateNode(const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strDriverId;
		std::string strWorkspaceToken;

		if( subs.GetValue( "device_id", strDriverId ) == false )
			return NULL;
		if( subs.GetValue( "workspace", strWorkspaceToken ) == false )
			return NULL;

		IVistaDeviceDriver *pDriver = m_pMap->GetDeviceDriver( strDriverId );
		if(!pDriver)
			return NULL;

		VistaDriverWorkspaceAspect *pWsA
		          = dynamic_cast<VistaDriverWorkspaceAspect*>(pDriver->GetAspectById(
				                 VistaDriverWorkspaceAspect::GetAspectId()));

		VdfnReadWorkspaceNode *pNode = new VdfnReadWorkspaceNode;
		pNode->SetWorkspaceToken(strWorkspaceToken);
		pNode->SetWorkspaceAspect(pWsA);
		return pNode;
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


