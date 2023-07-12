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

#include "VistaHapticDeviceEmulatorDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>
#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <VistaBase/VistaExceptionBase.h>

#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
static const int CONTROLCHANNEL = 2;
static const int FORCECHANNEL   = 1;
static const int DATACHANNEL    = 0;

class VistaHapticDeviceEmulatorControlAttachSequence
    : public VistaDriverConnectionAspect::IVistaConnectionSequence {
 public:
  VistaHapticDeviceEmulatorControlAttachSequence(VistaHapticDeviceEmulatorDriver* pDriver)
      : m_pDriver(pDriver) {
  }

  virtual bool operator()(VistaConnection* pCon) {
    if (!pCon->GetIsOpen())
      pCon->Open();

    bool bBlocking = pCon->GetIsBlocking();

    pCon->SetIsBlocking(true);

    int         iSize = 0;
    std::string vendor;
    std::string deviceType;
    std::string visualizerIP;

    try {
      pCon->ReadInt32(iSize);
      pCon->ReadString(visualizerIP, iSize);
      pCon->ReadInt32(iSize);
      pCon->ReadString(vendor, iSize);
      pCon->ReadInt32(iSize);
      pCon->ReadString(deviceType, iSize);
      pCon->ReadInt32(m_pDriver->m_inputDOF);
      pCon->ReadInt32(m_pDriver->m_outputDOF);
      pCon->ReadFloat32(m_pDriver->m_maxStiffness);
      pCon->ReadFloat32(m_pDriver->m_maxForce);
      pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMin[0]);
      pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMin[1]);
      pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMin[2]);
      pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMax[0]);
      pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMax[1]);
      pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMax[2]);
      pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMin[0]);
      pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMin[1]);
      pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMin[2]);
      pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMax[0]);
      pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMax[1]);
      pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMax[2]);

      std::cout << "Found haptic device emulator device with vendor: [" << vendor << "], device: ["
                << deviceType << "]\n";
    } catch (VistaExceptionBase& x) { x.PrintException(); }

    pCon->SetIsBlocking(bBlocking);
    pCon->SetIsBuffering(false);

    m_pDriver->m_pWorkSpace->SetWorkspace("MAXWORKSPACE",
        VistaBoundingBox(&m_pDriver->m_maxWorkspaceMin[0], &m_pDriver->m_maxWorkspaceMax[0]));
    m_pDriver->m_pWorkSpace->SetWorkspace(
        "USABLEWORKSPACE", VistaBoundingBox(&m_pDriver->m_maxUsableWorkspaceMin[0],
                               &m_pDriver->m_maxUsableWorkspaceMax[0]));

    return true;
  }

 private:
  VistaHapticDeviceEmulatorDriver* m_pDriver;
};

class VistaHapticDeviceEmulatorAttachSequence
    : public VistaDriverConnectionAspect::IVistaConnectionSequence {
 public:
  virtual bool operator()(VistaConnection* pCon) {
    if (!pCon->GetIsOpen())
      if (!pCon->Open())
        return false;

    pCon->SetIsBuffering(false);
    pCon->SetIsBlocking(false);

    return true;
  }
};

class VistaHapticDeviceEmulatorControlDetachSequence
    : public VistaDriverConnectionAspect::IVistaConnectionSequence {
 public:
  virtual bool operator()(VistaConnection* pCon) {
    if (pCon->GetIsOpen()) {
      pCon->WriteInt32('Q');     // send a quit statement
      pCon->WaitForSendFinish(); // really flush that
      pCon->Close();             // close connection
    }
    return true;
  }
};

typedef TVistaDriverEnableAspect<VistaHapticDeviceEmulatorDriver>
    VistaHapticDeviceEmulatorEnableAspect;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pConAspect(new VistaDriverConnectionAspect)
    , m_pWorkSpace(new VistaDriverWorkspaceAspect)
    , m_inputDOF(0)
    , m_outputDOF(0)
    , m_maxForce(0.0f)
    , m_maxStiffness(0.0f) {
  SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);

  // create connections
  RegisterAspect(m_pConAspect);

  RegisterAspect(new VistaHapticDeviceEmulatorEnableAspect(
      this, &VistaHapticDeviceEmulatorDriver::PhysicalEnable));

  m_pConAspect->SetConnection(
      DATACHANNEL, NULL, "DATACHANNEL", VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT);
  m_pConAspect->SetConnection(FORCECHANNEL, NULL, "FORCECHANNEL",
      VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT);
  m_pConAspect->SetConnection(CONTROLCHANNEL, NULL, "CONTROLCHANNEL",
      VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT);

  m_pConAspect->SetAttachSequence(DATACHANNEL, new VistaHapticDeviceEmulatorAttachSequence);
  m_pConAspect->SetAttachSequence(FORCECHANNEL, new VistaHapticDeviceEmulatorAttachSequence);
  m_pConAspect->SetAttachSequence(
      CONTROLCHANNEL, new VistaHapticDeviceEmulatorControlAttachSequence(this));
  m_pConAspect->SetDetachSequence(
      CONTROLCHANNEL, new VistaHapticDeviceEmulatorControlDetachSequence);

  // create and register a sensor for the haptic device emulator
  VistaDeviceSensor* pSensor = new VistaDeviceSensor;
  pSensor->SetTypeHint("");
  AddDeviceSensor(pSensor);

  // new VistaHapticDeviceEmulatorDriverMeasureTranscode
  pSensor->SetMeasureTranscode(GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder());

  // register force feedback aspect
  m_pForceFeedbackAspect = new VistaHapticDeviceEmulatorForceFeedbackAspect(this);
  RegisterAspect(m_pForceFeedbackAspect);

  // m_pForceFeedbackAspect->SetForcesEnabled(true);

  // register workspace aspect
  RegisterAspect(m_pWorkSpace);
}

