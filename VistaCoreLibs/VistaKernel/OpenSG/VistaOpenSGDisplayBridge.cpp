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

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(disable : 4231)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#endif

#ifdef WIN32
#include <winsock2.h> // sigh, we dont need it. but glew includes windows.h, which may not be included
                      // before winsocks, whcih may be included by oculus sdk, so...
#endif
#include <GL/glew.h>
#include <GL/gl.h>

#include "VistaOpenSGDisplayBridge.h"

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaUtilityMacros.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaTools/VistaFileSystemDirectory.h>
#include <VistaTools/VistaFileSystemFile.h>

#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/DisplayManager/VistaDisplay.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/DisplayManager/VistaWindowingToolkit.h>
#ifdef VISTA_WITH_GLUT
#include <VistaKernel/DisplayManager/GlutWindowImp/VistaGlutWindowingToolkit.h>
#endif
#ifdef VISTA_WITH_OSG
#include <VistaKernel/DisplayManager/OpenSceneGraphWindowImp/VistaOSGWindowingToolkit.h>
#endif

#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGTextForeground.h>

#include <OpenSG/OSGColorMaskChunk.h>
#include <OpenSG/OSGDrawAction.h>
#include <OpenSG/OSGFileGrabForeground.h>
#include <OpenSG/OSGMatrixCamera.h>
#include <OpenSG/OSGNode.h>
#include <OpenSG/OSGPassiveBackground.h>
#include <OpenSG/OSGPerspectiveCamera.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGSimpleAttachments.h>
#include <OpenSG/OSGSolidBackground.h>
#include <OpenSG/OSGStereoBufferViewport.h>

#include "VistaBase/VistaTimer.h"
#include "VistaOpenSGViewportShaders.h"
#include <VistaKernel/GraphicsManager/VistaGLTexture.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaImage.h>

#ifdef VISTA_WITH_OCULUSSDK
#include "VistaKernel/DisplayManager/OculusGlutWindowImp/VistaOculusGlutWindowingToolkit.h"
#include <OVR.h>
#include <OVR_CAPI_GL.h>
#endif
#ifdef VISTA_WITH_OPENVR
#include "VistaKernel/DisplayManager/OpenVRGlutWindowImp/VistaOpenVRGlutWindowingToolkit.h"
#include <openvr/openvr.h>
#include <openvr/openvr_capi.h>
#endif
#include "VistaAspects/VistaObserver.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;

/*============================================================================*/
/*  DISPLAY SYSTEM HELPER CLASS IMPLEMENTATION                                */
/*============================================================================*/

namespace {
// helper function to sort viewports
bool ViewportSortCompare(const VistaViewport* pLeft, const VistaViewport* pRight) {
  // order switched, so that high-priority viewports are rendered first
  return (pLeft->GetViewportProperties()->GetPriority() >
          pRight->GetViewportProperties()->GetPriority());
}

#ifdef VISTA_WITH_OCULUSSDK
class OculusViewportData : public VistaOpenSGDisplayBridge::ViewportData {
 public:
  class ProjectionObserver : public IVistaObserver {
   public:
    ProjectionObserver(OculusViewportData* pData)
        : m_pData(pData) {
    }

    virtual void ObserverUpdate(IVistaObserveable* pObserveable, int nMsg, int nTicket) {
      if (nMsg == VistaProjection::VistaProjectionProperties::MSG_CLIPPING_RANGE_CHANGE ||
          nMsg == VistaProjection::VistaProjectionProperties::MSG_PROBABLY_ALL_CHANGED ||
          nMsg == VistaProjection::VistaProjectionProperties::MSG_SETCHANGE) {
        double                                      fNear = 0;
        double                                      fFar  = 0;
        VistaProjection::VistaProjectionProperties* pProps =
            Vista::assert_cast<VistaProjection::VistaProjectionProperties*>(pObserveable);
        pProps->GetClippingRange(fNear, fFar);
        m_pData->UpdateOculusProjection((float)fNear, (float)fFar);
      }
    }
    OculusViewportData* m_pData;
  };

  OculusViewportData()
      : VistaOpenSGDisplayBridge::ViewportData()
      , m_pHmd(NULL)
      , m_pProjectionObserver(NULL) {
    m_pProjectionObserver = new ProjectionObserver(this);
  }

  ~OculusViewportData() {
    delete m_pProjectionObserver;
  }

  void UpdateOculusProjection(float fNear, float fFar) {
    ovrMatrix4f matLeftProjection =
        ovrMatrix4f_Projection(m_apRenderDescs[0].Fov, fNear, fFar, true);
    osg::Matrix matOSGProjection;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        matOSGProjection[j][i] = matLeftProjection.M[i][j];
      }
    }
    beginEditCP(m_pLeftCamera);
    m_pLeftCamera->setProjectionMatrix(matOSGProjection);
    endEditCP(m_pLeftCamera);

    ovrMatrix4f matRightProjection =
        ovrMatrix4f_Projection(m_apRenderDescs[1].Fov, fNear, fFar, true);
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        matOSGProjection[j][i] = matRightProjection.M[i][j];
      }
    }
    beginEditCP(m_pRightCamera);
    m_pRightCamera->setProjectionMatrix(matOSGProjection);
    endEditCP(m_pRightCamera);
  }

  ovrHmd           m_pHmd;
  ovrEyeRenderDesc m_apRenderDescs[2];
  ovrGLTexture     m_aTextures[2];
  ovrTrackingState m_oTrackingState;

  osg::TransformRefPtr    m_pLeftSubTransform;
  osg::TransformRefPtr    m_pRightSubTransform;
  osg::MatrixCameraRefPtr m_pLeftCamera;
  osg::MatrixCameraRefPtr m_pRightCamera;

  ProjectionObserver* m_pProjectionObserver;
};
#endif

#ifdef VISTA_WITH_OPENVR
class OpenVRViewportData : public VistaOpenSGDisplayBridge::ViewportData {
 public:
  class ProjectionObserver : public IVistaObserver {
   public:
    ProjectionObserver(OpenVRViewportData* pData)
        : m_pData(pData) {
    }

    virtual void ObserverUpdate(IVistaObserveable* pObserveable, int nMsg, int nTicket) {
      if (nMsg == VistaProjection::VistaProjectionProperties::MSG_CLIPPING_RANGE_CHANGE ||
          nMsg == VistaProjection::VistaProjectionProperties::MSG_PROBABLY_ALL_CHANGED ||
          nMsg == VistaProjection::VistaProjectionProperties::MSG_SETCHANGE) {
        double                                      fNear = 0;
        double                                      fFar  = 0;
        VistaProjection::VistaProjectionProperties* pProps =
            Vista::assert_cast<VistaProjection::VistaProjectionProperties*>(pObserveable);
        pProps->GetClippingRange(fNear, fFar);
        m_pData->UpdateOpenVRProjection((float)fNear, (float)fFar);
      }
    }
    OpenVRViewportData* m_pData;
  };

  OpenVRViewportData()
      : VistaOpenSGDisplayBridge::ViewportData()
      , m_pVRSystem(NULL)
      , m_pProjectionObserver(NULL) {
    m_pProjectionObserver = new ProjectionObserver(this);
  }

  ~OpenVRViewportData() {
    delete m_pProjectionObserver;
  }

  void UpdateOpenVRProjection(float fNear, float fFar) {
    // ovrMatrix4f matLeftProjection = ovrMatrix4f_Projection( m_apRenderDescs[0].Fov, fNear, fFar,
    // true );

    auto const& matLeftProjection =
        m_pVRSystem->GetProjectionMatrix(vr::EVREye::Eye_Left, fNear, fFar);
    osg::Matrix matOSGProjection;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        matOSGProjection[j][i] = matLeftProjection.m[i][j]; // !!!!!!!TEST: row or column major?????
      }
    }

#ifdef VISTA_OPENVR_INFINITE_REVERSE_PROJECTION
    // If a reverse infinite projection is used, we have to modify the projection matrix.
    // See Equation (4) in https://dx.doi.org/10.18420/vrar2021_12
    float n2               = 2.f * fNear;
    matOSGProjection[2][2] = 1.f;
    matOSGProjection[3][2] = n2;
#endif // VISTA_OPENVR_INFINITE_REVERSE_PROJECTION

    beginEditCP(m_pLeftCamera);
    m_pLeftCamera->setProjectionMatrix(matOSGProjection);
    endEditCP(m_pLeftCamera);

    // ovrMatrix4f matRightProjection = ovrMatrix4f_Projection( m_apRenderDescs[1].Fov, fNear, fFar,
    // true );
    auto const& matRightProjection =
        m_pVRSystem->GetProjectionMatrix(vr::EVREye::Eye_Right, fNear, fFar);
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        matOSGProjection[j][i] =
            matRightProjection.m[i][j]; // !!!!!!!TEST: row or column major?????
      }
    }

#ifdef VISTA_OPENVR_INFINITE_REVERSE_PROJECTION
    // If a reverse infinite projection is used, we have to modify the projection matrix.
    // See Equation (4) in https://dx.doi.org/10.18420/vrar2021_12
    matOSGProjection[2][2] = 1.f;
    matOSGProjection[3][2] = n2;
#endif // VISTA_OPENVR_INFINITE_REVERSE_PROJECTION

    beginEditCP(m_pRightCamera);
    m_pRightCamera->setProjectionMatrix(matOSGProjection);
    endEditCP(m_pRightCamera);
  }

  vr::IVRSystem* m_pVRSystem;
  // ovrEyeRenderDesc m_apRenderDescs[2];
  // ovrGLTexture m_aTextures[2];
  // ovrTrackingState m_oTrackingState;

  osg::TransformRefPtr    m_pLeftSubTransform;
  osg::TransformRefPtr    m_pRightSubTransform;
  osg::MatrixCameraRefPtr m_pLeftCamera;
  osg::MatrixCameraRefPtr m_pRightCamera;

  ProjectionObserver* m_pProjectionObserver;
};
#endif
} // namespace

#ifdef DEBUG
#define CHECKFORGLERROR(sMessage)                                                                  \
  {                                                                                                \
    GLuint nError = glGetError();                                                                  \
    if (nError != GL_NO_ERROR) {                                                                   \
      vstr::warnp() << "GLError at " << __FILE__ << ":" << __LINE__ << " - " << sMessage << "\n";  \
      vstr::warn() << vstr::singleindent << gluErrorString(nError) << std::endl;                   \
    }                                                                                              \
  }
#else
#define CHECKFORGLERROR(sMessage)
#endif

// #############################################################################
// LOCAL CLASSES
// #############################################################################

// ##########################
// CDisplaySystemData
// ##########################

VistaOpenSGDisplayBridge::DisplaySystemData::DisplaySystemData()
    : IVistaDisplayEntityData()
    , m_v3LeftEyeOffset()
    , m_v3RightEyeOffset()
    , m_bLocalViewer(false)
    , m_bHMDModeActive(false) {
  m_ptrPlatformBeacon = osg::makeCoredNode<osg::Transform>();
  osg::setName(m_ptrPlatformBeacon, "PlatformBeacon");

  m_ptrCameraBeacon = osg::makeCoredNode<osg::Transform>();
  osg::setName(m_ptrCameraBeacon, "CameraBeacon");
}

VistaOpenSGDisplayBridge::DisplaySystemData::~DisplaySystemData() {
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetPlatformTransformation(
    const VistaTransformMatrix& matrix) {
  osg::Matrix m;
  m[0][0] = matrix[0][0];
  m[0][1] = matrix[1][0];
  m[0][2] = matrix[2][0];
  m[0][3] = matrix[3][0];
  m[1][0] = matrix[0][1];
  m[1][1] = matrix[1][1];
  m[1][2] = matrix[2][1];
  m[1][3] = matrix[3][1];
  m[2][0] = matrix[0][2];
  m[2][1] = matrix[1][2];
  m[2][2] = matrix[2][2];
  m[2][3] = matrix[3][2];
  m[3][0] = matrix[0][3];
  m[3][1] = matrix[1][3];
  m[3][2] = matrix[2][3];
  m[3][3] = matrix[3][3];

  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore());
  beginEditCP(t, osg::Transform::MatrixFieldMask);
  t->setMatrix(m);
  endEditCP(t, osg::Transform::MatrixFieldMask);
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetPlatformTranslation(
    const VistaVector3D& v3Pos) {
  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore());
  beginEditCP(t, osg::Transform::MatrixFieldMask);
  osg::Pnt3f& trans = (osg::Pnt3f&)t->getMatrix()[3];
  trans[0]          = v3Pos[0];
  trans[1]          = v3Pos[1];
  trans[2]          = v3Pos[2];
  endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaVector3D VistaOpenSGDisplayBridge::DisplaySystemData::GetPlatformTranslation() const {
  osg::TransformPtr t     = osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore());
  osg::Pnt3f&       trans = (osg::Pnt3f&)t->getMatrix()[3];

  return VistaVector3D(trans[0], trans[1], trans[2]);
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetPlatformOrientation(
    const VistaQuaternion& qOri) {
  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore());
  osg::Matrix       m(t->getMatrix());
  osg::Vec3f        trans, scale;
  osg::Quaternion   ori, scale_ori;
  m.getTransform(trans, ori, scale, scale_ori); // clean but slow
  VistaAxisAndAngle aaa = qOri.GetAxisAndAngle();
  ori =
      osg::Quaternion(osg::Vec3f(aaa.m_v3Axis[0], aaa.m_v3Axis[1], aaa.m_v3Axis[2]), aaa.m_fAngle);
  m.setTransform(trans, ori, scale, scale_ori);

  beginEditCP(t, osg::Transform::MatrixFieldMask);
  t->setMatrix(m);
  endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaQuaternion VistaOpenSGDisplayBridge::DisplaySystemData::GetPlatformOrientation() const {
  osg::Matrix     m(osg::TransformPtr::dcast(m_ptrPlatformBeacon->getCore())->getMatrix());
  osg::Vec3f      trans, scale;
  osg::Quaternion ori, scale_ori;
  m.getTransform(trans, ori, scale, scale_ori); // clean but slow
  /** @todo: remove conversion to/from quaternion */
  float ax, ay, az, ang;
  ori.getValueAsAxisRad(ax, ay, az, ang);
  return VistaQuaternion(VistaAxisAndAngle(VistaVector3D(ax, ay, az), ang));
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetCameraPlatformTransformation(
    const VistaTransformMatrix& matrix) {
  osg::Matrix m;
  m[0][0] = matrix[0][0];
  m[0][1] = matrix[1][0];
  m[0][2] = matrix[2][0];
  m[0][3] = matrix[3][0];
  m[1][0] = matrix[0][1];
  m[1][1] = matrix[1][1];
  m[1][2] = matrix[2][1];
  m[1][3] = matrix[3][1];
  m[2][0] = matrix[0][2];
  m[2][1] = matrix[1][2];
  m[2][2] = matrix[2][2];
  m[2][3] = matrix[3][2];
  m[3][0] = matrix[0][3];
  m[3][1] = matrix[1][3];
  m[3][2] = matrix[2][3];
  m[3][3] = matrix[3][3];

  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());

  beginEditCP(t, osg::Transform::MatrixFieldMask);
  t->setMatrix(m);
  endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaTransformMatrix
VistaOpenSGDisplayBridge::DisplaySystemData::GetCameraPlatformTransformation() const {
  osg::TransformPtr    t = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());
  osg::Matrix          m(t->getMatrix());
  VistaTransformMatrix oTransMat;
  oTransMat[0][0] = m[0][0];
  oTransMat[0][1] = m[1][0];
  oTransMat[0][2] = m[2][0];
  oTransMat[0][3] = m[3][0];
  oTransMat[1][0] = m[0][1];
  oTransMat[1][1] = m[1][1];
  oTransMat[1][2] = m[2][1];
  oTransMat[1][3] = m[3][1];
  oTransMat[2][0] = m[0][2];
  oTransMat[2][1] = m[1][2];
  oTransMat[2][2] = m[2][2];
  oTransMat[2][3] = m[3][2];
  oTransMat[3][0] = m[0][3];
  oTransMat[3][1] = m[1][3];
  oTransMat[3][2] = m[2][3];
  oTransMat[3][3] = m[3][3];

  return oTransMat;
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetCameraPlatformTranslation(
    const VistaVector3D& v3Pos) {
  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());
  beginEditCP(t, osg::Transform::MatrixFieldMask);
  osg::Pnt3f& trans = (osg::Pnt3f&)t->getMatrix()[3];
  trans[0]          = v3Pos[0];
  trans[1]          = v3Pos[1];
  trans[2]          = v3Pos[2];
  endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaVector3D VistaOpenSGDisplayBridge::DisplaySystemData::GetCameraPlatformTranslation() const {
  osg::TransformPtr t     = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());
  osg::Pnt3f&       trans = (osg::Pnt3f&)t->getMatrix()[3];

  return VistaVector3D(trans[0], trans[1], trans[2]);
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetCameraPlatformOrientation(
    const VistaQuaternion& qOri) {
  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore());
  osg::Matrix       m(t->getMatrix());
  osg::Vec3f        trans, scale;
  osg::Quaternion   ori, scale_ori;
  m.getTransform(trans, ori, scale, scale_ori); // clean but slow
  VistaAxisAndAngle aaa = qOri.GetAxisAndAngle();
  ori =
      osg::Quaternion(osg::Vec3f(aaa.m_v3Axis[0], aaa.m_v3Axis[1], aaa.m_v3Axis[2]), aaa.m_fAngle);
  m.setTransform(trans, ori, scale, scale_ori);

  beginEditCP(t, osg::Transform::MatrixFieldMask);
  t->setMatrix(m);
  endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaQuaternion VistaOpenSGDisplayBridge::DisplaySystemData::GetCameraPlatformOrientation() const {
  osg::Matrix     m(osg::TransformPtr::dcast(m_ptrCameraBeacon->getCore())->getMatrix());
  osg::Vec3f      trans, scale;
  osg::Quaternion ori, scale_ori;
  m.getTransform(trans, ori, scale, scale_ori); // clean but slow
  float ax, ay, az, ang;
  ori.getValueAsAxisRad(ax, ay, az, ang);
  return VistaQuaternion(VistaAxisAndAngle(VistaVector3D(ax, ay, az), ang));
}

void VistaOpenSGDisplayBridge::DisplaySystemData::SetHMDModeActive(const bool bSet) {
  m_bHMDModeActive = bSet;
}

bool VistaOpenSGDisplayBridge::DisplaySystemData::GetHMDModeActive() const {
  return m_bHMDModeActive;
}

VistaVector3D VistaOpenSGDisplayBridge::DisplaySystemData::GetLeftEyeOffset() const {
  return m_v3LeftEyeOffset;
}

VistaVector3D VistaOpenSGDisplayBridge::DisplaySystemData::GetRightEyeOffset() const {
  return m_v3RightEyeOffset;
}

// ##########################
// CDisplayData
// ##########################

VistaOpenSGDisplayBridge::DisplayData::DisplayData()
    : m_sDisplayName("") {
}

VistaOpenSGDisplayBridge::DisplayData::DisplayData(const string& sDisplayName)
    : m_sDisplayName(sDisplayName) {
}

std::string VistaOpenSGDisplayBridge::DisplayData::GetDisplayName() const {
  return m_sDisplayName;
}

// ##########################
// CWindowData
// ##########################

// WindowData::WindowObserver;

class VistaOpenSGDisplayBridge::WindowData::WindowObserver : public IVistaObserver {
 public:
  WindowObserver(VistaWindow* pWindow, VistaOpenSGDisplayBridge* pBridge)
      : m_pWindow(pWindow)
      , m_pBridge(pBridge) {
    pWindow->GetWindowProperties()->AttachObserver(this);
  }
  ~WindowObserver() {
    if (m_pWindow)
      m_pWindow->GetWindowProperties()->DetachObserver(this);
  }

  virtual bool Observes(IVistaObserveable* pObserveable) {
    if (m_pWindow == NULL)
      return false;
    return pObserveable == m_pWindow->GetWindowProperties();
  }
  virtual void Observe(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    // no thanks
  }
  virtual void ObserveableDelete(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    m_pWindow = NULL;
  }
  virtual void ObserverUpdate(IVistaObserveable* pObserveable, int nMsg, int nTicket) {
    if (nMsg == VistaWindow::VistaWindowProperties::MSG_SIZE_CHANGE)
      m_pBridge->OnWindowSizeUpdate(m_pWindow);
  }
  virtual void ReleaseObserveable(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    if (m_pWindow)
      m_pWindow->GetWindowProperties()->DetachObserver(this);
    m_pWindow = NULL;
  }

  virtual bool ObserveableDeleteRequest(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    return true;
  }

 private:
  VistaWindow*              m_pWindow;
  VistaOpenSGDisplayBridge* m_pBridge;
};

VistaOpenSGDisplayBridge::WindowData::WindowData()
    : m_ptrWindow(osg::NullFC)
    , m_pObserver(NULL) {
}

VistaOpenSGDisplayBridge::WindowData::~WindowData() {
  delete m_pObserver;
}

void VistaOpenSGDisplayBridge::WindowData::ObserveWindow(
    VistaWindow* pWindow, VistaOpenSGDisplayBridge* pBridge) {
  if (m_pObserver)
    delete m_pObserver;
  m_pObserver = new WindowObserver(pWindow, pBridge);
}

osg::WindowPtr VistaOpenSGDisplayBridge::WindowData::GetOpenSGWindow() const {
  return m_ptrWindow;
}

// ##########################
// CViewportData
// ##########################

VistaOpenSGDisplayBridge::ViewportData::ViewportData()
    : IVistaDisplayEntityData()
    , m_Viewport(osg::NullFC)
    , m_RightViewport(osg::NullFC)
    , m_TextForeground(osg::NullFC)
    , m_pOverlays(osg::NullFC)
    , m_oBitmaps(osg::NullFC)
    , m_pSolidBackground(osg::NullFC)
    , m_pPassiveBackground(osg::NullFC)
    , m_bHasPassiveBackground(false)
    , m_bIsEnabled(true)
    , m_eType(VistaViewport::VistaViewportProperties::VT_MONO)
    , m_nPriority(0)
    , m_bUseTextureSwapping(true)
    , m_bTextureSizeSetExplicitely(false)
    , m_bResizeWithWindow(true)
    , m_fRelativeViewportToWindowWidth(1.0f)
    , m_fRelativeViewportToWindowHeight(1.0f)
    , m_bRepositionWithWindow(true)
    , m_fRelativeViewportToWindowPosX(0.0f)
    , m_fRelativeViewportToWindowPosY(0.0f)
    , m_bIsPerformingWindowResizeUpdate(false) {
  for (int i = 0; i < 2; ++i) {
    m_pReadTextureTarget[i]  = NULL;
    m_pWriteTextureTarget[i] = NULL;
    m_nFBOId[i]              = 0;
    m_nDepthBufferId[i]      = 0;
  }
}

VistaOpenSGDisplayBridge::ViewportData::~ViewportData() {
  // cleanup RTT data
  ClearRenderTargets();
}

bool VistaOpenSGDisplayBridge::ViewportData::GetStereo() const {
  return m_bStereo;
}

osg::ViewportPtr VistaOpenSGDisplayBridge::ViewportData::GetOpenSGViewport() const {
  return m_Viewport;
}

osg::ViewportPtr VistaOpenSGDisplayBridge::ViewportData::GetOpenSGRightViewport() const {
  return m_RightViewport;
}

osg::VistaOpenSGTextForegroundPtr
VistaOpenSGDisplayBridge::ViewportData::GetTextForeground() const {
  return m_TextForeground;
}

osg::VistaOpenSGGLOverlayForegroundPtr
VistaOpenSGDisplayBridge::ViewportData::GetOverlayForeground() const {
  return m_pOverlays;
}

osg::SolidBackgroundPtr VistaOpenSGDisplayBridge::ViewportData::GetSolidBackground() const {
  return m_pSolidBackground;
}

osg::PassiveBackgroundPtr VistaOpenSGDisplayBridge::ViewportData::GetPassiveBackground() const {
  return m_pPassiveBackground;
}

void VistaOpenSGDisplayBridge::ViewportData::ReplaceViewport(
    osg::ViewportPtr pNewPort, bool bCopyData, bool bRight) {
  osg::ViewportPtr pOldPort = bRight ? m_RightViewport : m_Viewport;

  if (bCopyData) {
    beginEditCP(pNewPort);
    {
      pNewPort->setLeft(pOldPort->getLeft());
      pNewPort->setBottom(pOldPort->getBottom());
      pNewPort->setRight(pOldPort->getRight());
      pNewPort->setTop(pOldPort->getTop());
      pNewPort->setBackground(pOldPort->getBackground());
      pNewPort->setCamera(pOldPort->getCamera());
      pNewPort->setRoot(pOldPort->getRoot());
      for (osg::UInt32 i = 0; i < pOldPort->getForegrounds().size(); ++i) {
        pNewPort->getForegrounds().push_back(pOldPort->getForegrounds()[i]);
      }
    }
    endEditCP(pNewPort);
  }

  osg::WindowPtr pParent = pOldPort->getParent();
  beginEditCP(pParent, osg::Window::PortFieldMask);
  pParent->replacePortBy(pOldPort, pNewPort);
  endEditCP(pParent, osg::Window::PortFieldMask);

  if (bRight) {
    m_RightViewport = pNewPort;
  } else {
    m_Viewport = pNewPort;
  }
}

void VistaOpenSGDisplayBridge::ViewportData::OnWindowResize(
    VistaViewport* pViewport, const int nNewWindowWidth, const int nNewWindowHeight) {
  if (m_bIsPerformingWindowResizeUpdate)
    return;

  m_bIsPerformingWindowResizeUpdate = true;
  if (m_bResizeWithWindow) {
    int nViewportWidth  = m_fRelativeViewportToWindowWidth * nNewWindowWidth;
    int nViewportHeight = m_fRelativeViewportToWindowHeight * nNewWindowHeight;
    pViewport->GetViewportProperties()->SetSize(nViewportWidth, nViewportHeight);
  }
  if (m_bRepositionWithWindow) {
    int nViewportPosX = m_fRelativeViewportToWindowPosX * nNewWindowWidth;
    int nViewportPosY = m_fRelativeViewportToWindowPosY * nNewWindowHeight;
    pViewport->GetViewportProperties()->SetPosition(nViewportPosX, nViewportPosY);
  }
  m_bIsPerformingWindowResizeUpdate = false;
}

void VistaOpenSGDisplayBridge::ViewportData::SetupRenderTargets(
    const int nWidth, const int nHeight) {
  ClearRenderTargets(); // @IMGTODO: better: incremental update

  CHECKFORGLERROR("SetupRenderTargets::OnEntry");

  glewInit(); // @IMGTODO: where to put?
  assert(glewGetExtension("GL_EXT_framebuffer_object") == GL_TRUE);
  assert(__glewGenFramebuffersEXT != NULL);

  CHECKFORGLERROR("SetupRenderTargets::PostGLEWInit");

  int nCount = (m_bStereo ? 2 : 1);

  glGenFramebuffers(nCount, m_nFBOId);
  glGenRenderbuffers(nCount, m_nDepthBufferId);

  CHECKFORGLERROR("SetupRenderTargets::PostGenBuffers");

  for (int i = 0; i < nCount; ++i) {
    if (m_bUseTextureSwapping) {
      m_pReadTextureTarget[i] = new VistaGLTexture();
      m_pReadTextureTarget[i]->Set2DData(nWidth, nHeight, NULL, false);
      m_pReadTextureTarget[i]->SetMinFilter(GL_LINEAR);
      m_pReadTextureTarget[i]->SetMagFilter(GL_LINEAR);
    }
    m_pWriteTextureTarget[i] = new VistaGLTexture();
    m_pWriteTextureTarget[i]->Set2DData(
        nWidth, nHeight, NULL, false, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    m_pWriteTextureTarget[i]->SetMinFilter(GL_LINEAR);
    m_pWriteTextureTarget[i]->SetMagFilter(GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOId[i]);

    CHECKFORGLERROR("SetupRenderTargets::FramebufferBind");

    if (m_bStencilBufferEnabled) {
      glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthBufferId[i]);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, nWidth, nHeight);
      glFramebufferRenderbuffer(
          GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_nDepthBufferId[i]);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
    } else {
      glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthBufferId[i]);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, nWidth, nHeight);
      glFramebufferRenderbuffer(
          GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nDepthBufferId[i]);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    CHECKFORGLERROR("SetupRenderTargets::PostBufferSetup");

    GLenum nStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (nStatus != GL_FRAMEBUFFER_COMPLETE) {
      vstr::warnp()
          << "[VistaOpenSGDisplayBridge]: SetupRenderTargets reports incomplete framebuffer status"
          << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CHECKFORGLERROR("SetupRenderTargets::End");
  }
}

void VistaOpenSGDisplayBridge::ViewportData::ClearRenderTargets() {
  for (int i = 0; i < 2; ++i) {
    CHECKFORGLERROR("ViewportData::DTOR::Entry");
    if (m_pReadTextureTarget[i]) {
      delete m_pReadTextureTarget[i];
      delete m_pWriteTextureTarget[i];
      glDeleteFramebuffers(1, &m_nFBOId[i]);
      glDeleteRenderbuffers(1, &m_nDepthBufferId[i]);
    }
    CHECKFORGLERROR("ViewportData::DTOR::exit");
  }
}

void VistaOpenSGDisplayBridge::ViewportData::UpdateRenderTargetSize(
    const int nWidth, const int nHeight) {
  if (m_pWriteTextureTarget[0] == NULL)
    return;
  int nCount = (m_bStereo ? 2 : 1);

  CHECKFORGLERROR("UpdateRenderTargetSize::entry");

  for (int i = 0; i < nCount; ++i) {

    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOId[i]);

    if (m_bUseTextureSwapping)
      m_pReadTextureTarget[i]->Set2DData(
          nWidth, nHeight, NULL, false, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
    m_pWriteTextureTarget[i]->Set2DData(
        nWidth, nHeight, NULL, false, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
    // glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    // m_pWriteTextureTarget[i]->GetGLId(), 0 );

    if (m_bStencilBufferEnabled) {
      glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthBufferId[i]);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, nWidth, nHeight);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
    } else {
      glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthBufferId[i]);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, nWidth, nHeight);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    CHECKFORGLERROR("UpdateRenderTargetSize::setup");

    GLenum nStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (nStatus != GL_FRAMEBUFFER_COMPLETE) {
      vstr::warnp() << "[VistaOpenSGDisplayBridge]: UpdateRenderTargetSize reports incomplete "
                       "framebuffer status"
                    << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CHECKFORGLERROR("UpdateRenderTargetSize::finish");
  }
}

void VistaOpenSGDisplayBridge::ViewportData::BindRenderTargets(const int nIndex) {
  CHECKFORGLERROR("VistaViewport::BindRenderTargets PreCheck");

  glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOId[nIndex]);
  m_nMinFilterState = m_pWriteTextureTarget[nIndex]->GetMinFilter();
  m_pWriteTextureTarget[nIndex]->SetMinFilter(GL_NEAREST);
  m_nMagFilterState = m_pWriteTextureTarget[nIndex]->GetMagFilter();
  m_pWriteTextureTarget[nIndex]->SetMagFilter(GL_NEAREST);
  glFramebufferTexture(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_pWriteTextureTarget[nIndex]->GetGLId(), 0);

#ifdef DEBUG
  GLenum nStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (nStatus != GL_FRAMEBUFFER_COMPLETE) {
    vstr::warnp()
        << "[VistaOpenSGDisplayBridge]: BindRenderTargets reports incomplete framebuffer status"
        << std::endl;
  }
#endif

  CHECKFORGLERROR("VistaViewport::BindRenderTargets");
}

void VistaOpenSGDisplayBridge::ViewportData::UnbindRenderTargets(const int nIndex) {
  CHECKFORGLERROR("VistaViewport::UnbindRenderTargets PreCheck");

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  m_pWriteTextureTarget[nIndex]->SetMinFilter(m_nMinFilterState);
  m_pWriteTextureTarget[nIndex]->SetMagFilter(m_nMagFilterState);

  if (m_bUseTextureSwapping)
    m_pWriteTextureTarget[nIndex]->SwapTextures(*m_pReadTextureTarget[nIndex]);

  CHECKFORGLERROR("VistaViewport::UnbindRenderTargets");
}

VistaGLTexture* VistaOpenSGDisplayBridge::ViewportData::GetMainTextureTarget() const {
  if (m_bUseTextureSwapping)
    return m_pReadTextureTarget[0];
  else
    return m_pWriteTextureTarget[0];
}

VistaGLTexture* VistaOpenSGDisplayBridge::ViewportData::GetSecondaryTextureTarget() const {
  if (m_bUseTextureSwapping)
    return m_pReadTextureTarget[1];
  else
    return m_pWriteTextureTarget[1];
}

void VistaOpenSGDisplayBridge::ViewportData::DrawViewportQuad(
    const bool bSplit, const bool bSplitIsLeft) {
  if (bSplit == false) {
    glViewport(m_nRTTViewportSizePosX, m_nRTTViewportSizePosY, m_nRTTViewportSizeWidth,
        m_nRTTViewportSizeHeight);
    glScissor(m_nRTTViewportSizePosX, m_nRTTViewportSizePosY, m_nRTTViewportSizeWidth,
        m_nRTTViewportSizeHeight);
  } else {
    int nOffset = (bSplitIsLeft ? 0 : (m_nRTTViewportSizeWidth / 2));
    glViewport(m_nRTTViewportSizePosX + nOffset, m_nRTTViewportSizePosY,
        m_nRTTViewportSizeWidth / 2, m_nRTTViewportSizeHeight);
    glScissor(m_nRTTViewportSizePosX + nOffset, m_nRTTViewportSizePosY, m_nRTTViewportSizeWidth / 2,
        m_nRTTViewportSizeHeight);
  }
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(0, 0);
  glTexCoord2f(1, 0);
  glVertex2f(1, 0);
  glTexCoord2f(1, 1);
  glVertex2f(1, 1);
  glTexCoord2f(0, 1);
  glVertex2f(0, 1);
  glEnd();
  glPopAttrib();
}

// ##########################
// CProjectionData
// ##########################

VistaOpenSGDisplayBridge::ProjectionData::ProjectionData()
    : m_dLeft(-1)
    , m_dRight(1)
    , m_dBottom(-1)
    , m_dTop(1)
    , m_iStereoMode(VistaProjection::VistaProjectionProperties::SM_MONO)
    , m_v3NormalVector(0, 0, 1)
    , m_v3UpVector(0, 1, 0) {
  m_ptrBeacon = osg::makeCoredNode<osg::Transform>();
  setName(m_ptrBeacon, "Beacon");

  m_ptrEyeBeacon = osg::makeCoredNode<osg::Transform>();
  setName(m_ptrEyeBeacon, "EyeBeacon");

  m_ptrRightEyeBeacon = osg::makeCoredNode<osg::Transform>();
  setName(m_ptrRightEyeBeacon, "RightEyeBeacon");

  m_ptrCamDeco      = osg::ProjectionCameraDecorator::create();
  m_ptrRightCamDeco = osg::ProjectionCameraDecorator::create();
}

VistaOpenSGDisplayBridge::ProjectionData::~ProjectionData() {
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetLeftEye() const {
  return m_v3LeftEye;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetRightEye() const {
  return m_v3RightEye;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetMidPoint() const {
  return m_v3MidPoint;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetNormalVector() const {
  return m_v3NormalVector;
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetUpVector() const {
  return m_v3UpVector;
}

double VistaOpenSGDisplayBridge::ProjectionData::GetLeft() const {
  return m_dLeft;
}

double VistaOpenSGDisplayBridge::ProjectionData::GetRight() const {
  return m_dRight;
}

double VistaOpenSGDisplayBridge::ProjectionData::GetBottom() const {
  return m_dBottom;
}

double VistaOpenSGDisplayBridge::ProjectionData::GetTop() const {
  return m_dTop;
}

int VistaOpenSGDisplayBridge::ProjectionData::GetStereoMode() const {
  return m_iStereoMode;
}

void VistaOpenSGDisplayBridge::ProjectionData::SetCameraTransformation(
    const VistaVector3D& v3CamPos, const VistaQuaternion& qCamOri) {
  VistaTransformMatrix matrix(qCamOri, v3CamPos);

  osg::Matrix m;
  m[0][0] = matrix[0][0];
  m[0][1] = matrix[1][0];
  m[0][2] = matrix[2][0];
  m[0][3] = matrix[3][0];
  m[1][0] = matrix[0][1];
  m[1][1] = matrix[1][1];
  m[1][2] = matrix[2][1];
  m[1][3] = matrix[3][1];
  m[2][0] = matrix[0][2];
  m[2][1] = matrix[1][2];
  m[2][2] = matrix[2][2];
  m[2][3] = matrix[3][2];
  m[3][0] = matrix[0][3];
  m[3][1] = matrix[1][3];
  m[3][2] = matrix[2][3];
  m[3][3] = matrix[3][3];

  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());
  beginEditCP(t, osg::Transform::MatrixFieldMask);
  t->setMatrix(m);
  endEditCP(t, osg::Transform::MatrixFieldMask);

  // osg::Vec3f trans, scale;
  // osg::Quaternion ori, scale_ori;
  // m.getTransform( trans, ori, scale, scale_ori ); // clean but slow
  // trans = osg::Vec3f( v3CamPos[0], v3CamPos[1], v3CamPos[2] );
  // VistaAxisAndAngle aaa = qCamOri.GetAxisAndAngle();
  // ori = osg::Quaternion( osg::Vec3f(aaa.m_v3Axis[0],aaa.m_v3Axis[1],aaa.m_v3Axis[2]),
  // aaa.m_fAngle ); m.setTransform( trans, ori, scale, scale_ori );

  // osg::TransformPtr t = osg::Transform::create();
  // beginEditCP(t);
  // t->setMatrix(m);
  // endEditCP(t);

  // beginEditCP(m_Beacon, osg::Node::CoreFieldMask );
  // m_Beacon->setCore(t);
  // endEditCP(m_Beacon, osg::Node::CoreFieldMask );
}

void VistaOpenSGDisplayBridge::ProjectionData::GetCameraTransformation(
    VistaVector3D& v3CamPos, VistaQuaternion& qCamOri) const {
  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());
  osg::Matrix       m(t->getMatrix());
  osg::Vec3f        trans, scale;
  osg::Quaternion   ori, scale_ori;
  /// @todo use the faster householder decomposition?
  m.getTransform(trans, ori, scale, scale_ori); // clean but slow
  v3CamPos[0] = trans[0];
  v3CamPos[1] = trans[1];
  v3CamPos[2] = trans[2];

  float ax, ay, az, ang;
  ori.getValueAsAxisRad(ax, ay, az, ang);
  qCamOri = VistaQuaternion(VistaAxisAndAngle(VistaVector3D(ax, ay, az), ang));
}

void VistaOpenSGDisplayBridge::ProjectionData::SetCameraTranslation(const VistaVector3D& v3CamPos) {
  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());
  osg::Matrix       m(t->getMatrix());

  m.setTranslate(v3CamPos[0], v3CamPos[1], v3CamPos[2]);

  beginEditCP(t, osg::Transform::MatrixFieldMask);
  t->setMatrix(m);
  endEditCP(t, osg::Transform::MatrixFieldMask);
}

VistaVector3D VistaOpenSGDisplayBridge::ProjectionData::GetCameraTranslation() const {
  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());
  osg::Pnt3f&       p = (osg::Pnt3f&)t->getMatrix()[3];
  return VistaVector3D(p[0], p[1], p[2]);
}

void VistaOpenSGDisplayBridge::ProjectionData::SetCameraOrientation(
    const VistaQuaternion& qCamOri) {
  osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrBeacon->getCore());

  osg::Matrix     m(t->getMatrix());
  osg::Vec3f      trans, scale;
  osg::Quaternion ori, scale_ori;
  m.getTransform(trans, ori, scale, scale_ori); // clean but slow
  VistaAxisAndAngle aaa = qCamOri.GetAxisAndAngle();
  ori =
      osg::Quaternion(osg::Vec3f(aaa.m_v3Axis[0], aaa.m_v3Axis[1], aaa.m_v3Axis[2]), aaa.m_fAngle);
  m.setTransform(trans, ori, scale, scale_ori);

  beginEditCP(t, osg::Transform::MatrixFieldMask);
  t->setMatrix(m);
  endEditCP(t, osg::Transform::MatrixFieldMask);

  // osg::Matrix invmat;
  // invmat.setRotate( ori );
  // invmat.invert();

  // osg::TransformPtr eye = osg::TransformPtr::dcast(m_ptrEyeBeacon->getCore());
  // beginEditCP(eye, osg::Transform::MatrixFieldMask);
  // eye->setMatrix(invmat);
  // endEditCP(eye, osg::Transform::MatrixFieldMask);

  // if( m_ptrRightCamDeco )
  //{
  //	osg::TransformPtr righteye = osg::TransformPtr::dcast(m_ptrRightEyeBeacon->getCore());
  //	beginEditCP(righteye, osg::Transform::MatrixFieldMask);
  //	righteye->setMatrix(invmat);
  //	endEditCP(righteye, osg::Transform::MatrixFieldMask);
  //}
}

VistaQuaternion VistaOpenSGDisplayBridge::ProjectionData::GetCameraOrientation() const {
  osg::Matrix     m(osg::TransformPtr::dcast(m_ptrBeacon->getCore())->getMatrix());
  osg::Vec3f      trans, scale;
  osg::Quaternion ori, scale_ori;
  m.getTransform(trans, ori, scale, scale_ori); // clean but slow
  float ax, ay, az, ang;
  ori.getValueAsAxisRad(ax, ay, az, ang);
  return VistaQuaternion(VistaAxisAndAngle(VistaVector3D(ax, ay, az), ang));
}

void VistaOpenSGDisplayBridge::ProjectionData::SetProjectionPlane() {
  m_v3UpVector.Normalize();
  m_v3NormalVector.Normalize();
  VistaVector3D v3Right = m_v3UpVector.Cross(m_v3NormalVector);

  // compute corners of projection plane in WCS
  VistaVector3D v3Edges[4] = {
      m_v3MidPoint + m_v3UpVector * (float)m_dBottom + v3Right * (float)m_dLeft,  // bottom left
      m_v3MidPoint + m_v3UpVector * (float)m_dBottom + v3Right * (float)m_dRight, // bottom right
      m_v3MidPoint + m_v3UpVector * (float)m_dTop + v3Right * (float)m_dRight,    // top right
      m_v3MidPoint + m_v3UpVector * (float)m_dTop + v3Right * (float)m_dLeft      // top left
  };

  // transfer to OpenSG
  osg::Pnt3f pProjPlaneEdges[4];
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 3; ++j)
      pProjPlaneEdges[i][j] = v3Edges[i][j];

  beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::SurfaceFieldMask);
  m_ptrCamDeco->getSurface().clear();
  // push the points defining the projection surface's edges
  // MIND THE ORDER OF HOW THE EDGES ARE PUSHED...
  m_ptrCamDeco->getSurface().push_back(pProjPlaneEdges[0]);
  m_ptrCamDeco->getSurface().push_back(pProjPlaneEdges[1]);
  m_ptrCamDeco->getSurface().push_back(pProjPlaneEdges[2]);
  m_ptrCamDeco->getSurface().push_back(pProjPlaneEdges[3]);
  endEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::SurfaceFieldMask);

  // debug output
  // vstr::warnp() << "##############################"
  //	<< "\n" << pProjPlaneEdges[0][0] << ", " << pProjPlaneEdges[0][1] << ", " <<
  //pProjPlaneEdges[0][2]
  //	<< "\n" << pProjPlaneEdges[1][0] << ", " << pProjPlaneEdges[1][1] << ", " <<
  //pProjPlaneEdges[1][2]
  //	<< "\n" << pProjPlaneEdges[2][0] << ", " << pProjPlaneEdges[2][1] << ", " <<
  //pProjPlaneEdges[2][2]
  //	<< "\n" << pProjPlaneEdges[3][0] << ", " << pProjPlaneEdges[3][1] << ", " <<
  //pProjPlaneEdges[3][2]
  //	<< std::endl;

  beginEditCP(m_ptrRightCamDeco, osg::ProjectionCameraDecorator::SurfaceFieldMask);
  m_ptrRightCamDeco->getSurface().clear();
  // push the points defining the projection surface's edges
  // MIND THE ORDER OF HOW THE EDGES ARE PUSHED...
  m_ptrRightCamDeco->getSurface().push_back(pProjPlaneEdges[0]);
  m_ptrRightCamDeco->getSurface().push_back(pProjPlaneEdges[1]);
  m_ptrRightCamDeco->getSurface().push_back(pProjPlaneEdges[2]);
  m_ptrRightCamDeco->getSurface().push_back(pProjPlaneEdges[3]);
  endEditCP(m_ptrRightCamDeco, osg::ProjectionCameraDecorator::SurfaceFieldMask);
}

void VistaOpenSGDisplayBridge::ProjectionData::SetEyes(
    const VistaVector3D& v3LeftOffset, const VistaVector3D& v3RightOffset) {
  m_v3LeftEye  = v3LeftOffset;
  m_v3RightEye = v3RightOffset;

  switch (m_iStereoMode) {
  case VistaProjection::VistaProjectionProperties::SM_MONO: {
    beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
    m_ptrCamDeco->setUser(m_ptrEyeBeacon);
    endEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
  } break;
  case VistaProjection::VistaProjectionProperties::SM_LEFT_EYE: {
    osg::Matrix m = osg::Matrix::identity();
    m.setTranslate(m_v3LeftEye[0], m_v3LeftEye[1], m_v3LeftEye[2]);

    osg::TransformPtr t = osg::TransformPtr::dcast(m_ptrEyeBeacon->getCore());
    assert(t != osg::NullFC);
    t = osg::Transform::create();
    beginEditCP(m_ptrEyeBeacon, osg::Node::CoreFieldMask);
    m_ptrEyeBeacon->setCore(t);
    endEditCP(m_ptrEyeBeacon, osg::Node::CoreFieldMask);

    beginEditCP(t, osg::Transform::MatrixFieldMask);
    t->setMatrix(m);
    endEditCP(t, osg::Transform::MatrixFieldMask);

    beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
    m_ptrCamDeco->setUser(m_ptrEyeBeacon);
    endEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
  } break;
  case VistaProjection::VistaProjectionProperties::SM_RIGHT_EYE: {
    osg::Matrix m = osg::Matrix::identity();
    m.setTranslate(m_v3RightEye[0], m_v3RightEye[1], m_v3RightEye[2]);

    osg::TransformPtr t = osg::Transform::create();
    beginEditCP(t, osg::Transform::MatrixFieldMask);
    t->setMatrix(m);
    endEditCP(t, osg::Transform::MatrixFieldMask);

    beginEditCP(m_ptrEyeBeacon, osg::Node::CoreFieldMask);
    m_ptrEyeBeacon->setCore(t);
    endEditCP(m_ptrEyeBeacon, osg::Node::CoreFieldMask);

    beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
    m_ptrCamDeco->setUser(m_ptrEyeBeacon);
    endEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
  } break;
  case VistaProjection::VistaProjectionProperties::SM_FULL_STEREO: {
    // left eye
    osg::Matrix m = osg::Matrix::identity();
    m.setTranslate(m_v3LeftEye[0], m_v3LeftEye[1], m_v3LeftEye[2]);
    osg::TransformPtr t = osg::Transform::create();

    beginEditCP(t, osg::Transform::MatrixFieldMask);
    t->setMatrix(m);
    endEditCP(t, osg::Transform::MatrixFieldMask);

    beginEditCP(m_ptrEyeBeacon, osg::Node::CoreFieldMask);
    m_ptrEyeBeacon->setCore(t);
    endEditCP(m_ptrEyeBeacon, osg::Node::CoreFieldMask);

    beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
    m_ptrCamDeco->setUser(m_ptrEyeBeacon);
    endEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);

    // right eye
    osg::Matrix mr = osg::Matrix::identity();
    mr.setTranslate(m_v3RightEye[0], m_v3RightEye[1], m_v3RightEye[2]);
    osg::TransformPtr tr = osg::Transform::create();

    beginEditCP(tr, osg::Transform::MatrixFieldMask);
    tr->setMatrix(mr);
    endEditCP(tr, osg::Transform::MatrixFieldMask);

    beginEditCP(m_ptrRightEyeBeacon, osg::Node::CoreFieldMask);
    m_ptrRightEyeBeacon->setCore(tr);
    endEditCP(m_ptrRightEyeBeacon, osg::Node::CoreFieldMask);

    beginEditCP(m_ptrRightCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
    m_ptrRightCamDeco->setUser(m_ptrRightEyeBeacon);
    endEditCP(m_ptrRightCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);

    // av006ce: since the ProjectionCameraDecorator
    //          inherits from the StereoCameraDecorator we
    //          could also use the following code instead
    //          of the EyeBeacons. But we would be restricted
    //          to keep the users eyes on the X-axis (literally)
    // pData->m_Camera->setEyeSeparation( -0.03 );
    // pData->m_Camera->setLeftEye( true  );
    // pData->m_RightCamera->setEyeSeparation(  0.03 );
    // pData->m_RightCamera->setLeftEye( false );
  } break;
  default: {
    vstr::warnp() << "Unknown stereo mode -> defaulting to mono mode" << std::endl;
    beginEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
    m_ptrCamDeco->setUser(m_ptrBeacon);
    endEditCP(m_ptrCamDeco, osg::ProjectionCameraDecorator::UserFieldMask);
  } break;
  }
}

// ##########################
// VistaOpenSG2DBitmap
// ##########################

VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::VistaOpenSG2DBitmap(
    IVistaDisplayBridge* pBr, const std::string& sVpName)
    : Vista2DBitmap(pBr)
    , m_sVpName(sVpName)
    , m_oImage(osg::NullFC) {
}

VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::~VistaOpenSG2DBitmap() {
}

bool VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::SetBitmap(const std::string& strNewFName) {
  osg::ImageRefPtr oImage(osg::Image::create());

  std::string sF = VistaFileSystemDirectory::GetCurrentWorkingDirectory() +
                   VistaFileSystemDirectory::GetOSSpecificSeparator() + strNewFName;

  if (oImage->read(sF.c_str()) == true) {
    VistaOpenSGDisplayBridge* pBr = static_cast<VistaOpenSGDisplayBridge*>(m_pDispBridge);
    if (pBr->ReplaceForegroundImage(m_sVpName, m_oImage, oImage, m_fXPos, m_fYPos) == true) {
      m_oImage = oImage;
    }
    return true;
  }

  return false;
}

bool VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::GetDimensions(int& nWidth, int& nHeight) {
  if (m_oImage != osg::NullFC) {
    nWidth  = int(m_oImage->getWidth());
    nHeight = int(m_oImage->getHeight());
    return true;
  }
  return false;
}

bool VistaOpenSGDisplayBridge::VistaOpenSG2DBitmap::SetPosition(float fPosX, float fPosY) {
  // set position in members
  Vista2DDrawingObject::SetPosition(fPosX, fPosY);

  VistaOpenSGDisplayBridge* pBr = static_cast<VistaOpenSGDisplayBridge*>(m_pDispBridge);

  // refcnt the image so it will not get deleted
  bool bRet = pBr->ReplaceForegroundImage(m_sVpName, m_oImage, m_oImage, m_fXPos, m_fYPos);

  return bRet;
}

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaOpenSGDisplayBridge::VistaOpenSGDisplayBridge(
    osg::RenderAction* pRenderAction, osg::NodePtr pRealRoot)
    : m_bShowCursor(true)
    , m_pWindowingToolkit(NULL)
    , m_pDisplayManager(NULL)
    , m_pRealRoot(pRealRoot)
    , m_pRenderAction(pRenderAction)
    , m_bOculusOffsetPositionRequiresInitialization(true)
    , m_bOculusOffsetOrientationRequiresInitialization(true)
    , m_pRenderInfo(new VistaDisplayManager::RenderInfo()) {
}

VistaOpenSGDisplayBridge::~VistaOpenSGDisplayBridge() {
  delete m_pWindowingToolkit;
}

void VistaOpenSGDisplayBridge::SetDisplayManager(VistaDisplayManager* pDisplayManager) {
  m_pDisplayManager = pDisplayManager;
}

VistaDisplayManager* VistaOpenSGDisplayBridge::GetDisplayManager() const {
  return m_pDisplayManager;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

class ColorTraverse {
 public:
  ColorTraverse(bool bR, bool bG, bool bB, bool bA)
      : m_bR(bR)
      , m_bG(bG)
      , m_bB(bB)
      , m_bA(bA) {
  }
  void Traverse(osg::NodePtr oNode) {
    osg::traverse(oNode,
        osg::osgTypedMethodFunctor1ObjPtrCPtrRef<osg::Action::ResultE, ColorTraverse, osg::NodePtr>(
            this, &ColorTraverse::Enter),
        osg::osgTypedMethodFunctor2ObjPtrCPtrRef<osg::Action::ResultE, ColorTraverse, osg::NodePtr,
            osg::Action::ResultE>(this, &ColorTraverse::Leave));
  }

  OSG::Action::ResultE Enter(osg::NodePtr& oNode) {
    if (oNode->getCore()->getType().isDerivedFrom(osg::MaterialDrawable::getClassType())) {
      OSG::MaterialDrawablePtr oMat      = osg::MaterialDrawablePtr::dcast(oNode->getCore());
      osg::ChunkMaterialPtr    pMaterial = osg::ChunkMaterialPtr::dcast(oMat->getMaterial());
      if (pMaterial == osg::NullFC) {
        // create new one
        pMaterial = osg::ChunkMaterial::create();
        beginEditCP(oMat);
        oMat->setMaterial(pMaterial);
        endEditCP(oMat);
      }
      osg::ColorMaskChunkPtr pChunk =
          osg::ColorMaskChunkPtr::dcast(pMaterial->find(osg::ColorMaskChunk::getClassType()));
      if (pChunk == osg::NullFC) {
        beginEditCP(pMaterial);
        pChunk = osg::ColorMaskChunk::create();
        pMaterial->addChunk(pChunk);
        endEditCP(pMaterial);
      }

      beginEditCP(pChunk);
      pChunk->setMask(m_bR, m_bG, m_bB, m_bA);
      endEditCP(pChunk);
    }
    return OSG::Action::Continue;
  }
  OSG::Action::ResultE Leave(osg::NodePtr& oNode, osg::Action::ResultE oRes) {
    return oRes;
  }

  bool m_bR, m_bG, m_bB, m_bA;
};

bool VistaOpenSGDisplayBridge::DrawFrame() {
  // render to all windows in the display manager
  const std::map<std::string, VistaWindow*>& mapWindows = m_pDisplayManager->GetWindowsConstRef();
  std::map<std::string, VistaWindow*>::const_reverse_iterator itWindow = mapWindows.rbegin();
  for (; itWindow != mapWindows.rend(); ++itWindow) {
    (*itWindow).second->Render();
    m_pWindowingToolkit->UnbindWindow((*itWindow).second);
  }

  return true;
}

bool VistaOpenSGDisplayBridge::RenderWindow(VistaWindow* pWindow) {
  m_pRenderInfo->m_pWindow = pWindow;
  m_pWindowingToolkit->BindWindow(pWindow);
  WindowData* pData = static_cast<WindowData*>(pWindow->GetData());

  osg::PassiveWindowPtr pOSGWindow = pData->m_ptrWindow;

  // pData->m_ptrWindow->render(m_pRenderAction);
  // instead of rendering the window, we manually render our viewports, so that we can
  // do RTT, shader viewports etc.
  m_pRenderAction->setWindow(pData->GetOpenSGWindow().getCPtr());

  // pOSGWindow->render( m_pRenderAction );
  pOSGWindow->activate();
  pOSGWindow->frameInit(); // query recently registered GL extensions

  // pOSGWindow->renderAllViewports(action);
  // @IMGTODO: sort for priority, somewhere else
  std::vector<VistaViewport*> vecViewports = pWindow->GetViewports();
  std::sort(vecViewports.begin(), vecViewports.end(), ViewportSortCompare);
  for (std::vector<VistaViewport*>::iterator itViewport = vecViewports.begin();
       itViewport != vecViewports.end(); ++itViewport) {
    if ((*itViewport)->GetViewportProperties()->GetIsEnabled())
      (*itViewport)->Render();
  }

  pOSGWindow->swap();
  pOSGWindow->frameExit();

  m_pWindowingToolkit->UnbindWindow(pWindow);

  m_pRenderInfo->m_pWindow = NULL;
  return true; // @IMGTODO: proper return or none at all
}

void VistaOpenSGDisplayBridge::FillRenderInfo(VistaViewport* pViewport,
    osg::ViewportPtr                                         pOsgViewport,
    const VistaDisplayManager::RenderInfo::EyeRenderMode     eMode) const {
  m_pRenderInfo->m_pViewport      = pViewport;
  m_pRenderInfo->m_eEyeRenderMode = eMode;

  int           nWidth  = pOsgViewport->getPixelWidth();
  int           nHeight = pOsgViewport->getPixelHeight();
  osg::Matrix4f matProjectionMatrix, matProjectionTrans;
  pOsgViewport->getCamera()->getProjection(matProjectionMatrix, nWidth, nHeight);
  pOsgViewport->getCamera()->getProjectionTranslation(matProjectionTrans, nWidth, nHeight);
  matProjectionMatrix.mult(matProjectionTrans);
  for (int nRow = 0; nRow < 4; ++nRow) {
    for (int nCol = 0; nCol < 4; ++nCol) {
      m_pRenderInfo->m_matProjection[nRow][nCol] = matProjectionMatrix[nCol][nRow];
    }
  }
  osg::Matrix4f matViewMatrix;
  pOsgViewport->getCamera()->getViewing(matViewMatrix, nWidth, nHeight);
  for (int nRow = 0; nRow < 4; ++nRow) {
    for (int nCol = 0; nCol < 4; ++nCol) {
      m_pRenderInfo->m_matCameraTransform[nRow][nCol] = matViewMatrix[nCol][nRow];
    }
  }
}

void VistaOpenSGDisplayBridge::FillRenderInfo(
    VistaViewport* pViewport, osg::ViewportPtr pOsgViewport) const {
  int eMode = pViewport->GetProjection()->GetProjectionProperties()->GetStereoMode();
  if (eMode == VistaProjection::VistaProjectionProperties::SM_LEFT_EYE)
    FillRenderInfo(pViewport, pOsgViewport, VistaDisplayManager::RenderInfo::ERM_LEFT);
  else if (eMode == VistaProjection::VistaProjectionProperties::SM_RIGHT_EYE)
    FillRenderInfo(pViewport, pOsgViewport, VistaDisplayManager::RenderInfo::ERM_RIGHT);
  else
    FillRenderInfo(pViewport, pOsgViewport, VistaDisplayManager::RenderInfo::ERM_MONO);
}

bool VistaOpenSGDisplayBridge::RenderViewport(VistaViewport* pViewport) {

  ViewportData*    pData              = static_cast<ViewportData*>(pViewport->GetData());
  osg::ViewportPtr pMainViewport      = pData->GetOpenSGViewport();
  osg::ViewportPtr pSecondaryViewport = pData->GetOpenSGRightViewport();

  VistaViewport::CustomRenderAction* pCustomRender = pViewport->GetCustomRenderAction();

  switch (pData->GetType()) {
  case VistaViewport::VistaViewportProperties::VT_MONO: {
    assert(pMainViewport);

    FillRenderInfo(pViewport, pMainViewport);

    if (pCustomRender)
      pCustomRender->DoRender(pData, false);
    else
      pMainViewport->render(m_pRenderAction);
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_QUADBUFFERED_STEREO: {
    // @IMGTODO: buffer sets currently not needed - remove stereo from osg viewports
    glDrawBuffer(GL_BACK_LEFT);
    glReadBuffer(GL_BACK_LEFT);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_LEFT);
    assert(pMainViewport);
    if (pCustomRender)
      pCustomRender->DoRender(pData, false);
    else
      pMainViewport->render(m_pRenderAction);
    glDrawBuffer(GL_BACK_RIGHT);
    glReadBuffer(GL_BACK_RIGHT);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_RIGHT);
    assert(pSecondaryViewport);
    if (pCustomRender)
      pCustomRender->DoRender(pData, true);
    else
      pSecondaryViewport->render(m_pRenderAction);
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_RENDER_TO_TEXTURE: {
    assert(pMainViewport);
    FillRenderInfo(pViewport, pMainViewport);
    pData->BindRenderTargets(0);
    if (pCustomRender)
      pCustomRender->DoRender(pData, false);
    else
      pMainViewport->render(m_pRenderAction);
    pData->UnbindRenderTargets(0);
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_POSTPROCESS: {
    assert(pMainViewport);
    FillRenderInfo(pViewport, pMainViewport);
    pData->BindRenderTargets(0);
    if (pCustomRender)
      pCustomRender->DoRender(pData, false);
    else
      pMainViewport->render(m_pRenderAction);
    pData->UnbindRenderTargets(0);

    pData->m_pPostProcessShader->activate(m_pRenderAction);
    pData->GetMainTextureTarget()->Bind(GL_TEXTURE0);
    pData->DrawViewportQuad();
    pData->GetMainTextureTarget()->Release(GL_TEXTURE0);
    pData->m_pPostProcessShader->deactivate(m_pRenderAction);

    break;
  }
  case VistaViewport::VistaViewportProperties::VT_ANAGLYPH:
  case VistaViewport::VistaViewportProperties::VT_ANAGLYPH_MONOCHROME:
  case VistaViewport::VistaViewportProperties::VT_POSTPROCESS_STEREO: {
    pData->BindRenderTargets(0);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_LEFT);
    if (pCustomRender)
      pCustomRender->DoRender(pData, false);
    else
      pMainViewport->render(m_pRenderAction);
    pData->UnbindRenderTargets(0);

    pData->BindRenderTargets(1);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_RIGHT);
    if (pCustomRender)
      pCustomRender->DoRender(pData, true);
    else
      pSecondaryViewport->render(m_pRenderAction);
    pData->UnbindRenderTargets(1);

    // pData->GetMainTextureTarget()->LoadTextureDataToImage().WriteToFile( "ana_l.jpg" );
    // pData->GetSecondaryTextureTarget()->LoadTextureDataToImage().WriteToFile( "ana_r.jpg" );

    pData->m_pPostProcessShader->activate(m_pRenderAction);
    pData->GetMainTextureTarget()->Bind(GL_TEXTURE0);
    pData->GetSecondaryTextureTarget()->Bind(GL_TEXTURE1);
    pData->DrawViewportQuad();
    pData->GetMainTextureTarget()->Release(GL_TEXTURE0);
    pData->GetSecondaryTextureTarget()->Release(GL_TEXTURE1);
    pData->m_pPostProcessShader->deactivate(m_pRenderAction);

    break;
  }
  case VistaViewport::VistaViewportProperties::VT_POSTPROCESS_QUADBUFFERED_STEREO: {
    pData->BindRenderTargets(0);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_LEFT);
    if (pCustomRender)
      pCustomRender->DoRender(pData, false);
    else
      pMainViewport->render(m_pRenderAction);
    pData->UnbindRenderTargets(0);

    pData->BindRenderTargets(1);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_RIGHT);
    if (pCustomRender)
      pCustomRender->DoRender(pData, true);
    else
      pSecondaryViewport->render(m_pRenderAction);
    pData->UnbindRenderTargets(1);

    pData->m_pPostProcessShader->activate(m_pRenderAction);

    // @IMGTODO: this part is copy pasted from the quadbuffer stereo case.
    // There it was mentioned that buffer sets currently are not needed
    // is this the same here?
    glDrawBuffer(GL_BACK_LEFT);
    glReadBuffer(GL_BACK_LEFT);

    pData->GetMainTextureTarget()->Bind(GL_TEXTURE0);
    pData->DrawViewportQuad();
    pData->GetMainTextureTarget()->Release(GL_TEXTURE0);

    glDrawBuffer(GL_BACK_RIGHT);
    glReadBuffer(GL_BACK_RIGHT);

    pData->GetSecondaryTextureTarget()->Bind(GL_TEXTURE0);
    pData->DrawViewportQuad();
    pData->GetSecondaryTextureTarget()->Release(GL_TEXTURE0);

    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);

    pData->m_pPostProcessShader->deactivate(m_pRenderAction);

    break;
  }
  case VistaViewport::VistaViewportProperties::VT_OCULUS_RIFT: {
#ifdef VISTA_WITH_OCULUSSDK
    OculusViewportData* pOcuData = Vista::assert_cast<OculusViewportData*>(pData);

    ovrHmd_BeginFrame(pOcuData->m_pHmd, 0);

    static VistaType::microtime nStartTime = VistaTimeUtils::GetStandardTimer().GetMicroTime();
    if (nStartTime > 0 && VistaTimeUtils::GetStandardTimer().GetMicroTime() - nStartTime > .1f) {
      nStartTime = 0;
      ovrHmd_DismissHSWDisplay(pOcuData->m_pHmd);
    }

    ovrVector3f a2v3HmdToEyeViewOffset[2] = {pOcuData->m_apRenderDescs[0].HmdToEyeViewOffset,
        pOcuData->m_apRenderDescs[1].HmdToEyeViewOffset};

    ovrPosef a2EyeRenderPose[2];
    ovrHmd_GetEyePoses(
        pOcuData->m_pHmd, 0, a2v3HmdToEyeViewOffset, a2EyeRenderPose, &pOcuData->m_oTrackingState);

    VistaDisplaySystem::VistaDisplaySystemProperties* pProps =
        pViewport->GetDisplaySystem()->GetDisplaySystemProperties();

    // calculate and set the head pose and eye offset accordingly
    // @IMGTODO: hack - this is definitely not the right way to do it...
    VistaVector3D   v3HeadPosition(pOcuData->m_oTrackingState.HeadPose.ThePose.Position.x,
        pOcuData->m_oTrackingState.HeadPose.ThePose.Position.y,
        pOcuData->m_oTrackingState.HeadPose.ThePose.Position.z);
    VistaQuaternion qHeadOrientation(pOcuData->m_oTrackingState.HeadPose.ThePose.Orientation.x,
        pOcuData->m_oTrackingState.HeadPose.ThePose.Orientation.y,
        pOcuData->m_oTrackingState.HeadPose.ThePose.Orientation.z,
        pOcuData->m_oTrackingState.HeadPose.ThePose.Orientation.w);

    // calculate eye offsets
    VistaQuaternion qInverseHeadOri = qHeadOrientation.GetInverted();
    VistaVector3D   v3LeftEyeOffset(a2EyeRenderPose[0].Position.x, a2EyeRenderPose[0].Position.y,
        a2EyeRenderPose[0].Position.z);
    v3LeftEyeOffset -= v3HeadPosition;
    v3LeftEyeOffset = qInverseHeadOri.Rotate(v3LeftEyeOffset);
    VistaVector3D v3RightEyeOffset(a2EyeRenderPose[1].Position.x, a2EyeRenderPose[1].Position.y,
        a2EyeRenderPose[1].Position.z);
    v3RightEyeOffset -= v3HeadPosition;
    v3RightEyeOffset = qInverseHeadOri.Rotate(v3RightEyeOffset);

    // @IMGTODO hacky
    static VistaQuaternion qOffset;
    static VistaVector3D   v3Offset;
    if (m_bOculusOffsetPositionRequiresInitialization) {
      VistaVirtualPlatform* pPlatform      = pViewport->GetDisplaySystem()->GetReferenceFrame();
      VistaTransformMatrix  matInvNavTrans = pPlatform->GetMatrixInverse();
      v3Offset = matInvNavTrans.Transform(pProps->GetViewerPosition()); // what we should be
      // v3Offset -= v3HeadPosition;
      // vstr::outi() << "Resetting oculus pos offset to " << v3Offset << "(result: " <<
      // v3HeadPosition + v3Offset << " )" << std::endl;
    }
    if (m_bOculusOffsetOrientationRequiresInitialization) {
      VistaVirtualPlatform* pPlatform      = pViewport->GetDisplaySystem()->GetReferenceFrame();
      VistaTransformMatrix  matInvNavTrans = pPlatform->GetMatrixInverse();
      qOffset = matInvNavTrans.Transform(pProps->GetViewerOrientation()); // what we should be
      // qOffset = qOffset * qHeadOrientation.GetInverted();
      // qOffset[0] = 0;
      // qOffset[2] = 0;
      // qOffset.Normalize();
      // vstr::outi() << "Resetting oculus ori offset to " << qOffset << "(result: " << qOffset *
      // qHeadOrientation << " )" << std::endl;
    }
    // vstr::outi() << "Oculus Pos: " << v3HeadPosition << std::endl;
    qHeadOrientation = qOffset * qHeadOrientation;
    v3HeadPosition   = qOffset.Rotate(v3HeadPosition) + v3Offset;
    // vstr::outi() << "Head Pos: " << v3HeadPosition << std::endl;

    pProps->SetViewerPosition(v3HeadPosition);
    pProps->SetViewerOrientation(qHeadOrientation);
    pProps->SetLeftEyeOffset(v3LeftEyeOffset[0], v3LeftEyeOffset[1], v3LeftEyeOffset[2]);
    pProps->SetRightEyeOffset(v3RightEyeOffset[0], v3RightEyeOffset[1], v3RightEyeOffset[2]);

    m_bOculusOffsetPositionRequiresInitialization    = false;
    m_bOculusOffsetOrientationRequiresInitialization = false;

    // matrix cameras ignore beacons, so we have to set the modelview explicitely
    osg::NodePtr pLeftBeacon = pOcuData->m_pLeftCamera->getBeacon();
    osg::Matrix  matLeftModelview(pLeftBeacon->getToWorld());
    matLeftModelview.invert();
    beginEditCP(pOcuData->m_pLeftCamera);
    pOcuData->m_pLeftCamera->setModelviewMatrix(matLeftModelview);
    endEditCP(pOcuData->m_pLeftCamera);

    osg::NodePtr pRightBeacon = pOcuData->m_pRightCamera->getBeacon();
    osg::Matrix  matRightModelview(pRightBeacon->getToWorld());
    matRightModelview.invert();
    beginEditCP(pOcuData->m_pRightCamera);
    pOcuData->m_pRightCamera->setModelviewMatrix(matRightModelview);
    endEditCP(pOcuData->m_pRightCamera);

    assert(pMainViewport);
    pOcuData->BindRenderTargets(0);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_LEFT);
    if (pCustomRender)
      pCustomRender->DoRender(pData, false);
    else
      pMainViewport->render(m_pRenderAction);
    pOcuData->UnbindRenderTargets(0);

    assert(pSecondaryViewport);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_RIGHT);
    pOcuData->BindRenderTargets(1);
    if (pCustomRender)
      pCustomRender->DoRender(pData, true);
    else
      pSecondaryViewport->render(m_pRenderAction);
    pOcuData->UnbindRenderTargets(1);

    // assert( pSecondaryViewport );
    // pOcuData->BindRenderTargets( 1 );
    //( pCustomRender ? pCustomRender->DoRender( pData, true ) : pSecondaryViewport->render(
    //m_pRenderAction ) ); pOcuData->UnbindRenderTargets( 1 );

    // ugh... joy with unions - &pOcuData->m_aTextures[0].Texture basically "reinterpret_cast"s
    // the ovrGLTexture array to a ovrTexture array
    ovrHmd_EndFrame(pOcuData->m_pHmd, a2EyeRenderPose, &pOcuData->m_aTextures[0].Texture);

    glFinish();
#else
    VISTA_THROW(
        "Oculus-based rendering requested, but VistaKernel was not build with OculusSDK support",
        -1);
#endif
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_OCULUS_RIFT_CLONE: {
#ifdef VISTA_WITH_OCULUSSDK
    const std::map<std::string, VistaViewport*>& mapViewports =
        GetDisplayManager()->GetViewportsConstRef();
    VistaViewport* pOculusViewport = NULL;
    for (std::map<std::string, VistaViewport*>::const_iterator itVp = mapViewports.begin();
         itVp != mapViewports.end(); ++itVp) {
      if ((*itVp).second->GetViewportProperties()->GetType() ==
          VistaViewport::VistaViewportProperties::VT_OCULUS_RIFT) {
        pOculusViewport = (*itVp).second;
        break;
      }
    }
    if (pOculusViewport == NULL) {
      vstr::warnp()
          << "RenderViewport: OculusClone Viewport did not find an original oculus viewport"
          << std::endl;
      return false;
    }

    OculusViewportData* pOcuData =
        Vista::assert_cast<OculusViewportData*>(pOculusViewport->GetData());

    int nSizeX, nSizeY, nPosX, nPosY;
    pViewport->GetViewportProperties()->GetSize(nSizeX, nSizeY);
    pViewport->GetViewportProperties()->GetPosition(nPosX, nPosY);

    glViewport(nPosX, nPosY, nSizeX, nSizeY);
    glScissor(nPosX, nPosY, nSizeX, nSizeY);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    pOcuData->GetMainTextureTarget()->Bind(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(1, 0);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glTexCoord2f(0, 1);
    glVertex2f(0, 1);
    glEnd();
    pOcuData->GetMainTextureTarget()->Release(GL_TEXTURE0);
    glPopAttrib();
    break;
#else
    VISTA_THROW(
        "Oculus-based rendering requested, but VistaKernel was not build with OculusSDK support",
        -1);
#endif
  }

  case VistaViewport::VistaViewportProperties::VT_OPENVR: {
#ifdef VISTA_WITH_OPENVR

    OpenVRViewportData* pOpenVRData = Vista::assert_cast<OpenVRViewportData*>(pData);

    VistaDisplaySystem::VistaDisplaySystemProperties* pProps =
        pViewport->GetDisplaySystem()->GetDisplaySystemProperties();

    vr::TrackedDevicePose_t devices[vr::k_unMaxTrackedDeviceCount];
    vr::HmdMatrix34_t       pose;
    auto                    posesError =
        vr::VRCompositor()->WaitGetPoses(devices, vr::k_unMaxTrackedDeviceCount, NULL, 0);
    if (posesError != vr::VRCompositorError_None) {
      vstr::errp() << "Error while trying to get poses: " << posesError << std::endl;
    }

    for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
      if (devices[i].bPoseIsValid) {
        if (pOpenVRData->m_pVRSystem->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_HMD) {
          pose = devices[i].mDeviceToAbsoluteTracking;
          break;
        }
      }
    }

    VistaTransformMatrix orientation(pose.m[0][0], pose.m[1][0], pose.m[2][0], 0.0, pose.m[0][1],
        pose.m[1][1], pose.m[2][1], 0.0, pose.m[0][2], pose.m[1][2], pose.m[2][2], 0.0,
        pose.m[0][3], pose.m[1][3], pose.m[2][3], 1.0);

    orientation = orientation.GetInverted();

    vr::HmdMatrix34_t left_eye =
        pOpenVRData->m_pVRSystem->GetEyeToHeadTransform(vr::EVREye::Eye_Left);
    vr::HmdMatrix34_t right_eye =
        pOpenVRData->m_pVRSystem->GetEyeToHeadTransform(vr::EVREye::Eye_Right);
    VistaVector3D   v3HeadPosition(pose.m[0][3], pose.m[1][3], pose.m[2][3]);
    VistaQuaternion qHeadOrientation(orientation.GetRotationAsQuaternion());

    pProps->SetViewerPosition(v3HeadPosition);
    pProps->SetViewerOrientation(qHeadOrientation);
    pProps->SetLeftEyeOffset(left_eye.m[0][3], left_eye.m[1][3], left_eye.m[2][3]);
    pProps->SetRightEyeOffset(right_eye.m[0][3], right_eye.m[1][3], right_eye.m[2][3]);

    // matrix cameras ignore beacons, so we have to set the modelview explicitely
    osg::NodePtr pLeftBeacon = pOpenVRData->m_pLeftCamera->getBeacon();
    osg::Matrix  matLeftModelview(pLeftBeacon->getToWorld());
    matLeftModelview.invert();
    beginEditCP(pOpenVRData->m_pLeftCamera);
    pOpenVRData->m_pLeftCamera->setModelviewMatrix(matLeftModelview);
    endEditCP(pOpenVRData->m_pLeftCamera);

    osg::NodePtr pRightBeacon = pOpenVRData->m_pRightCamera->getBeacon();
    osg::Matrix  matRightModelview(pRightBeacon->getToWorld());
    matRightModelview.invert();
    beginEditCP(pOpenVRData->m_pRightCamera);
    pOpenVRData->m_pRightCamera->setModelviewMatrix(matRightModelview);
    endEditCP(pOpenVRData->m_pRightCamera);

    assert(pMainViewport);
    pOpenVRData->BindRenderTargets(0);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_LEFT);
    if (pCustomRender)
      pCustomRender->DoRender(pData, false);
    else
      pMainViewport->render(m_pRenderAction);
    pOpenVRData->UnbindRenderTargets(0);

    assert(pSecondaryViewport);
    FillRenderInfo(pViewport, pMainViewport, VistaDisplayManager::RenderInfo::ERM_RIGHT);
    pOpenVRData->BindRenderTargets(1);
    if (pCustomRender)
      pCustomRender->DoRender(pData, true);
    else
      pSecondaryViewport->render(m_pRenderAction);
    pOpenVRData->UnbindRenderTargets(1);

    vr::Texture_t leftEyeTexture{(void*)(pOpenVRData->m_pWriteTextureTarget[0]->GetGLId()),
        vr::ETextureType::TextureType_OpenGL, vr::ColorSpace_Gamma};
    auto          submitError = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
    if (submitError != vr::VRCompositorError_None) {
      vstr::errp() << "Error while submitting left eye texture: " << submitError << std::endl;
    }

    vr::Texture_t rightEyeTexture{(void*)(pOpenVRData->m_pWriteTextureTarget[1]->GetGLId()),
        vr::ETextureType::TextureType_OpenGL, vr::ColorSpace_Gamma};
    submitError = vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
    if (submitError != vr::VRCompositorError_None) {
      vstr::errp() << "Error while submitting right eye texture: " << submitError << std::endl;
    }

    glFlush();

    // Show left eye in mirror window.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, pOpenVRData->m_nFBOId[0]);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    int nSizeX, nSizeY, nPosX, nPosY, nWinSizeX, nWinSizeY;
    pViewport->GetViewportProperties()->GetSize(nSizeX, nSizeY);
    pViewport->GetViewportProperties()->GetPosition(nPosX, nPosY);
    pViewport->GetWindow()->GetWindowProperties()->GetSize(nWinSizeX, nWinSizeY);

    glBlitFramebuffer(
        nPosX, nPosY, nSizeX, nSizeY, 0, 0, nWinSizeX, nWinSizeY, GL_COLOR_BUFFER_BIT, GL_LINEAR);

#else
    VISTA_THROW(
        "OpenVR-based rendering requested, but VistaKernel was not build with OculusSDK support",
        -1);
#endif
    break;
  }

  default:
    VISTA_THROW("VistaOpenSGDisplayBridge::RenderViewport invalid viewport type", -1);
  }

  m_pRenderInfo->m_pViewport = NULL;
  return true;
}

bool VistaOpenSGDisplayBridge::DisplayFrame() {
  // swap all windows in the display manager
  m_pWindowingToolkit->DisplayAllWindows();
  return true;
}

bool VistaOpenSGDisplayBridge::SetDisplayUpdateCallback(
    IVistaExplicitCallbackInterface* pCallback) {
  m_pWindowingToolkit->SetWindowUpdateCallback(pCallback);
  return true;
}

bool VistaOpenSGDisplayBridge::UpdateDisplaySystems(VistaDisplayManager* pDisplayManager) {
  return true; // the OpenSG implementation doesn't need any DisplaySystem updates
}

