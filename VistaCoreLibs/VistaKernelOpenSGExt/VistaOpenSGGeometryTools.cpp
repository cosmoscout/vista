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


#if defined(WIN32)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4312)
#pragma warning(disable: 4267)
#pragma warning(disable: 4275)
#endif

#include <GL/glew.h>

#include "VistaOpenSGGeometryTools.h"

#include <VistaBase/VistaVersion.h>

#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGDisplayBridge.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGGeoFunctions.h>
#include <OpenSG/OSGSHLChunk.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOpenSGGeometryTools::GeometryGrabber::GeometryGrabber()
{
}

VistaOpenSGGeometryTools::GeometryGrabber::~GeometryGrabber()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
void VistaOpenSGGeometryTools::GeometryGrabber::Traverse(OSG::NodePtr node)
{
	m_lstAllGeoNodes.clear();
	m_setAllGeos.clear();

	OSG::traverse(node,
		OSG::osgTypedMethodFunctor1ObjPtrCPtrRef<
			OSG::Action::ResultE,
			GeometryGrabber,
			OSG::NodePtr>
			(this, &GeometryGrabber::Enter),
		OSG::osgTypedMethodFunctor2ObjPtrCPtrRef<
			OSG::Action::ResultE,
			GeometryGrabber,
			OSG::NodePtr,
			OSG::Action::ResultE>
			(this, &GeometryGrabber::Leave));
}


const std::list<OSG::NodePtr>* VistaOpenSGGeometryTools::GeometryGrabber::GetNodes() const
{
	return &m_lstAllGeoNodes;
}

const std::set<OSG::GeometryPtr>* VistaOpenSGGeometryTools::GeometryGrabber::GetGeometries() const
{
	return &m_setAllGeos;
}

OSG::Action::ResultE VistaOpenSGGeometryTools::GeometryGrabber::Enter(OSG::NodePtr& node)
{
	if(node->getCore()->getType().isDerivedFrom(OSG::Geometry::getClassType()))
	{
		m_lstAllGeoNodes.push_back(node);
		OSG::GeometryPtr geo = OSG::GeometryPtr::dcast(node->getCore());
		m_setAllGeos.insert(geo);
	}
	return OSG::Action::Continue;
}

OSG::Action::ResultE VistaOpenSGGeometryTools::GeometryGrabber::Leave(OSG::NodePtr& node, OSG::Action::ResultE res)
{
	return res;
}

static OSG::NodePtr GetOpenSGNode(IVistaNode *pNode)
{
	VistaNode *pVistaNode = dynamic_cast<VistaNode*>(pNode);
	if(!pVistaNode) return OSG::NullFC;

	VistaOpenSGNodeData *pData
		= dynamic_cast<VistaOpenSGNodeData*>(pVistaNode->GetData());

	if(!pData) return OSG::NullFC;

	return pData->GetNode();
}

bool VistaOpenSGGeometryTools::CalcVertexNormalsOnSubtree(IVistaNode *pNode, const float &fCreaseAngle)
{
	OSG::NodePtr node = GetOpenSGNode(pNode);
	if(!node)
		return false;

	GeometryGrabber grabber;
	grabber.Traverse(node);

	const GeometryGrabber::GeoSet* geos = grabber.GetGeometries();
	GeometryGrabber::GeoSet::const_iterator it;
	for(it = geos->begin(); it != geos->end(); ++it)
	{
		OSG::calcVertexNormals(*it, fCreaseAngle);
	}

	return true;
}

bool VistaOpenSGGeometryTools::CalcFaceNormalsOnSubtree  (IVistaNode *pNode)
{
	OSG::NodePtr node = GetOpenSGNode(pNode);
	if(!node)
		return false;

	GeometryGrabber grabber;
	grabber.Traverse(node);

	const GeometryGrabber::GeoSet* geos = grabber.GetGeometries();
	GeometryGrabber::GeoSet::const_iterator it;
	for(it = geos->begin(); it != geos->end(); ++it)
	{
		OSG::calcFaceNormals(*it);
	}

	return true;
}

bool VistaOpenSGGeometryTools::CalcVertexNormals(VistaGeometry *pGeo, const float &fCreaseAngle)
{
	VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(pGeo->GetData());
	OSG::calcVertexNormals(pData->GetGeometry(), fCreaseAngle);

	return true;
}


bool VistaOpenSGGeometryTools::CalcFaceNormals(VistaGeometry *pGeo)
{
	VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(pGeo->GetData());
	OSG::calcFaceNormals(pData->GetGeometry());

	return true;
}

bool VistaOpenSGGeometryTools::CalcVertexNormalsGeo(VistaGeometry *pGeo, const float &fCreaseAngle, bool pCalc)
{
	VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(pGeo->GetData());
	if(pCalc)
		OSG::calcVertexNormals(pData->GetGeometry());
	OSG::NodePtr p = OSG::calcVertexNormalsGeo(pData->GetGeometry(),0.1f);
	osg::NodePtr parent = pData->GetGeometry()->getParents()[0];
	beginEditCP(parent);
		parent->addChild(p);
	endEditCP(parent);
	
	return true;
}

bool VistaOpenSGGeometryTools::CalcFaceNormalsGeo(VistaGeometry *pGeo, bool pCalc)
{
	VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>(pGeo->GetData());
	if(pCalc)
		OSG::calcFaceNormals(pData->GetGeometry());
	OSG::NodePtr p = OSG::calcFaceNormalsGeo(pData->GetGeometry(),1);
	osg::NodePtr parent = pData->GetGeometry()->getParents()[0];
	beginEditCP(parent);
		parent->addChild(p);
	endEditCP(parent);
	
	return true;
}

bool VistaOpenSGGeometryTools::SetUseVBOForGeometry( VistaGeometry *pGeo, const bool bSet )
{
	VistaOpenSGGeometryData* pData = dynamic_cast<VistaOpenSGGeometryData*>( pGeo->GetData() );
	assert( pData );
	osg::GeometryPtr pOsgGeom = pData->GetGeometry();
	pOsgGeom->setVbo( bSet );

	return true;
}

bool VistaOpenSGGeometryTools::SetUseVBOOnSubtree( IVistaNode* pNode, const bool bSet )
{
	OSG::NodePtr node = GetOpenSGNode(pNode);
	if(!node)
		return false;

	GeometryGrabber grabber;
	grabber.Traverse(node);

	const GeometryGrabber::GeoSet* geos = grabber.GetGeometries();
	GeometryGrabber::GeoSet::const_iterator it;
	for(it = geos->begin(); it != geos->end(); ++it)
	{
		(*it)->setVbo( bSet );
	}
	return true;
}

VISTAKERNELOPENSGEXTAPI
bool VistaOpenSGGeometryTools::PreloadRenderData( IVistaNode* pNode, VistaDisplayManager* pDispManager )
{
	OSG::NodePtr node = GetOpenSGNode(pNode);
	GeometryGrabber grabber;
	grabber.Traverse(node);

	bool bSuccess = true;

	const std::map<std::string, VistaWindow*>& mapWindwos = pDispManager->GetWindowsConstRef();
	for( std::map<std::string, VistaWindow*>::const_iterator itWin = mapWindwos.begin();
			itWin != mapWindwos.end(); ++itWin )
	{
		VistaOpenSGDisplayBridge::WindowData* pData = dynamic_cast<VistaOpenSGDisplayBridge::WindowData*>(
															(*itWin).second->GetData() );
		const GeometryGrabber::GeoSet* geos = grabber.GetGeometries();
		GeometryGrabber::GeoSet::const_iterator it;
		for(it = geos->begin(); it != geos->end(); ++it)
		{
			bSuccess &= PreloadRenderData( (*it), pData->GetOpenSGWindow() );
		}
	}
	return bSuccess;
}

VISTAKERNELOPENSGEXTAPI
bool VistaOpenSGGeometryTools::PreloadRenderData( VistaGeometry* pGeom, VistaDisplayManager* pDispManager )
{
	VistaOpenSGGeometryData* pGeoData = dynamic_cast<VistaOpenSGGeometryData*>( pGeom->GetData() );

	bool bSuccess = true;

	const std::map<std::string, VistaWindow*>& mapWindwos = pDispManager->GetWindowsConstRef();
	for( std::map<std::string, VistaWindow*>::const_iterator itWin = mapWindwos.begin();
			itWin != mapWindwos.end(); ++itWin )
	{
		VistaOpenSGDisplayBridge::WindowData* pWinData = dynamic_cast<VistaOpenSGDisplayBridge::WindowData*>(
															(*itWin).second->GetData() );
		bSuccess &= PreloadRenderData( pGeoData->GetGeometry(), pWinData->GetOpenSGWindow() );
	}
	return bSuccess;
}

VISTAKERNELOPENSGEXTAPI
bool VistaOpenSGGeometryTools::PreloadRenderData( osg::GeometryPtr pGeom, osg::WindowPtr pWindow )
{
	if( pGeom->getDlistCache() || pGeom->getVbo() )
	{
		// @todo: getGLID() is protected :/
		osg::SFInt32* pGLField = dynamic_cast<osg::SFInt32*>( pGeom->getField( osg::Geometry::GLIdFieldId ) );
		if( pGLField )
		{
			pWindow->validateGLObject( pGLField->getValue() );
		}
		
	}
	osg::ChunkMaterialPtr pMaterial = osg::ChunkMaterialPtr::dcast( pGeom->getMaterial() );
	if( pMaterial != osg::NullFC )
	{
		PreloadRenderData( pMaterial, pWindow );
	}
	return true;
}

VISTAKERNELOPENSGEXTAPI
bool VistaOpenSGGeometryTools::PreloadRenderData( osg::ChunkMaterialPtr pMaterial, osg::WindowPtr pWindow )
{
	osg::SHLChunkPtr pShader = osg::SHLChunkPtr::dcast( pMaterial->find( osg::SHLChunk::getClassType() ) );
	if( pShader != osg::NullFC )
	{
		pWindow->validateGLObject( pShader->getGLId() );
	}
	osg::SimpleTexturedMaterialPtr pSimpleTexMat = osg::SimpleTexturedMaterialPtr::dcast( pMaterial );
	if( pSimpleTexMat != osg::NullFC )
	{
		osg::StatePtr pState = pMaterial->getState();
		osg::MFStateChunkPtr oStates = pState->getChunks();
		for( osg::MFStateChunkPtr::iterator itState = oStates.begin();
				itState != oStates.end(); ++itState )
		{
			osg::TextureChunkPtr pTex = osg::TextureChunkPtr::dcast( (*itState) );
			if( pTex != osg::NullFC )
				pWindow->validateGLObject( pTex->getGLId() );
		}
	}
	
	for( osg::Int32 i = 0; i < 8; ++i )
	{
		osg::TextureChunkPtr pTex = osg::TextureChunkPtr::dcast( pMaterial->find( osg::TextureChunk::getClassType(), i ) );
		if( pTex != osg::NullFC )
			pWindow->validateGLObject( pTex->getGLId() );
	}	
	
	return true;
}

VISTAKERNELOPENSGEXTAPI
bool VistaOpenSGGeometryTools::PreloadAllRenderData( VistaDisplayManager* pDispManager )
{
	const std::map<std::string, VistaWindow*>& mapWindwos = pDispManager->GetWindowsConstRef();
	for( std::map<std::string, VistaWindow*>::const_iterator itWin = mapWindwos.begin();
			itWin != mapWindwos.end(); ++itWin )
	{
		VistaOpenSGDisplayBridge::WindowData* pWinData = dynamic_cast<VistaOpenSGDisplayBridge::WindowData*>(
															(*itWin).second->GetData() );
		pWinData->GetOpenSGWindow()->validateAllGLObjects();
	}

	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


