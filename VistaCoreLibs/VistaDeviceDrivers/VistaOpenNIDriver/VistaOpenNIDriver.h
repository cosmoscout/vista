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

#ifndef __VISTAOPENNIDRIVER_H
#define __VISTAOPENNIDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaBaseTypes.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTAOPENNIDRIVER_STATIC)
#ifdef VISTAOPENNIDRIVER_EXPORTS
#define OPENNIDRIVERAPI __declspec(dllexport)
#else
#define OPENNIDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define OPENNIDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverInfoAspect;
class VistaDriverThreadAspect;
class VistaDriverSensorMappingAspect;

namespace xn {
class Context;
class DepthGenerator;
class UserGenerator;
class ImageGenerator;
} // namespace xn

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class OPENNIDRIVERAPI VistaOpenNIDriver : public IVistaDeviceDriver {
 public:
  enum {
    SKELETON_FIRST = 0,
    SKELETON_HEAD  = 0,
    SKELETON_NECK,
    SKELETON_TORSO,
    SKELETON_LEFT_SHOULDER,
    SKELETON_LEFT_ELBOW,
    SKELETON_LEFT_HAND,
    SKELETON_LEFT_HIP,
    SKELETON_LEFT_KNEE,
    SKELETON_LEFT_FOOT,
    SKELETON_RIGHT_SHOULDER,
    SKELETON_RIGHT_ELBOW,
    SKELETON_RIGHT_HAND,
    SKELETON_RIGHT_HIP,
    SKELETON_RIGHT_KNEE,
    SKELETON_RIGHT_FOOT,
    SKELETON_COUNT
  };

  struct Joint {
    float m_a3fPosition[3];
    float m_a3x3fOrientation[3];
    float m_fConfidence;
  };
  struct SkeletonData {
    Joint m_aJoints[SKELETON_COUNT];
  };

  struct ImageData {
    ImageData()
        : m_pBuffer(NULL) {
    }
    int m_iResX;
    int m_iResY;
    struct Pixel {
      unsigned char cR;
      unsigned char cG;
      unsigned char cB;
    };
    Pixel* m_pBuffer;
  };

  struct DepthData {
    DepthData()
        : m_pBuffer(NULL) {
    }
    int             m_iResX;
    int             m_iResY;
    unsigned short* m_pBuffer;
  };

  VistaOpenNIDriver(IVistaDriverCreationMethod* pCreationMethod);
  ~VistaOpenNIDriver();

  xn::Context*        GetContext() const;
  xn::DepthGenerator* GetDepthGen() const;
  xn::UserGenerator*  GetUserGen() const;
  xn::ImageGenerator* GetImageGen() const;

 protected:
  bool         DoConnect();
  bool         DoDisconnect();
  virtual bool DoSensorUpdate(VistaType::microtime nTime);
  virtual bool PhysicalEnable(bool bEnable);

 private:
  void ReadDepthMeasure(VistaType::microtime nTime);
  void ReadImageMeasure(VistaType::microtime nTime);
  void DetermineTrackedUser(VistaType::microtime nTime);
  void ReadSkeletonData(VistaType::microtime nTime);

  void Cleanup();

 private:
  VistaDriverThreadAspect*        m_pThreadAspect;
  VistaDriverSensorMappingAspect* m_pMappingAspect;

  xn::Context*        m_pContext;
  xn::DepthGenerator* m_pDepthGen;
  xn::UserGenerator*  m_pUserGen;
  xn::ImageGenerator* m_pImageGen;

  int                  m_iTrackedUser;
  VistaType::microtime m_dTrackedUserLastSeen;

  std::vector<int> m_vecDepthMapSensorIDs;
  std::vector<int> m_vecImageSensorIDs;
  std::vector<int> m_vecSkeletonSensorIDs;
};

class OPENNIDRIVERAPI VistaOpenNIDriverCreationMethod : public IVistaDriverCreationMethod {
 public:
  VistaOpenNIDriverCreationMethod(IVistaTranscoderFactoryFactory* pFactory);
  virtual IVistaDeviceDriver* CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

//#endif // LINUX

#endif //__VISTAOPENNIDRIVER_H
