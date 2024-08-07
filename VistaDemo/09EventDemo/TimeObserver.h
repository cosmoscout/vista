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

#ifndef _TIMEOBSERVER_H
#define _TIMEOBSERVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/EventManager/VistaEventObserver.h>

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VistaEventManager;

/**
 * TimeObserver - an example for an application-defined event
 */
class TimeObserver : public VistaEventObserver {
 public:
  /**
   * Creates a TimeObserver object
   *
   * @AUTHOR  Marc Schirski
   * @DATE    08.11.2002
   */
  TimeObserver(VistaEventManager* pEventManager, int iEventType);

  /**
   * Notify the observer of the given event.
   *
   * @AUTHOR  Marc Schirski
   * @DATE    08.11.2002
   *
   * @INPUT   VistaEvent * pEvent    (MUST NOT BE NULL!)
   * @RETURN  --
   */
  virtual void Notify(const VistaEvent* pEvent);

 protected:
  double             m_dNextTime;
  VistaEventManager* m_pEventManager;
  int                m_iEventType;
};

#endif // _TIMEOBSERVER_H
