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

#include "VistaCPUInfo.h"

#if defined(WIN32)
#include <PowrProf.h>
#include <Windows.h>

typedef LPTSTR sString;

#define HIDIGIT(d) ((BYTE)(d >> 4))
#define LODIGIT(d) ((BYTE)(d & 0x0F))

#define CPU_REG_KEY HKEY_LOCAL_MACHINE
#define CPU_REG_SUBKEY "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"

#define CPU_SPEED "~MHz"
#define CPU_IDENTIFIER "Identifier"
#define CPU_NAME "ProcessorNameString"
#define CPU_VENDOR "VendorIdentifier"

// due to a mistake in the msdn include files, we have
// to replicate this structure here. Might be that upon compiler
// update, this may lead to errors
#if !defined(PROCESSOR_POWER_INFORMATION)
typedef struct _PROCESSOR_POWER_INFORMATION {
  ULONG Number;
  ULONG MaxMhz;
  ULONG CurrentMhz;
  ULONG MhzLimit;
  ULONG MaxIdleState;
  ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
#endif // PROCESSOR_POWER_INFORMATION

#else
#include <cerrno>
#include <cstdio>
#include <cstdlib>

#define MAX_LINE_LENGTH 255

#define CPU_FLAGS_SSE 0x0001
#define CPU_FLAGS_SSE2 0x0002
#define CPU_FLAGS_3DNOW 0x0004
#define CPU_FLAGS_MMX 0x0008

typedef char* sString;

#endif

#include <cstdio>
#include <cstring>

class VistaCPUInfo::_CPU_STAT {
 public:
  _CPU_STAT()
      : m_Stats()
      , m_nCpuCount(1) {
    // memset( &m_Stats, 0, sizeof(_sStats) );
  }

  struct _sStats {
    _sStats()
        : dwSpeed(0)
        , sIdentifier(0L)
        , sProcessorName(0L)
        , sVendorName()
        , iFamily(0)
        , iModel(0)
        , iStepping(0)
        , iFlags(0) {
    }

    ~_sStats() {
      delete[] sIdentifier;
      delete[] sProcessorName;
    }

    VistaType::uint64 dwSpeed;        // in MHz
    sString           sIdentifier;    // eg: x86 Family 6 Model 6 Stepping 2
    sString           sProcessorName; // eg: AMD Athlon(tm) XP 1600+
    std::string       sVendorName;    // eg: AuthenticAMD
    short int         iFamily;        // family number
    short int         iModel;         // model number
    short int         iStepping;      // stepping number,
    short int         iFlags;
  } m_Stats;

  unsigned int m_nCpuCount;
};

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaCPUInfo::VistaCPUInfo()
    : m_pStat(new _CPU_STAT) {
  QueryCPUInfo();
}

VistaCPUInfo::~VistaCPUInfo() {
  delete m_pStat;
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
int VistaCPUInfo::GetProcessorCount() {
  return m_pStat->m_nCpuCount;
}

VistaType::uint64 VistaCPUInfo::GetSpeed() const {
  return m_pStat->m_Stats.dwSpeed;
}

unsigned int VistaCPUInfo::GetSpeedMHz() const {
  return (unsigned int)(GetSpeed() / (1000 * 1000));
}

std::string VistaCPUInfo::GetCPUName() const {
  return GetCPUNameString();
}

std::string VistaCPUInfo::GetVendorName() const {
  return m_pStat->m_Stats.sVendorName;
}

short int VistaCPUInfo::GetCPUFamily() const {
  return m_pStat->m_Stats.iFamily;
}

short int VistaCPUInfo::GetCPUModel() const {
  return m_pStat->m_Stats.iModel;
}

short int VistaCPUInfo::GetCPUStepping() const {
  return m_pStat->m_Stats.iStepping;
}

bool VistaCPUInfo::GetIsMMXSupported() const {
#if defined(WIN32)
  return (IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE) != false);
#else
  return ((m_pStat->m_Stats.iFlags & CPU_FLAGS_MMX) == CPU_FLAGS_MMX);
#endif
}

bool VistaCPUInfo::GetIsSSESupported() const {
#if defined(WIN32)
  return (IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE) != false);
#else
  return ((m_pStat->m_Stats.iFlags & CPU_FLAGS_SSE) == CPU_FLAGS_SSE);
#endif
}

bool VistaCPUInfo::GetIsSSE2Supported() const {
#if defined(WIN32)
  return (IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE) != false);
#else
  return ((m_pStat->m_Stats.iFlags & CPU_FLAGS_SSE2) == CPU_FLAGS_SSE2);
#endif
}

bool VistaCPUInfo::GetIs3DNowSupported() const {
#if defined(WIN32)
  return (IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE) != false);
#else
  return ((m_pStat->m_Stats.iFlags & CPU_FLAGS_3DNOW) == CPU_FLAGS_3DNOW);
#endif
}

bool VistaCPUInfo::QueryCPUInfo() {
#if defined(WIN32)
  SYSTEM_INFO siSysInfo;
  GetSystemInfo(&siSysInfo);

  VistaCPUInfo::_CPU_STAT::_sStats& sCPUInfo = (m_pStat->m_Stats);

  m_pStat->m_nCpuCount = siSysInfo.dwNumberOfProcessors;

  // CPU family
  switch (siSysInfo.wProcessorArchitecture) {
  case PROCESSOR_ARCHITECTURE_INTEL:
    sCPUInfo.iFamily = siSysInfo.wProcessorLevel;
    break;
  case PROCESSOR_ARCHITECTURE_IA64:
    sCPUInfo.iFamily = 1;
    break;
  default:
    return false;
  }

  // CPU model and stepping
  switch (sCPUInfo.iFamily) {
  // Intel 80386 and Intel 80486
  case 3:
  case 4: {
    if (HIBYTE(siSysInfo.wProcessorRevision) == 0xFF) {
      sCPUInfo.iModel    = HIDIGIT(LOBYTE(siSysInfo.wProcessorRevision)) - 0x0A;
      sCPUInfo.iStepping = LODIGIT(LOBYTE(siSysInfo.wProcessorRevision));
    } else {
      sCPUInfo.iStepping = HIBYTE(siSysInfo.wProcessorRevision) + 'A';
    }
  }
  // Intel Pentium, Cyrix, or NextGen 586
  case 5:
  case 6:
  case 15: {
    sCPUInfo.iModel    = HIBYTE(siSysInfo.wProcessorRevision);
    sCPUInfo.iStepping = LOBYTE(siSysInfo.wProcessorRevision);
  }
  }

  HKEY  hKey;
  DWORD dwSize = 0;
  DWORD dwType = 0;

  LONG ret = RegOpenKeyEx(CPU_REG_KEY, CPU_REG_SUBKEY, 0, KEY_READ, &hKey);

  ret                = RegQueryValueEx(hKey, CPU_VENDOR, NULL, &dwType, NULL, &dwSize);
  TCHAR* sVendorName = new TCHAR[dwSize];

  ret = RegQueryValueEx(hKey, CPU_VENDOR, NULL, &dwType, (LPBYTE)sVendorName, &dwSize);
  m_pStat->m_Stats.sVendorName = sVendorName; // std::string( sVendorName, dwSize );

  delete[] sVendorName;
  RegCloseKey(hKey);

  PROCESSOR_POWER_INFORMATION info;
  CallNtPowerInformation(ProcessorInformation, NULL, 0, &info, sizeof(info));

  m_pStat->m_Stats.dwSpeed = info.MaxMhz * (1000 * 1000);

  return true;
#elif defined(LINUX)
  // copied from http://www.kernel.org/pub/linux/utils/kernel/cpufreq/cpufreq-info.html
  // couting the number cpu(x) entries in /proc/stat
  // seems to be the most reliable way in user space
  // for linux users.
  const int LINE_LEN = 10;

  FILE*        fp;
  char         value[LINE_LEN];
  unsigned int ret   = 0;
  unsigned int cpunr = 0;

  fp = fopen("/proc/stat", "r");
  if (!fp) {
    printf(
        "Couldn't count the number of CPUs (%s: %s), assuming 1\n", "/proc/stat", strerror(errno));
    return 1;
  }

  while (!feof(fp)) {
    fgets(value, LINE_LEN, fp);
    value[LINE_LEN - 1] = '\0';
    if (strlen(value) < (LINE_LEN - 2))
      continue;
    if (strstr(value, "cpu "))
      continue;
    if (sscanf(value, "cpu%d ", &cpunr) != 1)
      continue;
    if (cpunr > ret)
      ret = cpunr;
  }
  fclose(fp);

  /* cpu count starts from 0, on error return 1 (UP) */
  m_pStat->m_nCpuCount = (ret + 1);

  // we should detect all attribs for all CPUs independently, to be honest...
  // but we will skip this here and assume a symmetric layout

  fp = fopen("/proc/cpuinfo", "r");
  if (fp) {
    char pcLine[MAX_LINE_LENGTH];

    while (!feof(fp)) {
      fgets(pcLine, MAX_LINE_LENGTH, fp);
      if (strlen(pcLine) > 1) // if there is more than 1 cpu, we have a blank line ("\n") in between
      {
        // find first ":"
        char* pcLeft = pcLine;
        char* pcCol  = index(pcLine, ':');
        if (pcCol == NULL)
          continue;

        *pcCol = 0x0;
        // there is a ": " layout here, obviously
        char* pcRight = pcCol + 2;

        int iPcRightLen = strlen(pcRight);

        if (strstr(pcLeft, "vendor_id")) {
          m_pStat->m_Stats.sIdentifier = new char[iPcRightLen];
          strncpy(m_pStat->m_Stats.sIdentifier, pcRight, iPcRightLen - 1); // omit trailing '\n'
          m_pStat->m_Stats.sIdentifier[iPcRightLen - 1] = '\0';
        } else if (strstr(pcLeft, "cpu family")) {
          m_pStat->m_Stats.iFamily = strtol(pcRight, NULL, 10);
        } else if (strstr(pcLeft, "model name")) {
          m_pStat->m_Stats.sProcessorName = new char[iPcRightLen];
          strncpy(m_pStat->m_Stats.sProcessorName, pcRight, iPcRightLen - 1); // omit trailing '\n'
          m_pStat->m_Stats.sProcessorName[iPcRightLen - 1] = '\0';
        } else if (strstr(pcLeft, "model")) {
          m_pStat->m_Stats.iModel = strtol(pcRight, NULL, 10);
        }

        else if (strstr(pcLeft, "stepping")) {
          m_pStat->m_Stats.iStepping = strtol(pcRight, NULL, 10);
        } else if (strstr(pcLeft, "cpu MHz")) {
          double cpuMHz            = strtod(pcRight, NULL);
          m_pStat->m_Stats.dwSpeed = (VistaType::uint64)cpuMHz * (1000 * 1000);
        } else if (strstr(pcLeft, "flags")) {
          // flags are white-space separated strings, we do only look
          // for some specific ones
          char  buffer[32];
          char* pcItem = pcRight;
          while (*pcItem != 0x0) {
            int nIdx = 0;
            memset(buffer, 0, 32);
            while (*pcItem && (*pcItem != ' ')) {
              buffer[nIdx++] = *pcItem++;
            }
            // ok found next token in buf
            if (strstr(buffer, "sse2")) {
              m_pStat->m_Stats.iFlags = m_pStat->m_Stats.iFlags | CPU_FLAGS_SSE;
            } else if (strstr(buffer, "sse")) {
              m_pStat->m_Stats.iFlags = m_pStat->m_Stats.iFlags | CPU_FLAGS_SSE2;
            } else if (strstr(buffer, "mmx")) {
              m_pStat->m_Stats.iFlags = m_pStat->m_Stats.iFlags | CPU_FLAGS_MMX;
            } else if (strstr(buffer, "3dnow")) {
              m_pStat->m_Stats.iFlags = m_pStat->m_Stats.iFlags | CPU_FLAGS_3DNOW;
            }
            if (*pcItem != 0x0)
              pcItem++; // standing on \n or ' '
          }
        }

        // printf("[%s] = [%s]\n", pcLeft, pcRight);
      } else {
        // printf("<empty-line> -- skip rest\n");
        break; // leave while-loop
      }
    }
    fclose(fp);
  }

  m_pStat->m_Stats.sVendorName = GetCPUNameString();
  return true;

#else
  return false;
#endif
}

std::string VistaCPUInfo::GetCPUNameString() const {
  std::string vendor((m_pStat->m_Stats.sIdentifier ? m_pStat->m_Stats.sIdentifier : ""));

  int family    = m_pStat->m_Stats.iFamily;
  int model     = m_pStat->m_Stats.iModel;
  int family_ex = 0;
  int model_ex  = 0;

  if (vendor == "GenuineIntel") {
    switch (family) {
    case 4: {
      switch (model) {
      case 0:
        return "INTEL 486 DX-25/33";
      case 1:
        return "INTEL 486 DX-50";
      case 2:
        return "INTEL 486 SX";
      case 3:
        return "INTEL 486 DX2";
      case 4:
        return "INTEL 486 SL";
      case 5:
        return "INTEL 486 SX2";
      case 7:
        return "INTEL 486 DX2-WB";
      case 8:
        return "INTEL 486 DX4";
      case 9:
        return "INTEL 486 DX4-WB";
      }
    } break;
    case 5: {
      switch (model) {
      case 0:
        return "INTEL Pentium 60/66 A-step";
      case 1:
        return "INTEL Pentium 60/66";
      case 2:
        return "INTEL Pentium 75-200";
      case 3:
        return "INTEL OverDrive PODP5V83";
      case 4:
        return "INTEL Pentium MMX";
      case 7:
        return "INTEL Mobile Pentium 75-200";
      case 8:
        return "INTEL Mobile Pentium MMX";
      }
      break;
    }
    case 6: {
      switch (model) {
      case 1:
        return "INTEL  Pentium Pro A-Step";
      case 2:
        return "INTEL  Pentium Pro";
      case 3:
        return "INTEL  Pentium II Klamath";
      case 5:
        return "INTEL  Pentium II Deschutes";
      case 6:
        return "INTEL  Celeron Mendocino";
      case 7:
        return "INTEL  Pentium III Katmai";
      case 8:
        return "INTEL  Pentium III Coppermine";
      case 9:
        return "INTEL  Mobile Pentium III";
      case 10:
        return "INTEL  Pentium III (0.18micron)";
      case 11:
        return "INTEL  Pentium III (0.13micron)";
      }
      break;
    }
    case 7:
      return "INTEL Itanium";
    case 15: {
      switch (family_ex) {
      case 0: {
        switch (model_ex) {
        case 0:
        case 1:
          return "INTEL Pentium IV (0.18micron)";
        case 2:
          return "INTEL Pentium IV (0.13micron)";
        case 3:
          return "INTEL Pentium IV (0.09micron)";
        }
      } break;
      case 1:
        return "INTEL Itanium 2 (IA-64)";
      }
    } break;
    }
  } else if (vendor.compare("AuthenticAMD") == 0) {
    switch (family) {
    case 4: {
      switch (model) {
      case 3:
        return "AMD 486 DX2";
      case 7:
        return "AMD 486 DX2-WB";
      case 8:
        return "AMD 486 DX4";
      case 9:
        return "AMD 486 DX4-WB";
      case 14:
        return "AMD Am5x86-WT";
      case 15:
        return "AMD Am5x86-WB";
      }
    } break;
    case 5: {
      switch (model) {
      case 0:
        return "AMD K5 SSA5";
      case 1:
      case 2:
      case 3:
        return "AMD K5";
      case 6:
      case 7:
        return "AMD K6";
      case 8:
        return "AMD K6-2";
      case 9:
        return "AMD K6-3";
      case 13:
        return "AMD K6-3+";
      }
    } break;
    case 6: {
      switch (model) {
      case 0:
      case 1:
        return "AMD Athlon (25um)";
      case 2:
        return "AMD Athlon (18um)";
      case 3:
        return "AMD Duron";
      case 4:
        return "AMD Athlon Thunderbird";
      case 6:
        return "AMD Athlon Palamino";
      case 7:
        return "AMD Duron Morgan";
      case 8:
        return "AMD Athlon Thoroughbred";
      case 10:
        return "AMD Athlon Barton";
      }
    } break;
    case 15: {
      switch (family_ex) {
      case 0: {
        switch (model_ex) {
        case 4:
          return "AMD Athlon 64";
        case 5:
          return "AMD Athlon 64FX Operon";
        }
      }
      }
    }
    }
  } else if (vendor.compare("CyrixInstead") == 0)
    return "Cyrix";
  else if (vendor.compare("CentaurHauls") == 0)
    return "Centaur";

  return "unknown";
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
