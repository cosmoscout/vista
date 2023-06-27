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

#ifndef _VISTA3DTEXTOVERLAY_H
#define _VISTA3DTEXTOVERLAY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/VistaKernelConfig.h>

#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaViewport;
class IVistaTextEntity;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI Vista3DTextOverlay : public IVistaSceneOverlay {
 public:
  Vista3DTextOverlay(VistaDisplayManager* pDisplayManager, const std::string& sViewportName = "");
  Vista3DTextOverlay(VistaViewport* pViewport);
  virtual ~Vista3DTextOverlay();

  virtual bool Do();

  bool AddText(IVistaTextEntity* pText, bool bManageDeletion = false);
  bool RemText(IVistaTextEntity* pText);

  bool GetIsEnabled() const;
  void SetIsEnabled(bool bEnabled);

  virtual void UpdateOnViewportChange(int iWidth, int iHeight, int iPosX, int iPosY);

 private:
  std::list<IVistaTextEntity*> m_liTexts;
  std::list<IVistaTextEntity*> m_liMemoryManagedTexts;
  bool                         m_bEnabled;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASIMPLETEXTOVERLAY_H
