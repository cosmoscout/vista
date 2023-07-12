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

#ifndef _VISTACENTRALEVENTHANDLER_H
#define _VISTACENTRALEVENTHANDLER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
//#include "Kernel/VistaSystem.h"
#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaEvent;
class VistaSystemEvent;
class VistaCommandEvent;
class VistaEventManager;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaCentralEventHandler - collecting callbacks for different event types
 * in one place.
 */
class VISTAKERNELAPI VistaCentralEventHandler : public VistaEventHandler {
 public:
  VistaCentralEventHandler(VistaEventManager*);
  virtual ~VistaCentralEventHandler();

  // the following function implements the VistaEventHandler interface:
  virtual void HandleEvent(VistaEvent* pEvent);

 protected:
  // The following handlers can and should be overwritten by the
  // application programmer. There are three levels at which you can treat events
  //
  // 1. General Event Handler: Receives all events and is the first handler that gets "offered"
  //    incoming events. Use this one if you want to take care of all events
  // 2. Handlers for different Event Categories
  //    These receive all events from a certain category (e.g. SYSTEM or INPUTDEVICES)
  //    Overwrite these if you want to react to a certain class of events (e.g. all input events)
  // 3. Special Event Handlers: These have been included for convenience. They do not receive any
  // events
  //    that are not also sent to the handlers on level 2, but e.g. make it easier to capture only
  //    mouse events

  // Main Event Handler (receives all event types)
  virtual void GeneralEventHandler(VistaEvent* pEvent);

  // Handlers for different Event Types
  virtual void SystemEventHandler(VistaSystemEvent* pEvent);
  virtual void CommandEventHandler(VistaCommandEvent* pEvent);
  virtual void ExternalEventHandler(VistaEvent* pEvent);

  virtual void PreAppEventHandler(VistaSystemEvent*);
  virtual void PostAppEventHandler(VistaSystemEvent*);

  // Special Event and Notification Handlers

  // Input Events
  virtual bool ProcessKeyPress(int keyCode);

  // System Events

  /**
   * InitVistaEvent is called by the SystemEventHandler to generate applicationdefined WTK-objects.
   * (Event generated by VistaSystem)
   * @return void true/false
   */
  virtual void InitVistaEvent(VistaSystemEvent* pEvent);

  /**
   * QuitVistaEvent is called by the SystemEventHandler before Vista is going to exit the programm
   * and the application can tell the system to exit or not to exit. (Event generated by
   * VistaSystem)
   * @return void true/false
   */
  virtual void QuitVistaEvent(VistaSystemEvent* pEvent);

  /**
   * ExitVistaEvent is called by the SystemEventHandler to free memoryspace or objects etc. in the
   * moment when Vista exit. After this function is called there is no return to the program. (Event
   * generated by VistaCentralEventHandler by keyboardcode 'q')
   * @return void true/false
   */
  virtual void ExitVistaEvent(VistaSystemEvent* pEvent);

  /**
   * PreDrawGraphicsEvent is called by the SystemEventHandler after WTK rendering before any
   * keyboard- & etc.- events are generated. (Event generated by VistaSystem after
   * WTuniverse_stop())
   * @return void true/false
   */
  virtual void PreDrawGraphicsEvent(VistaSystemEvent* pEvent);

  /**
   * PostDrawGraphicsEvent is called by the SystemEventHandler before WTK rendering and after
   * keyboard- & etc.- events. (Event generated by VistaOldGraphicsManager)
   * @return void true/false
   */
  virtual void PostDrawGraphicsEvent(VistaSystemEvent* pEvent);

 private:
  // Event Dispatchers. They pipe the events to the right local handlers
  void DispatchSystemEvent(VistaSystemEvent* pEvent);
  void DispatchCommandEvent(VistaCommandEvent* pEvent);

  VistaEventManager* m_pEvMgr;

 public:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACENTRALEVENTHANDLER_H
