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


#include "VistaKernel/GraphicsManager/VistaLeafNode.h"
#include "VistaKernel/GraphicsManager/VistaNodeBridge.h"
#include "VistaKernel/GraphicsManager/VistaTransformNode.h"

#include <VistaBase/VistaStreamUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaLeafNode::VistaLeafNode()
{
	m_nType = VISTA_LEAFNODE;
}

VistaLeafNode::VistaLeafNode(	VistaGroupNode*				pParent, 
								IVistaNodeBridge*				pBridge,
								IVistaNodeData*					pData,
								const std::string	&strName) 
: VistaNode(pParent,pBridge,pData,strName)
{
	m_nType = VISTA_LEAFNODE;
}

VistaLeafNode::~VistaLeafNode()
{
	// IAR: is alread done by VistaNode
	//if(m_pParent)
	//    m_pParent->DisconnectChild(this);
}

void VistaLeafNode::Debug( std::ostream& oOut, int nLevel /*= 0 */ ) const
{
	VistaNode::Debug( oOut, nLevel );
	
	VistaVector3D v3Position;
	GetWorldPosition( v3Position );
	VistaVector3D v3Min, v3Max;
	GetWorldBoundingBox( v3Min, v3Max );
	oOut << vstr::indent;
	for(int j=0; j<nLevel; j++)
		oOut << "  ";
	oOut << "   Leaf Position: "
		<< v3Position << "\n";
	oOut << vstr::indent;
	for(int j=0; j<nLevel; j++)
		oOut << "  ";
	oOut << "   Leaf Global Bounds: ( "
		<< v3Min[0] << ", " << v3Min[1] << ", " << v3Min[2] << " ) - ( "
		<< v3Max[0] << ", " << v3Max[1] << ", " << v3Max[2] << " )" << std::endl;
}

// ============================================================================
// ============================================================================
