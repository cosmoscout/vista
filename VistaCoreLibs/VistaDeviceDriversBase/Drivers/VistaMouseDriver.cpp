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

#include "VistaMouseDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

REFL_IMPLEMENT_FULL(IVistaMouseDriver::MouseDriverParameters,
    VistaDriverGenericParameterAspect::IParameterContainer);

namespace {
IVistaPropertyGetFunctor* SaParameterGetter[] = {
    new TVistaPropertyGet<bool, IVistaMouseDriver::MouseDriverParameters,
        VistaProperty::PROPT_BOOL>("CAPTURE_CURSOR",
        SsReflectionName, // SsReflectionName is defined in the REFL_IMPLEMENT_FULL macro above!
        &IVistaMouseDriver::MouseDriverParameters::GetCaptureCursor,
        "returns if the driver captures the cursor, i.e. holds it in the window center and only "
        "reports deltas"),
    NULL};

IVistaPropertySetFunctor* SaParameterSetter[] = {
    new TVistaPropertySet<bool, bool, IVistaMouseDriver::MouseDriverParameters>("CAPTURE_CURSOR",
        SsReflectionName, &IVistaMouseDriver::MouseDriverParameters::SetCaptureCursor,
        "sets if the driver captures the cursor, i.e. holds it in the window center and only "
        "reports deltas"),

    NULL};
} // namespace

class VistaMouseDriverMeasureTranscode : public IVistaMeasureTranscode {
 public:
  VistaMouseDriverMeasureTranscode();
  virtual ~VistaMouseDriverMeasureTranscode();

  static std::string GetTypeString();

  REFL_DECLARE
};

VistaMouseDriverMeasureTranscode::VistaMouseDriverMeasureTranscode() {
  // inherited as protected member
  m_nNumberOfScalars = IVistaMouseDriver::BT_LAST;
}

VistaMouseDriverMeasureTranscode::~VistaMouseDriverMeasureTranscode() {
}

std::string VistaMouseDriverMeasureTranscode::GetTypeString() {
  return "VistaMouseDriverMeasureTranscode";
}

REFL_IMPLEMENT(VistaMouseDriverMeasureTranscode, IVistaMeasureTranscode);

// ######################################################################
// TRANSCODER FACTORY
// ######################################################################

IVistaMeasureTranscode* VistaMouseDriverTranscodeFactory::CreateTranscoder() {
  return new VistaMouseDriverMeasureTranscode;
}

void VistaMouseDriverTranscodeFactory::DestroyTranscoder(IVistaMeasureTranscode* trans) {
  delete trans;
}

class VistaMousePosTranscode : public IVistaMeasureTranscode::V3Get {
 public:
  VistaMousePosTranscode()
      : IVistaMeasureTranscode::V3Get("POSITION", VistaMouseDriverMeasureTranscode::GetTypeString(),
            "mouse pixel coords in comp (0,1) -> (x,y)") {
  }

  virtual VistaVector3D GetValue(const VistaSensorMeasure* pMeasure) const {
    const IVistaMouseDriver::_sMouseMeasure* m =
        pMeasure->getRead<IVistaMouseDriver::_sMouseMeasure>();

    return VistaVector3D(float(m->m_nX), float(m->m_nY), 0);
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, VistaVector3D& v3Pos) const {
    v3Pos = GetValue(pMeasure);
    return true;
  }
};

class VistaMouseScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet {
 public:
  VistaMouseScalarTranscode()
      : IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
            VistaMouseDriverMeasureTranscode::GetTypeString(),
            "mouse buttons in comp (0,3) -> (l,m,r,ws,wd)") {
  }

  virtual bool GetValueIndexed(
      const VistaSensorMeasure* pMeasure, double& dScalar, unsigned int nIndex) const {
    if (!pMeasure || nIndex >= (IVistaMouseDriver::BT_LAST))
      return false;

    const IVistaMouseDriver::_sMouseMeasure* m =
        pMeasure->getRead<IVistaMouseDriver::_sMouseMeasure>();

    dScalar = m->m_nButtons[nIndex];
    return true;
  }
};

class VistaMouseWheelStateTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaMouseWheelStateTranscode()
      : IVistaMeasureTranscode::TTranscodeValueGet<int>("WHEEL_STATE",
            VistaMouseDriverMeasureTranscode::GetTypeString(), "wheel state as an int") {
  }

  virtual int GetValue(const VistaSensorMeasure* pMeasure) const {
    const IVistaMouseDriver::_sMouseMeasure* m =
        pMeasure->getRead<IVistaMouseDriver::_sMouseMeasure>();
    return m->m_nButtons[IVistaMouseDriver::BT_WHEEL_STATE];
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, int& nState) const {
    nState = GetValue(pMeasure);
    return true;
  }
};

class VistaMouseWheelDirTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaMouseWheelDirTranscode()
      : IVistaMeasureTranscode::TTranscodeValueGet<int>("WHEEL_DIR",
            VistaMouseDriverMeasureTranscode::GetTypeString(), "wheel dir (-1 up, 1 down)") {
  }

  virtual int GetValue(const VistaSensorMeasure* pMeasure) const {
    const IVistaMouseDriver::_sMouseMeasure* m =
        pMeasure->getRead<IVistaMouseDriver::_sMouseMeasure>();
    return m->m_nButtons[IVistaMouseDriver::BT_WHEEL_DIR];
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, int& nState) const {
    nState = GetValue(pMeasure);
    return true;
  }
};

class VistaMousePixelPosTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int> {
 public:
  VistaMousePixelPosTranscode(int nPosId, const std::string& strLabel, const std::string& strDesc)
      : IVistaMeasureTranscode::TTranscodeValueGet<int>(
            strLabel, VistaMouseDriverMeasureTranscode::GetTypeString(), strDesc)
      , m_nPosId(nPosId) {
  }

  virtual int GetValue(const VistaSensorMeasure* pMeasure) const {
    const IVistaMouseDriver::_sMouseMeasure* m =
        pMeasure->getRead<IVistaMouseDriver::_sMouseMeasure>();
    return m_nPosId ? m->m_nY : m->m_nX;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, int& nState) const {
    nState = GetValue(pMeasure);
    return true;
  }

  int m_nPosId;
};

class VistaMouseButtonTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<bool> {
 public:
  VistaMouseButtonTranscode(
      IVistaMouseDriver::eBt Bt, const std::string& strLabel, const std::string& strDesc)
      : IVistaMeasureTranscode::TTranscodeValueGet<bool>(
            strLabel, VistaMouseDriverMeasureTranscode::GetTypeString(), strDesc)
      , m_bt(Bt) {
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure) const {
    const IVistaMouseDriver::_sMouseMeasure* m =
        pMeasure->getRead<IVistaMouseDriver::_sMouseMeasure>();
    return m->m_nButtons[m_bt] ? true : false;
  }

  virtual bool GetValue(const VistaSensorMeasure* pMeasure, bool& nState) const {
    nState = GetValue(pMeasure);
    return true;
  }

  IVistaMouseDriver::eBt m_bt;
};

static IVistaPropertyGetFunctor* SapGetter[] = {new VistaMousePosTranscode,
    new VistaMouseScalarTranscode, new VistaMouseWheelStateTranscode,
    new VistaMouseWheelDirTranscode,
    new VistaMousePixelPosTranscode(0, "X_POS", "x pos in pixel space"),
    new VistaMousePixelPosTranscode(1, "Y_POS", "y pos in pixel space"),
    new VistaMouseButtonTranscode(
        IVistaMouseDriver::BT_LEFT, "LEFT_BUTTON", "left button state (true: pressed)"),
    new VistaMouseButtonTranscode(
        IVistaMouseDriver::BT_RIGHT, "RIGHT_BUTTON", "right button state (true: pressed)"),
    new VistaMouseButtonTranscode(
        IVistaMouseDriver::BT_MIDDLE, "MIDDLE_BUTTON", "middle button state (true: pressed)"),
    NULL};

