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


#include "VistaSceneGraph.h"

#include "VistaNodeBridge.h"
#include "VistaGraphicsBridge.h"

//Load all node-definitions
#include "VistaNodeInterface.h"
#include "VistaNode.h"
#include "VistaGroupNode.h"
#include "VistaLeafNode.h"
#include "VistaGeomNode.h"
#include "VistaLightNode.h"
#include "VistaSwitchNode.h"
#include "VistaLODNode.h"
#include "VistaTextNode.h"
#include "VistaTransformNode.h"

#include "VistaGeometry.h"

#include "VistaOpenGLDraw.h"
#include "VistaOpenGLNode.h"
#include "Vista3DText.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaAspects/VistaPropertyList.h>

#include <cassert>
#include <stack>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaSceneGraph::VistaSceneGraph()
: m_pRoot( NULL )
, m_pModelRoot( NULL )
, m_pNodeBridge( NULL )
, m_pGraphicsBridge( NULL )
{
}

VistaSceneGraph::~VistaSceneGraph()
{
	// this will clear the SG
	delete m_pRoot;

	m_pRoot = NULL;
	m_pModelRoot = NULL;
}
// ============================================================================
// ============================================================================
bool VistaSceneGraph::Init(IVistaNodeBridge* pNodeBridge,
					IVistaGraphicsBridge* pGraphicsBridge)
{
	if(pNodeBridge && pGraphicsBridge)
	{
		m_pNodeBridge		= pNodeBridge;
		m_pGraphicsBridge	= pGraphicsBridge;

		//construct rootnode structure
		// m_pRoot = NewGroupNode(NULL);

		if(!m_pNodeBridge)
			return false;

		//New implementation specific representation of rootnode
		IVistaNodeData*		pNewData =	m_pNodeBridge->NewRootNodeData();

		if(!pNewData)
			return false;

		m_pRoot = new VistaTransformNode(NULL,m_pNodeBridge,pNewData);
		m_pRoot->SetName("ViSTA-root");

		//init base transform of rootnode
		//base transform is set via vista.ini and hidden from the user hereafter
		/// @todo make this a group node
		m_pModelRoot = NewTransformNode(m_pRoot);
		m_pModelRoot->SetName("ViSTA-model-root");
		//hide actual rootnode from user
		m_pModelRoot->m_pParent = NULL;
		return true;
	}
	return false;
}
// ============================================================================
// ============================================================================
VistaGroupNode* VistaSceneGraph::NewGroupNode(VistaGroupNode* pParent)
{
	if(!m_pNodeBridge)
		return NULL;

	VistaGroupNode*	pNewnode =	NULL;
	//New implementation specific representation of groupnode
	IVistaNodeData*		pNewData =	m_pNodeBridge->NewGroupNodeData();

	if(!pNewData)
		return NULL;

	pNewnode = new VistaGroupNode(pParent,m_pNodeBridge,pNewData);
	if(pNewnode && pParent)
		pParent->AddChild(pNewnode);

	return pNewnode;
}
// ============================================================================
// ============================================================================
VistaSwitchNode* VistaSceneGraph::NewSwitchNode(VistaGroupNode* pParent)
{
	if(!m_pNodeBridge)
		return NULL;

	VistaSwitchNode*	pNewnode =	NULL;
	//New implementation specific representation of switchnode
	IVistaNodeData*		pNewData =	m_pNodeBridge->NewSwitchNodeData();

	if(!pNewData)
		return NULL;

	pNewnode = new VistaSwitchNode(pParent,m_pNodeBridge,pNewData);
	if(pNewnode && pParent)
		pParent->AddChild(pNewnode);
	return pNewnode;
}
// ============================================================================
// ============================================================================
VistaLODNode* VistaSceneGraph::NewLODNode(VistaGroupNode* pParent)
{
	if(!m_pNodeBridge)
		return NULL;

	VistaLODNode*	pNewnode =	NULL;
	//New implementation specific representation of switchnode
	IVistaNodeData*		pNewData =	m_pNodeBridge->NewLODNodeData();

	if(!pNewData)
		return NULL;

	pNewnode = new VistaLODNode(pParent,m_pNodeBridge,pNewData);
	if(pNewnode && pParent)
		pParent->AddChild(pNewnode);
	return pNewnode;
}
// ============================================================================
// ============================================================================
VistaGeomNode* VistaSceneGraph::NewGeomNode(VistaGroupNode* pParent, VistaGeometry* pGeom)
{
	if(!m_pNodeBridge || !pGeom)
		return NULL;

	VistaGeomNode*	pNewNode = NULL;
	//New implementation specific representation of geomnode
	IVistaNodeData*	pNewData = m_pNodeBridge->NewGeomNodeData(pGeom->m_pData);

	if(!pNewData)
		return NULL;

	pNewNode = new VistaGeomNode(pParent, pGeom, m_pNodeBridge, pNewData);
	if(pNewNode && pParent)
		pParent->AddChild(pNewNode);
	return pNewNode;
}

