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

#ifndef _VISTASIMPLETEXTOVERLAY_H
#define _VISTASIMPLETEXTOVERLAY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/DisplayManager/VistaViewport.h>

#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaViewport;
class VistaEventManager;
class IVistaTextEntity;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This provides a simple overlay to draw text line-wise
 * Each added text is positioned based on it's x- and y-pos,
 * where y-pos is the line number it is to appear in
 * and x-pos is the number of preceeding spaces
 */
class VISTAKERNELAPI VistaSimpleTextOverlay : public IVistaSceneOverlay {
 public:
  VistaSimpleTextOverlay(
      VistaDisplayManager* pDisplayManager, const std::string& sViewportName = "");
  VistaSimpleTextOverlay(VistaViewport* pViewport);
  virtual ~VistaSimpleTextOverlay();

  virtual bool Do();

  bool AddText(IVistaTextEntity* pText, bool bManageDeletion = false);
  bool RemText(IVistaTextEntity* pText);

  virtual bool GetIsEnabled() const;
  virtual void SetIsEnabled(bool bEnabled);

  virtual void UpdateOnViewportChange(int iWidth, int iHeight, int iPosX, int iPosY);

 private:
  class RatioChangeObserver;

  int m_nWidth;
  int m_nHeight;
  int m_nPosX;
  int m_nPosY;

  std::list<IVistaTextEntity*> m_liTexts;
  std::list<IVistaTextEntity*> m_liMemoryManagedTexts;
  bool                         m_bEnabled;

  VistaSimpleTextOverlay::RatioChangeObserver* m_pRatioObeserver;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VistaSimpleTextOverlay_H
