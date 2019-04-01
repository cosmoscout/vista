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
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#endif

#include <GL/glew.h>

#include "VistaOpenSGShadow.h"

#include <VistaBase/VistaVersion.h>
#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaProjection.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>

#include <VistaKernel/OpenSG/VistaOpenSGDisplayBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>

#include <OpenSG/OSGShadowViewport.h>
#include <OpenSG/OSGWindow.h>
#include <OpenSG/OSGGradientBackground.h>
#include <OpenSG/OSGSolidBackground.h>

#ifdef WIN32
#include <windows.h>
#endif

#if defined(DARWIN)
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES */
/*============================================================================*/

using namespace OSG;

//class StereoBufferShadowViewport;
//typedef FCPtr<ShadowViewportPtr, StereoBufferShadowViewport> StereoBufferShadowViewportPtr;

//class StereoBufferShadowViewport: public ShadowViewport
//{
//public:
//    enum
//    {
//        LeftBufferFieldId = ShadowViewport::NextFieldId,
//        RightBufferFieldId = LeftBufferFieldId + 1,
//        NextFieldId = RightBufferFieldId + 1
//    };
//
//    static const OSG::BitVector LeftBufferFieldMask;
//
//    static const OSG::BitVector RightBufferFieldMask;
//
//    static const OSG::BitVector MTInfluenceMask;
//
//
//    StereoBufferShadowViewport()
//        : ShadowViewport()
//        , _sfLeftBuffer( true )
//		, _sfRightBuffer( true )
//	{
//	}
//
//    static OSG::FieldContainerType &getClassType(void)
//    {
//        return _type;
//    }
//
//    static OSG::UInt32 getClassTypeId(void)
//    {
//        return _type.getId();
//    }
//
//    SFBool *getSFLeftBuffer(void)
//    {
//        return &_sfLeftBuffer;
//    }
//
//    SFBool *getSFRightBuffer(void)
//    {
//        return &_sfRightBuffer;
//    }
//
//
//    bool &getLeftBuffer(void)
//    {
//        return _sfLeftBuffer.getValue();
//    }
//
//    const bool &getLeftBuffer(void) const
//    {
//        return _sfLeftBuffer.getValue();
//    }
//
//    void setLeftBuffer(const bool bValue)
//    {
//		_sfLeftBuffer.setValue(bValue);
//	}
//
//    bool &getRightBuffer(void)
//    {
//        return _sfRightBuffer.getValue();
//    }
//
//    const bool &getRightBuffer(void) const
//    {
//        return _sfRightBuffer.getValue();
//    }
//
//    void setRightBuffer(const bool bValue)
//    {
//		_sfRightBuffer.setValue(bValue);
//	}
//
//    void activate(void)
//    {
//        if(getLeftBuffer())
//        {
//            if(getRightBuffer())
//            {
//                glDrawBuffer(GL_BACK);
//                glReadBuffer(GL_BACK);
//            }
//            else
//            {
//                glDrawBuffer(GL_BACK_LEFT);
//                glReadBuffer(GL_BACK_LEFT);
//            }
//        }
//        else
//        {
//            if(getRightBuffer())
//            {
//                glDrawBuffer(GL_BACK_RIGHT);
//                glReadBuffer(GL_BACK_RIGHT);
//            }
//            else
//            {
//                glDrawBuffer(GL_NONE);
//                glReadBuffer(GL_NONE);
//            }
//        }
//        ShadowViewport::activate();
//    }
//
//    void deactivate(void)
//    {
//        ShadowViewport::deactivate();
//        glDrawBuffer(GL_BACK);
//        glReadBuffer(GL_BACK);
//    }
//
//    static StereoBufferShadowViewportPtr create (void)
//    {
//        /*StereoBufferShadowViewportPtr fc;
//
//        if(getClassType().getPrototype() != OSG::NullFC)
//        {
//        fc = StereoBufferShadowViewportPtr::dcast(
//        getClassType().getPrototype()-> shallowCopy());
//        }
//
//        return fc;*/
//        return createEmpty();
//    }
//    static StereoBufferShadowViewportPtr createEmpty (void)
//    {
//        StereoBufferShadowViewportPtr returnValue;
//
//        newPtr(returnValue);
//
//        return returnValue;
//    }
//
//    static void initMethod (void)
//    {
//    }
//
//
//    UInt32 getBinSize(const BitVector &whichField)
//    {
//        UInt32 returnValue = ShadowViewport::getBinSize(whichField);
//
//        if(FieldBits::NoField != (LeftBufferFieldMask & whichField))
//        {
//            returnValue += _sfLeftBuffer.getBinSize();
//        }
//
//        if(FieldBits::NoField != (RightBufferFieldMask & whichField))
//        {
//            returnValue += _sfRightBuffer.getBinSize();
//        }
//
//
//        return returnValue;
//    }
//
//    void copyToBin(      BinaryDataHandler &pMem,
//        const BitVector         &whichField)
//    {
//        ShadowViewport::copyToBin(pMem, whichField);
//
//        if(FieldBits::NoField != (LeftBufferFieldMask & whichField))
//        {
//            _sfLeftBuffer.copyToBin(pMem);
//        }
//
//        if(FieldBits::NoField != (RightBufferFieldMask & whichField))
//        {
//            _sfRightBuffer.copyToBin(pMem);
//        }
//
//
//    }
//
//    void copyFromBin(      BinaryDataHandler &pMem,
//        const BitVector    &whichField)
//    {
//        ShadowViewport::copyFromBin(pMem, whichField);
//
//        if(FieldBits::NoField != (LeftBufferFieldMask & whichField))
//        {
//            _sfLeftBuffer.copyFromBin(pMem);
//        }
//
//        if(FieldBits::NoField != (RightBufferFieldMask & whichField))
//        {
//            _sfRightBuffer.copyFromBin(pMem);
//        }
//
//
//    }
//
//    
//FieldContainerPtr shallowCopy(void) const 
//{ 
//    StereoBufferShadowViewportPtr returnValue; 
//
//    newPtr(returnValue, dynamic_cast<const StereoBufferShadowViewport *>(this)); 
//
//    return returnValue; 
//}
//
//UInt32 getContainerSize(void) const 
//{ 
//    return sizeof(StereoBufferShadowViewport); 
//}
//
//
//#if !defined(OSG_FIXED_MFIELDSYNC)
//void executeSync(      FieldContainer &other,
//                                    const BitVector      &whichField)
//{
//    this->executeSyncImpl(static_cast<StereoBufferShadowViewport *>(&other),
//                          whichField);
//}
//#else
//void executeSync(      FieldContainer &other,
//                                    const BitVector      &whichField,                                    const SyncInfo       &sInfo     )
//{
//    this->executeSyncImpl((StereoBufferShadowViewport *) &other, whichField, sInfo);
//}
//void execBeginEdit(const BitVector &whichField, 
//                                            UInt32     uiAspect,
//                                            UInt32     uiContainerSize) 
//{
//    this->execBeginEditImpl(whichField, uiAspect, uiContainerSize);
//}
//
//void onDestroyAspect(UInt32 uiId, UInt32 uiAspect)
//{
//    Inherited::onDestroyAspect(uiId, uiAspect);
//
//}
//#endif
//
//
//#if !defined(OSG_FIXED_MFIELDSYNC)
//    void executeSyncImpl(      StereoBufferShadowViewport *pOther,
//        const BitVector         &whichField)
//    {
//
//        ShadowViewport::executeSyncImpl(pOther, whichField);
//
//        if(FieldBits::NoField != (LeftBufferFieldMask & whichField))
//            _sfLeftBuffer.syncWith(pOther->_sfLeftBuffer);
//
//        if(FieldBits::NoField != (RightBufferFieldMask & whichField))
//            _sfRightBuffer.syncWith(pOther->_sfRightBuffer);
//
//
//    }
//#else
//    void executeSyncImpl(      StereoBufferShadowViewport *pOther,
//        const BitVector         &whichField,
//        const SyncInfo          &sInfo      )
//    {
//
//        ShadowViewport::executeSyncImpl(pOther, whichField, sInfo);
//
//        if(FieldBits::NoField != (LeftBufferFieldMask & whichField))
//            _sfLeftBuffer.syncWith(pOther->_sfLeftBuffer);
//
//        if(FieldBits::NoField != (RightBufferFieldMask & whichField))
//            _sfRightBuffer.syncWith(pOther->_sfRightBuffer);
//
//
//
//    }
//
//    void execBeginEditImpl (const BitVector &whichField, 
//        UInt32     uiAspect,
//        UInt32     uiContainerSize)
//    {
//        ShadowViewport::execBeginEditImpl(whichField, uiAspect, uiContainerSize);
//
//    }
//#endif
//
//
//
//protected:
//    SFBool _sfLeftBuffer;
//    SFBool _sfRightBuffer;
//
//    friend class FieldContainer;
//
//    static FieldDescription *_desc[];
//    static FieldContainerType _type;
//
//};
//
//const OSG::BitVector StereoBufferShadowViewport::LeftBufferFieldMask =
//(TypeTraits<BitVector>::One << StereoBufferShadowViewport::LeftBufferFieldId);
//
//const OSG::BitVector StereoBufferShadowViewport::RightBufferFieldMask =
//(TypeTraits<BitVector>::One << StereoBufferShadowViewport::RightBufferFieldId);
//
//const OSG::BitVector StereoBufferShadowViewport::MTInfluenceMask =
//(ShadowViewport::MTInfluenceMask) |
//(static_cast<BitVector>(0x0) << ShadowViewport::NextFieldId);
//
//FieldDescription *StereoBufferShadowViewport::_desc[] =
//{
//	new FieldDescription(SFBool::getClassType(),
//				"leftBuffer",
//				StereoBufferShadowViewport::LeftBufferFieldId, LeftBufferFieldMask,
//				false,
//				(FieldAccessMethod) &StereoBufferShadowViewport::getSFLeftBuffer),
//	new FieldDescription(SFBool::getClassType(),
//				"rightBuffer",
//				StereoBufferShadowViewport::RightBufferFieldId, RightBufferFieldMask,
//				false,
//				(FieldAccessMethod) &StereoBufferShadowViewport::getSFRightBuffer)
//};
//
//FieldContainerType StereoBufferShadowViewport::_type(
//	"StereoBufferShadowViewport",
//	"StereoBufferViewport",
//	NULL,
//	(PrototypeCreateF) &StereoBufferShadowViewport::createEmpty,
//	StereoBufferShadowViewport::initMethod,
//	_desc,
//	sizeof(_desc));


// small helper class to get rid of the OpenSG dependencies in the header file
class tShadowViewPortNameMap: public std::map<std::string, OSG::ShadowViewportPtr>{};

OSG::ShadowViewportPtr CreateShadowViewport( const std::vector<VistaLightNode*>& vecLights,
											const bool bStereo, const bool bLeft )
{
	OSG::ShadowViewportPtr pShadowVP;
	if( bStereo )
	{
		pShadowVP = OSG::ShadowViewport::create();
		pShadowVP->setLeftBuffer( bLeft );
		pShadowVP->setRightBuffer( !bLeft );
	}
	else
		pShadowVP = OSG::ShadowViewport::create();

	// create Shadow viewport
	beginEditCP(pShadowVP);
	{
		pShadowVP->setOffFactor(4.0);
		pShadowVP->setOffBias(8.0);

		// used to set global shadow intensity, ignores shadow intensity from light sources if != 0.0
		pShadowVP->setGlobalShadowIntensity(0.0f);
		pShadowVP->setMapSize(512);
		pShadowVP->setSize(0, 0, 1, 1);


		// ShadowSmoothness used for PCF_SHADOW_MAP and VARIANCE_SHADOW_MAP, defines Filter Width.
		// Range can be 0.0 ... 1.0.
		// ShadowSmoothness also used to define the light size for PCSS_SHADOW_MAP
		pShadowVP->setShadowSmoothness(0.5);


		// add all light nodes to the shadow viewport
		for (std::vector<VistaLightNode*>::size_type j=0;
			j < vecLights.size(); j++)
		{
			VistaOpenSGLightNodeData* pOpenSGLightData = static_cast<VistaOpenSGLightNodeData*>
				(vecLights[j]->GetData());
			pShadowVP->getLightNodes().push_back(pOpenSGLightData->GetLightNode());
		}

		// we start with enabled perspective shadows
		pShadowVP->setShadowOn(true);
		pShadowVP->setShadowMode(OSG::ShadowViewport::PERSPECTIVE_SHADOW_MAP);
	}
	endEditCP(pShadowVP);

	return pShadowVP;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR */
/*============================================================================*/

VistaOpenSGShadow::VistaOpenSGShadow(VistaDisplayManager *pMgr, VistaGraphicsManager *pGrMgr)
: m_bInit(false)
, m_bEnabled(false)
, m_pDispMgr(pMgr)
, m_pGrMgr(pGrMgr)
, m_pShadowVPs( new tShadowViewPortNameMap )
{
	const std::map<std::string, VistaWindow*> mapWindows = m_pDispMgr->GetWindowsConstRef();
	std::map<std::string, VistaWindow*>::const_iterator itWindow = mapWindows.begin();

	bool bOk = true;
	while (itWindow != mapWindows.end())
	{
		bOk = bOk && InitWindow (itWindow->first, itWindow->second, m_pGrMgr);
		itWindow++;
	}
	m_bInit = bOk;
	m_bEnabled = true;
}

VistaOpenSGShadow::~VistaOpenSGShadow()
{
	if( m_pShadowVPs )
		delete m_pShadowVPs;
	m_pShadowVPs = NULL;
}

/*============================================================================*/
/* IMPLEMENTATION */
/*============================================================================*/

/*============================================================================*/
/* */
/* NAME : Init */
/* */
/*============================================================================*/

static void dive(const VistaGroupNode &pParent,
				 std::set<VistaLightNode*> &setLightNodes)
{
	for(unsigned int n=0; n < pParent.GetNumChildren(); ++n)
	{
		IVistaNode *pNode = pParent.GetChild(int(n));
		//pNode->Debug(std::cerr);
		switch(pNode->GetType())
		{
		case VISTA_LIGHTNODE:
		case VISTA_AMBIENTLIGHTNODE:
		case VISTA_DIRECTIONALLIGHTNODE:
		case VISTA_POINTLIGHTNODE:
		case VISTA_SPOTLIGHTNODE:
			{
				setLightNodes.insert(static_cast<VistaLightNode*>(pNode));
				break;
			}
		case VISTA_GROUPNODE:
		case VISTA_TRANSFORMNODE:
		case VISTA_LODNODE:
		case VISTA_SWITCHNODE:
			{
				VistaGroupNode *pGroup = dynamic_cast<VistaGroupNode*>(pNode);
				if(!pGroup)
					continue;
				dive( *pGroup, setLightNodes );
				break;
			}
		default:
			break;
		}
	}
}


bool VistaOpenSGShadow::InitWindow (const std::string & strName,
									VistaWindow* pWindow,
									VistaGraphicsManager *pGrMgr)
{
	VistaGroupNode* pRoot = pGrMgr->GetSceneGraph()->GetRealRoot();

	// get root node data
	VistaOpenSGNodeData* pOpenSGNodeData = static_cast<VistaOpenSGNodeData*> (pRoot->GetData());

	if (!pOpenSGNodeData)
		return false;

	// get list of all light nodes
	std::set<VistaLightNode*> setLights;

	dive( *pGrMgr->GetSceneGraph()->GetRealRoot(), setLights);
	for(std::set<VistaLightNode*>::const_iterator cit = setLights.begin();
		cit != setLights.end(); ++cit)
	{
		VistaLightNode* pLightNode = *cit;
		m_vecLights.push_back (pLightNode);

		// set shadow intensity value for light
		SetLightShadowIntensity (pLightNode, 1.0f);
	}

	// now, for every viewport, create a shadow viewport
	int iVP = pWindow->GetNumberOfViewports();
	for (int i=0; i < iVP; i++)
	{
		VistaViewport* pVistaVP = pWindow->GetViewport (i);

		VistaOpenSGDisplayBridge::ViewportData* pViewportData
			= static_cast<VistaOpenSGDisplayBridge::ViewportData*>( pVistaVP->GetData() );

		// get old OpenSG Viewport
		OSG::ViewportPtr pOldVP = pViewportData->GetOpenSGViewport();		

		if( pViewportData->GetStereo() )
		{
			OSG::ShadowViewportPtr pShadowLeftVP = CreateShadowViewport( m_vecLights, true, true );
			pViewportData->ReplaceViewport( pShadowLeftVP, true, false );
			(*m_pShadowVPs)[pVistaVP->GetNameForNameable()+"_left"] = pShadowLeftVP;

			OSG::ShadowViewportPtr pRightShadowVP = CreateShadowViewport( m_vecLights, true, false );
			pViewportData->ReplaceViewport( pRightShadowVP, true, true );
			(*m_pShadowVPs)[pVistaVP->GetNameForNameable() + "_right"] = pRightShadowVP;
		}
		else
		{
			OSG::ShadowViewportPtr pShadowVP = CreateShadowViewport( m_vecLights, false, false );
			pViewportData->ReplaceViewport( pShadowVP, true, false );
			(*m_pShadowVPs)[pVistaVP->GetNameForNameable()] = pShadowVP;
		}



	}
	return true;
}

/*============================================================================*/
/* */
/* NAME : AddLight */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::AddLight (VistaLightNode* pLight)
{
	if (!m_bInit)
		return false;

	m_vecLights.push_back (pLight);
	VistaOpenSGLightNodeData* pOpenSGLightData
		= dynamic_cast<VistaOpenSGLightNodeData*> (pLight->GetData());

	// set new light to all shadow viewports
	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		OSG::ShadowViewportPtr ptrShadowVP = it->second;
		beginEditCP(ptrShadowVP, osg::ShadowViewport::LightNodesFieldMask);
		ptrShadowVP->getLightNodes().push_back(pOpenSGLightData->GetLightNode());
		endEditCP(ptrShadowVP, osg::ShadowViewport::LightNodesFieldMask);
	}

	return true;
}

/*============================================================================*/
/* */
/* NAME : RemoveLight */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::RemoveLight (VistaLightNode* pLight)
{
	if (!m_bInit)
		return false;


	// remove light from internal list of lights
	std::vector<VistaLightNode*>::iterator itLight;
	itLight = std::find(m_vecLights.begin(), m_vecLights.end(), pLight);
	if(itLight != m_vecLights.end())
		m_vecLights.erase(itLight);

	// remove light from all shadow viewports
	//VistaOpenSGNodeData* pOpenSGLightData = static_cast<VistaOpenSGNodeData*> (pLight->GetData());
	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		OSG::ShadowViewportPtr ptrShadowVP = it->second;

		/** this fails for some reason :( */
		//// find light node
		//OSG::MFNodePtr::iterator itNode;
		//itNode = ptrShadowVP->getLightNodes().find(pOpenSGLightData->GetNode());
		//if(itNode != ptrShadowVP->getLightNodes().end())
		//{
		//	// remove light node
		//	beginEditCP(ptrShadowVP, OSG::ShadowViewport::LightNodesFieldMask);
		//	ptrShadowVP->getLightNodes().erase(itNode);
		//	endEditCP(ptrShadowVP, OSG::ShadowViewport::LightNodesFieldMask);
		//}
		/** workaround: clear lightnodes and readd all... */
		beginEditCP(ptrShadowVP, osg::ShadowViewport::LightNodesFieldMask);
		ptrShadowVP->getLightNodes().clear();
		for (std::vector<VistaLightNode*>::size_type j=0;
			j < m_vecLights.size(); j++)
		{
			VistaOpenSGLightNodeData* pOpenSGLightData = static_cast<VistaOpenSGLightNodeData*>
				(m_vecLights[j]->GetData());
			ptrShadowVP->getLightNodes().push_back(pOpenSGLightData->GetLightNode());
		}
		endEditCP(ptrShadowVP, osg::ShadowViewport::LightNodesFieldMask);
	}

	return true;
}

bool VistaOpenSGShadow::ClearAllLights()
{
	if (!m_bInit)
		return false;
	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		OSG::ShadowViewportPtr ptrShadowVP = it->second;
		beginEditCP(ptrShadowVP, osg::ShadowViewport::LightNodesFieldMask);
		ptrShadowVP->getLightNodes().clear();
		endEditCP(ptrShadowVP, osg::ShadowViewport::LightNodesFieldMask);
		m_vecLights.clear();
	}
	return true;
}

/*============================================================================*/
/* */
/* NAME : AddExcludeNode */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::AddExcludeNode( IVistaNode* pNode )
{
	if (!m_bInit)
		return false;

	VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*> (static_cast<VistaNode*>(pNode)->GetData());

	// add node to exclude list
	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		beginEditCP((*it).second);
		(*it).second->getExcludeNodes().push_back (pOpenSGData->GetNode());
		endEditCP((*it).second);
	}

	return true;
}

/*============================================================================*/
/* */
/* NAME : RemoveExcludeNode */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::RemoveExcludeNode (IVistaNode* pNode)
{
	if (!m_bInit)
		return false;

	VistaOpenSGNodeData* pOpenSGData = static_cast<VistaOpenSGNodeData*> (static_cast<VistaNode*>(pNode)->GetData());

	bool allFound = true;
	// remove node from exclude list
	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		// find node
		OSG::MFNodePtr &excludes = it->second->getExcludeNodes();
		OSG::NodePtr osgNode = pOpenSGData->GetNode();

		OSG::MFNodePtr::iterator nIt = excludes.find(osgNode);
		if(nIt != excludes.end())
		{
			beginEditCP(it->second);
			excludes.erase (nIt);
			endEditCP(it->second);
		}
		else
		{
			allFound = false;
		}
	}

	return allFound;
}

/*============================================================================*/
/* */
/* NAME : ClearExcludeNodes */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::ClearExcludeNodes ()
{
	if (!m_bInit)
		return false;

	// clear exclude lists
	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		it->second->getExcludeNodes().clear();
	}

	return true;
}

/*============================================================================*/
/* */
/* NAME : SetLightShadowIntensity */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::SetLightShadowIntensity (VistaLightNode *pLightNode, float fIntensity)
{
	VistaOpenSGNodeBridge* pOpenSGNodeBridge = static_cast <VistaOpenSGNodeBridge*>
		(m_pGrMgr->GetNodeBridge());

	if (!pOpenSGNodeBridge)
		return false;

	IVistaNodeData *pNodeData = pLightNode->GetData();
	VistaOpenSGLightNodeData *pOpenSGData = dynamic_cast <VistaOpenSGLightNodeData*>(pNodeData);


	osg::LightPtr pLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
	if(pLight == osg::NullFC)
		return false;

	beginEditCP(pLight, OSG::Light::ShadowIntensityFieldMask);
	pLight->setShadowIntensity(fIntensity);
	endEditCP(pLight, OSG::Light::ShadowIntensityFieldMask);

	return true;
}

float VistaOpenSGShadow::GetLightShadowIntensity( const VistaLightNode* pLight ) const
{	
	VistaOpenSGNodeBridge* pOpenSGNodeBridge = static_cast <VistaOpenSGNodeBridge*>
		(m_pGrMgr->GetNodeBridge());

	if (!pOpenSGNodeBridge)
		return 0.0f;

	IVistaNodeData *pNodeData = pLight->GetData();
	VistaOpenSGLightNodeData *pOpenSGData = dynamic_cast <VistaOpenSGLightNodeData*>(pNodeData);

	osg::LightPtr pOSGLight = osg::LightPtr::dcast(pOpenSGData->GetLightCore());
	if(pOSGLight == osg::NullFC)
		return 0.0f;
	return pOSGLight->getShadowIntensity();
}

/*============================================================================*/
/* */
/* NAME : SetLightsShadowIntensity */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::SetLightsShadowIntensity (float fIntensity)
{
	bool ret = true;
	for(std::vector<VistaLightNode*>::size_type i = 0; i < m_vecLights.size(); ++i)
	{
		ret = ret && SetLightShadowIntensity(m_vecLights[i], fIntensity);
	}
	return ret;
}

/*============================================================================*/
/* */
/* NAME : SetGlobalShadowIntensity */
/* */
/*============================================================================*/

void VistaOpenSGShadow::SetGlobalShadowIntensity(float fIntensity)
{
	if (!m_bInit)
		return;

	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		beginEditCP(it->second, OSG::ShadowViewport::GlobalShadowIntensityFieldMask);
		it->second->setGlobalShadowIntensity(fIntensity);
		endEditCP(it->second, OSG::ShadowViewport::GlobalShadowIntensityFieldMask);
	}

	return;
}

float VistaOpenSGShadow::GetGlobalShadowIntensity() const
{
	if( m_pShadowVPs->empty() )
		return 0;
	return (*m_pShadowVPs->begin()).second->getGlobalShadowIntensity();
}

/*============================================================================*/
/* */
/* NAME : SetOffBias */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::SetOffBias (float fBias)
{
	if (!m_bInit)
		return false;

	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		beginEditCP(it->second, OSG::ShadowViewport::OffFactorFieldMask);
		it->second->setOffBias(fBias);
		endEditCP(it->second, OSG::ShadowViewport::OffFactorFieldMask);
	}

	return true;
}

float VistaOpenSGShadow::GetOffBias() const
{
	if( m_pShadowVPs->empty() )
		return 0;
	return (*m_pShadowVPs->begin()).second->getOffBias();
}