VistaTransformNode *VistaSceneGraph::NewTransformNode(VistaGroupNode *pParent)
{
	if(!m_pNodeBridge)
		return NULL;

	VistaTransformNode*	pNewnode =	NULL;

	IVistaNodeData*		pNewData =	m_pNodeBridge->NewTransformNodeData();

	if(!pNewData)
		return NULL;

	pNewnode = m_pNodeBridge->NewTransformNode(pParent, pNewData, "");

	if(pNewnode && pParent)
		pParent->AddChild(pNewnode);
	return pNewnode;
}

// ============================================================================
// ============================================================================
/*
VistaLightNode* VistaSceneGraph::NewLightNode(VistaGroupNode* pParent, VISTA_LIGHTTYPE t)
{
	if(!m_pNodeBridge)
		return NULL;

	VistaLightNode*	pNewnode = NULL;
	//New implementation specific representation of lightnode
	IVistaNodeData*		pNewData = m_pNodeBridge->NewLightNodeData(t);

	if(!pNewData)
		return NULL;

	pNewnode = new VistaLightNode(pParent, t, m_pNodeBridge, pNewData);
	if(pNewnode && pParent)
		pParent->AddChild(pNewnode);
	return pNewnode;
}
*/

VistaAmbientLight*	VistaSceneGraph::NewAmbientLight (VistaGroupNode* pParent)
{
	if(!m_pNodeBridge)
		return NULL;

	VistaAmbientLight*	pNewnode = NULL;
	//New implementation specific representation of lightnode
	IVistaNodeData*		pNewData = m_pNodeBridge->NewLightNodeData(VISTA_AMBIENT_LIGHT);

	if(!pNewData)
		return NULL;

	pNewnode = m_pNodeBridge->NewAmbientLight(pParent, pNewData, "AmbientLight");
	if(!pNewnode)
	{
		delete pNewData;
		return NULL;
	}

	if(pParent)
		pParent->AddChild(pNewnode);

	m_pNodeBridge->RegisterLightNode( pNewnode );

	return pNewnode;
}

VistaDirectionalLight* VistaSceneGraph::NewDirectionalLight (VistaGroupNode* pParent)
{
	if(!m_pNodeBridge)
		return NULL;

	VistaDirectionalLight*	pNewnode = NULL;
	//New implementation specific representation of lightnode
	IVistaNodeData*		pNewData = m_pNodeBridge->NewLightNodeData(VISTA_DIRECTIONAL_LIGHT);

	if(!pNewData)
		return NULL;

	pNewnode = m_pNodeBridge->NewDirectionalLight(pParent, pNewData, "DirectionalLight");

	if(!pNewnode)
	{
		delete pNewData;
		return NULL;
	}

	if(pParent)
		pParent->AddChild(pNewnode);
	
	m_pNodeBridge->RegisterLightNode( pNewnode );

	return pNewnode;
}

VistaPointLight* VistaSceneGraph::NewPointLight (VistaGroupNode* pParent)
{
	if(!m_pNodeBridge)
		return NULL;

	VistaPointLight*	pNewnode = NULL;
	//New implementation specific representation of lightnode
	IVistaNodeData*		pNewData = m_pNodeBridge->NewLightNodeData(VISTA_POINT_LIGHT);

	if(!pNewData)
		return NULL;

	pNewnode = m_pNodeBridge->NewPointLight(pParent, pNewData, "PointLight");
	if(!pNewnode)
	{
		delete pNewData;
		return NULL;
	}

	if(pParent)
		pParent->AddChild(pNewnode);

	m_pNodeBridge->RegisterLightNode( pNewnode );

	return pNewnode;
}

VistaSpotLight* VistaSceneGraph::NewSpotLight (VistaGroupNode* pParent)
{
	if(!m_pNodeBridge)
		return NULL;

	VistaSpotLight*	pNewnode = NULL;
	//New implementation specific representation of lightnode
	IVistaNodeData*		pNewData = m_pNodeBridge->NewLightNodeData(VISTA_SPOT_LIGHT);

	if(!pNewData)
		return NULL;

	pNewnode = m_pNodeBridge->NewSpotLight(pParent, pNewData, "SpotLight");
	if(!pNewnode)
	{
		delete pNewData;
		return NULL;
	}

	if(pParent)
		pParent->AddChild(pNewnode);

	m_pNodeBridge->RegisterLightNode( pNewnode );

	return pNewnode;
}

// ============================================================================
// ============================================================================

VistaOpenGLNode* VistaSceneGraph::NewOpenGLNode(VistaGroupNode* pParent, IVistaOpenGLDraw* pDI)
{
	//New implementation specific representation of lightnode
	IVistaNodeData*		pNewData = m_pNodeBridge->NewOpenGLNodeData();

	if(!pNewData)
		return NULL;

	VistaOpenGLNode*pNewNode = new VistaOpenGLNode(pParent, pDI, m_pNodeBridge, pNewData);
	if(pNewNode->Init())
	{
		if(pParent)
			pParent->AddChild(pNewNode);
		return pNewNode;
	}
	else
	{
		delete pNewNode;
		delete pNewData;
	}

	return NULL;
}

VistaExtensionNode *VistaSceneGraph::NewExtensionNode(VistaGroupNode *pParent, IVistaExplicitCallbackInterface *pExt)
{
	IVistaNodeData*		pNewData = m_pNodeBridge->NewExtensionNodeData();

	if(!pNewData)
		return NULL;

	VistaExtensionNode*pNewNode = new VistaExtensionNode(pParent, pExt, m_pNodeBridge, pNewData);
	if(pNewNode->Init())
	{
		if(pParent)
			pParent->AddChild(pNewNode);
		return pNewNode;
	}
	else
	{
		delete pNewNode;
		delete pNewData;
	}

	return NULL;
}

// ============================================================================
// ============================================================================

VistaGeometry* VistaSceneGraph::NewIndexedGeometry(
		const vector<VistaIndexedVertex>& vertices,
		const vector<VistaVector3D>& coords,
		const vector<VistaVector3D>& textureCoords,
		const vector<VistaVector3D>& normals,
		const vector<VistaColor>& colors,
		const VistaVertexFormat& vFormat,
		const VistaGeometry::FaceType fType)
{
	//create empty geom data object
	IVistaGeometryData* pData = m_pGraphicsBridge->NewGeometryData();
	//create an empty geometry
	VistaGeometry* pNewGeom = new VistaGeometry(m_pGraphicsBridge, pData);
	//fill the geometry with content
	if(pData && pNewGeom && pNewGeom->CreateIndexedGeometry(vertices, coords, textureCoords,
										normals, colors, vFormat, fType))
		return pNewGeom;
	else
		delete pData;

	return NULL;
}

VistaGeometry* VistaSceneGraph::NewIndexedGeometry(
		const vector<VistaIndexedVertex>& vertices,
		const vector<float>& coords,
		const vector<float>& textureCoords,
		const vector<float>& normals,
		const vector<VistaColor>& colors,
		const VistaVertexFormat& vFormat,
		const VistaGeometry::FaceType fType)
{
	//create empty geom data object
	IVistaGeometryData* pData = m_pGraphicsBridge->NewGeometryData();
	//create an empty geometry
	VistaGeometry* pNewGeom = new VistaGeometry(m_pGraphicsBridge, pData);
	//fill the geometry with content
	if(pData && pNewGeom && pNewGeom->CreateIndexedGeometry(vertices, coords, textureCoords,
										normals, colors, vFormat, fType))
		return pNewGeom;
	else
		delete pData;

	return NULL;
}

// ============================================================================
// ============================================================================
bool VistaSceneGraph::DeleteLeaf(VistaLeafNode* pLeafNode)
{
	if(!pLeafNode)
		return false;

	VistaGroupNode* pParent = pLeafNode->GetParent();

	if(pParent)
	{
		 pParent->DisconnectChild(pLeafNode);
	}

	m_pNodeBridge->DeleteNode(pLeafNode);
	return true;
}
// ============================================================================
// ============================================================================
bool VistaSceneGraph::DeleteGroupNode(VistaGroupNode* pGroupNode, bool bMoveChildren)
{
	if((pGroupNode != NULL) && (pGroupNode != m_pModelRoot))
	{
		bool success = true;
		//1. determine if node has VistaParent. If so, detach it from parent
		VistaGroupNode* pParent = pGroupNode->GetParent();
		if(pParent != NULL)
		{
			if(bMoveChildren)
			{
				pParent->DisconnectChild(pGroupNode);
				//2. Move children of pGroupNode to pParent
				IVistaNode* pCurrChild = NULL;
				while(pGroupNode->HasChildren() && success)
				{
					pCurrChild = pGroupNode->DisconnectChild((unsigned int)(0));
					success &= pParent->AddChild(pCurrChild);
				}
			}
			//3. delete GroupNode
			m_pNodeBridge->DeleteNode(pGroupNode);
			return success;
		}

		m_pNodeBridge->DeleteNode(pGroupNode);

		return true;
	}
	return false;
}
// ============================================================================
// ============================================================================
bool VistaSceneGraph::DeleteSubTree(VistaGroupNode* pSubTreeRoot)
{
	if(!pSubTreeRoot)
		return false;
	std::stack<VistaGroupNode*> NodeStack;
	VistaGroupNode*	pCurrNode	= NULL;
	VistaGroupNode*	pTmpNode	= NULL;
	VistaNode*			pCurrChild	= NULL;

	NodeStack.push(pSubTreeRoot);
	while(!NodeStack.empty())
	{
		pCurrNode = NodeStack.top();
		NodeStack.pop();
		//if there are still children left -> enqueue them for destruction
		while(pCurrNode->HasChildren())
		{
			pCurrChild = dynamic_cast<VistaNode*>(pCurrNode->DisconnectChild((unsigned int)(0)));
			if(pCurrChild)
			{
				//VISTA_NODE can be deleted directly (rem.: VISTA_NODEs can be created during Rebuild-Process)
				if(pCurrChild->GetType() == VISTA_NODE)
					delete pCurrChild;
				else
				{
					//LeafNodes of any type can be deleted directly
					if(!pCurrChild->CanHaveChildren())
						DeleteLeaf(dynamic_cast<VistaLeafNode*>(pCurrChild));
					else
					{
						//GroupNodes need to be enqueued for later destruction
						pTmpNode = dynamic_cast<VistaGroupNode*>(pCurrChild);
						if(pTmpNode)
							NodeStack.push(pTmpNode);
						else return false;
					}
				}
			}
		}
		//after enqueueing all children for destruction -> Delete current node
		DeleteGroupNode(pCurrNode);
	}
	return true;
}
// ============================================================================
// ============================================================================
bool VistaSceneGraph::DeleteGeometry(VistaGeometry* pGeom)
{
	delete pGeom;
	return true;
}
// ============================================================================
// ============================================================================
bool VistaSceneGraph::Delete()
{
	return DeleteSubTree(m_pModelRoot);
}
// ============================================================================
// ============================================================================
void VistaSceneGraph::Debug(std::ostream& out) const
{
	VistaTransformMatrix matrix;
	m_pRoot->GetTransform(matrix);
	out << vstr::indent << "[ViSG] ----------------------------------------------------------------\n";
	out << vstr::indent << "[ViSG] Current ROOT_NODE transform is: \n";
	out << vstr::indent << matrix;
	out << vstr::indent << "\n[ViSG] Current SG layout is : \n";

	//TNodePair holds a node to be printed and a number indicating the depth in the SG
	typedef std::pair<IVistaNode*,int> TNodePair;
	std::stack<TNodePair> NodeStack;
	//start with ModelRoot -> real root is hidden from the user
	TNodePair AktPair(m_pModelRoot,1);
	TNodePair NewPair;
	NodeStack.push(AktPair);
	while(!NodeStack.empty())
	{
		AktPair = NodeStack.top();
		NodeStack.pop();
		if((AktPair.first)->CanHaveChildren())
		{
			VistaGroupNode* pGroup = dynamic_cast<VistaGroupNode*>(AktPair.first);
			int num = pGroup->GetNumChildren();
			for(int i=num-1; i>=0; i--)
			{
				NewPair.first  = pGroup->GetChild(i);
				NewPair.second = AktPair.second+1;
				NodeStack.push(NewPair);
			}
		}
		//Print out node data...
		(AktPair.first)->Debug(out, AktPair.second);
	}

	VistaVector3D min, max;

	m_pRoot->GetBoundingBox(min, max);
	out << vstr::indent << "Model bounding box\n"
		<< "[" << min
		<< "],[" << max << "]" << std::endl;

	std::vector<IVistaNode*> vecGeomNodes;
	GetAllSubNodesOfType( vecGeomNodes, VISTA_GEOMNODE );
	if( vecGeomNodes.empty() )
	{
		out << vstr::indent << "No GeomNodes" << std::endl;
	}
	else
	{
		int nNumVerts = 0;
		int nNumFaces = 0;
		for( std::vector<IVistaNode*>::const_iterator itGeom = vecGeomNodes.begin();
			itGeom != vecGeomNodes.end(); ++itGeom )
		{
			VistaGeomNode* pGeomNode = static_cast<VistaGeomNode*>( (*itGeom) );
			VistaGeometry* pGeom = pGeomNode->GetGeometry();
			nNumFaces += pGeom->GetNumberOfFaces();
			nNumVerts += pGeom->GetNumberOfVertices();
		}
		out << vstr::indent << "Total of " 
			<< std::setw( 4 ) << vecGeomNodes.size () << " GeomNodes with "
			<< std::setw( 10 ) << nNumVerts << " verts, "
			<< std::setw( 10 ) << nNumFaces << " faces"
			<< std::endl;
	}

	out << vstr::indent << " [ViSG] ----------------------------------------------------------------" << std::endl;
}
// ============================================================================
// ============================================================================
VistaGroupNode* VistaSceneGraph::GetRealRoot() const
{
	return m_pRoot;
}

VistaGroupNode* VistaSceneGraph::GetRoot() const
{
	return m_pModelRoot;
}
// ============================================================================
// ============================================================================
IVistaNode* VistaSceneGraph::GetNode(const std::string &strName,
							  IVistaNode *pSubRoot ) const
{
	//depth-first search through the tree
	IVistaNode* pFoundNode = NULL;
	IVistaNode* pCurrNode = NULL;
	VistaGroupNode* pFather = NULL;

	std::stack<IVistaNode*> NodeStack;

	IVistaNode *pSearchRoot = pSubRoot ? pSubRoot : m_pModelRoot;

	NodeStack.push(pSearchRoot);
	do
	{
		pCurrNode = NodeStack.top();
		NodeStack.pop();

		if(pCurrNode->GetName() == strName)
			pFoundNode = pCurrNode;

		if(pCurrNode->CanHaveChildren())
		{
			//push all children of current node onto the stack
			pFather = static_cast<VistaGroupNode*>(pCurrNode);
			int num = pFather->GetNumChildren();
			int i;
			for(i=0; i<num; i++)
				NodeStack.push(pFather->GetChild(i));
		}
	}
	while ( (pFoundNode==NULL) && (!(NodeStack.empty())));
	return pFoundNode;
}
// ============================================================================
// ============================================================================
VistaGroupNode* VistaSceneGraph::LoadSubTree(const std::string &strFileName,
									   VistaSceneGraph::eOptFlags eOpts,
									   float fScale,
									   bool bVerbose )
{
	IVistaNode* pLoadedNode = m_pNodeBridge->LoadNode( strFileName,
										fScale,
										VistaSceneGraph::eOptFlags(eOpts),
										bVerbose);
	if( pLoadedNode && (pLoadedNode->GetType() == VISTA_GROUPNODE || pLoadedNode->GetType() == VISTA_TRANSFORMNODE ))
	{
		return static_cast<VistaGroupNode*>(pLoadedNode);
	}	
	else
	{
		delete pLoadedNode;
		return NULL;
	}
}
// ============================================================================
// ============================================================================
bool VistaSceneGraph::SaveSubTree(std::string strFileName, IVistaNode* pNode)
{
	if(!pNode)
		return false;
	return m_pNodeBridge->SaveNode(strFileName, pNode);
}
// ============================================================================
// ============================================================================
VistaGeomNode* VistaSceneGraph::LoadGeomNode( const std::string &strFileName,
										VistaSceneGraph::eOptFlags eOpts,
										float fScale,
										bool bVerbose )
{
	IVistaNode* pLoadedNode = m_pNodeBridge->LoadNode( strFileName,
														fScale,
														VistaSceneGraph::eOptFlags(eOpts),
														bVerbose );
	if(pLoadedNode)
	{
		// is geometry node?
		if( pLoadedNode->GetType() == VISTA_GEOMNODE )
		{
			return static_cast<VistaGeomNode*>(pLoadedNode);
		}
		// try to skip a prefixed group node and look if we can find the geo node beneath...
		else if(pLoadedNode->CanHaveChildren())
		{
			VistaGroupNode *pNode = dynamic_cast<VistaGroupNode*>(pLoadedNode);
			if(pNode->GetNumChildren() == 1)
			{
				if( pNode->GetChild(0)->GetType() == VISTA_GEOMNODE )
				{
					VistaGeomNode *pGeom = dynamic_cast<VistaGeomNode*>(pNode->GetChild(0));

					pNode->DisconnectChild((unsigned int)0);
					DeleteGroupNode( pNode );
					return pGeom;
				}
			}
		}
		// no geometry node found...delete the loaded stuff
		m_pNodeBridge->DeleteNode(pLoadedNode);
	}

	return NULL;
}

IVistaNode * VistaSceneGraph::LoadNode( const std::string &sFileName,
								VistaSceneGraph::eOptFlags eOpts,
								float fScale,
								bool bVerbose )
{
	return m_pNodeBridge->LoadNode( sFileName , fScale, VistaSceneGraph::eOptFlags(eOpts), bVerbose );
}

bool VistaSceneGraph::ApplyOptimizationToNode( IVistaNode *pNode,
										eOptFlags eFlags,
										bool bVerbose )
{
	return m_pNodeBridge->ApplyOptimizationToNode( pNode, VistaSceneGraph::eOptFlags(eFlags), bVerbose );
}

int VistaSceneGraph::GetNumberOfMaterials(void)
{
	return m_pGraphicsBridge->GetNumberOfMaterials();
}

int VistaSceneGraph::AddMaterial(const VistaMaterial &Material)
{
	return m_pGraphicsBridge->AddMaterial(Material);
}

bool VistaSceneGraph::GetMaterialByIndex(int iIndex, VistaMaterial &oIn) const
{
	return m_pGraphicsBridge->GetMaterialByIndex(iIndex, oIn);
}

bool VistaSceneGraph::GetMaterialByName(const std::string &sName, VistaMaterial &oIn) const
{
	return m_pGraphicsBridge->GetMaterialByName(sName, oIn);
}

VistaTextNode *VistaSceneGraph::NewTextNode(VistaGroupNode *pParent, const std::string &sFontDesc, IVista3DText *pText)
{
	if(!pText)
	{
		pText = m_pNodeBridge->CreateFontRepresentation(sFontDesc);
		if(!pText)
			return NULL;
	}
	else
		pText->SetFontName(sFontDesc);

	IVistaNodeData*		pNewData = m_pNodeBridge->NewTextNodeData(pText);

	if(!pNewData)
	{
		m_pNodeBridge->DestroyFontRepresentation(pText);
		return NULL;
	}

	VistaTextNode *pNode = new VistaTextNode(pParent, pText, m_pNodeBridge, pNewData);

	if(pParent)
		pParent->AddChild(pNode);

	return pNode;
}

IVistaNode* VistaSceneGraph::CloneSubtree( IVistaNode* pNode,
									VistaGroupNode* pNewParent )
{
	//IVistaNode* pNewNode = m_pNodeBridge->CloneSubtree( static_cast<VistaNode*>( pNode )->GetData() );
	//if( pNewNode && pNewParent )
	//	pNewParent->AddChild( pNewNode );
	//if( pNewNode )
	//{
	//	std::string sName = pNode->GetName();
	//	if( sName.empty() == false )
	//	{
	//		pNewNode->SetName( sName + "_clone" );
	//	}
	//}
	//return pNewNode;

	IVistaNode *pReturnNode = NULL;

	switch( pNode->GetType() )
	{
		case VISTA_GROUPNODE:
		{
			VistaGroupNode *pOrigNode = dynamic_cast<VistaGroupNode*>( pNode );
			assert( pOrigNode != NULL );
			VistaGroupNode *pNewNode = NewGroupNode( pNewParent );

			// clone children and append them
			for(unsigned int n=0; n < pOrigNode->GetNumChildren(); ++n)
			{
				CloneSubtree( pOrigNode->GetChild(n), pNewNode );
			}

			pReturnNode = pNewNode;
			pReturnNode->SetName( pOrigNode->GetName() );
			pReturnNode->SetIsEnabled( pOrigNode->GetIsEnabled() );
			break;
		}
		case VISTA_SWITCHNODE:
		{
			VistaSwitchNode *pOrigNode = dynamic_cast<VistaSwitchNode*>( pNode );
			VistaSwitchNode *pNewNode = NewSwitchNode( pNewParent );
			for( unsigned int n=0; n < pOrigNode->GetNumChildren(); ++n)
			{
				IVistaNode *pChildNode = pOrigNode->GetChild( (unsigned int)n );
				CloneSubtree( pChildNode, pNewNode );
			}

			pNewNode->SetActiveChild( pOrigNode->GetActiveChild() );

			pReturnNode = pNewNode;
			pReturnNode->SetName( pOrigNode->GetName() );
			break;
		}
		case VISTA_LEAFNODE:
		{
			vstr::warnp() << "[VistaSceneGraph::CloneSubtree]: found node of type LEAFNODE!"
					<< " Cannot handle this node - omitting:" << std::endl;
			break;
		}
		case VISTA_LIGHTNODE:
		case VISTA_AMBIENTLIGHTNODE:
		case VISTA_DIRECTIONALLIGHTNODE:
		case VISTA_POINTLIGHTNODE:
		case VISTA_SPOTLIGHTNODE:
		{
			VistaLightNode *pOrigNode = dynamic_cast<VistaLightNode*>( pNode );
			assert( pOrigNode != NULL );
			VistaLightNode *pNewNode;
			switch( pOrigNode->GetLightType() )
			{
	
				case VISTA_AMBIENT_LIGHT:
				{
					pNewNode = NewAmbientLight( pNewParent );
					break;
				}
				case VISTA_DIRECTIONAL_LIGHT:
				{
					pNewNode = NewDirectionalLight( pNewParent );
					break;
				}
				case VISTA_POINT_LIGHT:
				{
					pNewNode = NewPointLight( pNewParent );
					break;
				}
				case VISTA_SPOT_LIGHT:
				{
					pNewNode = NewSpotLight( pNewParent );
					break;
				}
				case VISTA_LIGHTTYPE_NONE:
				default:
				{
					vstr::errp() << "[VistaSceneGraph::CloneSubtree] Found light with unknown type!" << std::endl
								<< " Cannot handle this node - omitting." << std::endl;
					return NULL;
					break;
				}
			}
	
			float fR, fG, fB;

			pOrigNode->GetAmbientColor( fR, fG, fB );
			pNewNode->SetAmbientColor( fR, fG, fB );

			pOrigNode->GetDiffuseColor( fR, fG, fB );
			pNewNode->SetDiffuseColor( fR, fG, fB );

			pOrigNode->GetSpecularColor( fR, fG, fB );
			pNewNode->SetSpecularColor( fR, fG, fB );

			pNewNode->SetIntensity( pOrigNode->GetIntensity() );
			pNewNode->SetAttenuation( pOrigNode->GetAttenuation() );

			pReturnNode = pNewNode;
			pReturnNode->SetName( pOrigNode->GetName() );
			pReturnNode->SetIsEnabled( pOrigNode->GetIsEnabled() );
			break;
		}
		case VISTA_GEOMNODE:
		{
			VistaGeomNode *pOrigNode = dynamic_cast<VistaGeomNode*>( pNode );
			assert( pOrigNode );
			VistaGeomNode *pNewNode = NewGeomNode( pNewParent, pOrigNode->GetGeometry() );

			pReturnNode = pNewNode;
			pReturnNode->SetName( pNode->GetName() );
			pReturnNode->SetIsEnabled( pOrigNode->GetIsEnabled() );
			break;
		}
		case VISTA_EXTENSIONNODE:
		{
			VistaExtensionNode *pOrigNode = dynamic_cast<VistaExtensionNode*>( pNode );
			assert( pOrigNode );
			VistaExtensionNode *pNewNode = NewExtensionNode( pNewParent, pOrigNode->GetExtension() );

			pReturnNode = pNewNode;
			pReturnNode->SetName( pOrigNode->GetName() );
			pReturnNode->SetIsEnabled( pOrigNode->GetIsEnabled() );
			break;
		}
		case VISTA_OPENGLNODE:
		{
			VistaOpenGLNode *pOrigNode = dynamic_cast<VistaOpenGLNode*>( pNode );
			assert( pOrigNode );
			IVistaOpenGLDraw* pDrawCB = dynamic_cast<IVistaOpenGLDraw*>( pOrigNode->GetExtension() );
			assert( pDrawCB != NULL );
			VistaOpenGLNode *pNewNode = NewOpenGLNode( pNewParent, pDrawCB );

			pReturnNode = pNewNode;
			pReturnNode->SetName( pOrigNode->GetName() );
			pReturnNode->SetIsEnabled( pOrigNode->GetIsEnabled() );
			break;
		}
		case VISTA_LODNODE:
		{
			VistaLODNode *pOrigNode = dynamic_cast<VistaLODNode*>( pNode );
			assert( pOrigNode );
			VistaLODNode *pNewNode = NewLODNode( pNewParent );

			for( unsigned int i=0; i < pNewNode->GetNumChildren(); ++i )
			{
				IVistaNode *pChildNode = pNewNode->GetChild( (unsigned int)i );
				CloneSubtree( pChildNode, pNewNode );
			}

			std::vector<float> vecRanges;
			pOrigNode->GetRange( vecRanges );
			pNewNode->SetRange( vecRanges );

			VistaVector3D v3Center;
			pOrigNode->GetCenter( v3Center );
			pNewNode->SetCenter( v3Center );

			pReturnNode = pNewNode;
			pReturnNode->SetName( pOrigNode->GetName() );
			pReturnNode->SetIsEnabled( pOrigNode->GetIsEnabled() );
			break;
		}
		case VISTA_TRANSFORMNODE:
		{
			VistaTransformNode *pOrigNode = dynamic_cast<VistaTransformNode*>( pNode );
			assert( pOrigNode != NULL );
			VistaTransformNode *pNewNode = NewTransformNode( pNewParent );

			// copy transform
			VistaTransformMatrix m;
			pOrigNode->GetTransform( m );
			pNewNode->SetTransform( m );

			// clone children and append them
			for(unsigned int n=0; n < pOrigNode->GetNumChildren(); ++n)
			{
				CloneSubtree( pOrigNode->GetChild(n), pNewNode );
			}

			pReturnNode = pNewNode;
			pReturnNode->SetName( pOrigNode->GetName() );
			pReturnNode->SetIsEnabled( pOrigNode->GetIsEnabled() );
			break;
		}
		case VISTA_TEXTNODE:
		{
			VistaTextNode *pOrigNode = dynamic_cast<VistaTextNode*>( pNode );
			assert( pOrigNode != NULL );
			VistaTextNode *pNewNode = NewTextNode( pNewParent,
													pOrigNode->GetTextImp()->GetFontName() );

			IVista3DText* pOrigText = pOrigNode->GetTextImp();
			IVista3DText* pNewText = pNewNode->GetTextImp();

			pNewText->SetFontColor( pOrigText->GetFontColor() );
			pNewText->SetFontDepth( pOrigText->GetFontDepth() );
			//pNewText->SetFontName( pOrigText->GetFontName() );
			pNewText->SetFontSize( pOrigText->GetFontSize() );
			pNewText->SetText( pOrigText->GetText() );
	
			pReturnNode = pNewNode;
			pReturnNode->SetName( pOrigNode->GetName() );
			pReturnNode->SetIsEnabled( pOrigNode->GetIsEnabled() );
			break;
		}
		default:
		{
			vstr::errp() << "[VistaSceneGraph::CloneSubtree] Found unknown scenegraph node!" << std::endl;
			break;
		}
	}

	return pReturnNode;
}

VistaLightNode* VistaSceneGraph::NewLightFromProplist( const VistaPropertyList& oProps,
											  VistaGroupNode* pParent )
{
	VistaColor colAmbient = oProps.GetValueOrDefault<VistaColor>( "AMBIENTCOLOR", VistaColor::BLACK );
	VistaColor colDiffuse = oProps.GetValueOrDefault<VistaColor>( "DIFFUSECOLOR", VistaColor::BLACK );
	VistaColor colSpecular = oProps.GetValueOrDefault<VistaColor>( "SPECULARCOLOR", VistaColor::BLACK );
	VistaVector3D v3Direction = oProps.GetValueOrDefault<VistaVector3D>(
										"DIRECTION", VistaVector3D( 0, 0, -1 ) );
	VistaVector3D v3Position = oProps.GetValueOrDefault<VistaVector3D>(
										"POSITION", VistaVector3D( 0, 0, 0 ) );
	VistaVector3D v3Attenuation = oProps.GetValueOrDefault<VistaVector3D>(
										"ATTENUATION", VistaVector3D( 1, 0, 0 ) );
	float fIntensity = oProps.GetValueOrDefault<float>( "INTENSITY", 1.0f );

	std::string sType;
	oProps.GetValue( "TYPE", sType );

	VistaLightNode* pLight = NULL;
	VistaAspectsComparisonStuff::StringCompareObject oCompare( false );
	if( oCompare( sType, "DIRECTIONAL" ) )
	{
		VistaDirectionalLight* pDirLight = NewDirectionalLight( pParent );
		pDirLight->SetDirection( v3Direction );
		pLight = pDirLight;
	}
	else if( oCompare( sType, "POINT" ) )
	{
		VistaPointLight* pPointLight = NewPointLight( pParent );
		pPointLight->SetPosition( v3Position );
		pLight = pPointLight;
	}
	else if( oCompare( sType, "SPOT" ) )
	{
		VistaSpotLight* pSpotLight = NewSpotLight( pParent );
		pSpotLight->SetDirection( v3Direction );
		pSpotLight->SetPosition( v3Position );

		float fSpotCharacter = oProps.GetValueOrDefault<float>( "CHARACTER", 180.0 );
		pSpotLight->SetSpotCharacter( fSpotCharacter );

		int iSpotDistribution = oProps.GetValueOrDefault<int>( "DISTRIBUTION", 1 );
		pSpotLight->SetSpotDistribution( iSpotDistribution );

		pLight = pSpotLight;
	}
	else
	{
		vstr::warnp() << "VistaSceneGraph::NewLightFromProplist() -- Type ["
					<< sType << "] unknown/unsupported" << std::endl;
		return NULL;
	}

	pLight->SetAmbientColor( colAmbient );
	pLight->SetDiffuseColor( colDiffuse );
	pLight->SetSpecularColor( colSpecular );
	pLight->SetIntensity( fIntensity );
	pLight->SetAttenuation( v3Attenuation );

	return pLight;
}

