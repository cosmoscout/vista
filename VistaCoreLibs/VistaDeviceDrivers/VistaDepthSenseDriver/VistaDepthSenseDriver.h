/*============================================================================*/
/*                    ViSTA VR toolkit - DepthSense Camera Driver             */
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

#ifndef __VISTADEPTHSENSEDRIVER_H
#define __VISTADEPTHSENSEDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
#if defined(WIN32) && !defined(VISTADEPTHSENSEDRIVER_STATIC)
#ifdef VISTADEPTHSENSEDRIVER_EXPORTS
#define VISTADEPTHSENSEDRIVERAPI __declspec(dllexport)
#else
#define VISTADEPTHSENSEDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTADEPTHSENSEDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverThreadAspect;
class VistaDriverInfoAspect;

class VistaDepthSenseMeasurementThread;
struct DepthSensePrivate;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTADEPTHSENSEDRIVERAPI VistaDepthSenseDriver : public IVistaDeviceDriver {
 public:
  VistaDepthSenseDriver(IVistaDriverCreationMethod*);
  virtual ~VistaDepthSenseDriver();

  struct ColorMeasure {
    // we measure as RGB888 320x240 format
    unsigned char frame[320 * 240 * 3];
  };
  struct DepthMeasure {
    // 16 bit depth at 320x240
    VistaType::ushort16 frame[320 * 240];
  };
  struct UVMapMeasure {
    // float u,v map at 320x240
    float frame[320 * 240 * 2];
  };

  class DepthSenseParameters : public VistaDriverGenericParameterAspect::IParameterContainer {
    REFL_DECLARE
   public:
    DepthSenseParameters(VistaDepthSenseDriver* pDriver);

    enum {
      MSG_WHITEBALANCE_AUTO_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
      MSG_DUMP_AVAILABLE_CONFIGS_CHG,
      MSG_DEPTH_DENOISING_CHG,
      MSG_LAST
    };

    bool GetWhiteBalanceAuto() const;
    bool SetWhiteBalanceAuto(bool bWhiteBalanceAuto);

    bool GetDumpAvailableConfigs() const;
    bool SetDumpAvailableConfigs(bool bDumpAvailableConfigs);

    bool GetDepthDenoising() const;
    bool SetDepthDenoising(bool bDepthDenoising);

   private:
    VistaDepthSenseDriver* m_parent;

    bool m_bWhiteBalanceAuto;
    bool m_bDumpAvailableConfigs;
    bool m_bDepthDenoising;
  };

  void SignalPropertyChanged(int msg);

 protected:
  virtual bool DoConnect();
  virtual bool DoDisconnect();

 private:
  virtual bool PhysicalEnable(bool bEnable);
  virtual bool DoSensorUpdate(VistaType::microtime nTs);

  VistaDriverInfoAspect*             m_pInfo;
  VistaDriverGenericParameterAspect* m_pParamAspect;

  friend class VistaDepthSenseMeasurementThread;
  VistaDepthSenseMeasurementThread* m_pMeasureThread;

  DepthSensePrivate* m_pPrivate;
  ColorMeasure       m_sColorMeasure;
  DepthMeasure       m_sDepthMeasure;
  UVMapMeasure       m_sUVMapMeasure;
};

class VISTADEPTHSENSEDRIVERAPI VistaDepthSenseDriverCreationMethod
    : public IVistaDriverCreationMethod {
 public:
  VistaDepthSenseDriverCreationMethod(IVistaTranscoderFactoryFactory* fac);
  virtual IVistaDeviceDriver* CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif //__VISTADEPTHSENSEDRIVER_H