VistaHapticDeviceEmulatorDriver::~VistaHapticDeviceEmulatorDriver() {
  VistaDeviceSensor* pSensor = GetSensorByIndex(0);
  RemDeviceSensor(pSensor);
  IVistaMeasureTranscode* pTC = pSensor->GetMeasureTranscode();
  pSensor->SetMeasureTranscode(NULL);
  delete pSensor;
  delete pTC;

  UnregisterAspect(m_pConAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pConAspect;

  UnregisterAspect(GetAspectById(IVistaDriverForceFeedbackAspect::GetAspectId()));

  VistaHapticDeviceEmulatorEnableAspect* enabler =
      GetAspectAs<VistaHapticDeviceEmulatorEnableAspect>(VistaDriverEnableAspect::GetAspectId());
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaHapticDeviceEmulatorDriver::DoSensorUpdate(VistaType::microtime dTs) {
  try {
    VistaConnection* pIncoming = m_pConAspect->GetConnection(DATACHANNEL);
    if (pIncoming && pIncoming->GetIsOpen()) {
      unsigned long nReadSize = pIncoming->PendingDataSize();

      if (nReadSize > 0) {
        std::vector<VistaType::byte> msg(
            sizeof(VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure));

        // we assume a packet oriented protocol here
        // otherwise, we might end up in a state where we read off
        // the header and tail is still missing
        int nDataRead = 0;
        for (unsigned int i = 0; i < nReadSize; i += nDataRead) {
          nDataRead = pIncoming->ReadRawBuffer((void*)&msg[0],
              sizeof(VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure));
          if (nDataRead < 0) {
            // TROUBLE!
            pIncoming->WaitForSendFinish();
            pIncoming->Close();
            return false;
          }
        }

        VistaByteBufferDeSerializer deSer;
        deSer.SetBuffer(
            &msg[0], sizeof(VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure));

        VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(0));
        if (pM == NULL)
          return false;

        // claim a new sensor measure
        MeasureStart(0, dTs);

        VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure* m =
            pM->getWrite<VistaHapticDeviceEmulatorMeasures::sHapticDeviceEmulatorMeasure>();
        deSer.ReadFloat32(m->m_afPosition[0]);
        deSer.ReadFloat32(m->m_afPosition[1]);
        deSer.ReadFloat32(m->m_afPosition[2]);

        for (int i = 0; i < 9; ++i)
          deSer.ReadFloat32(m->m_afRotMatrix[i]);

        deSer.ReadFloat32(m->m_afPosSCP[0]);
        deSer.ReadFloat32(m->m_afPosSCP[1]);
        deSer.ReadFloat32(m->m_afPosSCP[2]);

        deSer.ReadFloat32(m->m_afVelocity[0]);
        deSer.ReadFloat32(m->m_afVelocity[1]);
        deSer.ReadFloat32(m->m_afVelocity[2]);
        deSer.ReadFloat32(m->m_afForce[0]);
        deSer.ReadFloat32(m->m_afForce[1]);
        deSer.ReadFloat32(m->m_afForce[2]);
        deSer.ReadInt32(m->m_nButtonState);
        deSer.ReadFloat32(m->m_nUpdateRate);

        deSer.ReadFloat32(m->m_afOverheatState[0]);
        deSer.ReadFloat32(m->m_afOverheatState[1]);
        deSer.ReadFloat32(m->m_afOverheatState[2]);
        deSer.ReadFloat32(m->m_afOverheatState[3]);
        deSer.ReadFloat32(m->m_afOverheatState[4]);
        deSer.ReadFloat32(m->m_afOverheatState[5]);

        MeasureStop(0);
        VistaVector3D v3Force;
        VistaVector3D v3Torque;

        IVistaDriverForceFeedbackAspect::IForceAlgorithm* pMd =
            m_pForceFeedbackAspect->GetForceAlgorithm();
        if (pMd) {
          VistaVector3D pos(m->m_afPosition[0], m->m_afPosition[1], m->m_afPosition[2]);
          VistaVector3D vel(m->m_afVelocity[0], m->m_afVelocity[1], m->m_afVelocity[2]);

          VistaTransformMatrix t(float(m->m_afRotMatrix[0]), float(m->m_afRotMatrix[1]),
              float(m->m_afRotMatrix[2]), 0, float(m->m_afRotMatrix[3]), float(m->m_afRotMatrix[4]),
              float(m->m_afRotMatrix[5]), 0, float(m->m_afRotMatrix[6]), float(m->m_afRotMatrix[7]),
              float(m->m_afRotMatrix[8]), 0, 0, 0, 0, 1);
          VistaQuaternion      qQuat = -VistaQuaternion(t);

          // calc update force from that information
          // @todo: think about the timestamps
          pMd->UpdateForce(double(dTs), pos, vel, qQuat, v3Force, v3Torque);

          m_pForceFeedbackAspect->SetForce(v3Force, v3Torque);
        }
      }
    }

    VistaConnection* pControl =
        m_pConAspect->GetConnection(CONTROLCHANNEL); // claim control connection
    if (pControl && pControl->GetIsOpen()) {
      unsigned int nDataSize = 0;
      if ((nDataSize = pControl->PendingDataSize()) > 0) {
        unsigned int nDataRead = 0;
        int          nCommand  = 0;
        for (unsigned int i = 0; i < nDataSize; i += nDataRead) {
          nDataRead = pControl->ReadInt32(nCommand);
          if (nDataRead <= 0) {
            std::cerr << "[VistaHapticDeviceEmulator] Read error" << endl;
            pControl->Close();
            return false;
          }
        }
        switch (nCommand) {
        case 'E': // error flag sent
        {
          int         errorId = 0;
          int         nSize   = 0;
          std::string sErrorMsg;

          pControl->ReadInt32(errorId);
          pControl->ReadInt32(nSize);
          if (nSize > 0)
            pControl->ReadString(sErrorMsg, nSize);
          std::cerr << "[VistaHapticDeviceEmulator] Error: " << errorId << " " << sErrorMsg
                    << std::endl;

          break;
        }
        default:
          break;
        }
      }
    }
    return true;
  } catch (VistaExceptionBase& x) { x.PrintException(); }
  return false;
}

bool VistaHapticDeviceEmulatorDriver::DoConnect() {
  return true;
}

bool VistaHapticDeviceEmulatorDriver::DoDisconnect() {
  return true;
}

bool VistaHapticDeviceEmulatorDriver::PhysicalEnable(bool bEnable) {
  VistaConnection* pCon = m_pConAspect->GetConnection(CONTROLCHANNEL);
  return (pCon->WriteInt32(bEnable ? 'S' : 's') == sizeof(VistaType::sint32));
}

void VistaHapticDeviceEmulatorDriver::AttachMaximalBoundary() const {
  VistaConnection* pCon = m_pConAspect->GetConnection(2);
  if (pCon) {
    pCon->WriteInt32('B');
  }
}

void VistaHapticDeviceEmulatorDriver::DetachBoundary() const {
  VistaConnection* pCon = m_pConAspect->GetConnection(2);
  if (pCon) {
    pCon->WriteInt32('b');
  }
}

void VistaHapticDeviceEmulatorDriver::ResetEncoders() const {
  VistaConnection* pCon = m_pConAspect->GetConnection(2);
  if (pCon) {
    pCon->WriteInt32('R');
  }
}

void VistaHapticDeviceEmulatorDriver::GetMaxWorkspace(
    VistaVector3D& min, VistaVector3D& max) const {
  min = m_maxWorkspaceMin;
  max = m_maxWorkspaceMax;
}

void VistaHapticDeviceEmulatorDriver::GetMaxUsableWorkspace(
    VistaVector3D& min, VistaVector3D& max) const {
  min = m_maxUsableWorkspaceMin;
  max = m_maxUsableWorkspaceMax;
}

// #############################################################################

VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::
    VistaHapticDeviceEmulatorForceFeedbackAspect(VistaHapticDeviceEmulatorDriver* pDriver)
    : IVistaDriverForceFeedbackAspect()
    , m_pParent(pDriver)
    , m_pSerializer(new VistaByteBufferSerializer(256))
    , m_nDefaultStiffness(1.0f)
    , m_bForcesEnabled(false)
    , m_bDynamicFriction(false) {
}

VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::
    ~VistaHapticDeviceEmulatorForceFeedbackAspect() {
  delete m_pSerializer;
}

bool VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::SetForce(
    const VistaVector3D& v3Force, const VistaQuaternion& qAngularForce) {
  VistaConnection* pFC = GetChannel(FORCECHANNEL);
  m_pSerializer->SetByteorderSwapFlag(pFC->GetByteorderSwapFlag());
  m_pSerializer->ClearBuffer();

  m_pSerializer->WriteInt32('F');
  m_pSerializer->WriteFloat32(v3Force[0]);
  m_pSerializer->WriteFloat32(v3Force[1]);
  m_pSerializer->WriteFloat32(v3Force[2]);
  m_pSerializer->WriteFloat32(qAngularForce[0]);
  m_pSerializer->WriteFloat32(qAngularForce[1]);
  m_pSerializer->WriteFloat32(qAngularForce[2]);
  m_pSerializer->WriteFloat32(qAngularForce[3]);

  if (pFC->WriteRawBuffer(m_pSerializer->GetBuffer(), m_pSerializer->GetBufferSize()) ==
      m_pSerializer->GetBufferSize())
    return true;

  return false;
}

bool VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::SetConstraint(
    const VistaVector3D& contactPoint, const VistaVector3D& normal, const float stiffness,
    const VistaVector3D& internalForce) const {
  VistaConnection* pFC = GetChannel(FORCECHANNEL);

  m_pSerializer->SetByteorderSwapFlag(pFC->GetByteorderSwapFlag());
  m_pSerializer->ClearBuffer();

  m_pSerializer->WriteInt32('C');
  m_pSerializer->WriteFloat32(contactPoint[0]);
  m_pSerializer->WriteFloat32(contactPoint[1]);
  m_pSerializer->WriteFloat32(contactPoint[2]);
  m_pSerializer->WriteFloat32(normal[0]);
  m_pSerializer->WriteFloat32(normal[1]);
  m_pSerializer->WriteFloat32(normal[2]);

  float phantomStiffness = stiffness;

  if (stiffness <= 0.0f)
    phantomStiffness = m_nDefaultStiffness;
  else
    phantomStiffness = stiffness;

  if (phantomStiffness > 1.0f)
    phantomStiffness = 1.0f;

  m_pSerializer->WriteFloat32(phantomStiffness);

  m_pSerializer->WriteFloat32(internalForce[0]);
  m_pSerializer->WriteFloat32(internalForce[1]);
  m_pSerializer->WriteFloat32(internalForce[2]);

  if (pFC->WriteRawBuffer(m_pSerializer->GetBuffer(), m_pSerializer->GetBufferSize()) ==
      m_pSerializer->GetBufferSize())
    return true;
  else
    return false;
}

bool VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::
    SetForcesEnabled(bool bEnabled) {
  if (bEnabled != m_bForcesEnabled) {
    m_bForcesEnabled = bEnabled;

    VistaConnection* pFC = GetChannel(CONTROLCHANNEL);
    return (pFC->WriteInt32(m_bForcesEnabled ? 'F' : 'f') == sizeof(VistaType::sint32));
  }

  return false;
}

bool VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::
    SetDynamicFrictionEnabled(bool bEnabled) {
  if (bEnabled != m_bDynamicFriction) {
    m_bDynamicFriction = bEnabled;

    VistaConnection* pFC = GetChannel(CONTROLCHANNEL);
    return (pFC->WriteInt32(m_bDynamicFriction ? 'D' : 'd') == sizeof(VistaType::sint32));
  }

  return false;
}

bool VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::
    GetDynamicFrictionEnabled() const {
  return m_bDynamicFriction;
}

bool VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::
    GetForcesEnabled() const {
  return m_bForcesEnabled;
}

VistaConnection*
VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::GetChannel(
    int nIdx) const {
  if (m_pParent) {
    VistaDriverConnectionAspect* pConAsp = static_cast<VistaDriverConnectionAspect*>(
        m_pParent->GetAspectById(VistaDriverConnectionAspect::GetAspectId()));
    return pConAsp->GetConnection(nIdx);
  }
  return NULL;
}

int VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::GetNumInputDOF()
    const {
  return m_pParent->m_inputDOF;
}

int VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::GetNumOutputDOF()
    const {
  return m_pParent->m_outputDOF;
}

float VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::
    GetMaximumStiffness() const {
  return m_pParent->m_maxStiffness;
}

float VistaHapticDeviceEmulatorDriver::VistaHapticDeviceEmulatorForceFeedbackAspect::
    GetMaximumForce() const {
  return m_pParent->m_maxForce;
}