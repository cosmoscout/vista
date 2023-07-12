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

#ifndef _VISTADISPLAYSYSTEM_H
#define _VISTADISPLAYSYSTEM_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include "VistaDisplayEntity.h"
#include <VistaAspects/VistaObserver.h>

#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplayManager;
class VistaViewport;
class IVistaDisplayBridge;
class VistaVirtualPlatform;
class VistaVector3D;
class VistaQuaternion;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaDisplayDevice is ...
 */
class VISTAKERNELAPI VistaDisplaySystem : public VistaDisplayEntity, public IVistaObserver {
  friend class IVistaDisplayBridge;
  friend class VistaDisplayManager;

 public:
  virtual ~VistaDisplaySystem();

  VistaDisplayManager* GetDisplayManager() const;

  std::string           GetReferenceFrameName() const;
  VistaVirtualPlatform* GetReferenceFrame() const;

  unsigned int           GetNumberOfViewports() const;
  std::list<std::string> GetViewportNames() const;

  VistaViewport*               GetViewport(unsigned int iIndex) const;
  std::vector<VistaViewport*>& GetViewports();

  virtual void Debug(std::ostream& out) const;

  /**
   * Implement interface from IVistaObserver.
   */
  virtual bool ObserveableDeleteRequest(IVistaObserveable* pObserveable, int nTicket);
  virtual void ObserveableDelete(IVistaObserveable* pObserveable, int nTicket);
  virtual void ReleaseObserveable(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE);
  virtual void ObserverUpdate(IVistaObserveable* pObserveable, int msg, int ticket);
  virtual bool Observes(IVistaObserveable* pObserveable);
  virtual void Observe(
      IVistaObserveable* pObservable, int eTicket = IVistaObserveable::TICKET_NONE);

  /**
   * Set/get display system properties. The following keys are understood:
   *
   *  VIEWER_POSITION         -   [list of floats, 3 items]
   *  VIEWER_ORIENTATION      -   [list of floats, 4 items]
   *  LEFT_EYE_OFFSET         -   [list of floats, 3 items]
   *  RIGHT_EYE_OFFSET        -   [list of floats, 3 items]
   *  LOCAL_VIEWER            -   [bool]
   *  HMD_MODE                -   [bool]
   *  REFERENCE_FRAME_NAME    -   [string][read only]
   *  NUMBER_OF_VIEWPORTS     -   [int][read only]
   *  VIEWPORT_NAMES          -   [list of strings][read only]
   */

  enum EObserverTickets { FRAME_CHANGE, LAST_TICKET };

  class VISTAKERNELAPI VistaDisplaySystemProperties : public IVistaDisplayEntityProperties {
    friend class VistaDisplaySystem;

   public:
    enum {
      MSG_VIEWER_POSITION_CHANGE = IVistaDisplayEntityProperties::MSG_LAST,
      MSG_VIEWER_ORIENTATION_CHANGE,
      MSG_LEFT_EYE_OFFSET_CHANGE,
      MSG_RIGHT_EYE_OFFSET_CHANGE,
      MSG_LOCAL_VIEWER_CHANGE,
      MSG_HMDMODE_CHANGE,
      MSG_LAST
    };

    bool SetName(const std::string& sName);

    bool GetViewerPosition(float& x, float& y, float& z) const;
    bool SetViewerPosition(const float x, const float y, const float z);

    bool GetViewerOrientation(float& x, float& y, float& z, float& w) const;
    bool SetViewerOrientation(const float x, const float y, const float z, const float w);

    bool GetLeftEyeOffset(float& x, float& y, float& z) const;
    bool SetLeftEyeOffset(const float x, const float y, const float z);

    bool GetRightEyeOffset(float& x, float& y, float& z) const;
    bool SetRightEyeOffset(const float x, const float y, const float z);

    bool GetLocalViewer() const;
    bool SetLocalViewer(bool bLocalViewer);

    bool SetHMDModeActive(bool bSet);
    bool GetHMDModeActive() const;

    /* convenience API begin */
    bool          SetViewerPosition(const VistaVector3D& v3Pos);
    void          GetViewerPosition(VistaVector3D& v3Pos) const;
    VistaVector3D GetViewerPosition() const;

    bool            SetViewerOrientation(const VistaQuaternion& qOrient);
    void            GetViewerOrientation(VistaQuaternion& qOrient) const;
    VistaQuaternion GetViewerOrientation() const;

    void          GetViewerViewDirection(VistaVector3D& v3Dir) const;
    VistaVector3D GetViewerViewDirection() const;

    void GetEyeOffsets(VistaVector3D& v3LeftOffset, VistaVector3D& v3RightOffset) const;
    void SetEyeOffsets(const VistaVector3D& v3LeftOffset, const VistaVector3D& v3RightOffset);
    /* convenience API end */

    virtual std::string GetReflectionableType() const;

   protected:
    virtual int AddToBaseTypeList(std::list<std::string>& rBtList) const;
    virtual ~VistaDisplaySystemProperties();

   private:
    VistaDisplaySystemProperties(VistaDisplaySystem* pParent, IVistaDisplayBridge* pDisplayBridge);
  };

  VistaDisplaySystemProperties* GetDisplaySystemProperties() const;

 protected:
  virtual IVistaDisplayEntityProperties* CreateProperties();

  VistaDisplaySystem(VistaDisplayManager* pDisplayManager, VistaVirtualPlatform* pPlatform,
      IVistaDisplayEntityData* pData, IVistaDisplayBridge* pBridge);

  /*	VistaDisplaySystem(VistaVirtualPlatform *pPlatform,
                                                  IVistaDisplayEntityData *pData,
                                                  IVistaDisplayBridge *pBridge);*/

  VistaDisplayManager*        m_pDisplayManager;
  VistaVirtualPlatform*       m_pPlatform;
  std::vector<VistaViewport*> m_vecViewports;

  std::list<IVistaObserveable*> m_liObserveables;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif
