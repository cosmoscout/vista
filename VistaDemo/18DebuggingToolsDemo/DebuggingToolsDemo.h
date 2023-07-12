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

#ifndef _DEBUGGINGTOOLSDEMOAPPL_H
#define _DEBUGGINGTOOLSDEMOAPPL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/EventManager/VistaEventHandler.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class DebuggingToolsDemoAppl : VistaEventHandler {
 public:
  DebuggingToolsDemoAppl(int argc = 0, char* argv[] = NULL);
  virtual ~DebuggingToolsDemoAppl();

  void Run();

  virtual void HandleEvent(VistaEvent* pEvent);

 private:
  VistaSystem* m_pVistaSystem;

  class StackWalkerKeyCallback;
  class PrintColorStreamPairsKeyCallback;
  StackWalkerKeyCallback*           m_pStackWalkerCallback;
  PrintColorStreamPairsKeyCallback* m_pColorCallback;
};

#endif // _DEBUGGINGTOOLSDEMOAPPL_H
