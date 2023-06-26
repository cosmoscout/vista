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

/* The Windows-specific code (especially the InitDriver routine) has been
 * copied from the VRPN toolkit, which is released in the public domain.
 * Those program parts stay in the public domain, eventually to be replaced by
 * an own implementation.
 * All other parts are Copyright of RWTH Aachen university and covered under the
 * terms of the GNU Lesser General Public License, as stated above.
 */

// include header here
#include "VistaHIDCommonShare.h"

#include <iomanip>
#include <iostream>
#include <string>

#include <cstdio>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include "VistaHIDDriverForceFeedbackAspect.h"
#include <VistaDeviceDriversBase/DriverAspects/VistaDeviceIdentificationAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverForceFeedbackAspect.h>

#include <VistaInterProcComm/Connections/VistaConnection.h>
#include <VistaInterProcComm/Connections/VistaConnectionFile.h>

#include <VistaBase/VistaStreamUtils.h>

#include "VistaHIDDriver.h"

#ifdef LINUX

#include <linux/input.h>
#include <sys/types.h>

#include <dirent.h>
#include <fcntl.h>

#include <cstring>
#include <stdio.h>

#elif defined WIN32

#include <VistaBase/VistaBaseTypes.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverThreadAspect.h>
#include <Windows.h>
#include <setupapi.h>

#endif

#include <stdlib.h>
/* -------------------------------------------------------------------------- */

namespace {
struct sDevLookup {
  sDevLookup()
      : m_nDevId(0)
      , m_nVendorId(0) {
  }

  sDevLookup(int nVendor, int DevId, const std::string& strTransName)
      : m_nDevId(DevId)
      , m_nVendorId(nVendor)
      , m_strTranscoder(strTransName) {
  }

  int         m_nDevId, m_nVendorId;
  std::string m_strTranscoder;
};

sDevLookup SDevCaps[] = {sDevLookup(0x046d, 0xc626, "3DCONNEXION_SPACENAVIGATOR"),
    sDevLookup(0x046d, 0xc218, "LOGITECH_RUMBLEPAD2")};

/* -------------------------------------------------------------------------- */

int constant_test_bit(int nr, const volatile unsigned long* addr) {
  return ((1UL << (nr & 31)) & (addr[nr >> 5])) != 0;
}

#if defined(WIN32)
// Include some of the relevant definitions from hidsdi.h
// You're free to use the real hidsdi.h from the Windows DDK if
// you want to, but it requires a lot of compiler magic to get
// all the security checks working. This doesn't require a CD
// download from Microsoft either...
#ifndef _HIDSDI_H
#define _HIDSDI_H

typedef USHORT USAGE;

typedef struct {
  ULONG  Size;
  USHORT VendorID;
  USHORT ProductID;
  USHORT VersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

typedef struct _HIDP_CAPS {
  USAGE  Usage;
  USAGE  UsagePage;
  USHORT InputReportByteLength;
  USHORT OutputReportByteLength;
  USHORT FeatureReportByteLength;
  USHORT Reserved[17];

  USHORT NumberLinkCollectionNodes;

  USHORT NumberInputButtonCaps;
  USHORT NumberInputValueCaps;
  USHORT NumberInputDataIndices;

  USHORT NumberOutputButtonCaps;
  USHORT NumberOutputValueCaps;
  USHORT NumberOutputDataIndices;

  USHORT NumberFeatureButtonCaps;
  USHORT NumberFeatureValueCaps;
  USHORT NumberFeatureDataIndices;
} HIDP_CAPS, *PHIDP_CAPS;

// getProc: Quick-and-dirty wrapper around LoadLibrary/GetProcAddress
// - entryPoint: Name of exported function (decorated if __dllexport)
static void* getProc(const char* entryPoint) {
  static HMODULE hdll = NULL;

  if (!hdll)
    hdll = LoadLibrary("HID.DLL");

  void* p = GetProcAddress(hdll, entryPoint);
  if (!p) {
    throw __FILE__ ": GetProcAddress failed!"; // Should never happen in here
  }
  return p;
}

// Macro to avoid retyping HID.dll function signatures
// Also ensures that we HAVE our function pointers before we need them
#define HID_FUNC(name, type, args)                                                                 \
  type(__stdcall* name) args = (type(__stdcall*) args)getProc(#name)

HID_FUNC(HidD_GetHidGuid, void, (LPGUID));
HID_FUNC(HidD_GetPreparsedData, BOOLEAN, (HANDLE, void*));
HID_FUNC(HidD_FreePreparsedData, BOOLEAN, (void*));
HID_FUNC(HidD_GetAttributes, BOOLEAN, (HANDLE, PHIDD_ATTRIBUTES));
HID_FUNC(HidD_GetManufacturerString, BOOLEAN, (HANDLE, PVOID, ULONG));
HID_FUNC(HidD_GetProductString, BOOLEAN, (HANDLE, PVOID, ULONG));
HID_FUNC(HidD_GetSerialNumberString, BOOLEAN, (HANDLE, PVOID, ULONG));
HID_FUNC(HidP_GetCaps, LONG, (void*, PHIDP_CAPS));

#endif

#endif // WIN32

} // end of namespace

class VistaHIDAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence {
 public:
  VistaHIDAttachSequence(VistaHIDDriver* pDriver)
      : m_pDriver(pDriver) {
  }

  virtual bool operator()(VistaConnection* pCon) {
#ifdef LINUX
    bool ret = false;

    if (pCon->GetIsOpen())
      ret = true;

    if (pCon->Open()) {
      void* evtype_b;

      std::cout << "[HIDDriver] checking wether device supports Force Feedback.. ";

      evtype_b = malloc(8);
      memset(evtype_b, 0, 8);
      if ((ioctl(pCon->GetConnectionDescriptor(), EVIOCGBIT(0, EV_MAX), evtype_b)) < 0) {
        perror("[HIDDriver] evdev ioctl, get event types");
      }

      if (constant_test_bit(EV_FF, (const volatile unsigned long int*)evtype_b)) {
        std::cout << "yes" << std::endl;
        VistaHIDDriverForceFeedbackAspect* pForce = new VistaHIDDriverForceFeedbackAspect(pCon);
        m_pDriver->RegisterAspect(pForce);
      } else
        std::cout << "no" << std::endl;

      free(evtype_b);

      ret = true;
    }

    return ret;
#else
    if (pCon->GetIsOpen())
      return true;
    else
      return pCon->Open();
#endif
  }

  VistaHIDDriver* m_pDriver;
};

class VistaHIDDetachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence {
 public:
  VistaHIDDetachSequence(VistaHIDDriver* pDriver) {
    m_pDriver = pDriver;
  }

  virtual bool operator()(VistaConnection* pCon) {
    IVistaDriverForceFeedbackAspect* pForce = dynamic_cast<IVistaDriverForceFeedbackAspect*>(
        m_pDriver->GetAspectById(IVistaDriverForceFeedbackAspect::GetAspectId()));

    if (pForce) {
      m_pDriver->UnregisterAspect(pForce, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
      delete pForce;
    }

    pCon->Close();
    return !pCon->GetIsOpen();
  }

 private:
  VistaHIDDriver* m_pDriver;
};

typedef TVistaDriverEnableAspect<VistaHIDDriver> VistaHIDDriverEnableAspect;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaHIDDriver::VistaHIDDriver(IVistaDriverCreationMethod* crm)
    : IVistaDeviceDriver(crm)
    , m_pSensorMappingAsp(new VistaDriverSensorMappingAspect(crm))
    , m_pIdentAsp(new VistaDeviceIdentificationAspect)
    ,
#ifdef LINUX
    m_pConnection(new VistaDriverConnectionAspect)
    , m_pForce(NULL)
    , m_vEvents(10)
    ,
#endif
#ifdef WIN32
    m_pThreadAspect(NULL)
    ,
#endif
    m_pMeasure(new VistaHIDCommonShare::sHIDMeasure) {
  memset(m_pMeasure, 0, sizeof(VistaHIDCommonShare::sHIDMeasure));

#ifdef LINUX
  SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);

  m_pConnection->SetConnection(0, NULL, "MAIN",
      VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT,
      VistaDriverConnectionAspect::REPLACE_ONLY_WHEN_NOT_SET);
  m_pConnection->SetDetachSequence(0, new VistaHIDDetachSequence(this));
  m_pConnection->SetAttachSequence(0, new VistaHIDAttachSequence(this));

  RegisterAspect(m_pConnection);
#endif
#ifdef WIN32
  SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

  m_hReadEvent = CreateEvent(NULL /* security */, TRUE /* manual-reset */,
      TRUE /* signaled at start */, NULL /* no name needed */
  );

  m_pThreadAspect = new VistaDriverThreadAspect(this);
  RegisterAspect(m_pThreadAspect);
#endif

  VistaDeviceSensor* pSen = new VistaDeviceSensor();

  pSen->SetMeasureTranscode(
      GetFactory()->GetTranscoderFactoryForSensor("HID_RAW")->CreateTranscoder());
  pSen->SetTypeHint("HID_RAW");
  AddDeviceSensor(pSen);

  // m_pSensorMappingAsp->RegisterType("GRIFFIN_POWERMATE",
  //								  sizeof(_sHIDMeasure),
  //								  100,
  //GetFactory()->GetTranscoderFactoryForSensor("GRIFFIN_POWERMATE"));

  // m_pSensorMappingAsp->RegisterType("3DCONNEXION_SPACENAVIGATOR",
  //									sizeof(_sHIDMeasure),
  //									100,
  //GetFactory()->GetTranscoderFactoryForSensor("3DCONNEXION_SPACENAVIGATOR"));

  // m_pSensorMappingAsp->RegisterType("3DCONNEXION_SPACENAVIGATOR_CMD",
  //								  sizeof(_sHIDMeasure),
  //								  100,
  //GetFactory()->GetTranscoderFactoryForSensor("3DCONNEXION_SPACENAVIGATOR_CMD"));

  // m_pSensorMappingAsp->RegisterType("LOGITECH_RUMBLEPAD2",
  //								  sizeof(_sHIDMeasure),
  //								  100,
  //GetFactory()->GetTranscoderFactoryForSensor("LOGITECH_RUMBLEPAD2"));

  // m_pSensorMappingAsp->RegisterType("GRIFFIN_POWERMATE",
  //								  sizeof(_sHIDMeasure),
  //								  100,
  //GetFactory()->GetTranscoderFactoryForSensor("GRIFFIN_POWERMATE"));

  // m_pSensorMappingAsp->RegisterType("HID_RAW",
  //								  sizeof(_sHIDMeasure),
  //								  100,
  //GetFactory()->GetTranscoderFactoryForSensor("HID_RAW"));

  m_pSensorMappingAsp->RegisterType("GRIFFIN_POWERMATE");

  m_pSensorMappingAsp->RegisterType("3DCONNEXION_SPACENAVIGATOR");

  m_pSensorMappingAsp->RegisterType("3DCONNEXION_SPACENAVIGATOR_CMD");

  m_pSensorMappingAsp->RegisterType("LOGITECH_RUMBLEPAD2");

  m_pSensorMappingAsp->RegisterType("GRIFFIN_POWERMATE");

  m_pSensorMappingAsp->RegisterType("HID_RAW");

  RegisterAspect(m_pSensorMappingAsp);
  RegisterAspect(m_pIdentAsp);

  RegisterAspect(new VistaHIDDriverEnableAspect(this, &VistaHIDDriver::PhysicalEnable));
}

VistaHIDDriver::~VistaHIDDriver() {
  VistaHIDDriverEnableAspect* enabler =
      GetAspectAs<VistaHIDDriverEnableAspect>(VistaDriverEnableAspect::GetAspectId());
  UnregisterAspect(enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete enabler;

  UnregisterAspect(m_pIdentAsp, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pIdentAsp;

  UnregisterAspect(m_pSensorMappingAsp, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pSensorMappingAsp;

#ifdef LINUX
  UnregisterAspect(m_pConnection, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pConnection;
  if (m_pForce) {
    UnregisterAspect(m_pForce, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
    delete m_pForce;
  }
#endif

#if defined(WIN32)
  UnregisterAspect(m_pThreadAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT);
  delete m_pThreadAspect;
#endif

  delete m_pMeasure;
}

/*============================================================================*/
/* IMPLEMENTATION
 */
/*============================================================================*/

bool VistaHIDDriver::DoSensorUpdate(VistaType::microtime dTs) {

  VistaDeviceSensor* pSensor = GetSensorByIndex(0);
  if (!pSensor) {
    // std::cerr << "VistaHIDDriver: failed to retreive sensor, not updating!" << std::endl;
    return false;
  }

#ifdef LINUX
  int bytes_read = 0;

  // read-off raw input events from the HID layer
  // up to 10 at the time
  for (int offset = 0; m_pConnection->GetConnection(0)->HasPendingData() && offset < 10; offset++) {
    bytes_read += m_pConnection->GetConnection(0)->ReadRawBuffer(
        &m_vEvents[offset], sizeof(struct input_event));
  }

  if (bytes_read > 0) {
    // cache pointers beforehand
    for (int i = 0; i < bytes_read / (int)sizeof(struct input_event); ++i) {
      switch (m_vEvents[i].type) {
      case EV_KEY: {
        m_pMeasure->nButtons[m_vEvents[i].code] = m_vEvents[i].value;
        break;
      }
      case EV_REL: {
        /*				switch( m_vEvents[i].code )
                                        {
                                        case 0:
                                                m_pMeasure->nAxes[0] = m_vEvents[i].value;
                                                break;

                                        case 1:
                                                m_pMeasure->nAxes[2] = m_vEvents[i].value;
                                                break;

                                        case 2:
                                                m_pMeasure->nAxes[1] = m_vEvents[i].value;
                                                break;
                                        }
        */
        m_pMeasure->nAxes[m_vEvents[i].code] = m_vEvents[i].value;
        break;
      }
      case EV_ABS: {
        m_pMeasure->nAxes[m_vEvents[i].code] = m_vEvents[i].value;
        break;
      }
      /*case EV_RST:
        {
        std::cout << "event type: " << m_vEvents[i].type
        << std::endl
        << "event code: " << m_vEvents[i].code << std::endl;
        break;
        }*/
      default:

        break;
      }
      // advance write head
    }

    MeasureStart(0, dTs);

    VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

    VistaHIDCommonShare::sHIDMeasure* pMeasure = pM->getWrite<VistaHIDCommonShare::sHIDMeasure>();
    memcpy(pMeasure, m_pMeasure, sizeof(VistaHIDCommonShare::sHIDMeasure));

    MeasureStop(0);
    // mark sensor dirty
    //		pSensor->SetUpdateTimeStamp(dTs);
    return true;
  }
#endif

#ifdef WIN32
  DWORD rv = WaitForSingleObject(m_hReadEvent, 1);
  switch (rv) {
  case WAIT_OBJECT_0: {
    // Data received from device
    DWORD numBytes = 0;
    rv             = GetOverlappedResult(m_hDevice, &m_readOverlap, &numBytes, TRUE);
    if (rv) {
      // Decode all full reports.
      // Full reports for all of the pro devices are 7 bytes long.
      for (size_t i = 0; i < numBytes / 7; i++) {
        BYTE* report = m_readBuffer + (i * 7);

        // There are three types of reports.  Parse whichever type
        // this is.
        char                 report_type = report[0];
        VistaType::sshort16* bufptr      = reinterpret_cast<VistaType::sshort16*>(&report[1]);
        // const float scale = static_cast<float>(1.0/400.0);
        switch (report_type) {
          // Report types 1 and 2 come one after the other.  Each seems
          // to change when the puck is moved.  It looks like each pair
          // of values records a signed value for one channel; report
          // type 1 is translation and report type 2 is rotation.
          // The minimum and maximum values seem to vary somewhat.
          // The first byte is the low-order byte and the second is the
          // high-order byte.
        case 1:
          m_pMeasure->nAxes[0] = *bufptr++;
          m_pMeasure->nAxes[1] = *bufptr++;
          m_pMeasure->nAxes[2] = *bufptr++;
          break;

        case 2:
          m_pMeasure->nAxes[3] = *bufptr++;
          m_pMeasure->nAxes[4] = *bufptr++;
          m_pMeasure->nAxes[5] = *bufptr++;
          break;

        case 3: { // Button report
          int btn;

          // std::cout << "*****" << std::endl;
          // Button reports are encoded as bits in the first byte
          // after the type.  Presumably, there can be more if there
          // are more then 8 buttons on a future device but for now
          // we just unpack this byte into however many buttons we
          // have.
          for (btn = 0; btn < 8; btn++) {
            BYTE *location, mask;

            location = report + 1;
            mask     = 1 << (btn % 8);

            m_pMeasure->nButtons[btn] = (*location & mask) != 0;
          }
          // std::cout << "nButtons[0]: " << m_pMeasure->nButtons[0] << std::endl;
          // std::cout << "nButtons[1]: " << m_pMeasure->nButtons[1] << std::endl;
          break;
        }

        default:
          vstr::warnp() << "[HIDDriver] Unknown report type encountered!" << std::endl;
        }
      }

      MeasureStart(0, dTs);

      VistaSensorMeasure* pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

      VistaHIDCommonShare::sHIDMeasure* pMeasure = pM->getWrite<VistaHIDCommonShare::sHIDMeasure>();
      memcpy(pMeasure, m_pMeasure, sizeof(VistaHIDCommonShare::sHIDMeasure));

      MeasureStop(0);
      // mark sensor dirty
      //	pSensor->SetUpdateTimeStamp(dTs);
#if 0
			vstr::outi() << "[HIDDriver] number of bytes transferred: " << numBytes << std::endl;
			for( int i = 0 ; i < numBytes ; i++ )
			{
				vstr::outi() << vstr::singleindent << "byte " << i << ": " <<
					hex << setw(2) << setfill('0') << (unsigned int)(m_readBuffer[i]) 
					<< std::dec << std::endl;
			}
#endif

    } else {
      if (GetLastError() == ERROR_DEVICE_NOT_CONNECTED) {
        vstr::warnp() << "[HIDDriver] Device removed unexpectedly" << std::endl;
        ResetEvent(m_hReadEvent);
        return false;
      } else {
        fprintf(stderr, "[HIDDriver] GetOverlappedResult returned error status (GLE %u)\n",
            GetLastError());
        ResetEvent(m_hReadEvent);
      }
    }
    ResetEvent(m_hReadEvent);
    StartWinRead();
    break;
  }
  case WAIT_FAILED: {
    fprintf(
        stderr, "[HIDDriver] WaitForSingleObject returned error status (GLE %u)\n", GetLastError());
    ResetEvent(m_hReadEvent);
  }
  case WAIT_ABANDONED: {
    vstr::warnp() << "[HIDDriver]: WAIT_ABANDONED" << std::endl;
    break;
  }
  case WAIT_TIMEOUT: {
    // vstr::warnp() << "[HIDDriver]: WAIT_TIMEOUT" << std::endl;
    break;
  }
  default: {
    vstr::warnp() << "[HIDDriver] wait for object returned (" << rv << ")" << std::endl;
  }
  }
#endif

  return true;
}

bool VistaHIDDriver::DoConnect() {
  return InitDriver(m_pIdentAsp->GetVendorID(), m_pIdentAsp->GetProductID());
}

bool VistaHIDDriver::DoDisconnect() {
  return true;
}

bool VistaHIDDriver::PhysicalEnable(bool bEnable) {
  if (bEnable) {
#if defined(WIN32)
    if (m_hDevice != NULL)
      return StartWinRead();
    return false;
#elif defined(LINUX)
    // test
#endif
  }

  return true;
}

/**
 * enumerate devices, looking for match with vendor/product
 * initialize filehandle, don't start read yet
 */
bool VistaHIDDriver::InitDriver(int nVendor, int nDevId) {
#ifdef WIN32
  // Get a "device information set" of connected HID drivers
  GUID     hidGuid;
  HDEVINFO devInfoSet;
  HidD_GetHidGuid(&hidGuid);
  devInfoSet = SetupDiGetClassDevs(
      &hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_PROFILE | DIGCF_DEVICEINTERFACE);
  if (devInfoSet == INVALID_HANDLE_VALUE) {
    vstr::warnp() << "[HIDDriver] Unable to open device information set, GLE:" << GetLastError()
                  << std::endl;
    return false;
  }

  // Loop through all devices searching for a match
  DWORD                    deviceIndex = 0;
  SP_DEVICE_INTERFACE_DATA ifData      = {sizeof(SP_DEVICE_INTERFACE_DATA)};
  while (SetupDiEnumDeviceInterfaces(devInfoSet, NULL, &hidGuid, deviceIndex++, &ifData)) {
    void*                            preparsed = 0;
    HIDD_ATTRIBUTES                  hidAttrs  = {sizeof(HIDD_ATTRIBUTES)};
    PSP_DEVICE_INTERFACE_DETAIL_DATA detailData;
    SP_DEVINFO_DATA                  infoData = {sizeof(SP_DEVINFO_DATA)};
    HIDP_CAPS                        hidCaps  = {0};
    // vrpn_HIDDEVINFO vrpnInfo = {0};

    // Figure out how much storage we'll need
    DWORD neededSize = 0;
    SetupDiGetDeviceInterfaceDetail(devInfoSet, &ifData, NULL, 0, &neededSize, NULL);

    detailData = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(LocalAlloc(LPTR, neededSize));
    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    // Ensure we can retrieve the device path
    if (!SetupDiGetDeviceInterfaceDetail(
            devInfoSet, &ifData, detailData, neededSize, NULL, &infoData)) {
      vstr::warnp() << "[HIDDriver] Unable to get device path!" << std::endl;
      LocalFree(detailData);
      continue;
    }

    // Try to open the device; if we can't, skip it
    vstr::outi() << "[HIDDriver] trying to open device at path " << detailData->DevicePath << "..."
                 << std::endl;
    m_hDevice = CreateFile(detailData->DevicePath, GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (!m_hDevice || (m_hDevice == INVALID_HANDLE_VALUE)) {
      vstr::warnp() << "[HIDDriver] Unable to open device at path " << detailData->DevicePath
                    << std::endl;
      LocalFree(detailData);
      continue;
    }

    // Get the HID-specific attributes
    if (!HidD_GetAttributes(m_hDevice, &hidAttrs)) {
      vstr::warnp() << "[HIDDriver] Unable to get HID attributes!" << std::endl;
      CloseHandle(m_hDevice);
      m_hDevice = NULL;
      LocalFree(detailData);
      continue;
    }

    // Use the Windows preparsed data to find the usage page info
    if (!HidD_GetPreparsedData(m_hDevice, &preparsed) || !preparsed) {
      vstr::warnp() << "[HIDDriver] Unable to get windows preparsed data!" << std::endl;
      CloseHandle(m_hDevice);
      m_hDevice = NULL;
      LocalFree(detailData);
      continue;
    }

    // Get capabilities of the device and then clean up some
    HidP_GetCaps(preparsed, &hidCaps);
    HidD_FreePreparsedData(preparsed);

    if ((hidAttrs.ProductID == nDevId) && (hidAttrs.VendorID == nVendor)) {
      LocalFree(detailData);
      SetupDiDestroyDeviceInfoList(devInfoSet);
      return true;
    }
    CloseHandle(m_hDevice);
    m_hDevice = NULL;
    LocalFree(detailData);
  }
#endif
#ifdef LINUX
  std::string sDir   = "/dev/input/";
  DIR*        devdir = opendir((const char*)sDir.data());
  dirent*     entry  = 0;

  while ((entry = readdir(devdir))) {
    // enumerate all event* files in /dev/input
    if (strncmp(entry->d_name, "event", 5) == 0) {
      VistaConnectionFile* pFileCon =
          new VistaConnectionFile(sDir + (entry->d_name), VistaConnectionFile::READ);

      m_pConnection->SetConnection(0, pFileCon, "MAIN",
          VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT,
          VistaDriverConnectionAspect::REPLACE_ONLY_WHEN_NOT_SET);

      if (!pFileCon->Open()) {
        perror("[HIDDriver] unable to open device file");
        closedir(devdir);
        return 1;
      }

      input_id device_info;
      /* suck out device information */
      if (ioctl(pFileCon->GetConnectionDescriptor(), EVIOCGID, &device_info)) {
        perror("[HIDDriver] failed to get device information");
      }

      if (device_info.vendor == nVendor && device_info.product == nDevId) {
        vstr::outi() << std::hex << "[HIDDriver] device with vendor/product id " << nVendor << "/"
                     << nDevId << " detected. keeping fileconnection open." << std::dec
                     << std::endl;
        closedir(devdir);
        return true;
      }

      pFileCon->Close();
    }
  }

  closedir(devdir);

#endif
  return false;
}

#ifdef WIN32
bool VistaHIDDriver::StartWinRead() {
  ZeroMemory(&m_readOverlap, sizeof(OVERLAPPED));
  m_readOverlap.hEvent = m_hReadEvent;

  // Latter check added because pending I/O is OK by us
  if (!ReadFile(m_hDevice, m_readBuffer, 512, NULL, &m_readOverlap)) {
    switch (GetLastError()) {
    case ERROR_IO_PENDING:
      // Expected condition; do nothing
      // break;
      return true;
    case ERROR_DEVICE_NOT_CONNECTED:
      vstr::warnp() << "VistaHIDDriver: Device removed unexpectedly" << std::endl;
      return false;
    default:
      vstr::warnp() << "VistaHIDDriver: ReadFile couldn't start async input (GLE " << GetLastError()
                    << ")" << std::endl;
      return false;
    }
  }
  return false;
}
#endif