void VistaSceneGraph::GetAllSubNodesOfType( std::vector<IVistaNode*>& vecNodesOfWantedType, const VISTA_NODETYPE& eWantedNodeType, IVistaNode *pNode)
{
	assert( pNode != NULL );

	std::stack<IVistaNode*> NodeStack;
	IVistaNode*	pCurrNode	= NULL;
	VistaGroupNode*	pCurrNodeAsGroupNode	= NULL;

	NodeStack.push(pNode);
	while(!NodeStack.empty())
	{
		pCurrNode = NodeStack.top();
		NodeStack.pop();
		//if it has the wanted type, add to results
		if(pCurrNode->GetType() == eWantedNodeType) vecNodesOfWantedType.push_back(pCurrNode);
		//check if there are further kids and enqueue them for further search
		pCurrNodeAsGroupNode = dynamic_cast<VistaGroupNode*>(pCurrNode);
		if(pCurrNodeAsGroupNode!=NULL)
		{
			for (int childIndex = 0; childIndex < (int)pCurrNodeAsGroupNode->GetNumChildren(); ++childIndex)
			{
				NodeStack.push(pCurrNodeAsGroupNode->GetChild(childIndex));
			}
		}
	}
}

void VistaSceneGraph::GetAllSubNodesOfType( std::vector<IVistaNode*>& vecNodesOfWantedType, const VISTA_NODETYPE& eWantedNodeType ) const
{
	return GetAllSubNodesOfType( vecNodesOfWantedType, eWantedNodeType, GetRoot() );
}

const std::vector<VistaLightNode*>& VistaSceneGraph::GetAllLightNodes() const
{
	return m_pNodeBridge->GetAllLightNodes();
}

void VistaSceneGraph::GetAllLightNodes( std::vector<VistaLightNode*>& vecLights ) const
{
	vecLights = m_pNodeBridge->GetAllLightNodes();
}

VistaLightNode* VistaSceneGraph::GetLightNode( const int nIndex ) const
{
	return m_pNodeBridge->GetAllLightNodes()[nIndex];
}

int VistaSceneGraph::GetNumberOfLights() const
{
	return (int)m_pNodeBridge->GetAllLightNodes().size();
}

int VistaSceneGraph::GetNumberOfActiveLights() const
{
	const std::vector<VistaLightNode*>& vecLights = m_pNodeBridge->GetAllLightNodes();
	int nCount = 0;
	for( std::vector<VistaLightNode*>::const_iterator itLight = vecLights.begin();
			itLight != vecLights.end(); ++itLight )
	{
		if( (*itLight)->GetIsEnabled() )
			++nCount;
	}
	return nCount;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   operator<<                                                  */
/*                                                                            */
/*============================================================================*/
ostream & operator<< ( ostream & out, const VistaSceneGraph & device )
{
	device.Debug ( out );
	return out;
}