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

#include "VistaDfnViewportSourceNode.h"
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaKernel/DisplayManager/VistaViewport.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/EventManager/VistaEventManager.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <VistaAspects/VistaObserver.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
class VistaDfnViewportSourceNode::ViewportObserver : public IVistaObserver {
 public:
  ViewportObserver(VistaDfnViewportSourceNode* pParent)
      : IVistaObserver()
      , m_pParent(pParent)
      , m_bVpChange(false)
      , m_pObs(NULL) {
  }

  ~ViewportObserver() {
    if (Observes(m_pObs))
      ReleaseObserveable(m_pObs);
  }

  virtual bool ObserveableDeleteRequest(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    return true;
  }

  virtual void ObserveableDelete(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    ReleaseObserveable(pObserveable, nTicket);
  }

  virtual void ReleaseObserveable(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    pObserveable->DetachObserver(this);
    m_pParent->m_pViewport = NULL;
  }

  virtual void ObserverUpdate(IVistaObserveable* pObserveable, int msg, int ticket) {
    switch (msg) {
    case VistaViewport::VistaViewportProperties::MSG_SIZE_CHANGE: {
      m_bVpChange = true;
      break;
    }
    default:
      break;
    }
  }

  virtual bool Observes(IVistaObserveable* pObserveable) {
    return (m_pObs == pObserveable);
  }

  virtual void Observe(
      IVistaObserveable* pObservable, int eTicket = IVistaObserveable::TICKET_NONE) {
    if (m_pObs)
      m_pObs->DetachObserver(this);

    m_pObs = pObservable;

    if (m_pObs)
      m_pObs->AttachObserver(this, eTicket);
  }

  bool GetPropsChangeFlag() const {
    return (m_bVpChange == 0 ? false : true);
  }

  bool GetPropsChange() {
    bool bRet   = (m_bVpChange == 0 ? false : true);
    m_bVpChange = 0;
    return bRet;
  }

 private:
  VistaDfnViewportSourceNode* m_pParent;
  int                         m_bVpChange;
  IVistaObserveable*          m_pObs;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnViewportSourceNode::VistaDfnViewportSourceNode(VistaViewport* pViewport)
    : IVdfnNode()
    , m_pW(new TVdfnPort<int>)
    , m_pH(new TVdfnPort<int>)
    , m_pViewport(pViewport)
    , m_pObs(NULL)
    , m_nUpdateCount(0)
    , m_bNeedsUpdate(true) {
  RegisterOutPort("vp_w", m_pW);
  RegisterOutPort("vp_h", m_pH);

  m_pObs = new ViewportObserver(this);
  m_pObs->Observe(pViewport->GetViewportProperties());
}

VistaDfnViewportSourceNode::~VistaDfnViewportSourceNode() {
  delete m_pObs;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaDfnViewportSourceNode::GetIsValid() const {
  return (m_pViewport != NULL);
}

bool VistaDfnViewportSourceNode::DoEvalNode() {
  int nW, nH;
  m_pViewport->GetViewportProperties()->GetSize(nW, nH);

  m_pW->SetValue(nW, GetUpdateTimeStamp());
  m_pH->SetValue(nH, GetUpdateTimeStamp());

  return true;
}

unsigned int VistaDfnViewportSourceNode::CalcUpdateNeededScore() const {
  if (m_bNeedsUpdate ||
      m_pObs->GetPropsChangeFlag()) // use the const-version here, we need the flags later
  {
    m_bNeedsUpdate = false; // one-timer
    return ++m_nUpdateCount;
  }
  return m_nUpdateCount;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
