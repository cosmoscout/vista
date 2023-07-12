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

#ifndef _PHANTOMWORKSPACEHANDLER_H
#define _PHANTOMWORKSPACEHANDLER_H

#include <VistaKernel/EventManager/VistaEventHandler.h>

class IVistaTransformable;

class PhantomWorkspaceHandler : public VistaEventHandler {
 public:
  PhantomWorkspaceHandler(VdfnObjectRegistry*, IVistaTransformable*);
  virtual ~PhantomWorkspaceHandler();

  virtual void HandleEvent(VistaEvent* pEvent);

 private:
  VdfnObjectRegistry*  m_pDFNRegistry;
  IVistaTransformable* m_pTransformable;
};

#endif /* _PHANTOMWORKSPACEHANDLER_H */