namespace VistaMouseDriverUtil {
void ReleaseProperties() {
  IVistaPropertyGetFunctor** git = SapGetter;

  while (*git)
    delete *git++;

  // release parameter props
  IVistaPropertyGetFunctor** pGetter = SaParameterGetter;
  IVistaPropertySetFunctor** pSetter = SaParameterSetter;

  while (*pGetter)
    delete *pGetter++;

  while (*pSetter)
    delete *pSetter++;
}
} // namespace VistaMouseDriverUtil

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaMouseDriver::IVistaMouseDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm) {
  m_pParams = new VistaDriverGenericParameterAspect(
      new TParameterCreate<IVistaMouseDriver, IVistaMouseDriver::MouseDriverParameters>(this));
  RegisterAspect(m_pParams);

  SetUpdateType(IVistaDeviceDriver::UPDATE_EXPLICIT_POLL);
}

IVistaMouseDriver::~IVistaMouseDriver() {
  // as a kind of "service" delete all attached sensors
  // and remove from history.
  for (unsigned int n = 0; n < GetNumberOfSensors(); ++n) {
    VistaDeviceSensor* pSen = GetSensorByIndex(n);
    RemDeviceSensor(pSen);
    GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder(pSen->GetMeasureTranscode());
    pSen->SetMeasureTranscode(NULL);
    delete pSen;
  }
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
unsigned int IVistaMouseDriver::AddDeviceSensor(
    VistaDeviceSensor* pSensor, size_t pos, AllocMemoryFunctor* amf, ClaimMemoryFunctor* cmf) {
  // add to parent driver
  if (pSensor->GetTypeHint() == "") {
    pSensor->SetTypeHint("");
  }
  if (!pSensor->GetMeasureTranscode()) {
    pSensor->SetMeasureTranscode(
        GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder());
  }
  return IVistaDeviceDriver::AddDeviceSensor(pSensor, pos, amf, cmf);
}

std::string IVistaMouseDriver::GetMouseTranscodeName() {
  return VistaMouseDriverMeasureTranscode::GetTypeString();
}

bool IVistaMouseDriver::UpdateMousePosition(unsigned int nIdx, int x, int y) {
  VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(nIdx));

  if (pM == NULL)
    return false;

  _sMouseMeasure* m = pM->getWrite<_sMouseMeasure>();
  m->m_nX           = x;
  m->m_nY           = y;
  return true;
}

bool IVistaMouseDriver::UpdateMouseButton(unsigned int nIdx, eBt nMouseBt, double nPressed) {
  if (nMouseBt >= BT_LAST)
    return false;

  VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(nIdx));
  if (pM == NULL)
    return false;
  _sMouseMeasure* m       = pM->getWrite<_sMouseMeasure>();
  m->m_nButtons[nMouseBt] = int(nPressed);

  return true;
}

IVistaMouseDriver::MouseDriverParameters* IVistaMouseDriver::GetParameters() {
  return m_pParams->GetParameter<MouseDriverParameters>();
}

bool IVistaMouseDriver::MouseDriverParameters::GetCaptureCursor() const {
  return m_bCaptureCursor;
}

bool IVistaMouseDriver::MouseDriverParameters::SetCaptureCursor(bool bCapture) {
  if (m_bCaptureCursor == bCapture)
    return true;
  m_bCaptureCursor = bCapture;
  Notify(MSG_CAPTURE_CURSOR_CHG);
  return true;
}

IVistaMouseDriver::MouseDriverParameters::MouseDriverParameters(IVistaMouseDriver* pDriver)
    : m_bCaptureCursor(false) {
}