void VistaOpenSGDisplayBridge::UpdateDisplaySystem(VistaDisplaySystem* pDisplaySystem) {
  // the OpenSG implementation doesn't need any DisplaySystem updates
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   ObserverUpdate                                              */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::ObserverUpdate(
    IVistaObserveable* pObserveable, int msg, int ticket, VistaDisplaySystem* pTarget) {
  if (ticket == VistaDisplaySystem::FRAME_CHANGE) {
    ((DisplaySystemData*)pTarget->GetData())
        ->SetPlatformTransformation(pTarget->GetReferenceFrame()->GetMatrix());

    if (!pTarget->GetDisplaySystemProperties()->GetLocalViewer()) {
      switch (msg) {
      case VistaVirtualPlatform::MSG_TRANSLATION_CHANGE:
        pTarget->GetDisplaySystemProperties()->Notify(
            VistaDisplaySystem::VistaDisplaySystemProperties::MSG_VIEWER_POSITION_CHANGE);
        break;
      case VistaVirtualPlatform::MSG_ROTATION_CHANGE:
        pTarget->GetDisplaySystemProperties()->Notify(
            VistaDisplaySystem::VistaDisplaySystemProperties::MSG_VIEWER_ORIENTATION_CHANGE);
        break;

      default:
        break;
      }
    }
  }
}

VistaDisplaySystem* VistaOpenSGDisplayBridge::CreateDisplaySystem(
    VistaVirtualPlatform* pReferenceFrame, VistaDisplayManager* pDisplayManager,
    const VistaPropertyList& refProps) {
  // create new data container
  DisplaySystemData* pData = new DisplaySystemData;

  // get the real OpenSG root node to attach display system beacons to it
  // this means that display systems and cameras will be hidden for pure ViSTA applications

  beginEditCP(m_pRealRoot);
  m_pRealRoot->addChild(pData->m_ptrPlatformBeacon); // append beacon to root node
  endEditCP(m_pRealRoot);

  beginEditCP(pData->m_ptrPlatformBeacon);
  pData->m_ptrPlatformBeacon->addChild(pData->m_ptrCameraBeacon);
  endEditCP(pData->m_ptrPlatformBeacon);

  VistaDisplaySystem* pDisplaySystem = NewDisplaySystem(pReferenceFrame, pData, pDisplayManager);

  if (!pDisplaySystem) {
    vstr::errp() << " [VistaOpenSGDisplayBridge] - Unable to create display system" << std::endl;
    delete pData;
    return NULL;
  }

  pDisplaySystem->Observe(pReferenceFrame, VistaDisplaySystem::FRAME_CHANGE);
  pDisplaySystem->GetProperties()->SetPropertiesByList(refProps);

  // signal one platform update before the first draw is called
  pDisplaySystem->ObserverUpdate(
      pDisplaySystem->GetReferenceFrame(), 0, VistaDisplaySystem::FRAME_CHANGE);

  return pDisplaySystem;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyDisplaySystem                                        */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::DestroyDisplaySystem(VistaDisplaySystem* pDisplaySystem) {
  if (!pDisplaySystem)
    return true;

  // release reference frame
  VistaVirtualPlatform* pReferenceFrame = pDisplaySystem->GetReferenceFrame();
  pDisplaySystem->ReleaseObserveable(pReferenceFrame, IVistaObserveable::TICKET_NONE);

  // destroy containers
  DisplaySystemData* pData = (DisplaySystemData*)pDisplaySystem->GetData();

  beginEditCP(pData->m_ptrPlatformBeacon);
  pData->m_ptrPlatformBeacon->subChild(pData->m_ptrCameraBeacon); // remove beacon from root node
  endEditCP(pData->m_ptrPlatformBeacon);

  beginEditCP(m_pRealRoot);
  m_pRealRoot->subChild(pData->m_ptrPlatformBeacon); // remove beacon from root node
  endEditCP(m_pRealRoot);

  delete pData;
  delete pDisplaySystem;

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DebugDisplaySystem                                          */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::DebugDisplaySystem(std::ostream& out, VistaDisplaySystem* pTarget) {
  pTarget->Debug(out);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetViewerPosition                                       */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetViewerPosition(
    const VistaVector3D& v3Pos, VistaDisplaySystem* pTarget) {
  if (!pTarget)
    return;

  DisplaySystemData* pData = (DisplaySystemData*)pTarget->GetData();

  // @TODO: hacky hacky hacky
  m_bOculusOffsetPositionRequiresInitialization = true;

  if (pData->GetHMDModeActive()) {
    pData->SetCameraPlatformTranslation(v3Pos);
  } else {
    int i, iViewportCount = pTarget->GetNumberOfViewports();
    if (iViewportCount == 0) {
      // we are in the initialization phase of ViSTA
      // so we store the initial viewer position for the creation
      // of subsequent projections in this display system
      pData->m_v3InitialViewerPosition = v3Pos;
    } else {
      VistaVector3D v3Tmp(v3Pos);
      if (pData->m_bLocalViewer) {
        // pData->SetPlatformTranslation( v3Pos );
        v3Tmp = pTarget->GetReferenceFrame()->TransformPositionToFrame(v3Pos);
      }

      // well, let's iterate over all viewports/projections
      for (i = 0; i < iViewportCount; ++i) {
        ProjectionData* pPData =
            (ProjectionData*)pTarget->GetViewport(i)->GetProjection()->GetData();
        pPData->SetCameraTranslation(v3Tmp);
      }
    }
  }
}

void VistaOpenSGDisplayBridge::GetViewerPosition(
    VistaVector3D& v3Pos, const VistaDisplaySystem* pTarget) {
  if (!pTarget)
    return;

  DisplaySystemData* pData = (DisplaySystemData*)pTarget->GetData();

  if (pData->GetHMDModeActive()) {
    // Currently: Ignores Viewer Position dada in Projection::m_Beacon
    // Should be Unity/Zero
    // 		if( true )
    // 		{
    v3Pos = pData->GetCameraPlatformTranslation();
    // 		}
    // 		else
    // 		{
    // 			if( pTarget->GetNumberOfViewports() > 0 )
    // 			{
    // 				CProjectionData * pPData = (CProjectionData
    // *)pTarget->GetViewport(0)->GetProjection()->GetData(); 				v3Pos =
    // pPData->GetCameraTranslation(); 				v3Pos = pData->GetCameraPlatformTransformation() * v3Pos; 				if(
    // !pData->m_bLocalViewer )
    // 				{
    // 					v3Pos =
    // pTarget->GetReferenceFrame()->TransformPositionFromFrame(v3Pos);
    // 				}
    // 			}
    // 		}
    if (!pData->m_bLocalViewer)
      v3Pos = pTarget->GetReferenceFrame()->TransformPositionFromFrame(v3Pos);
  } else {
    // CProjectionData * pPData = (CProjectionData
    // *)pTarget->GetViewport(0)->GetProjection()->GetData();

    ProjectionData* pPData    = NULL;
    VistaViewport*  pViewport = pTarget->GetViewport(0);
    if (!pViewport)
      return;
    pPData = (ProjectionData*)pViewport->GetProjection()->GetData();

    // take the first viewports/projections position
    if (pTarget->GetNumberOfViewports() > 0) {
      // if( pData->m_bLocalViewer )
      //	v3Pos = pData->GetPlatformTranslation();
      // else

      v3Pos = pPData->GetCameraTranslation();
      if (!pData->m_bLocalViewer)
        v3Pos = pTarget->GetReferenceFrame()->TransformPositionFromFrame(v3Pos);
    }
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetViewerOrientation                                    */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetViewerOrientation(
    const VistaQuaternion& qOrient, VistaDisplaySystem* pTarget) {
  if (!pTarget)
    return;

  // @TODO: hacky hacky hacky
  m_bOculusOffsetOrientationRequiresInitialization = true;

  DisplaySystemData* pData = (DisplaySystemData*)pTarget->GetData();

  if (pData->GetHMDModeActive()) {
    pData->SetCameraPlatformOrientation(qOrient);
  } else {
    int i, iViewportCount = pTarget->GetNumberOfViewports();
    if (iViewportCount == 0) {
      // we are in the initialization phase of ViSTA
      // so we store the initial viewer orientation for the creation
      // of subsequent projections in this display system
      pData->m_qInitialViewerOrientation = qOrient;
    } else {
      VistaQuaternion q(qOrient);

      if (pData->m_bLocalViewer) {
        q = pTarget->GetReferenceFrame()->TransformOrientationToFrame(qOrient);
      }
      // well, let's iterate over all viewports/projections
      for (i = 0; i < iViewportCount; ++i) {
        ProjectionData* pPData =
            (ProjectionData*)pTarget->GetViewport(i)->GetProjection()->GetData();
        pPData->SetCameraOrientation(qOrient);
      }
    }
  }
}

void VistaOpenSGDisplayBridge::GetViewerOrientation(
    VistaQuaternion& qOrient, const VistaDisplaySystem* pTarget) {
  if (!pTarget)
    return;

  DisplaySystemData* pData = (DisplaySystemData*)pTarget->GetData();

  if (pData->GetHMDModeActive()) {
    // Currently: Ignores Viewer Position dada in Projection::m_Beacon
    // Should be Unity/Zero
    // 		if( true )
    // 		{
    qOrient = pData->GetCameraPlatformOrientation();
    // 		}
    // 		else
    // 		{
    // 			if( pTarget->GetNumberOfViewports() > 0 )
    // 			{
    // 				CProjectionData * pPData = (CProjectionData
    // *)pTarget->GetViewport(0)->GetProjection()->GetData(); 				qOrient =
    // pPData->GetCameraOrientation(); 				qOrient = pData->GetCameraPlatformTransformation() * qOrient;
    // 				if( !pData->m_bLocalViewer )
    // 				{
    // 					qOrient =
    // pTarget->GetReferenceFrame()->TransformOrientationFromFrame(qOrient);
    // 				}
    // 			}
    // 		}
    if (!pData->m_bLocalViewer) {
      qOrient = pTarget->GetReferenceFrame()->TransformOrientationFromFrame(qOrient);
    }
  } else {
    // take the first viewports/projections orientation
    if (pTarget->GetNumberOfViewports() > 0) {
      ProjectionData* pPData = (ProjectionData*)pTarget->GetViewport(0)->GetProjection()->GetData();
      qOrient                = pPData->GetCameraOrientation();
      // in case we are not a local viewer, we transform the orientation
      // back to the world reference frame using the reference frame
      // otherwise the camera orientation is about to be enough
      if (!pData->m_bLocalViewer) {
        qOrient = pTarget->GetReferenceFrame()->TransformOrientationFromFrame(qOrient);
      }
    }
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetEyeOffsets                                           */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetEyeOffsets(const VistaVector3D& v3LeftOffset,
    const VistaVector3D& v3RightOffset, VistaDisplaySystem* pTarget) {
  DisplaySystemData* pData  = (DisplaySystemData*)pTarget->GetData();
  pData->m_v3LeftEyeOffset  = v3LeftOffset;
  pData->m_v3RightEyeOffset = v3RightOffset;

  int i, iViewportCount = pTarget->GetNumberOfViewports();
  // well, let's iterate over all viewports/projections
  for (i = 0; i < iViewportCount; ++i) {
    ProjectionData* pPData = (ProjectionData*)pTarget->GetViewport(i)->GetProjection()->GetData();
    pPData->SetEyes(v3LeftOffset, v3RightOffset);
  }
}

void VistaOpenSGDisplayBridge::GetEyeOffsets(
    VistaVector3D& v3LeftOffset, VistaVector3D& v3RightOffset, const VistaDisplaySystem* pTarget) {
  if (!pTarget)
    return;

  DisplaySystemData* pData = (DisplaySystemData*)pTarget->GetData();
  v3LeftOffset             = pData->m_v3LeftEyeOffset;
  v3RightOffset            = pData->m_v3RightEyeOffset;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetLocalViewer                                          */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetLocalViewer(bool bLocalViewer, VistaDisplaySystem* pTarget) {
  DisplaySystemData* pData = (DisplaySystemData*)pTarget->GetData();
  pData->m_bLocalViewer    = bLocalViewer;
}

bool VistaOpenSGDisplayBridge::GetLocalViewer(const VistaDisplaySystem* pTarget) {
  DisplaySystemData* pData = (DisplaySystemData*)pTarget->GetData();
  return pData->m_bLocalViewer;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetHMDModeActive                                        */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetHMDModeActive(bool bSet, VistaDisplaySystem* pTarget) {
  DisplaySystemData* pData = (DisplaySystemData*)pTarget->GetData();
  pData->m_bHMDModeActive  = bSet;
}

bool VistaOpenSGDisplayBridge::GetHMDModeActive(const VistaDisplaySystem* pTarget) {
  DisplaySystemData* pData = (DisplaySystemData*)pTarget->GetData();
  return pData->m_bHMDModeActive;
}

/**
 * Methods for display management.
 */
VistaDisplay* VistaOpenSGDisplayBridge::CreateDisplay(
    VistaDisplayManager* pDisplayManager, const VistaPropertyList& refProps) {
  if (!refProps.HasProperty("DISPLAY_STRING")) {
    vstr::errp()
        << " [VistaOpenSGDisplayBridge - Unable to create display - no display string given"
        << std::endl;
    return NULL;
  }

  string strName;
  refProps.GetValue("DISPLAY_STRING", strName);

  // create new data container
  DisplayData* pData = new DisplayData;

  pData->m_sDisplayName = strName;

  VistaDisplay* pDisplay = NewDisplay(pData, pDisplayManager);
  if (!pDisplay) {
    vstr::errp() << " [VistaOpenSGDisplayBridge] - unable to create display" << std::endl;
    delete pData;
    return NULL;
  }
  // pDisplay->SetPropertiesByList(refProps);
  pDisplay->GetProperties()->SetPropertiesByList(refProps);

  return pDisplay;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyDisplay                                              */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::DestroyDisplay(VistaDisplay* pDisplay) {
  if (!pDisplay)
    return true;

  // destroy the data container but don't change the display strings
  // in order to do so, we would need some form of reference counting...
  IVistaDisplayEntityData* pData = pDisplay->GetData();

  delete pData;
  delete pDisplay;

  return true;
}

void VistaOpenSGDisplayBridge::DebugDisplay(std::ostream& out, const VistaDisplay* pTarget) {
  pTarget->Debug(out);
}

/**
 * Methods for window management.
 */
IVistaWindowingToolkit* VistaOpenSGDisplayBridge::GetWindowingToolkit() const {
  return m_pWindowingToolkit;
}

IVistaWindowingToolkit* VistaOpenSGDisplayBridge::CreateWindowingToolkit(const std::string& sName) {
  if (m_pWindowingToolkit != NULL) {
    vstr::warnp() << "Creating new WIndowingtoolkit, but one already"
                  << " exists - deleting old one!" << std::endl;
    delete m_pWindowingToolkit;
    m_pWindowingToolkit = NULL;
  }

#ifdef VISTA_WITH_GLUT
  if (VistaAspectsComparisonStuff::StringEquals(sName, "GLUT", false)) {
    // compare the string once and store the result as enum
    m_pWindowingToolkit = new VistaGlutWindowingToolkit;
    return m_pWindowingToolkit;
  }
#endif
#ifdef VISTA_WITH_OSG
  if (VistaAspectsComparisonStuff::StringEquals(sName, "OSG", false)) {
    // compare the string once and store the result as enum
    m_pWindowingToolkit = new VistaOSGWindowingToolkit;
    return m_pWindowingToolkit;
  }
#endif
#ifdef VISTA_WITH_OCULUSSDK
  if (VistaAspectsComparisonStuff::StringEquals(sName, "OCULUS", false)) {
    // compare the string once and store the result as enum
    m_pWindowingToolkit = new VistaOculusGlutWindowingToolkit();
    return m_pWindowingToolkit;
  }
#endif
#ifdef VISTA_WITH_OPENVR
  if (VistaAspectsComparisonStuff::StringEquals(sName, "OPENVR", false)) {
    // compare the string once and store the result as enum
    m_pWindowingToolkit = new VistaOpenVRGlutWindowingToolkit();
    return m_pWindowingToolkit;
  }
#endif

  vstr::errp() << "[VistaOpenSGSystemClassFactory::CreateWindowingToolkit] "
               << "Toolkit type [" << sName << "] is unknown." << std::endl;
  return NULL;
}

VistaWindow* VistaOpenSGDisplayBridge::CreateVistaWindow(
    VistaDisplay* pDisplay, const VistaPropertyList& oProps) {
  // create new data container
  WindowData* pData = new WindowData;

  VistaWindow* pVistaWindow = NewWindow(pDisplay, pData);
  if (!pVistaWindow) {
    vstr::errp() << "[VistaOpenSGDisplayBridge]: - unable to create window" << std::endl;
    delete pData;
    return NULL;
  }

  m_pWindowingToolkit->RegisterWindow(pVistaWindow);

  pVistaWindow->GetProperties()->SetPropertiesByList(oProps);

  pData->m_ptrWindow = osg::PassiveWindow::create();
  m_pRenderAction->setWindow(pData->m_ptrWindow.get().getCPtr());

  // finally create the window
  if (m_pWindowingToolkit->InitWindow(pVistaWindow) == false) {
    vstr::errp() << "[VistaOpenSGDisplayBridge]: - unable to initialize window" << std::endl;
    delete pData;
    return NULL;
  }

  pData->m_ptrWindow->init();

  // set mouse cursor visibility
  // to default on start.
  m_pWindowingToolkit->SetCursorIsEnabled(pVistaWindow, GetShowCursor());

  int nWidth, nHeight;
  m_pWindowingToolkit->GetWindowSize(pVistaWindow, nWidth, nHeight);
  pData->m_ptrWindow->setSize(nWidth, nHeight);

  pData->ObserveWindow(pVistaWindow, this);

  return pVistaWindow;
}

bool VistaOpenSGDisplayBridge::DestroyVistaWindow(VistaWindow* pWindow) {
  if (!pWindow)
    return true;

  // destroy data containers
  WindowData* pData = (WindowData*)pWindow->GetData();

  m_pWindowingToolkit->UnregisterWindow(pWindow);

  delete pData;
  vstr::debugi() << "VistaOpenSGDisplayBridge::DestroyVistaWindow(" << pWindow->GetNameForNameable()
                 << ")" << std::endl;
  delete pWindow;

  return true;
}

bool VistaOpenSGDisplayBridge::SetWindowStereo(bool bStereo, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetUseStereo(pTarget, bStereo);
}

bool VistaOpenSGDisplayBridge::GetWindowAccumBufferEnabled(const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetUseAccumBuffer(pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowAccumBufferEnabled(bool bUse, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetUseAccumBuffer(pTarget, bUse);
}

bool VistaOpenSGDisplayBridge::GetWindowStencilBufferEnabled(const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetUseStencilBuffer(pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowStencilBufferEnabled(bool bUse, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetUseStencilBuffer(pTarget, bUse);
}

bool VistaOpenSGDisplayBridge::GetWindowDrawBorder(const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetDrawBorder(pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowDrawBorder(bool bDrawBorder, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetDrawBorder(pTarget, bDrawBorder);
}

bool VistaOpenSGDisplayBridge::GetWindowStereo(const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetUseStereo(pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowPosition(int x, int y, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetWindowPosition(pTarget, x, y);
}

bool VistaOpenSGDisplayBridge::GetWindowPosition(int& x, int& y, const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetWindowPosition(pTarget, x, y);
}

bool VistaOpenSGDisplayBridge::SetWindowSize(int w, int h, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetWindowSize(pTarget, w, h);
}

bool VistaOpenSGDisplayBridge::GetWindowSize(int& w, int& h, const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetWindowSize(pTarget, w, h);
}

int VistaOpenSGDisplayBridge::GetWindowId(const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetWindowId(pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowVSync(bool bEnabled, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetVSyncMode(pTarget, bEnabled);
}
int VistaOpenSGDisplayBridge::GetWindowVSync(const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetVSyncMode(pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowFullScreen(bool bFullScreen, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetFullscreen(pTarget, bFullScreen);
}

bool VistaOpenSGDisplayBridge::GetWindowFullScreen(const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetFullscreen(pTarget);
}

bool VistaOpenSGDisplayBridge::GetWindowIsOffscreenBuffer(const VistaWindow* pWindow) const {
  return m_pWindowingToolkit->GetUseOffscreenBuffer(pWindow);
}

bool VistaOpenSGDisplayBridge::SetWindowIsOffscreenBuffer(VistaWindow* pWindow, const bool bSet) {
  return m_pWindowingToolkit->SetUseOffscreenBuffer(pWindow, bSet);
}

int VistaOpenSGDisplayBridge::GetWindowMultiSamples(const VistaWindow* pWindow) const {
  return m_pWindowingToolkit->GetMultiSamples(pWindow);
}

bool VistaOpenSGDisplayBridge::SetWindowMultiSamples(VistaWindow* pWindow, const int nNumSamples) {
  return m_pWindowingToolkit->SetMultiSamples(pWindow, nNumSamples);
}

bool VistaOpenSGDisplayBridge::GetWindowRGBImage(
    const VistaWindow* pWindow, std::vector<VistaType::byte>& vecData) const {
  return m_pWindowingToolkit->GetRGBImage(pWindow, vecData);
}

int VistaOpenSGDisplayBridge::GetWindowRGBImage(
    const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize) const {
  return m_pWindowingToolkit->GetRGBImage(pWindow, pData, nBufferSize);
}

VistaImage VistaOpenSGDisplayBridge::GetWindowRGBImage(const VistaWindow* pWindow) const {
  return m_pWindowingToolkit->GetRGBImage(pWindow);
}

bool VistaOpenSGDisplayBridge::GetWindowDepthImage(
    const VistaWindow* pWindow, std::vector<VistaType::byte>& vecData) const {
  return m_pWindowingToolkit->GetDepthImage(pWindow, vecData);
}

int VistaOpenSGDisplayBridge::GetWindowDepthImage(
    const VistaWindow* pWindow, VistaType::byte* pData, const int nBufferSize) const {
  return m_pWindowingToolkit->GetDepthImage(pWindow, pData, nBufferSize);
}

VistaImage VistaOpenSGDisplayBridge::GetWindowDepthImage(const VistaWindow* pWindow) const {
  return m_pWindowingToolkit->GetDepthImage(pWindow);
}

bool VistaOpenSGDisplayBridge::GetWindowContextVersion(
    int& nMajor, int& nMinor, const VistaWindow* pTarget) const {
  return m_pWindowingToolkit->GetContextVersion(nMajor, nMinor, pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowContextVersion(
    int nMajor, int nMinor, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetContextVersion(nMajor, nMinor, pTarget);
}

bool VistaOpenSGDisplayBridge::GetWindowIsDebugContext(const VistaWindow* pTarget) const {
  return m_pWindowingToolkit->GetIsDebugContext(pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowIsDebugContext(const bool bIsDebug, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetIsDebugContext(bIsDebug, pTarget);
}

bool VistaOpenSGDisplayBridge::GetWindowIsForwardCompatible(const VistaWindow* pTarget) const {
  return m_pWindowingToolkit->GetIsForwardCompatible(pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowIsForwardCompatible(
    const bool bIsForwardCompatible, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetIsForwardCompatible(bIsForwardCompatible, pTarget);
}

bool VistaOpenSGDisplayBridge::SetWindowTitle(const std::string& sTitle, VistaWindow* pTarget) {
  return m_pWindowingToolkit->SetWindowTitle(pTarget, sTitle);
}

std::string VistaOpenSGDisplayBridge::GetWindowTitle(const VistaWindow* pTarget) {
  return m_pWindowingToolkit->GetWindowTitle(pTarget);
}

void VistaOpenSGDisplayBridge::DebugVistaWindow(std::ostream& out, const VistaWindow* pTarget) {
  pTarget->Debug(out);
}

void VistaOpenSGDisplayBridge::OnWindowSizeUpdate(VistaWindow* pTarget) {
  osg::WindowPtr ptrWindow = dynamic_cast<WindowData*>(pTarget->GetData())->m_ptrWindow;
  int            nWidth, nHeight;
  m_pWindowingToolkit->GetWindowSize(pTarget, nWidth, nHeight);

  beginEditCP(ptrWindow);
  ptrWindow->resize(nWidth, nHeight);
  endEditCP(ptrWindow);

  std::vector<VistaViewport*>& vecViewports = pTarget->GetViewports();
  for (std::vector<VistaViewport*>::iterator itViewport = vecViewports.begin();
       itViewport != vecViewports.end(); ++itViewport) {
    ViewportData* pViewportData = Vista::assert_cast<ViewportData*>((*itViewport)->GetData());
    pViewportData->OnWindowResize((*itViewport), nWidth, nHeight);
  }
}

/**
 * Methods for viewport management.
 */
VistaViewport* VistaOpenSGDisplayBridge::CreateViewport(
    VistaDisplaySystem* pDisplaySystem, VistaWindow* pWindow, const VistaPropertyList& refProps) {
  if (!pDisplaySystem) {
    vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport - "
                 << "no display system given" << std::endl;
    return NULL;
  }

  if (!pWindow) {
    vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport - "
                 << "no window given" << std::endl;
    return NULL;
  }

  GetWindowingToolkit()->BindWindow(pWindow);

  // when creating, the viewport type is always MONO, will be set later
  // for now, we'll just create one standard viewport, either MONO or STEREO,
  // depending on the window
  VistaViewport::VistaViewportProperties::ViewportType eType;
  std::string                                          sTypeName;
  if (refProps.GetValue("TYPE", sTypeName) == false) {
    if (GetWindowStereo(pWindow))
      eType = VistaViewport::VistaViewportProperties::VT_QUADBUFFERED_STEREO;
    else
      eType = VistaViewport::VistaViewportProperties::VT_MONO;
  } else {
    eType = VistaViewport::VistaViewportProperties::GetViewportTypeFromString(sTypeName);
    if (eType == (VistaViewport::VistaViewportProperties::ViewportType)-1) {
      vstr::warnp() << "[VistaOpenSGDisplayBridge]: ViewportType \"" << sTypeName
                    << "\" for viewport \"" << refProps.GetValueOrDefault<std::string>("NAME")
                    << "\" invalid, falling back to MONO" << std::endl;
      eType = VistaViewport::VistaViewportProperties::VT_MONO;
    }
  }

  // create new data container
  // @IMGTODO: hacky... we should use different viewports, really
#ifdef VISTA_WITH_OCULUSSDK
  ViewportData* pData = (eType == VistaViewport::VistaViewportProperties::VT_OCULUS_RIFT)
                            ? new OculusViewportData()
                            : new ViewportData();
#elif VISTA_WITH_OPENVR
  ViewportData* pData = (eType == VistaViewport::VistaViewportProperties::VT_OPENVR)
                            ? new OpenVRViewportData()
                            : new ViewportData();
#else
  ViewportData* pData = new ViewportData();
#endif
  VistaViewport* pViewport = NewViewport(pDisplaySystem, pWindow, pData);
  if (!pViewport) {
    vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport" << std::endl;
    delete pData;
    return NULL;
  }

  // RWTH blue as background color
  pData->m_pSolidBackground = osg::SolidBackground::create();
  beginEditCP(pData->m_pSolidBackground);
  pData->m_pSolidBackground->setColor(osg::Color3f(0.0f, 0.4f, 0.8f));
  endEditCP(pData->m_pSolidBackground);

  pData->m_fRelativeViewportToWindowWidth  = 1.0f;
  pData->m_fRelativeViewportToWindowHeight = 1.0f;
  pData->m_fRelativeViewportToWindowPosX   = 0.0f;
  pData->m_fRelativeViewportToWindowPosY   = 0.0f;

  pData->m_bStereo               = false;
  pData->m_bAccumBufferEnabled   = GetWindowAccumBufferEnabled(pWindow);
  pData->m_bStencilBufferEnabled = GetWindowStencilBufferEnabled(pWindow);
  pData->m_bUseRenderToTexture   = false;

  if (SetViewportType(eType, pViewport) == false) {
    // hrrk! something still went wrong...
    vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport" << std::endl;
    delete pViewport;
    delete pData;
    return NULL;
  }

  VistaPropertyList oCleanedProps = refProps;
  oCleanedProps.RemoveProperty("PROJECTION");
  oCleanedProps.RemoveProperty("WINDOW");
  oCleanedProps.RemoveProperty("TYPE");
  pViewport->GetProperties()->SetPropertiesByList(oCleanedProps);

  return pViewport;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyViewport                                             */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::DestroyViewport(VistaViewport* pViewport) {
  if (!pViewport)
    return true;

  delete pViewport->GetData();
  delete pViewport;

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetViewportPosition                                         */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetViewportPosition(int x, int y, VistaViewport* pTarget) {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());

  if (pData->m_bUseRenderToTexture) {
    pData->m_nRTTViewportSizePosX = x;
    pData->m_nRTTViewportSizePosY = y;
    return;
  }

  osg::ViewportPtr pViewport = pData->m_Viewport;

  float fWidth  = (float)pViewport->getPixelWidth();
  float fHeight = (float)pViewport->getPixelHeight();

  float fLeft   = (float)x;
  float fRight  = fLeft + fWidth - 1;
  float fBottom = (float)y;
  float fTop    = fBottom + fHeight - 1;

  beginEditCP(pViewport);
  pViewport->setSize(fLeft, fBottom, fRight, fTop);
  endEditCP(pViewport);
  assert(pViewport->getPixelWidth() == fWidth);
  assert(pViewport->getPixelHeight() == fHeight);

  if (pData->m_bStereo) {
    pViewport = pData->m_RightViewport;
    beginEditCP(pViewport);
    pViewport->setSize(fLeft, fBottom, fRight, fTop);
    endEditCP(pViewport);
  }

  // we update our relative position, but only if we are not currently resizing due to a window
  // resize, in order to prevent integer sampling changing our relative size
  if (pData->m_bIsPerformingWindowResizeUpdate == false && pTarget->GetWindow() != NULL) {
    int nWindowWidth  = 0;
    int nWindowHeight = 0;
    pTarget->GetWindow()->GetWindowProperties()->GetSize(nWindowWidth, nWindowHeight);
    if (nWindowWidth > 0 && nWindowHeight > 0) {
      pData->m_fRelativeViewportToWindowPosX = (float)x / (float)nWindowWidth;
      pData->m_fRelativeViewportToWindowPosY = (float)y / (float)nWindowHeight;
    }
  }
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportPosition                                         */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::GetViewportPosition(int& x, int& y, const VistaViewport* pTarget) {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());

  if (pData->m_bUseRenderToTexture) {
    x = pData->m_nRTTViewportSizePosX;
    y = pData->m_nRTTViewportSizePosY;
    return;
  }

  osg::ViewportPtr pOsgViewport = pData->m_Viewport;
  x                             = pOsgViewport->getPixelLeft();
  y                             = pOsgViewport->getPixelBottom();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetViewportSize                                             */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetViewportSize(int iWidth, int iHeight, VistaViewport* pTarget) {
  ViewportData*    pData     = static_cast<ViewportData*>(pTarget->GetData());
  osg::ViewportPtr pViewport = pData->GetOpenSGViewport();

  if (pData->m_bUseRenderToTexture) {
    pData->m_nRTTViewportSizeWidth  = iWidth;
    pData->m_nRTTViewportSizeHeight = iHeight;
    if (pData->m_bTextureSizeSetExplicitely == false) {
      // if we are using a texture with the same resolution as our viewport, we
      // adjust it accordingly
      if (pData->m_eType == VistaViewport::VistaViewportProperties::VT_OCULUS_RIFT)
        SetViewportTextureSize(0.5f * iWidth, iHeight, pTarget);
      else
        SetViewportTextureSize(iWidth, iHeight, pTarget);
      pData->m_bTextureSizeSetExplicitely = false;
    }
    // for render to texture, we are done
    return;
  }

  // lower-left is (0,0)
  float fLeft   = (float)pViewport->getPixelLeft();
  float fBottom = (float)pViewport->getPixelBottom();
  float fWidth  = (float)iWidth;
  float fHeight = (float)iHeight;

  // OpenSG uses top and right pixels, which are simply left + 1
  float fRight = fLeft + fWidth - 1;
  float fTop   = fBottom + fHeight - 1;

  beginEditCP(pViewport);
  pViewport->setSize(fLeft, fBottom, fRight, fTop);
  endEditCP(pViewport);

  assert(pViewport->getPixelWidth() == iWidth);
  assert(pViewport->getPixelHeight() == iHeight);

  if (pData->m_bStereo) {
    pViewport = pData->GetOpenSGRightViewport();
    beginEditCP(pViewport);
    pViewport->setSize(fLeft, fBottom, fRight, fTop);
    endEditCP(pViewport);
  }

  // we update our relative size, but only if we are not currently resizing due to a window
  // resize, in order to prevent integer sampling changing our relative size
  if (pData->m_bIsPerformingWindowResizeUpdate == false && pTarget->GetWindow() != NULL) {
    int nWindowWidth  = 0;
    int nWindowHeight = 0;
    pTarget->GetWindow()->GetWindowProperties()->GetSize(nWindowWidth, nWindowHeight);
    if (nWindowWidth > 0 && nWindowHeight > 0) {
      pData->m_fRelativeViewportToWindowWidth  = (float)iWidth / (float)nWindowWidth;
      pData->m_fRelativeViewportToWindowHeight = (float)iHeight / (float)nWindowHeight;
    }
  }
}

void VistaOpenSGDisplayBridge::SetViewportTextureSize(
    int nWidth, int nHeight, VistaViewport* pTarget) {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());

  if (pData->m_bUseRenderToTexture == false)
    return;

  pData->UpdateRenderTargetSize(nWidth, nHeight);

  // lower-left is (0,0)
  float fWidth  = (float)nWidth;
  float fHeight = (float)nHeight;

  osg::ViewportPtr pViewport = pData->GetOpenSGViewport();
  beginEditCP(pViewport);
  pViewport->setSize(0, 0, fWidth - 1, fHeight - 1);
  endEditCP(pViewport);

  if (pData->m_bStereo) {
    pViewport = pData->GetOpenSGRightViewport();
    beginEditCP(pViewport);
    pViewport->setSize(0, 0, fWidth - 1, fHeight - 1);
    endEditCP(pViewport);
  }
}

void VistaOpenSGDisplayBridge::GetViewportTextureSize(
    int& nWidth, int& nHeight, const VistaViewport* pTarget) {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());
  nWidth              = pData->m_nRTTViewportSizeWidth;
  nHeight             = pData->m_nRTTViewportSizeHeight;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportSize                                             */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::GetViewportSize(int& w, int& h, const VistaViewport* pTarget) {
  osg::ViewportPtr viewport = ((ViewportData*)pTarget->GetData())->m_Viewport;
  w                         = viewport->getPixelWidth();
  h                         = viewport->getPixelHeight();
}

void VistaOpenSGDisplayBridge::SetViewportHasPassiveBackground(bool bSet, VistaViewport* pTarget) {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());
  if (bSet) {
    if (pData->m_pPassiveBackground == osg::NullFC)
      pData->m_pPassiveBackground = osg::PassiveBackground::create();

    osg::ViewportPtr pViewport = pData->GetOpenSGViewport();
    beginEditCP(pViewport);
    pViewport->setBackground(pData->m_pPassiveBackground);
    endEditCP(pViewport);

    if (pData->GetStereo()) {
      pViewport = pData->GetOpenSGRightViewport();
      beginEditCP(pViewport);
      pViewport->setBackground(pData->m_pPassiveBackground);
      endEditCP(pViewport);
    }
  } else {
    osg::ViewportPtr pViewport = pData->GetOpenSGViewport();
    beginEditCP(pViewport);
    pViewport->setBackground(pData->m_pSolidBackground);
    endEditCP(pViewport);

    if (pData->GetStereo()) {
      pViewport = pData->GetOpenSGRightViewport();
      beginEditCP(pViewport);
      pViewport->setBackground(pData->m_pSolidBackground);
      endEditCP(pViewport);
    }
  }
  pData->m_bHasPassiveBackground = bSet;
}

bool VistaOpenSGDisplayBridge::GetViewportHasPassiveBackground(const VistaViewport* pTarget) {
  return Vista::assert_cast<ViewportData*>(pTarget->GetData())->m_bHasPassiveBackground;
}

VistaViewport::VistaViewportProperties::ViewportType VistaOpenSGDisplayBridge::GetViewportType(
    const VistaViewport* pTarget) {
  const ViewportData* pData = static_cast<const ViewportData*>(pTarget->GetData());
  return pData->GetType();
}

bool VistaOpenSGDisplayBridge::SetViewportType(
    VistaViewport::VistaViewportProperties::ViewportType eType, VistaViewport* pTarget) {
  ViewportData* pData       = static_cast<ViewportData*>(pTarget->GetData());
  WindowData*   pWindowData = static_cast<WindowData*>(pTarget->GetWindow()->GetData());
  pData->SetType(eType);

  switch (eType) {
  case VistaViewport::VistaViewportProperties::VT_MONO: {
    pData->m_bStereo             = false;
    pData->m_bUseRenderToTexture = false;
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_QUADBUFFERED_STEREO: {
    if (pTarget->GetWindow()->GetWindowProperties()->GetStereo() == false) {
      vstr::warnp()
          << "[VistaViewport::SetType]: Viewport \"" << pTarget->GetNameForNameable()
          << "\" set to QUADBUFFERED_STEREO, but Window is only MONO - reverting Viewport to MONO"
          << std::endl;
      pData->m_bStereo             = false;
      pData->m_bUseRenderToTexture = false;
    } else {
      pData->m_bStereo             = true;
      pData->m_bUseRenderToTexture = false;
    }
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_RENDER_TO_TEXTURE: {
    pData->m_bStereo             = false;
    pData->m_bUseRenderToTexture = true;
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_RENDER_TO_TEXTURE_STEREO: {
    // @TODO: stereo check
    pData->m_bStereo             = true;
    pData->m_bUseRenderToTexture = false;
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_POSTPROCESS: {
    pData->m_bStereo             = false;
    pData->m_bUseRenderToTexture = true;
    pData->m_pPostProcessShader  = osg::SHLChunk::create();
    pData->m_pPostProcessShader->setUniformParameter("texture", GL_TEXTURE0);
    pData->m_pPostProcessShader->setVertexProgram(
        VistaOSGViewportShader::GetStandardVertexShader());
    pData->m_pPostProcessShader->setFragmentProgram(
        VistaOSGViewportShader::GetNullPostProcessFragmentShader());
    pData->m_bUseTextureSwapping = false;
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_POSTPROCESS_STEREO:
  case VistaViewport::VistaViewportProperties::VT_ANAGLYPH_MONOCHROME:
  case VistaViewport::VistaViewportProperties::VT_ANAGLYPH: {
    // @TODO: stereo check
    pData->m_bStereo             = true;
    pData->m_bUseRenderToTexture = true;
    pData->m_pPostProcessShader  = osg::SHLChunk::create();
    pData->m_pPostProcessShader->setUniformParameter("texture_lefteye", 1);
    pData->m_pPostProcessShader->setUniformParameter("texture_righteye", 0);
    pData->m_pPostProcessShader->setVertexProgram(
        VistaOSGViewportShader::GetStandardVertexShader());
    pData->m_bUseTextureSwapping = false;
    if (eType == VistaViewport::VistaViewportProperties::VT_ANAGLYPH) {
      pData->m_pPostProcessShader->setFragmentProgram(
          VistaOSGViewportShader::GetAnaglyphCyanMagentaFragmentShader());
    } else if (eType == VistaViewport::VistaViewportProperties::VT_ANAGLYPH_MONOCHROME) {
      pData->m_pPostProcessShader->setFragmentProgram(
          VistaOSGViewportShader::GetAnaglyphCyanMagentaMonochromeFragmentShader());
    } else
      break;
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_POSTPROCESS_QUADBUFFERED_STEREO: {
    if (pTarget->GetWindow()->GetWindowProperties()->GetStereo() == false) {
      vstr::warnp() << "[VistaViewport::SetType]: Viewport \"" << pTarget->GetNameForNameable()
                    << "\" set to POSTPROCESS_QUADBUFFERED_STEREO, but Window is only MONO - "
                       "reverting Viewport to POSTPROCESS"
                    << std::endl;
      pData->m_bStereo = false;
    } else {
      pData->m_bStereo = true;
    }

    pData->m_bUseRenderToTexture = true;
    pData->m_pPostProcessShader  = osg::SHLChunk::create();
    pData->m_pPostProcessShader->setUniformParameter("texture", 0);
    pData->m_pPostProcessShader->setVertexProgram(
        VistaOSGViewportShader::GetStandardVertexShader());
    pData->m_pPostProcessShader->setFragmentProgram(
        VistaOSGViewportShader::GetNullPostProcessFragmentShader());
    pData->m_bUseTextureSwapping = false;
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_OCULUS_RIFT: {
#ifndef VISTA_WITH_OCULUSSDK
    VISTA_THROW("Cannot create Oculus Viewport - VistaKernel not built with Oculus Support", -1);
#else

    pData->m_bStereo = true;
    pData->m_bUseRenderToTexture = true;
    pData->m_bUseTextureSwapping = false;
#endif
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_OCULUS_RIFT_CLONE: {

    pData->m_bStereo             = false;
    pData->m_bUseRenderToTexture = false;
    break;
  }
  case VistaViewport::VistaViewportProperties::VT_OPENVR: {
#ifndef VISTA_WITH_OPENVR
    VISTA_THROW("Cannot create OpenVR Viewport - VistaKernel not built with Oculus Support", -1);
#else

    pData->m_bStereo = true;
    pData->m_bUseRenderToTexture = true;
    pData->m_bUseTextureSwapping = false;
#endif
    break;
  }
  default:
    VISTA_THROW("Invalid VistaViewport Type", -1);
  };

  int nWindowWidth  = 0;
  int nWindowHeight = 0;
  pTarget->GetWindow()->GetWindowProperties()->GetSize(nWindowWidth, nWindowHeight);

  // @IMGTODO: maybe always use left/right viewport for projection?
  if (pData->m_Viewport == osg::NullFC) {
    // this can only happen during CreateViewport - lets create the viewport now with default params
    osg::ViewportPtr pViewport = osg::Viewport::create();
    beginEditCP(pViewport);
    pViewport->setSize(0, 0, nWindowWidth - 1, nWindowHeight - 1);
    pViewport->setBackground(pData->m_pSolidBackground);
    pViewport->setRoot(m_pRealRoot);
    endEditCP(pViewport);

    osg::beginEditCP(pWindowData->m_ptrWindow);
    pWindowData->m_ptrWindow->addPort(pViewport);
    osg::endEditCP(pWindowData->m_ptrWindow);

    pData->m_Viewport = pViewport;
    if (pData->m_bUseRenderToTexture) {
      pData->m_nRTTViewportSizePosX   = 0;
      pData->m_nRTTViewportSizePosY   = 0;
      pData->m_nRTTViewportSizeWidth  = pViewport->getPixelWidth();
      pData->m_nRTTViewportSizeHeight = pViewport->getPixelHeight();
    }
  }
  if (pData->m_bStereo) {
    // we now need a stereo viewport, we'll copy its setting from our first
    osg::ViewportPtr pViewport = osg::Viewport::create();
    beginEditCP(pViewport);
    pViewport->setSize(0, 0, nWindowWidth - 1, nWindowHeight - 1);
    pViewport->setBackground(pData->m_pSolidBackground);
    pViewport->setRoot(m_pRealRoot);
    endEditCP(pViewport);

    osg::beginEditCP(pWindowData->m_ptrWindow);
    pWindowData->m_ptrWindow->addPort(pViewport);
    osg::endEditCP(pWindowData->m_ptrWindow);

    pData->m_RightViewport = pViewport;
  } else {
    // we have a stereo viewport, but no longer need it - remove it
    osg::beginEditCP(pWindowData->m_ptrWindow);
    pWindowData->m_ptrWindow->subPort(pData->m_RightViewport);
    osg::endEditCP(pWindowData->m_ptrWindow);

    pData->m_RightViewport = osg::NullFC;
  }

  if (pData->m_bUseRenderToTexture) {
    pData->m_nRTTViewportSizeWidth  = pData->m_Viewport->getPixelWidth();
    pData->m_nRTTViewportSizeHeight = pData->m_Viewport->getPixelHeight();
    pData->SetupRenderTargets(pData->m_nRTTViewportSizeWidth, pData->m_nRTTViewportSizeHeight);
  } else {
    pData->ClearRenderTargets();
  }

  // @IMGTODO: adjust projection - it's only set at beginning
  // @IMGTODO: projection stereomode optional
  //
  if (eType == VistaViewport::VistaViewportProperties::VT_OCULUS_RIFT) {
#ifndef VISTA_WITH_OCULUSSDK
    VISTA_THROW("Cannot create Oculus Viewport - VistaKernel not built with Oculus Support", -1);
#else
    OculusViewportData* pOcuData = Vista::assert_cast<OculusViewportData*>(pData);

    VistaOculusGlutWindowingToolkit* pOcuWinTK =
        dynamic_cast<VistaOculusGlutWindowingToolkit*>(GetWindowingToolkit());
    if (pOcuWinTK == NULL) {
      vstr::errp() << "Trying to create Oculus Viewport without using Oculus Window Toolkit"
                   << std::endl;
      return false;
    }
    pOcuData->m_pHmd = pOcuWinTK->GetHmdForWindow(pTarget->GetWindow());
    if (pOcuData->m_pHmd == NULL) {
      vstr::errp()
          << "Trying to create Oculus Viewport, but no Hmd connected for corresponding window"
          << std::endl;
      return false;
    }

    SetViewportPosition(0, 0, pTarget);
    SetViewportSize(pOcuData->m_pHmd->Resolution.w / 2, pOcuData->m_pHmd->Resolution.h, pTarget);

    // @IMGTODO: relative resolution
    float fResFactor = 1.f;
    ovrSizei nRecommenedTex0Size = ovrHmd_GetFovTextureSize(
        pOcuData->m_pHmd, ovrEye_Left, pOcuData->m_pHmd->DefaultEyeFov[0], 1.0f);
    SetViewportTextureSize(
        fResFactor * nRecommenedTex0Size.w, fResFactor * nRecommenedTex0Size.h, pTarget);

    // setup rendering
    ovrGLConfig oOculusConfig;
    pOcuWinTK->FillOculusRenderConfigForWindow(pTarget->GetWindow(), oOculusConfig);

    if (ovrHmd_ConfigureRendering(pOcuData->m_pHmd, &oOculusConfig.Config,
            ovrDistortionCap_TimeWarp, pOcuData->m_pHmd->DefaultEyeFov,
            pOcuData->m_apRenderDescs) == false) {
      vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport - "
                   << "Oculus rendering could not be configured" << std::endl;
      return false;
    }

    for (int i = 0; i < 2; ++i) {
      pOcuData->m_aTextures[i].OGL.Header.API = ovrRenderAPI_OpenGL;
      pOcuData->m_aTextures[i].OGL.Header.TextureSize.w = pOcuData->m_nRTTViewportSizeWidth;
      pOcuData->m_aTextures[i].OGL.Header.TextureSize.h = pOcuData->m_nRTTViewportSizeHeight;
      pOcuData->m_aTextures[i].OGL.Header.RenderViewport.Pos.x = 0;
      pOcuData->m_aTextures[i].OGL.Header.RenderViewport.Pos.y = 0;
      pOcuData->m_aTextures[i].OGL.Header.RenderViewport.Size.w =
          pOcuData->m_pHmd->Resolution.w / 2;
      pOcuData->m_aTextures[i].OGL.Header.RenderViewport.Size.h = pOcuData->m_pHmd->Resolution.h;
      assert(pOcuData->m_pWriteTextureTarget[i] != NULL);
      pOcuData->m_aTextures[i].OGL.TexId = pOcuData->m_pWriteTextureTarget[i]->GetGLId();
    }
    pOcuData->m_aTextures[1].OGL.Header.RenderViewport.Pos.x = pOcuData->m_pHmd->Resolution.w / 2;
    ;

    // create our Projection
    VistaPropertyList oProjProps;
    oProjProps.SetValueInSubList(
        "NAME", "OCULUS_PROJ", pTarget->GetNameForNameable() + "OCULUS_PROJ");
    VistaProjection* pProj =
        GetDisplayManager()->CreateProjection(pTarget, "OCULUS_PROJ", oProjProps);
    ProjectionData* pProjData = Vista::assert_cast<ProjectionData*>(pProj->GetData());
    VistaProjection::VistaProjectionProperties* pProjProps = pProj->GetProjectionProperties();
    pProjProps->SetStereoMode(VistaProjection::VistaProjectionProperties::SM_FULL_STEREO);

    pProjProps->SetProjPlaneMidpoint(0, 0, -1);
    pProjProps->SetProjPlaneExtents(-1.6, 1.6, -1, 1);
    pProjProps->SetProjPlaneNormal(0, 0, 1);

    pOcuData->m_pLeftCamera = osg::MatrixCamera::create();
    beginEditCP(pOcuData->m_pLeftCamera);
    pOcuData->m_pLeftCamera->setBeacon(pProjData->m_ptrEyeBeacon);
    endEditCP(pOcuData->m_pLeftCamera);
    beginEditCP(pOcuData->m_Viewport);
    pOcuData->m_Viewport->setCamera(pOcuData->m_pLeftCamera);
    endEditCP(pOcuData->m_Viewport);

    pOcuData->m_pRightCamera = osg::MatrixCamera::create();
    beginEditCP(pOcuData->m_pRightCamera);
    pOcuData->m_pRightCamera->setBeacon(pProjData->m_ptrRightEyeBeacon);
    endEditCP(pOcuData->m_pRightCamera);
    beginEditCP(pOcuData->m_RightViewport);
    pOcuData->m_RightViewport->setCamera(pOcuData->m_pRightCamera);
    endEditCP(pOcuData->m_Viewport);

    double dNear = 0;
    double dFar = 0;
    pProjProps->GetClippingRange(dNear, dFar);

    pOcuData->m_pProjectionObserver->Observe(pProjProps);
    pOcuData->UpdateOculusProjection((float)dNear, (float)dFar);
#endif
  }

  if (eType == VistaViewport::VistaViewportProperties::VT_OPENVR) {
#ifndef VISTA_WITH_OPENVR
    VISTA_THROW("Cannot create OpenVR Viewport - VistaKernel not built with OpenVR Support", -1);
#else
    OpenVRViewportData* pOpenVRData = Vista::assert_cast<OpenVRViewportData*>(pData);

    VistaOpenVRGlutWindowingToolkit* pOpenVRWinTK =
        dynamic_cast<VistaOpenVRGlutWindowingToolkit*>(GetWindowingToolkit());
    if (pOpenVRWinTK == NULL) {
      vstr::errp() << "Trying to create OpenVR Viewport without using OpenVR Window Toolkit"
                   << std::endl;
      return false;
    }
    pOpenVRData->m_pVRSystem = pOpenVRWinTK->GetVRSystem();
    if (pOpenVRData->m_pVRSystem == NULL) {
      vstr::errp() << "Trying to create OpenVR Viewport, but no Hmd connected" << std::endl;
      return false;
    }

    SetViewportPosition(0, 0, pTarget);
    // SetViewportSize( pOcuData->m_pHmd->Resolution.w / 2, pOcuData->m_pHmd->Resolution.h, pTarget
    // );

    // @IMGTODO: relative resolution
    float fResFactor = 1.f;
    // ovrSizei nRecommenedTex0Size = ovrHmd_GetFovTextureSize(pOcuData->m_pHmd, ovrEye_Left,
    // pOcuData->m_pHmd->DefaultEyeFov[0], 1.0f);
    uint32_t width, height;
    pOpenVRData->m_pVRSystem->GetRecommendedRenderTargetSize(&width, &height);
    // SetViewportTextureSize(fResFactor * nRecommenedTex0Size.w, fResFactor *
    // nRecommenedTex0Size.h, pTarget);
    SetViewportTextureSize(fResFactor * width, fResFactor * height, pTarget);

    // setup rendering
    // ovrGLConfig oOculusConfig;
    // pOpenVRWinTK->FillOculusRenderConfigForWindow( pTarget->GetWindow(), oOculusConfig );

    // if( ovrHmd_ConfigureRendering( pOcuData->m_pHmd, &oOculusConfig.Config,
    // ovrDistortionCap_TimeWarp, 								pOcuData->m_pHmd->DefaultEyeFov, 								pOcuData->m_apRenderDescs ) ==
    // false )
    // {
    // 	vstr::errp() << "[VistaOpenSGDisplayBridge]: Unable to create viewport - "
    // 				<< "Oculus rendering could not be configured" << std::endl;
    // 	return false;
    // }

    // for( int i = 0; i < 2; ++i )
    // {
    // 	pOcuData->m_aTextures[i].OGL.Header.API = ovrRenderAPI_OpenGL;
    // 	pOcuData->m_aTextures[i].OGL.Header.TextureSize.w = pOcuData->m_nRTTViewportSizeWidth;
    // 	pOcuData->m_aTextures[i].OGL.Header.TextureSize.h = pOcuData->m_nRTTViewportSizeHeight;
    // 	pOcuData->m_aTextures[i].OGL.Header.RenderViewport.Pos.x = 0;
    // 	pOcuData->m_aTextures[i].OGL.Header.RenderViewport.Pos.y = 0;
    // 	pOcuData->m_aTextures[i].OGL.Header.RenderViewport.Size.w = pOcuData->m_pHmd->Resolution.w /
    // 2; 	pOcuData->m_aTextures[i].OGL.Header.RenderViewport.Size.h =
    // pOcuData->m_pHmd->Resolution.h; 	assert( pOcuData->m_pWriteTextureTarget[i] != NULL );
    // 	pOcuData->m_aTextures[i].OGL.TexId = pOcuData->m_pWriteTextureTarget[i]->GetGLId();
    // }
    // pOcuData->m_aTextures[1].OGL.Header.RenderViewport.Pos.x = pOcuData->m_pHmd->Resolution.w /
    // 2;;

    // // create our Projection
    // VistaPropertyList oProjProps;
    // oProjProps.SetValueInSubList( "NAME", "OCULUS_PROJ", pTarget->GetNameForNameable() +
    // "OCULUS_PROJ" ); VistaProjection* pProj = GetDisplayManager()->CreateProjection( pTarget,
    // "OCULUS_PROJ", oProjProps ); ProjectionData* pProjData = Vista::assert_cast< ProjectionData*
    // >( pProj->GetData() ); VistaProjection::VistaProjectionProperties* pProjProps =
    // pProj->GetProjectionProperties(); pProjProps->SetStereoMode(
    // VistaProjection::VistaProjectionProperties::SM_FULL_STEREO );

    // pProjProps->SetProjPlaneMidpoint( 0, 0, -1 );
    // pProjProps->SetProjPlaneExtents( -1.6, 1.6, -1, 1 );
    // pProjProps->SetProjPlaneNormal( 0, 0, 1 );

    // pOcuData->m_pLeftCamera = osg::MatrixCamera::create();
    // beginEditCP( pOcuData->m_pLeftCamera );
    // pOcuData->m_pLeftCamera->setBeacon( pProjData->m_ptrEyeBeacon );
    // endEditCP( pOcuData->m_pLeftCamera );
    // beginEditCP( pOcuData->m_Viewport );
    // pOcuData->m_Viewport->setCamera( pOcuData->m_pLeftCamera );
    // endEditCP( pOcuData->m_Viewport );

    // pOcuData->m_pRightCamera = osg::MatrixCamera::create();
    // beginEditCP( pOcuData->m_pRightCamera );
    // pOcuData->m_pRightCamera->setBeacon( pProjData->m_ptrRightEyeBeacon );
    // endEditCP( pOcuData->m_pRightCamera );
    // beginEditCP( pOcuData->m_RightViewport );
    // pOcuData->m_RightViewport->setCamera( pOcuData->m_pRightCamera );
    // endEditCP( pOcuData->m_Viewport );

    // double dNear = 0;
    // double dFar = 0;
    // pProjProps->GetClippingRange( dNear, dFar );

    // pOcuData->m_pProjectionObserver->Observe( pProjProps );
    // pOcuData->UpdateOculusProjection( (float)dNear, (float)dFar );

    // create our Projection
    VistaPropertyList oProjProps;
    oProjProps.SetValueInSubList(
        "NAME", "PROJECTION_OPENVR", pTarget->GetNameForNameable() + "PROJECTION_OPENVR");
    VistaProjection* pProj =
        GetDisplayManager()->CreateProjection(pTarget, "PROJECTION_OPENVR", oProjProps);
    ProjectionData* pProjData = Vista::assert_cast<ProjectionData*>(pProj->GetData());
    VistaProjection::VistaProjectionProperties* pProjProps = pProj->GetProjectionProperties();
    pProjProps->SetStereoMode(VistaProjection::VistaProjectionProperties::SM_FULL_STEREO);

    pProjProps->SetProjPlaneMidpoint(0, 0, -1);        // ???
    pProjProps->SetProjPlaneExtents(-1.6, 1.6, -1, 1); // ???
    pProjProps->SetProjPlaneNormal(0, 0, 1);

    pOpenVRData->m_pLeftCamera = osg::MatrixCamera::create();
    beginEditCP(pOpenVRData->m_pLeftCamera);
    pOpenVRData->m_pLeftCamera->setBeacon(pProjData->m_ptrEyeBeacon);
    endEditCP(pOpenVRData->m_pLeftCamera);
    beginEditCP(pOpenVRData->m_Viewport);
    pOpenVRData->m_Viewport->setCamera(pOpenVRData->m_pLeftCamera);
    endEditCP(pOpenVRData->m_Viewport);

    pOpenVRData->m_pRightCamera = osg::MatrixCamera::create();
    beginEditCP(pOpenVRData->m_pRightCamera);
    pOpenVRData->m_pRightCamera->setBeacon(pProjData->m_ptrRightEyeBeacon);
    endEditCP(pOpenVRData->m_pRightCamera);
    beginEditCP(pOpenVRData->m_RightViewport);
    pOpenVRData->m_RightViewport->setCamera(pOpenVRData->m_pRightCamera);
    endEditCP(pOpenVRData->m_Viewport);

    double dNear = 0.0;
    double dFar = 0.0;
    pProjProps->GetClippingRange(dNear, dFar);

    pOpenVRData->m_pProjectionObserver->Observe(pProjProps);
    pOpenVRData->UpdateOpenVRProjection((float)dNear, (float)dFar);

#endif
  }

  return true;
}

int VistaOpenSGDisplayBridge::GetViewportPriority(const VistaViewport* pTarget) {
  const ViewportData* pData = static_cast<const ViewportData*>(pTarget->GetData());
  return pData->GetPriority();
}

bool VistaOpenSGDisplayBridge::SetViewportPriority(int nPriority, VistaViewport* pTarget) {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());
  pData->SetPriority(nPriority);
  return true;
}

bool VistaOpenSGDisplayBridge::GetViewportIsEnabled(const VistaViewport* pTarget) {
  const ViewportData* pData = static_cast<const ViewportData*>(pTarget->GetData());
  return pData->GetIsEnabled();
}

bool VistaOpenSGDisplayBridge::SetViewportIsEnabled(bool bSet, VistaViewport* pTarget) {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());
  pData->SetIsEnabled(bSet);
  return true;
}

VistaGLTexture* VistaOpenSGDisplayBridge::GetViewportTextureTarget(
    const VistaViewport* pTarget) const {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());
  return pData->GetMainTextureTarget();
}

VistaGLTexture* VistaOpenSGDisplayBridge::GetViewportSecondaryTextureTarget(
    const VistaViewport* pTarget) const {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());
  return pData->GetSecondaryTextureTarget();
}
std::string VistaOpenSGDisplayBridge::GetViewportPostProcessShaderFile(
    const VistaViewport* pTarget) const {
  const ViewportData* pData = static_cast<const ViewportData*>(pTarget->GetData());
  return pData->m_sShaderFilename;
}

bool VistaOpenSGDisplayBridge::SetViewportPostProcessShaderFile(
    const std::string& sShader, VistaViewport* pTarget) {
  ViewportData* pData = static_cast<ViewportData*>(pTarget->GetData());
  switch (pData->m_eType) {
  case VistaViewport::VistaViewportProperties::VT_POSTPROCESS:
  case VistaViewport::VistaViewportProperties::VT_POSTPROCESS_STEREO:
  case VistaViewport::VistaViewportProperties::VT_POSTPROCESS_QUADBUFFERED_STEREO: {
    break;
  }
  default: {
    vstr::warnp() << "[Viewport::SetPostProcessShaderFile] only supported for postprocess types"
                  << std::endl;
    return false;
  }
  }
  pData->m_sShaderFilename = sShader;
  if (pData->m_pPostProcessShader != osg::NullFC) {
    if (pData->m_pPostProcessShader->readFragmentProgram(sShader.c_str()) == false) {
      vstr::warnp()
          << "[VistaViewport::SetViewportPostProcessShaderFile] - Loading fragment shader \""
          << sShader << "\" failed" << std::endl;
      pData->m_pPostProcessShader = osg::NullFC;
      return false;
    }
  }
  return true;
}

bool VistaOpenSGDisplayBridge::GetViewportResizeWithWindowResize(const VistaViewport* pTarget) {
  return Vista::assert_cast<ViewportData*>(pTarget->GetData())->m_bResizeWithWindow;
}

void VistaOpenSGDisplayBridge::SetViewportResizeWithWindowResize(
    bool bSet, VistaViewport* pTarget) {
  ViewportData* pData        = static_cast<ViewportData*>(pTarget->GetData());
  pData->m_bResizeWithWindow = bSet;
}

bool VistaOpenSGDisplayBridge::GetViewportRepositionWithWindowResize(const VistaViewport* pTarget) {
  return Vista::assert_cast<ViewportData*>(pTarget->GetData())->m_bRepositionWithWindow;
}

void VistaOpenSGDisplayBridge::SetViewportRepositionWithWindowResize(
    bool bSet, VistaViewport* pTarget) {
  ViewportData* pData            = static_cast<ViewportData*>(pTarget->GetData());
  pData->m_bRepositionWithWindow = bSet;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DebugViewport                                               */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::DebugViewport(std::ostream& out, const VistaViewport* pTarget) {
  pTarget->Debug(out);
}

/**
 * Methods for projection management.
 */
/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateProjection                                            */
/*                                                                            */
/*============================================================================*/
VistaProjection* VistaOpenSGDisplayBridge::CreateProjection(
    VistaViewport* pViewport, const VistaPropertyList& refProps) {
  if (!pViewport) {
    vstr::errp() << "[VistaOpenSGDisplayBridge] - Unable to create projection - "
                 << "no viewport given..." << std::endl;
    return NULL;
  }

  // create new data container
  ProjectionData* pData = new ProjectionData();

  DisplaySystemData* pDS =
      Vista::assert_cast<DisplaySystemData*>(pViewport->GetDisplaySystem()->GetData());

  // connect both beacons (display system and head)
  // since the camera decorator won't do it on it's own
  beginEditCP(pDS->m_ptrCameraBeacon);
  pDS->m_ptrCameraBeacon->addChild(pData->m_ptrBeacon);
  endEditCP(pDS->m_ptrCameraBeacon);

  // also append the eye offset beacons
  beginEditCP(pData->m_ptrBeacon);
  pData->m_ptrBeacon->addChild(pData->m_ptrEyeBeacon);
  pData->m_ptrBeacon->addChild(pData->m_ptrRightEyeBeacon);
  endEditCP(pData->m_ptrBeacon);

  // left camera
  beginEditCP(pData->m_ptrCamDeco);
  pData->m_ptrPerspCam = osg::PerspectiveCamera::create();
  beginEditCP(pData->m_ptrPerspCam);
  pData->m_ptrPerspCam->setBeacon(pDS->m_ptrCameraBeacon);
  // pData->m_ptrPerspCam->setNear( 0.01f );
  // pData->m_ptrPerspCam->setFar( 65000.0f );
  pData->m_ptrPerspCam->setFov(osg::deg2rad(90));
  pData->m_ptrPerspCam->setAspect(1);
  endEditCP(pData->m_ptrPerspCam);
  pData->m_ptrCamDeco->setDecoratee(pData->m_ptrPerspCam);
  // set the user beacon later on
  // once the projection knows about the eyes
  pData->m_ptrCamDeco->getSurface().clear();
  // push the points defining the projection surface's edges
  // MIND THE ORDER OF HOW THE EDGES ARE PUSHED...
  endEditCP(pData->m_ptrCamDeco);

  ViewportData* pViewportData = Vista::assert_cast<ViewportData*>(pViewport->GetData());
  // set the viewport's cameras
  osg::ViewportPtr osgViewport = pViewportData->m_Viewport;
  beginEditCP(osgViewport);
  osgViewport->setCamera(pData->m_ptrCamDeco);
  endEditCP(osgViewport);

  // deactivate the right eye iff stereo is off
  bool bVPStereo = pViewportData->m_bStereo;
  pData->m_ptrRightEyeBeacon->setActive(bVPStereo);

  if (bVPStereo) // STEREO ? // @IMGTODO
  {
    // right camera
    beginEditCP(pData->m_ptrRightCamDeco);
    pData->m_ptrRightPerspCam = osg::PerspectiveCamera::create();
    beginEditCP(pData->m_ptrRightPerspCam);
    pData->m_ptrRightPerspCam->setBeacon(pDS->m_ptrCameraBeacon); /** @todo camerabeacon */
    pData->m_ptrRightPerspCam->setNear(0.01f);
    pData->m_ptrRightPerspCam->setFar(65000.0f);
    pData->m_ptrRightPerspCam->setFov(osg::deg2rad(90));
    pData->m_ptrRightPerspCam->setAspect(1);
    endEditCP(pData->m_ptrRightPerspCam);
    pData->m_ptrRightCamDeco->setDecoratee(pData->m_ptrRightPerspCam);
    // set the user beacon later on
    // once the projection knows about the eyes
    pData->m_ptrRightCamDeco->getSurface().clear();
    // push the points defining the projection surface's edges
    // MIND THE ORDER OF HOW THE EDGES ARE PUSHED...
    // default 4:3 aspect ratio
    pData->m_ptrRightCamDeco->getSurface().push_back(osg::Pnt3f(-1, -0.75f, 0));
    pData->m_ptrRightCamDeco->getSurface().push_back(osg::Pnt3f(1, -0.75f, 0));
    pData->m_ptrRightCamDeco->getSurface().push_back(osg::Pnt3f(1, 0.75f, 0));
    pData->m_ptrRightCamDeco->getSurface().push_back(osg::Pnt3f(-1, 0.75f, 0));
    endEditCP(pData->m_ptrRightCamDeco);

    osgViewport = pViewportData->m_RightViewport;
    beginEditCP(osgViewport);
    osgViewport->setCamera(pData->m_ptrRightCamDeco);
    endEditCP(osgViewport);
  }

  pData->SetProjectionPlane();

  VistaProjection* pProjection = NewProjection(pViewport, pData);
  if (!pProjection) {
    vstr::errp() << "[VistaOpenSGDisplayBridge] - Unable to create projection" << std::endl;
    delete pData;
    return NULL;
  }

  ProjectionData* pProjData = (ProjectionData*)pProjection->GetData();

  // stereo key must be set before setpropertiesbylist, since the set-functors for e.g.
  // clipping range depend on it being already set (in the projection data!)
  // this should be cleaned up on occasion...
  std::string sMode;
  refProps.GetValue("STEREO_MODE", sMode);
  if (sMode == "FULL_STEREO")
    pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_FULL_STEREO;
  else if (sMode == "MONO")
    pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_MONO;
  if (sMode == "LEFT_EYE")
    pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_LEFT_EYE;
  if (sMode == "RIGHT_EYE")
    pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_RIGHT_EYE;
  // @IMGTODO: warning if invalid, default

  if (pProjData->m_iStereoMode == VistaProjection::VistaProjectionProperties::SM_FULL_STEREO &&
      bVPStereo == false) {
    vstr::warnp()
        << "[VisraProjection::Create] Requests FullStereo Projection, but viewport is mono"
        << std::endl;
    pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_MONO;
  } else if (pProjData->m_iStereoMode !=
                 VistaProjection::VistaProjectionProperties::SM_FULL_STEREO &&
             bVPStereo == true) {
    vstr::warnp()
        << "[VisraProjection::Create] Requests non-stereo projection, but viewport is stereo"
        << std::endl;
    pProjData->m_iStereoMode = VistaProjection::VistaProjectionProperties::SM_FULL_STEREO;
  }

  pProjection->GetProperties()->SetPropertiesByList(refProps);

  // get and then set the initial viewer position and orientation for the display system
  pProjData->SetCameraTranslation(pDS->m_v3InitialViewerPosition);
  pProjData->SetCameraOrientation(pDS->m_qInitialViewerOrientation);

  // set the appropriate user beacon for the camera decorator -> Head or Eye?
  pData->SetEyes(pDS->m_v3LeftEyeOffset, pDS->m_v3RightEyeOffset);

  return pProjection;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DestroyProjection                                           */
/*                                                                            */
/*============================================================================*/
bool VistaOpenSGDisplayBridge::DestroyProjection(VistaProjection* pProjection) {
  if (!pProjection)
    return true;

  ProjectionData* pData = static_cast<ProjectionData*>(pProjection->GetData());

  delete pData;
  delete pProjection;

  return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetProjectionPlane                                      */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetProjectionPlane(const VistaVector3D& v3MidPoint,
    const VistaVector3D& v3NormalVector, const VistaVector3D& v3UpVector,
    VistaProjection* pTarget) {
  ProjectionData* pData = (ProjectionData*)pTarget->GetData();

  pData->m_v3MidPoint     = v3MidPoint;
  pData->m_v3NormalVector = v3NormalVector.GetNormalized();
  pData->m_v3UpVector     = v3UpVector.GetNormalized();

  pData->SetProjectionPlane();
}

void VistaOpenSGDisplayBridge::GetProjectionPlane(VistaVector3D& v3MidPoint,
    VistaVector3D& v3NormalVector, VistaVector3D& v3UpVector, const VistaProjection* pTarget) {
  ProjectionData* pData = (ProjectionData*)pTarget->GetData();

  v3MidPoint     = pData->m_v3MidPoint;
  v3NormalVector = pData->m_v3NormalVector;
  v3UpVector     = pData->m_v3UpVector;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetProjPlaneExtents                                     */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetProjPlaneExtents(
    double dLeft, double dRight, double dBottom, double dTop, VistaProjection* pTarget) {
  ProjectionData* pData = (ProjectionData*)pTarget->GetData();

  pData->m_dLeft   = dLeft;
  pData->m_dRight  = dRight;
  pData->m_dBottom = dBottom;
  pData->m_dTop    = dTop;

  pData->SetProjectionPlane();
}

void VistaOpenSGDisplayBridge::GetProjPlaneExtents(
    double& dLeft, double& dRight, double& dBottom, double& dTop, const VistaProjection* pTarget) {
  ProjectionData* pData = (ProjectionData*)pTarget->GetData();

  dLeft   = pData->m_dLeft;
  dRight  = pData->m_dRight;
  dBottom = pData->m_dBottom;
  dTop    = pData->m_dTop;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetProjClippingRange                                    */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetProjClippingRange(
    double dNear, double dFar, VistaProjection* pTarget) {
  ProjectionData* pData = (ProjectionData*)pTarget->GetData();

  beginEditCP(pData->m_ptrCamDeco);
  pData->m_ptrCamDeco->setNear((float)dNear);
  pData->m_ptrCamDeco->setFar((float)dFar);
  endEditCP(pData->m_ptrCamDeco);

  if (pData->m_iStereoMode == VistaProjection::VistaProjectionProperties::SM_FULL_STEREO) {
    beginEditCP(pData->m_ptrRightCamDeco);
    pData->m_ptrRightCamDeco->setNear((float)dNear);
    pData->m_ptrRightCamDeco->setFar((float)dFar);
    endEditCP(pData->m_ptrRightCamDeco);
  }
}

void VistaOpenSGDisplayBridge::GetProjClippingRange(
    double& dNear, double& dFar, const VistaProjection* pTarget) {
  ProjectionData* pData = (ProjectionData*)pTarget->GetData();

  dNear = pData->m_ptrCamDeco->getNear();
  dFar  = pData->m_ptrCamDeco->getFar();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Set/GetProjStereoMode                                       */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::SetProjStereoMode(int iMode, VistaProjection* pTarget) {
  if (iMode >= VistaProjection::VistaProjectionProperties::SM_MONO &&
      iMode <= VistaProjection::VistaProjectionProperties::SM_FULL_STEREO) {
    ProjectionData* pData = Vista::assert_cast<ProjectionData*>(pTarget->GetData());

    if (pTarget->GetViewport()->GetWindow()->GetWindowProperties()->GetStereo()) {
      if (iMode != VistaProjection::VistaProjectionProperties::SM_FULL_STEREO) {
        vstr::warnp() << "[VistaOpenSGDisplayBridge] - invalid stereo mode [" << iMode
                      << "] for stereo window..." << std::endl;
      }
    }

    ViewportData* pViewportData =
        Vista::assert_cast<ViewportData*>(pTarget->GetViewport()->GetData());
    bool bShouldBeStereo = pViewportData->m_bStereo;
    if (bShouldBeStereo && iMode != VistaProjection::VistaProjectionProperties::SM_FULL_STEREO) {
      vstr::warnp() << "[VistaOpenSGDisplayBridge] - invalid stereo mode for stereo viewport - "
                       "falling back to FULL_STEREO"
                    << std::endl;
      iMode = VistaProjection::VistaProjectionProperties::SM_FULL_STEREO;
    } else if (!bShouldBeStereo &&
               iMode == VistaProjection::VistaProjectionProperties::SM_FULL_STEREO) {
      vstr::warnp() << "[VistaOpenSGDisplayBridge] - invalid stereo mode FULL_STEREO for "
                       "non-stereo viewport - falling back to MONO"
                    << std::endl;
      iMode = VistaProjection::VistaProjectionProperties::SM_MONO;
    }

    pData->m_iStereoMode = iMode;

    // update eyes after mode change
    pData->SetEyes(pData->m_v3LeftEye, pData->m_v3RightEye);
  }
}

int VistaOpenSGDisplayBridge::GetProjStereoMode(const VistaProjection* pTarget) {
  ProjectionData* pData = Vista::assert_cast<ProjectionData*>(pTarget->GetData());
  return pData->m_iStereoMode;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   DebugProjection                                             */
/*                                                                            */
/*============================================================================*/
void VistaOpenSGDisplayBridge::DebugProjection(std::ostream& out, const VistaProjection* pTarget) {
  pTarget->Debug(out);
}

/*============================================================================*/
/*                                                                            */
/*  NAME    :   RenderAction */
/*                                                                            */
/*============================================================================*/
osg::RenderAction* VistaOpenSGDisplayBridge::GetRenderAction() const {
  return m_pRenderAction;
}

bool VistaOpenSGDisplayBridge::MakeScreenshot(
    const VistaWindow& oWin, const std::string& strFilename) const {
  int nSizeX, nSizeY;
  oWin.GetWindowProperties()->GetSize(nSizeX, nSizeY);
  osg::ImageRefPtr oImage(osg::Image::create());
  oImage->set(osg::Image::OSG_RGB_PF, nSizeX, nSizeY);

  int nDataSize = nSizeX * nSizeY * 3;
  int nRead = oWin.ReadRGBImage(reinterpret_cast<VistaType::byte*>(oImage->getData()), nDataSize);
  if (nRead != nDataSize) {
    vstr::warnp() << "[VistaOpenSGDisplayBridge::MakeScreenshot]: Reading image from window failed"
                  << std::endl;
    return false;
  }

  // check if the image already has an extension
  std::size_t nDotPos       = strFilename.find_last_of('.');
  std::string sFullFileName = strFilename;
  if (nDotPos == std::string::npos)
    sFullFileName += ".jpg";

  VistaFileSystemFile      oFile(sFullFileName);
  VistaFileSystemDirectory oDir = oFile.GetParentDirectory();
  if (oDir.Exists() == false && oDir.CreateWithParentDirectories() == false) {
    vstr::warnp()
        << "[VistaOpenSGDisplayBridge::MakeScreenshot]: Cannot create directory for file \""
        << sFullFileName << "\"" << std::endl;
    return false;
  }
  return oImage->write(sFullFileName.c_str());
}

bool VistaOpenSGDisplayBridge::AddSceneOverlay(
    IVistaSceneOverlay* pDraw, VistaViewport* pViewport) {
  if (pViewport == NULL)
    return false;

  ViewportData* pViewportData = static_cast<ViewportData*>(pViewport->GetData());
  osg::VistaOpenSGGLOverlayForegroundPtr pOverlays = pViewportData->GetOverlayForeground();
  if (pOverlays == osg::NullFC) {
    pOverlays = osg::VistaOpenSGGLOverlayForeground::create();
    if (pOverlays == osg::NullFC)
      return false;

    pViewportData->m_pOverlays = pOverlays;

    osg::ViewportPtr osgvp = pViewportData->GetOpenSGViewport();

    osgvp->getForegrounds().push_back(pOverlays);

    if (pViewportData->GetOpenSGRightViewport()) {
      pViewportData->GetOpenSGRightViewport()->getForegrounds().push_back(pOverlays);
    }
  }

  beginEditCP(pOverlays, osg::VistaOpenSGGLOverlayForeground::GLOverlaysFieldMask);
  pOverlays->getMFGLOverlays()->push_back(pDraw);
  endEditCP(pOverlays, osg::VistaOpenSGGLOverlayForeground::GLOverlaysFieldMask);

  return true;
}

bool VistaOpenSGDisplayBridge::RemSceneOverlay(
    IVistaSceneOverlay* pDraw, VistaViewport* pViewport) {
  if (pViewport == NULL)
    return false;

  ViewportData* vpdata = static_cast<ViewportData*>(pViewport->GetData());

  osg::MField<void*>&          overls = vpdata->GetOverlayForeground()->getGLOverlays();
  osg::MField<void*>::iterator it     = overls.find(pDraw);

  if (it != overls.end()) {
    overls.erase(it);
    return true;
  }

  return false;
}

Vista2DText* VistaOpenSGDisplayBridge::New2DText(const std::string& strViewportName) {
  Vista2DText* Text2DObjekt = NULL;

  VistaViewport* viewport = m_pDisplayManager->GetViewportByName(strViewportName);
  if (viewport != NULL) {
    ViewportData* vpdata = (ViewportData*)viewport->GetData();

    osg::VistaOpenSGTextForegroundPtr textfg = vpdata->GetTextForeground();
    if (textfg == osg::NullFC) {
      osg::ViewportPtr osgviewport = vpdata->GetOpenSGViewport();
      textfg                       = osg::VistaOpenSGTextForeground::create();

      if (textfg == osg::NullFC)
        return NULL;

      vpdata->m_TextForeground = textfg;

      // we store a pointer ourselves
      osgviewport->getForegrounds().push_back(textfg);

      if (vpdata->GetOpenSGRightViewport()) {
        vpdata->GetOpenSGRightViewport()->getForegrounds().push_back(textfg);
      }
    }

    Text2DObjekt = new Vista2DText;
    vpdata->m_liOverlays.push_back(Text2DObjekt);

    beginEditCP(textfg);
    textfg->getMFTexts()->push_back(Text2DObjekt);
    endEditCP(textfg);
  } else {
    vstr::warnp() << "OpenSGDisplayBridge::Add2DText(): non-existent viewport '" << strViewportName
                  << "' specified!" << std::endl;
  }

  return Text2DObjekt;
}

Vista2DBitmap* VistaOpenSGDisplayBridge::New2DBitmap(const std::string& strViewportName) {
  Vista2DBitmap* pBm      = NULL;
  VistaViewport* viewport = m_pDisplayManager->GetViewportByName(strViewportName);
  if (viewport != NULL) {
    ViewportData*           vpdata  = (ViewportData*)viewport->GetData();
    osg::ImageForegroundPtr bitmaps = vpdata->m_oBitmaps;
    if (bitmaps == osg::NullFC) {
      // create image forground
      vpdata->m_oBitmaps  = osg::ImageForeground::create();
      osg::ViewportPtr vp = vpdata->GetOpenSGViewport();
      vp->getForegrounds().push_back(vpdata->m_oBitmaps);

      vp = vpdata->GetOpenSGRightViewport();
      if (vp != osg::NullFC)
        vp->getForegrounds().push_back(vpdata->m_oBitmaps);
    }

    pBm = new VistaOpenSG2DBitmap(this, strViewportName);
    vpdata->m_liOverlays.push_back(pBm);
  }
  return pBm;
}

Vista2DLine* VistaOpenSGDisplayBridge::New2DLine(const std::string& strWindowName) {
  return NULL;
}

Vista2DRectangle* VistaOpenSGDisplayBridge::New2DRectangle(const std::string& strWindowName) {
  return NULL;
}

bool VistaOpenSGDisplayBridge::Get2DOverlay(
    const std::string& strWindowName, std::list<Vista2DDrawingObject*>&) {
  return false;
}

bool VistaOpenSGDisplayBridge::DoLoadBitmap(const std::string& strNewFName,
    VistaType::byte** pBitmapData, int& nWidth, int& nHeight, bool& bAlpha) {
  // this call may return osg::NullFC, which is ok (clear image)
  osg::ImageRefPtr image(osg::Image::create());

  if (image == osg::NullFC)
    return false;

  std::string sCwd = VistaFileSystemDirectory::GetCurrentWorkingDirectory();
  if (image->read(
          (sCwd + VistaFileSystemDirectory::GetOSSpecificSeparator() + strNewFName).c_str()) ==
      true) {
    beginEditCP(image);

    if (image->getDataType() != osg::Image::OSG_UINT8_IMAGEDATA)
      image->convertDataTypeTo(osg::Image::OSG_UINT8_IMAGEDATA);

    endEditCP(image);

    VistaType::byte* pNewBitmap = image->getData();
    nWidth                      = image->getWidth();
    nHeight                     = image->getHeight();
    bAlpha                      = image->hasAlphaChannel();

    // copy buffer
    int iBuffersize = nWidth * nHeight * (bAlpha ? 4 : 3);

    // alloc memory
    *pBitmapData = new VistaType::byte[iBuffersize];

    // deep copy image data
    memcpy(*pBitmapData, pNewBitmap, iBuffersize);
  }

  return true;
}

bool VistaOpenSGDisplayBridge::Delete2DDrawingObject(Vista2DDrawingObject* p2DObject) {
  return false;
}

bool VistaOpenSGDisplayBridge::ReplaceForegroundImage(
    const std::string& sVpName, osg::ImagePtr oOld, osg::ImagePtr oNew, float xPos, float yPos) {
  VistaViewport* viewport = m_pDisplayManager->GetViewportByName(sVpName);
  if (viewport != NULL) {
    ViewportData*           vpdata  = (ViewportData*)viewport->GetData();
    osg::ImageForegroundPtr bitmaps = vpdata->m_oBitmaps;

    if (bitmaps == osg::NullFC) {
      // create image forground
      vpdata->m_oBitmaps  = osg::ImageForeground::create();
      osg::ViewportPtr vp = vpdata->GetOpenSGViewport();
      vp->getForegrounds().push_back(vpdata->m_oBitmaps);

      vp = vpdata->GetOpenSGRightViewport();
      if (vp != osg::NullFC)
        vp->getForegrounds().push_back(vpdata->m_oBitmaps);
      bitmaps = vpdata->m_oBitmaps;
    }

    if (oOld == osg::NullFC) {
      // simple, just add
      beginEditCP(bitmaps, osg::ImageForeground::ImagesFieldMask);
      bitmaps->addImage(oNew, osg::Pnt2f(xPos, yPos));
      endEditCP(bitmaps, osg::ImageForeground::ImagesFieldMask);

      return true;
    } else {
      // find and replace old image
      osg::MFImagePtr* images = bitmaps->getMFImages();
      for (osg::UInt32 n = 0; n < (*images).size(); ++n) {
        if (oOld == (*images)[n]) {
          // replace image
          beginEditCP(bitmaps,
              osg::ImageForeground::ImagesFieldMask | osg::ImageForeground::PositionsFieldMask);

          (*images)[n] = oNew;

          // replace positions
          osg::MFPnt2f* positions = bitmaps->getMFPositions();
          (*positions)[n]         = osg::Pnt2f(xPos, yPos);

          endEditCP(bitmaps,
              osg::ImageForeground::ImagesFieldMask | osg::ImageForeground::PositionsFieldMask);

          return true;
        }
      }
    }
  }
  return false;
}

void VistaOpenSGDisplayBridge::SetShowCursor(bool bShowCursor) {
  m_bShowCursor = bShowCursor;

  // iterate over all glut window contexts from the DM
  map<string, VistaWindow*>           mapWindows = GetDisplayManager()->GetWindows();
  map<string, VistaWindow*>::iterator itWindow   = mapWindows.begin();
  for (; itWindow != mapWindows.end(); ++itWindow) {
    // toggle cursor
    m_pWindowingToolkit->SetCursorIsEnabled((*itWindow).second, bShowCursor);
  }
}

bool VistaOpenSGDisplayBridge::GetShowCursor() const {
  return m_bShowCursor;
}

const VistaDisplayManager::RenderInfo* VistaOpenSGDisplayBridge::GetCurrentRenderInfo() const {
  return m_pRenderInfo.get();
}
