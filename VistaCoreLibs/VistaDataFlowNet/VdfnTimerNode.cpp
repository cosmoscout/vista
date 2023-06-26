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

#include "VdfnTimerNode.h"
#include "VdfnUtil.h"

#include <iostream>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnTimerNode::VdfnTimerNode(CGetTime* pGt, bool bReset)
    : IVdfnNode()
    ,
    // m_pUpd(new _sLastUpdate),
    m_pTimer(new TVdfnPort<double>)
    , m_pDt(new TVdfnPort<double>)
    , m_bReset(bReset)
    , m_nLastUpdate(0)
    , m_pGetTime(pGt) {
  SetEvaluationFlag(true); // unconditional update
  RegisterOutPort("time", m_pTimer);
  RegisterOutPort("dt", m_pDt);
}

VdfnTimerNode::~VdfnTimerNode() {
  // delete m_pUpd;
  delete m_pGetTime;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnTimerNode::GetIsValid() const {
  return (m_pGetTime != NULL);
}

void VdfnTimerNode::OnActivation(double dTs) {
  if (m_bReset) {
    const double dTime = m_pGetTime->GetTime();
    m_nLastUpdate      = dTime;
  }
}

bool VdfnTimerNode::DoEvalNode() {
  const double dTime = m_pGetTime->GetTime();
  // we try to avoid an update when there is no change in time
  // this can happen if we use a clock with a coarse granularity
  // and yes, this might not be what the user wants (e.g., she wants
  // frame-wise sampling where dt = 0 is possible with this clock)
  if (dTime != m_nLastUpdate) {
    m_pTimer->SetValue(m_pGetTime->GetTime(), 0);

    if (m_nLastUpdate) {
      m_pDt->SetValue((dTime - m_nLastUpdate), 0);
    }
    m_nLastUpdate = dTime;
  }
  return true;
}

// #############################################################################

CTimerGetTime::CTimerGetTime()
    : VdfnTimerNode::CGetTime()
    , m_oTimer() {
}

double CTimerGetTime::GetTime() {
  return m_oTimer.GetSystemTime();
}

// #############################################################################

VdfnTickTimerNode::VdfnTickTimerNode(VdfnTimerNode::CGetTime* pGetTime)
    : m_pGetTime(pGetTime)
    , m_pTickTime(new TVdfnPort<double>)
    , m_pFraction(new TVdfnPort<double>)
    , m_pTime(new TVdfnPort<double>)
    , m_pTimeStep(NULL)
    , m_nLastUpdate(0) {
  SetEvaluationFlag(true);
  RegisterInPortPrototype("step", new TVdfnPortTypeCompare<TVdfnPort<double>>);
  RegisterOutPort("time", m_pTime);
  RegisterOutPort("ticktime", m_pTickTime);
  RegisterOutPort("fraction", m_pFraction);
}

VdfnTickTimerNode::~VdfnTickTimerNode() {
  delete m_pGetTime;
}

bool VdfnTickTimerNode::GetIsValid() const {
  return (m_pGetTime && IVdfnNode::GetIsValid());
}

bool VdfnTickTimerNode::PrepareEvaluationRun() {
  m_pTimeStep = VdfnUtil::GetInPortTyped<TVdfnPort<double>*>("step", this);
  return GetIsValid();
}

bool VdfnTickTimerNode::DoEvalNode() {
  if (m_nLastUpdate == 0) {
    m_nLastUpdate = GetUpdateTimeStamp(); //(*m_pGetTime).GetTime();
  } else {
    double nStep = m_pTimeStep->GetValue();

    double nCurUpdate = GetUpdateTimeStamp(); // (*m_pGetTime).GetTime();
    double nTickTime  = nCurUpdate - m_nLastUpdate;
    if (nTickTime > nStep) {
      // ok, new step
      m_pTickTime->SetValue(nTickTime, GetUpdateTimeStamp());
      m_pFraction->SetValue(nTickTime - nStep, GetUpdateTimeStamp());
      m_pTime->SetValue(nCurUpdate, GetUpdateTimeStamp());

      m_nLastUpdate = nCurUpdate; // swizzle time steps
    }
  }
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
