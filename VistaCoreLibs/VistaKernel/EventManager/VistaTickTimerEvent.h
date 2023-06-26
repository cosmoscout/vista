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

#ifndef _VISTATICKTIMEREVENT_H
#define _VISTATICKTIMEREVENT_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaTickTimer;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaTickTimerEvent : public VistaEvent {
 public:
  /**
   * Constructor. Pass the source of this event.
   * @param pWatch the source element of this event.
   */
  VistaTickTimerEvent(VistaTickTimer* pWatch);

  /**
   * Destructor. Does nothing.
   */
  virtual ~VistaTickTimerEvent();

  /**
   * We define a type for this kind of event without deeper reason, yet.
   * This is for future enhancments.
   */
  enum VISTAKERNELAPI EV_TYPE { SWEV_TICK = 0 };

  enum VISTAKERNELAPI TTEV_ID { TTID_TIMEOUT = 0, TTID_UPPER_BOUND };

  /**
   * Returns the name of this event-type.
   * This is "TickTimerEvent" always.
   * @return "TickTimerEvent"
   */
  virtual std::string GetName() const;

  /**
   * Returns the source of this event. You will need this in case you set up
   * more than one TickTimer.
   * @return the source of this event
   * @see m_pSource()
   */
  VistaTickTimer* GetSourceTickTimer() const {
    return m_pSource;
  };

  void SetTickEventTime(double dTime);

  static std::string GetIdString(int nId);
  static int         GetTypeId();
  static void        SetTypeId(int nId);

  static void RegisterEventTypes(VistaEventManager* pEventManager);

 protected:
  /**
   * A pointer to the source of this Event.
   */
  VistaTickTimer* m_pSource;

 private:
  static int m_nEventId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTATICKTIMEREVENT_H
