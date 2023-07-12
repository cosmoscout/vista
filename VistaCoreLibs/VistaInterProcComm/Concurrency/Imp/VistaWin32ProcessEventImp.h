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

#if defined(VISTA_THREADING_WIN32)

#ifndef _VISTAWIN32PROCESSEVENTIMPL_H
#define _VISTAWIN32PROCESSEVENTIMPL_H

#include "VistaProcessEventImp.h"

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VistaProcessEventImp;

class VISTAINTERPROCCOMMAPI VistaWin32ProcessEventImp : public IVistaProcessEventImp {
 public:
  // signaller ctor
  VistaWin32ProcessEventImp(const std::string& sEventName);
  // receiver ctor
  VistaWin32ProcessEventImp(const std::string& sEventName, const int nMaxWaitForSignaller);
  virtual ~VistaWin32ProcessEventImp();

  bool GetIsValid() const;

  bool SignalEvent();
  bool WaitForEvent(bool bBlock);
  bool WaitForEvent(int iBlockTime);

 private:
  HANDLE m_pEventHandle;
};

/*============================================================================*/

#endif // _VISTAITERATIONTHREAD_H

#endif // VISTA_THREADING_WIN32
