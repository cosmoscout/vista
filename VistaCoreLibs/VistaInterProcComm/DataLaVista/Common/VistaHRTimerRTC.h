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

#if defined(_USE_HRRTC)

#ifndef DLVISTAHRTIMERTC_H
#define DLVISTAHRTIMERTC_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/DataLaVista/Base/VistaRTC.h>
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaHRTimerRTC : public IDLVistaRTC {
 public:
  DLVistaHRTimerRTC();

  virtual ~DLVistaHRTimerRTC();

  virtual DLV_INT64 GetTickCount() const;

  virtual DLV_INT32 GetTimeStamp() const;

  virtual double GetTickToSecond(DLV_INT64 nTs) const;

  virtual double GetSystemTime() const;

  // virtual const char *GetPrintfFormatSpecifier();

 private:
  /**
   * We prohibit copying
   */
  DLVistaHRTimerRTC(DLVistaHRTimerRTC&);

 protected:
  DLV_INT64 GetPentiumCounter() const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // DLVISTAHRTIMERTC_H
#endif // _USE_HRRTC
