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

#include "VistaIPComm.h"
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <VistaBase/VistaStreamUtils.h>

#if !defined(HOST_NAME_MAX)
#define HOST_NAME_MAX 255
#endif

#if defined(WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <errno.h>

#include <VistaBase/VistaAtomicCounter.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

namespace {
VistaSigned32Atomic& GetRefCounter() {
  static VistaSigned32Atomic s_RefCount;
  return s_RefCount;
}
// we create a static instance of the IPComm so that VistaIPComm::UseIPComm
// does not have to be called explicitely
static VistaIPComm s_oIPCommInstance;
} // namespace
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaIPComm::VistaIPComm() {
  UseIPComm();
}

VistaIPComm::~VistaIPComm() {
  CloseIPComm();
}

int VistaIPComm::UseIPComm() {
#ifdef WIN32
  if (GetRefCounter().ExchangeAndAdd(1) == 0) // test prior value
  {
    WSADATA dummyWsaData; // not needed

    if (WSAStartup(MAKEWORD(1, 1), &dummyWsaData) != 0) {
      vstr::errp() << "VistaIPComm::WSAstart() couldn't initialize WinSock." << std::endl;
      --GetRefCounter();
    }
  }
#endif
  return GetRefCounter();
}

int VistaIPComm::CloseIPComm() {
#if defined(WIN32)
  if (GetRefCounter().DecAndTestNull()) {
    WSACleanup();
  }
#endif
  return GetRefCounter();
}

std::string VistaIPComm::GetHostname() {
  char buffer[HOST_NAME_MAX + 1];
  if (gethostname(buffer, HOST_NAME_MAX + 1) < 0)
    vstr::errp() << "VistaIPComm::GetHostname() -- ERROR! errno = " << errno << std::endl;
  return std::string(buffer);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
