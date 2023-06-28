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

#ifndef _GREEDYOBSERVER_H
#define _GREEDYOBSERVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/EventManager/VistaEventObserver.h>
#include <fstream>
#include <iostream>

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * GreedyObserver - an example for an application-defined event
 */
class GreedyObserver : public VistaEventObserver {
 public:
  /**
   * Creates a GreedyObserver object
   *
   * @AUTHOR  Marc Schirski
   * @DATE    08.11.2002
   */
  GreedyObserver();

  virtual ~GreedyObserver();

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
  //@TODO Make this reference?
  std::ofstream* m_pOuputFileStream;
};

#endif // _GREEDYOBSERVER_H
