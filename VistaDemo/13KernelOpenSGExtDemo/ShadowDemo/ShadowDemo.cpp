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

#include "ShadowDemo.h"

#include "SwitchMapResolutionCallback.h"
#include "SwitchShadowCallback.h"
#include "ToggleShadowCallback.h"

#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaLightNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>

#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaBase/VistaExceptionBase.h>

#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

#include <list>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
static VistaGeometry* CreateFloor(const float& extent = 10.0f, const int& tesselation = 64);

class LightUpdater : public VistaEventHandler {
 public:
  LightUpdater(VistaDirectionalLight* pLight)
      : m_pLight(pLight) {
  }

  ~LightUpdater() {
  }

  virtual void HandleEvent(VistaEvent* pEvent) {
    // make the light go round in 60 seconds
    double               dSeconds = 60.0f;
    VistaType::microtime dTime    = pEvent->GetTime();
    float                t        = (float)(fmod(dTime, dSeconds) / dSeconds);

    float s = sin(t * Vista::Pi * 2.0f);
    float c = cos(t * Vista::Pi * 2.0f);
    m_pLight->SetDirection(VistaVector3D(s, 1, c));
  }

 private:
  VistaDirectionalLight* m_pLight;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
ShadowDemo::ShadowDemo(int argc, char* argv[])
    : m_pShadow(NULL)
    , m_nMapSize(1024) {
  // add default path for config-file search
  {
    std::list<std::string> liSearchPath;
    liSearchPath.push_back("../../configfiles/");
    mVistaSystem.SetIniSearchPaths(liSearchPath);
  }

  // ViSTA's standard intro message
  mVistaSystem.IntroMsg();

  // Create Userdefined EventHandler
  // mShadowDemoEventHandler(this, mVistaSystem.GetEventManager());

  if (!mVistaSystem.Init(argc, argv)) {
    VISTA_THROW("Initialization of VistaSystem instance failed!", 1);
  }

  if (!CreateScene()) {
    VISTA_THROW("Can not create scene.", 1);
  }
  if (mVistaSystem.GetDisplayManager()->GetDisplaySystem(0) == 0) {
    VISTA_THROW("No Displaysystem found", 1);
  }

  VistaDisplayManager*  pDisplayManager  = mVistaSystem.GetDisplayManager();
  VistaGraphicsManager* pGraphicsManager = mVistaSystem.GetGraphicsManager();
  mVistaSystem.GetEventManager()->AddEventHandler(
      new LightUpdater(m_pLight), VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS);

  // set background color
  pGraphicsManager->SetBackgroundColor(VistaColor(0.6f, 0.6f, 1.0f));

  // create shadow viewports
  m_pShadow = new VistaOpenSGShadow(pDisplayManager, pGraphicsManager);

  if (!m_pShadow) {
    VISTA_THROW("Can not create shadow.", 1);
  }

  m_pShadow->SetGlobalShadowIntensity(0.7f);
  m_pShadow->SetShadowMode(VistaOpenSGShadow::VOSGSHADOW_STD_SHADOW_MAP);
  m_pShadow->SetMapSize(m_nMapSize);
  m_pShadow->EnableShadow();

  VistaKeyboardSystemControl* pCtrl = mVistaSystem.GetKeyboardSystemControl();
  pCtrl->BindAction('s', new ToggleShadowCallback(this), "toggle shadow");
  pCtrl->BindAction('+', new SwitchShadowCallback(this, 1), "next shadow");
  pCtrl->BindAction('-', new SwitchShadowCallback(this, -1), "previous shadow");
  pCtrl->BindAction(
      '*', new SwitchMapResolutionCallback(this, 2.0f), "double map size", true, true, true);
  pCtrl->BindAction('/', new SwitchMapResolutionCallback(this, 0.5f), "half map size");

  mVistaSystem.GetDisplayManager()
      ->GetWindowByName("MAIN_WINDOW")
      ->GetWindowProperties()
      ->SetTitle(argv[0]);
}

ShadowDemo::~ShadowDemo() {
  if (m_pShadow)
    delete m_pShadow;
  m_pShadow = NULL;

  if (m_pLight)
    delete m_pLight;
  m_pLight = NULL;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool ShadowDemo::Run() {
  // Start Universe
  return mVistaSystem.Run();
}

bool ShadowDemo::CreateScene() {
  VistaSceneGraph* pSG = mVistaSystem.GetGraphicsManager()->GetSceneGraph();

  VistaTransformNode* pRootNode = pSG->NewTransformNode(pSG->GetRoot());

  // try loading the cached scene
  IVistaNode* pModelNode = pSG->LoadNode("_cached_scene.osb.gz");
  if (pModelNode) {
    pRootNode->AddChild(pModelNode);
  } else {
    //@TODO - find better name!
    const int      iTess          = 64;
    const float    fExtent        = 10.0f;
    VistaGeometry* pFloorGeometry = CreateFloor(fExtent, iTess);
    pSG->NewGeomNode(pRootNode, pFloorGeometry);

    VistaTransformNode* pTransBox    = pSG->NewTransformNode(pRootNode);
    VistaTransformNode* pTransSphere = pSG->NewTransformNode(pRootNode);

    pTransBox->SetTranslation(0, 0.5f, 0);
    pTransSphere->SetTranslation(0, 1.4f, 0);

    VistaGeometryFactory oGeometryFactory(pSG);
    VistaGeometry*       pGeomBox = oGeometryFactory.CreateBox();
    pSG->NewGeomNode(pTransBox, pGeomBox);
    pGeomBox->SetColor(VistaColor(0.6f, 0.8f, 0.8f));

    VistaGeometry* pGeomSphere = oGeometryFactory.CreateBox(0.4f, 64, 64);
    pSG->NewGeomNode(pTransSphere, pGeomSphere);
    pGeomSphere->SetColor(VistaColor(0.4f, 0.5f, 0.9f));

    // seed some trees
    VistaTransformNode *pTreeNode1, *pTreeNode2, *pTreeNode3, *pTreeNode4;
    pTreeNode1 = pSG->NewTransformNode(pRootNode);
    pTreeNode2 = pSG->NewTransformNode(pRootNode);
    pTreeNode3 = pSG->NewTransformNode(pRootNode);
    pTreeNode4 = pSG->NewTransformNode(pRootNode);

    /*
     * @todo: share subtree!? clone node? / clone geometry/shallow copy !
     */
    pTreeNode1->AddChild(pSG->LoadNode("../../data/tree.wrl.gz"));
    pTreeNode2->AddChild(pSG->LoadNode("../../data/tree.wrl.gz"));
    pTreeNode3->AddChild(pSG->LoadNode("../../data/tree.wrl.gz"));
    pTreeNode4->AddChild(pSG->LoadNode("../../data/tree.wrl.gz"));

    pTreeNode1->SetTranslation(-fExtent * 0.31f, 0.0, fExtent * 0.32f);
    pTreeNode2->SetTranslation(fExtent * 0.42f, 0.0, fExtent * 0.33f);
    pTreeNode3->SetTranslation(fExtent * 0.34f, 0.0, -fExtent * 0.25f);
    pTreeNode4->SetTranslation(-fExtent * 0.33f, 0.0, -fExtent * 0.34f);

    pTreeNode1->SetRotation(VistaQuaternion(VistaEulerAngles(0, 1.6f, 0)));
    pTreeNode2->SetRotation(VistaQuaternion(VistaEulerAngles(0, 2.4f, 0)));
    pTreeNode3->SetRotation(VistaQuaternion(VistaEulerAngles(0, 3.9f, 0)));
    pTreeNode4->SetRotation(VistaQuaternion(VistaEulerAngles(0, 4.6f, 0)));

    // cache the scene to disk
    pSG->SaveSubTree("_cached_scene.osb.gz", pSG->GetRoot());
  }

  // now add the sun ;-)
  m_pLight = pSG->NewDirectionalLight(pRootNode);
  m_pLight->SetAmbientColor(0.1f, 0.1f, 0.1f);
  m_pLight->SetDiffuseColor(0.7f, 0.7f, 0.6f);
  m_pLight->SetSpecularColor(0.2f, 0.2f, 0.3f);
  m_pLight->SetDirection(VistaVector3D(0, 1, 0));
  m_pLight->SetAttenuation(VistaVector3D(0.2f, 0.0, 0.0));

  // move the scene to a nicer view
  pRootNode->SetTranslation(VistaVector3D(2, -1.6f, -3));
  pRootNode->SetRotation(VistaQuaternion(VistaEulerAngles(0, 1, 0)));

  return true;
}

static VistaGeometry* CreateFloor(const float& fExtents, const int& iTess) {
  VistaGeometry*                  pGeo = NULL;
  VistaVertexFormat               oVertexFormat;
  VistaGeometry::FaceType         fType;
  std::vector<VistaIndexedVertex> vVerts;
  std::vector<float>              vCoords;
  std::vector<float>              vNormals;
  std::vector<VistaColor>         vColors;
  std::vector<float>              vTexCoords;

  oVertexFormat.coordinate   = VistaVertexFormat::COORDINATE;
  oVertexFormat.normal       = VistaVertexFormat::NORMAL;
  oVertexFormat.color        = VistaVertexFormat::COLOR_NONE;
  oVertexFormat.textureCoord = VistaVertexFormat::TEXTURE_COORD_NONE;
  fType                      = VistaGeometry::VISTA_FACE_TYPE_QUADS;

  // tesselation
  for (int i = 0; i < iTess; ++i) {
    for (int j = 0; j < iTess; ++j) {
      float a[2], b[2], c[2], d[2];
      a[0] = (i + 0) / (float)iTess;
      a[1] = (j + 0) / (float)iTess;

      b[0] = (i + 0) / (float)iTess;
      b[1] = (j + 1) / (float)iTess;

      c[0] = (i + 1) / (float)iTess;
      c[1] = (j + 1) / (float)iTess;

      d[0] = (i + 1) / (float)iTess;
      d[1] = (j + 0) / (float)iTess;

      vCoords.push_back(a[0] * fExtents - fExtents * 0.5f); // x
      vCoords.push_back(0.0);                               // y
      vCoords.push_back(a[1] * fExtents - fExtents * 0.5f); // z

      vCoords.push_back(b[0] * fExtents - fExtents * 0.5f); // x
      vCoords.push_back(0.0);                               // y
      vCoords.push_back(b[1] * fExtents - fExtents * 0.5f); // z

      vCoords.push_back(c[0] * fExtents - fExtents * 0.5f); // x
      vCoords.push_back(0.0);                               // y
      vCoords.push_back(c[1] * fExtents - fExtents * 0.5f); // z

      vCoords.push_back(d[0] * fExtents - fExtents * 0.5f); // x
      vCoords.push_back(0.0);                               // y
      vCoords.push_back(d[1] * fExtents - fExtents * 0.5f); // z

      vTexCoords.push_back(a[0]); // u
      vTexCoords.push_back(a[1]); // v
      vTexCoords.push_back(b[0]); // u
      vTexCoords.push_back(b[1]); // v
      vTexCoords.push_back(c[0]); // u
      vTexCoords.push_back(c[1]); // v
      vTexCoords.push_back(d[0]); // u
      vTexCoords.push_back(d[1]); // v
    }
  }

  vNormals.push_back(0.0f); // x
  vNormals.push_back(1.0f); // y
  vNormals.push_back(0.0f); // z

  VistaIndexedVertex oIndexedVertex;
  for (int i = 0; i < iTess * iTess * 4; ++i) {
    oIndexedVertex = VistaIndexedVertex(i);
    oIndexedVertex.SetNormalIndex(0);
    oIndexedVertex.SetTextureCoordinateIndex(i);
    vVerts.push_back(oIndexedVertex);
  }

  VistaSceneGraph* pSG = GetVistaSystem()->GetGraphicsManager()->GetSceneGraph();
  pGeo =
      pSG->NewIndexedGeometry(vVerts, vCoords, vTexCoords, vNormals, vColors, oVertexFormat, fType);

  pGeo->SetColor(VistaColor::WHITE);
  return pGeo;
}

void ShadowDemo::UpdateLightAnimation(VistaEvent* pEvent) {
  // make the light go round in 60 seconds
  double               dSeconds = (double)(60.0f);
  VistaType::microtime time     = pEvent->GetTime();
  float                t        = (float)(fmod(time, dSeconds) / dSeconds);

  float s = sin(t * Vista::Pi * 2.0f);
  float c = cos(t * Vista::Pi * 2.0f);
  m_pLight->SetDirection(VistaVector3D(s, 1, c));
}

void ShadowDemo::SetShadowEnabled(bool bShadowEnabled) {
  if (bShadowEnabled)
    m_pShadow->EnableShadow();
  else
    m_pShadow->DisableShadow();
}

bool ShadowDemo::GetShadowEnabled() {
  return m_pShadow->GetIsShadowEnabled();
}

std::string ShadowDemo::GetShadowModeName(VistaOpenSGShadow::eShadowMode mode) {
  switch (mode) {
  case VistaOpenSGShadow::VOSGSHADOW_NO_SHADOW:
    return "NO SHADOW";
  case VistaOpenSGShadow::VOSGSHADOW_STD_SHADOW_MAP:
    return "STANDARD SHADOW MAPPING";
  case VistaOpenSGShadow::VOSGSHADOW_PERSPECTIVE_SHADOW_MAP:
    return "PERSPECTIVE SHADOW MAPPING";
  case VistaOpenSGShadow::VOSGSHADOW_DITHER_SHADOW_MAP:
    return "DITHERED SHADOW MAPPING";
  case VistaOpenSGShadow::VOSGSHADOW_PCF_SHADOW_MAP:
    return "PERCENTAGE CLOSER FILTERED (PCF) SHADOW MAPPING";
  case VistaOpenSGShadow::VOSGSHADOW_PCSS_SHADOW_MAP:
    return "PERCENTAGE CLOSER SOFT SHADOW (PCSS) MAPPING";
  case VistaOpenSGShadow::VOSGSHADOW_VARIANCE_SHADOW_MAP:
    return "VARIANCE SHADOW MAPPING";
  case VistaOpenSGShadow::VOSGSHADOW_ERROR:
    return "ERROR";
  default:
    return "";
  }
}

void ShadowDemo::SetActiveShadowMode(VistaOpenSGShadow::eShadowMode mode) {
  m_pShadow->SetShadowMode(mode);
}

VistaOpenSGShadow::eShadowMode ShadowDemo::GetActiveShadowMode() {
  return m_pShadow->GetShadowMode();
}

void ShadowDemo::ScaleShadowMapResolution(const float nFactor) {
  m_nMapSize = (int)((float)m_nMapSize * nFactor);
  if (m_nMapSize < 8)
    m_nMapSize = 8;
  m_pShadow->SetMapSize(m_nMapSize);
  vstr::outi() << "Setting Shadow Map size to " << m_nMapSize << "x" << m_nMapSize << std::endl;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "ShadowDemo.cpp"                                              */
/*============================================================================*/
