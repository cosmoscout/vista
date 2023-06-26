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

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
#include "VistaPentiumBasedRTC.h"

#if defined(DLV_IX86)

#include <cstdio>
#include <sys/types.h>

#ifdef UXPV
#include <sys/timesu.h>
#else

#if defined(WIN32)
#include <limits.h>
#include <time.h>
#include <sys/timeb.h>
#include <Windows.h>
#pragma warning(disable : 4996)
#else
#include <unistd.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#endif
#endif

// create some ''local storage''
namespace {
unsigned int SnProcFreq = 1; // init default cpu freq with 1
#if defined(WIN32)
unsigned int CalcProcFreq() {
  return 1;
}
#else
unsigned int CalcProcFreq() {
  return 1;
}
#endif
} // namespace

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaPentiumBasedRTC::DLVistaPentiumBasedRTC()
    : m_nFrequency(1) {
  SnProcFreq = CalcProcFreq();
}

DLVistaPentiumBasedRTC::~DLVistaPentiumBasedRTC() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void DLVistaPentiumBasedRTC::SetClockFrequency(DLV_INT64 nFreq) {
  m_nFrequency = nFreq;
}

DLV_INT64 DLVistaPentiumBasedRTC::GetClockFrequency() const {
  return m_nFrequency;
}

double DLVistaPentiumBasedRTC::GetTickToSecond(DLV_INT64 nTs) const {
  return ((double)nTs / (double)m_nFrequency);
}

DLV_INT32 DLVistaPentiumBasedRTC::GetTimeStamp() const {
  return (DLV_INT32)GetSystemTime();
}

double DLVistaPentiumBasedRTC::GetSystemTime() const {
  double sysSeconds;

#ifdef WIN32
  _timeb timeSince1970;
  // localtime (&timeSince1970);
  _ftime(&timeSince1970);
  sysSeconds = ((double)timeSince1970.time) + ((double)timeSince1970.millitm) / 1e3;
#else
  /*
           struct timeval
           {
                    long tv_sec;        // seconds since Jan. 1, 1970 /
                    long tv_usec;       // and microseconds /
           }
  */

  struct timeval tv;
  // tv.tv_sec = 0;
  // tv.tv_usec = 0;
  gettimeofday(&tv, (struct timezone*)0);

  // seconds since 1.1.1970
  sysSeconds = ((double)tv.tv_sec + (double)tv.tv_usec / 1000000.0);
#endif

  return sysSeconds;
}

#if defined(WIN32)

#pragma warning(push)

#if defined(_MSC_VER)
#if (_MSC_VER >= 1400)
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#else
#pragma warning(disable : 4035) // disable no return value warning
#endif
#endif

__forceinline DLV_INT64 DLVistaPentiumBasedRTC::GetPentiumCounter() const {
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400)
  return __rdtsc();
#else
  __asm rdtsc
#endif
#endif
}

#pragma warning(pop)

#elif defined(LINUX)

// this is supposed to work on LINUX/i386
DLV_INT64 DLVistaPentiumBasedRTC::GetPentiumCounter() const {
  unsigned int lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return (unsigned long long)(lo) | ((unsigned long long)(hi) << 32);
}

#endif // LINUX -- should be gcc, though, not linux...

DLV_INT64 DLVistaPentiumBasedRTC::GetTickCount() const {
#if defined(WIN32) || defined(_USE_RTDSC)
  return GetPentiumCounter();
#else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (ts.tv_sec * 100000000) + (long long)ts.tv_nsec;
#endif
}

#endif // DLV_IX86
