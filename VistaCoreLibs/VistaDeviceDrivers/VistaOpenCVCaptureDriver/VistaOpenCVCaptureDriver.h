/*============================================================================*/
/*                    ViSTA VR toolkit - OpenCV2 driver                  */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/

#ifndef __VISTAOPENCVCAPTUREDRIVER_H
#define __VISTAOPENCVCAPTUREDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaAutoBufferPool.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

#if defined(WIN32) && !defined(VISTAOPENCVCAPTUREDRIVER_STATIC)
#ifdef VISTAOPENCVCAPTUREDRIVER_EXPORTS
#define VISTAOPENCVCAPTUREDRIVERAPI __declspec(dllexport)
#else
#define VISTAOPENCVCAPTUREDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAOPENCVCAPTUREDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverInfoAspect;
class VistaDriverThreadAspect;

namespace cv {
class VideoCapture;
class Mat;
} // namespace cv

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAOPENCVCAPTUREDRIVERAPI VistaOpenCVCaptureDriver : public IVistaDeviceDriver {
 public:
  struct _videotype {};

  struct _typetype {
    _typetype()
        : m_nWidth(0)
        , m_nHeight(0)
        , m_nFormat(0.0) {
    }

    VistaType::uint32  m_nWidth, m_nHeight;
    VistaType::float64 m_nFormat;
  };

  typedef _typetype VideoType;

  struct _frametype {
    _frametype()
        : m_Frame(0L) {
    }

    cv::Mat* m_Frame;
  };

  typedef _frametype FrameType;

  VistaOpenCVCaptureDriver(IVistaDriverCreationMethod*);
  virtual ~VistaOpenCVCaptureDriver();

  class Parameters : public VistaDriverGenericParameterAspect::IParameterContainer {
    REFL_DECLARE
   public:
    Parameters(VistaOpenCVCaptureDriver* pDriver);

    enum {
      MSG_CAPTUREDEVICE_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
      MSG_REQUESTEDWIDTH_CHG,
      MSG_REQUESTEDHEIGHT_CHG,
      MSG_WIDTH_CHG,
      MSG_HEIGHT_CHG,
      MSG_FORMAT_CHG,
      MSG_FRAMERATE_CHG,
      MSG_DOESHW2RGB_CHG,
      MSG_FROMFILE_CHG,
      MSG_WANTRGB_CHG,
      MSG_LAST
    };

    int  GetCaptureDevice() const;
    bool SetCaptureDevice(int);

    int  GetWidthRequested() const;
    bool SetWidthRequested(int width);

    int  GetHeightRequested() const;
    bool SetHeightRequested(int height);

    int  GetWidth() const;
    bool SetWidth(int width);

    int  GetHeight() const;
    bool SetHeight(int height);

    int  GetFormat() const;
    bool SetFormat(int);

    int  GetFrameRate() const;
    bool SetFrameRate(int);

    bool GetDoesHW2RGB() const;
    bool SetDoesHW2RGB(bool);

    bool GetWantRGB() const;
    bool SetWantRGB(bool);

    std::string GetFromFile() const;
    bool        SetFromFile(const std::string&);

    size_t GetMemorySize() const;

    virtual bool TurnDef(bool def);
    bool         GetIsDef() const;
    virtual bool Apply();

   private:
    VistaOpenCVCaptureDriver* m_parent;

    int  m_device, m_widthRQ, m_heightRQ, m_width, m_height, m_framerate;
    bool m_bDef, m_doesHw2RGB, m_bwantRGB;
    int  m_format;

    std::string m_fromFile;
  };

  void SignalPropertyChanged(int msg);

 protected:
  virtual bool PhysicalEnable(bool bEnable);
  bool         DoSensorUpdate(VistaType::microtime nTs);

  bool DoConnect();
  bool DoDisconnect();

 private:
  void OnReconnectCamera(bool bForce = false);
  void OnUpdateMeasureSize();

  VistaDriverInfoAspect*   m_pInfo;
  VistaDriverThreadAspect* m_pThread;

  VistaDriverGenericParameterAspect* m_pParams;

  cv::VideoCapture*                       m_pCapture;
  IVistaDeviceDriver::AllocMemoryFunctor* m_alloc;
  IVistaDeviceDriver::ClaimMemoryFunctor* m_dealloc;
};

class VISTAOPENCVCAPTUREDRIVERAPI VistaOpenCVCaptureDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  VistaOpenCVCaptureDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  virtual IVistaDeviceDriver* CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif //__VISTAOPENCVCAPTURE_H