/*============================================================================*/
/* */
/* NAME : SetOffFactor */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::SetOffFactor (float fFactor)
{
	if (!m_bInit)
		return false;

	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		beginEditCP(it->second, OSG::ShadowViewport::OffFactorFieldMask);
		it->second->setOffFactor(fFactor);
		endEditCP(it->second, OSG::ShadowViewport::OffFactorFieldMask);
	}

	return true;
}

float VistaOpenSGShadow::GetOffFactor() const
{
	if( m_pShadowVPs->empty() )
		return 0;
	return (*m_pShadowVPs->begin()).second->getOffFactor();
}

/*============================================================================*/
/* */
/* NAME : SetSmoothness */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::SetSmoothness (float fSmooth)
{
	if (!m_bInit)
		return false;

	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		beginEditCP(it->second, OSG::ShadowViewport::ShadowSmoothnessFieldMask);
		it->second->setShadowSmoothness(fSmooth);
		endEditCP(it->second, OSG::ShadowViewport::ShadowSmoothnessFieldMask);
	}

	return true;
}

float VistaOpenSGShadow::GetSmoothness() const
{	
	if( m_pShadowVPs->empty() )
		return 0;
	return (*m_pShadowVPs->begin()).second->getShadowSmoothness();
}

/*============================================================================*/
/* */
/* NAME : SetMapSize */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::SetMapSize (int iMapSize)
{
	if (!m_bInit)
		return false;

	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		beginEditCP(it->second, OSG::ShadowViewport::MapSizeFieldMask);
		it->second->setMapSize(iMapSize);
		endEditCP(it->second, OSG::ShadowViewport::MapSizeFieldMask);
	}

	return true;
}

int VistaOpenSGShadow::GetMapSize() const
{	
	if( m_pShadowVPs->empty() )
		return 0;
	return (*m_pShadowVPs->begin()).second->getMapSize();
}

/*============================================================================*/
/* */
/* NAME : EnableShadow */
/* */
/*============================================================================*/

void VistaOpenSGShadow::EnableShadow ()
{
	if (!m_bInit)
		return;

	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		beginEditCP(it->second, OSG::ShadowViewport::ShadowOnFieldMask);
		it->second->setShadowOn(true);
		endEditCP(it->second, OSG::ShadowViewport::ShadowOnFieldMask);
	}

	m_bEnabled = true;
}

/*============================================================================*/
/* */
/* NAME : DisableShadow */
/* */
/*============================================================================*/

void VistaOpenSGShadow::DisableShadow ()
{
	if (!m_bInit)
		return ;

	for (tShadowViewPortNameMap::iterator it =(*m_pShadowVPs).begin(); it != (*m_pShadowVPs).end(); ++it)
	{
		beginEditCP(it->second, OSG::ShadowViewport::ShadowOnFieldMask);
		it->second->setShadowOn(false);
		endEditCP(it->second, OSG::ShadowViewport::ShadowOnFieldMask);
	}

	m_bEnabled = false;
}

/*============================================================================*/
/* */
/* NAME : GetIsShadowEnabled */
/* */
/*============================================================================*/

bool VistaOpenSGShadow::GetIsShadowEnabled ()
{
	return m_bEnabled;
}

/*============================================================================*/
/* */
/* NAME : SetShadowMode */
/* */
/*============================================================================*/
void VistaOpenSGShadow::SetShadowMode(const eShadowMode &eMode)
{
	if (!m_bInit)
		return;

	for (tShadowViewPortNameMap::iterator it = (*m_pShadowVPs).begin();
		it != (*m_pShadowVPs).end();
		++it)
	{
		beginEditCP(it->second, OSG::ShadowViewport::ShadowModeFieldMask);
		it->second->setShadowMode(eMode);
		endEditCP(it->second, OSG::ShadowViewport::ShadowModeFieldMask);
	}

	return;
}


/*============================================================================*/
/* */
/* NAME : GetShadowMode */
/* */
/*============================================================================*/

VistaOpenSGShadow::eShadowMode VistaOpenSGShadow::GetShadowMode() const
{
	if( m_pShadowVPs->empty() )
		return VOSGSHADOW_ERROR;

	tShadowViewPortNameMap::const_iterator it = (*m_pShadowVPs).begin();
	eShadowMode mode = VistaOpenSGShadow::eShadowMode(it->second->getShadowMode());

	for (; it != (*m_pShadowVPs).end(); ++it)
	{
		if (mode != VistaOpenSGShadow::eShadowMode(it->second->getShadowMode()))
			return VOSGSHADOW_ERROR;
	}

	return mode;
}

