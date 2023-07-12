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

#ifndef _VISTASYSTEMEVENT_H
#define _VISTASYSTEMEVENT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaSystemEvent - well, guess?!
 */
class VISTAKERNELAPI VistaSystemEvent : public VistaEvent {
 public:
  /**
   * Possible ids for system events
   */
  enum VISTAKERNELAPI EVENT_ID {
    VSE_INVALID = -1,
    VSE_FIRST   = 0,
    VSE_INIT    = 0,
    VSE_QUIT,
    VSE_EXIT,
    VSE_UPDATE_INTERACTION,
    VSE_UPDATE_DELAYED_INTERACTION,
    VSE_UPDATE_DISPLAYS,
    VSE_PREGRAPHICS,
    VSE_POSTGRAPHICS,
    VSE_PREAPPLICATIONLOOP,
    VSE_POSTAPPLICATIONLOOP,
    VSE_UPPER_BOUND // this one has to remain here for sanity checks...
  };

 public:
  /**
   * Creates a VistaSystemEvent object
   *
   */
  VistaSystemEvent();
  VistaSystemEvent(const EVENT_ID eEventID);

  /**
   * Destroys a VistaSystemEvent object
   */
  virtual ~VistaSystemEvent();

  /**
   * Sets the id of the event.
   *
   * @param   int iId
   * @RETURN  bool    true=success / false=failure (i.e. id=INVALID)
   */
  bool SetId(int iId);

  /**
   * Returns the name of the event.
   *
   * @param   --
   * @RETURN  std::string
   */
  virtual std::string GetName() const;

  /**
   * Prints out some debug information to the given output stream.
   *
   * @param   std::ostream & out
   * @RETURN  void
   */
  virtual void Debug(std::ostream& out) const;

  static int         GetTypeId();
  static void        SetTypeId(int nId);
  static std::string GetIdString(int nId);

 protected:
 private:
  static int m_nEventId;
};

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/

#endif // _VISTASYSTEMEVENT_H
