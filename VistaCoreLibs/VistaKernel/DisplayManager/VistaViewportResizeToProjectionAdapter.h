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

#ifndef _VISTAVIEWPORTRESIZETOPROJECTIONADAPTER_H
#define _VISTAVIEWPORTRESIZETOPROJECTIONADAPTER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include "VistaAspects/VistaObserver.h"

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaProjection;
class VistaViewport;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaViewportResizeToProjectionAdapter : public IVistaObserver {
 public:
  VistaViewportResizeToProjectionAdapter(VistaViewport* pViewport);
  virtual ~VistaViewportResizeToProjectionAdapter();

  enum ProjectionUpdateMode {
    UNDEFINED_PROJECTION_UPDATE_MODE = -1,
    NO_PROJECTION_CHANGE,
    MAINTAIN_HORIZONTAL_FOV,
    MAINTAIN_VERTICAL_FOV,
    MAINTAIN_SCALE,
  };
  static std::string          GetNameForProjectionUpdateMode(const ProjectionUpdateMode eMode);
  static ProjectionUpdateMode GetProjectionUpdateModeFromString(const std::string& sString);

  VistaViewport* GetViewport() const;
  void           SetViewport(VistaViewport* pViewport);

  VistaViewportResizeToProjectionAdapter::ProjectionUpdateMode GetUpdateMode() const;
  void                                                         SetUpdateMode(
                                                              const VistaViewportResizeToProjectionAdapter::ProjectionUpdateMode eProjection);

  virtual void ObserverUpdate(IVistaObserveable* pObserveable, int nMsg, int nTicket);

  void UpdateProjectionAfterViewportChange();

 private:
  VistaViewport*       m_pViewport;
  ProjectionUpdateMode m_eUpdateMode;
  int                  m_nCurrentSizeX;
  int                  m_nCurrentSizeY;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTAVIEWPORT_H)
