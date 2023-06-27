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

#ifndef _VISTASYSTEMCONFIGURATORS_H
#define _VISTASYSTEMCONFIGURATORS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaKernel/VistaDriverPropertyConfigurator.h>
#include <VistaKernel/VistaKernelConfig.h>
//#include "VistaSensorTransformConfigurator.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaWindow;
class VistaConnection;
class VistaEventManager;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaWindowConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  VistaWindowConfigurator(VistaDisplayManager* pSys);
  virtual ~VistaWindowConfigurator();

  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(VistaWindowConfigurator, VistaDriverPropertyConfigurator::IConfigurator)

 private:
  VistaDriverAbstractWindowAspect::IWindowHandle* GetHandleForWindow(VistaWindow* pWindow);

 private:
  VistaDisplayManager*                                                    m_pDispMgr;
  std::map<VistaWindow*, VistaDriverAbstractWindowAspect::IWindowHandle*> m_mapHandles;
  static std::string                                                      SsReflectionType;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class VISTAKERNELAPI VistaSensorMappingConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  VistaSensorMappingConfigurator();
  virtual ~VistaSensorMappingConfigurator();

  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(VistaSensorMappingConfigurator, VistaDriverPropertyConfigurator::IConfigurator)
 private:
  static std::string SsReflectionType;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class VISTAKERNELAPI VistaGenericHistoryConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  VistaGenericHistoryConfigurator();
  virtual ~VistaGenericHistoryConfigurator();

  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(VistaGenericHistoryConfigurator, VistaDriverPropertyConfigurator::IConfigurator)
 private:
  static std::string SsReflectionType;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class VISTAKERNELAPI VistaConnectionConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  VistaConnectionConfigurator();
  virtual ~VistaConnectionConfigurator();

  class VISTAKERNELAPI IConnectionFactoryMethod {
    friend class VistaConnectionConfigurator;

   public:
    virtual VistaConnection* CreateConnection(const VistaPropertyList& oProps) = 0;

   protected:
    void FailOnConnection(VistaConnection* pCon, const std::string& sReason) const;
    IConnectionFactoryMethod() {
    }
    virtual ~IConnectionFactoryMethod() {
    }
  };

  typedef std::map<std::string, IConnectionFactoryMethod*>::const_iterator const_iterator;
  typedef std::map<std::string, IConnectionFactoryMethod*>::iterator       iterator;

  const_iterator begin() const;
  iterator       begin();
  const_iterator end() const;
  iterator       end();

  bool AddConFacMethod(const std::string& sKey, IConnectionFactoryMethod*);

  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(VistaConnectionConfigurator, VistaDriverPropertyConfigurator::IConfigurator)
 public:
  std::map<std::string, IConnectionFactoryMethod*> m_mpFactories;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class VISTAKERNELAPI VistaDriverProtocolConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(VistaDriverProtocolConfigurator, VistaDriverPropertyConfigurator::IConfigurator)
};

class VISTAKERNELAPI VistaDriverLoggingConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  VistaDriverLoggingConfigurator();
  ~VistaDriverLoggingConfigurator();

  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(VistaDriverLoggingConfigurator, VistaDriverPropertyConfigurator::IConfigurator)
 private:
};

class VISTAKERNELAPI VistaDriverAttachOnlyConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  VistaDriverAttachOnlyConfigurator();
  ~VistaDriverAttachOnlyConfigurator();

  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(VistaDriverAttachOnlyConfigurator, VistaDriverPropertyConfigurator::IConfigurator)
};

class VISTAKERNELAPI VistaDeviceIdentificationConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  VistaDeviceIdentificationConfigurator();
  ~VistaDeviceIdentificationConfigurator();

  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(
      VistaDeviceIdentificationConfigurator, VistaDriverPropertyConfigurator::IConfigurator)
};

class VISTAKERNELAPI VistaDriverReferenceFrameConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  VistaDriverReferenceFrameConfigurator();
  ~VistaDriverReferenceFrameConfigurator();

  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(
      VistaDriverReferenceFrameConfigurator, VistaDriverPropertyConfigurator::IConfigurator)
};

class VISTAKERNELAPI VistaDriverParameterConfigurator
    : public VistaDriverPropertyConfigurator::IConfigurator {
 public:
  VistaDriverParameterConfigurator();
  ~VistaDriverParameterConfigurator();

  bool Configure(IVistaDeviceDriver* pDriver, const VistaPropertyList& oDriverSection,
      const VistaPropertyList& oConfig);

  REFL_INLINEIMP(VistaDriverParameterConfigurator, VistaDriverPropertyConfigurator::IConfigurator)
 private:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEMCONFIGURATORS_H
