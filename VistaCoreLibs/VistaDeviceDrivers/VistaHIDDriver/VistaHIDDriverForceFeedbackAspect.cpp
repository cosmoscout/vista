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

#if defined(LINUX)

#include <cstdio>

#include <VistaInterProcComm/Connections/VistaConnection.h>

#include "VistaHIDDriverForceFeedbackAspect.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaHIDDriverForceFeedbackAspect::VistaHIDDriverForceFeedbackAspect(VistaConnection* pCon)
    : IVistaDriverForceFeedbackAspect() {
  m_pCon = pCon;

  m_effect.type                 = FF_PERIODIC;
  m_effect.id                   = -1;
  m_effect.u.periodic.waveform  = FF_SQUARE;
  m_effect.u.periodic.period    = 0x100;
  m_effect.u.periodic.magnitude = 0x8000;
  m_effect.u.periodic.offset    = 0x0000;
  m_effect.u.periodic.phase     = 0;

#if 0 // check version of linux kernel?
	m_effect.direction = 0x8000;
	m_effect.u.periodic.envelope.attack_length = 0x000;
	m_effect.u.periodic.envelope.attack_level = 0;
	m_effect.u.periodic.envelope.fade_length = 0x000;
	m_effect.u.periodic.envelope.fade_level = 0;
#endif
  m_effect.trigger.button   = 0;
  m_effect.trigger.interval = 0;
  m_effect.replay.length    = 1000;
  m_effect.replay.delay     = 0;

  if (ioctl(pCon->GetConnectionDescriptor(), EVIOCSFF, &m_effect) == -1) {
    perror("Upload effect");
  }
}

VistaHIDDriverForceFeedbackAspect::~VistaHIDDriverForceFeedbackAspect() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaHIDDriverForceFeedbackAspect::SetForce(const VistaVector3D& force, const VistaVector3D&) {
  input_event start_ff;

  start_ff.type  = EV_FF;
  start_ff.code  = 0;
  start_ff.value = 1;

  int nRet = m_pCon->Send(&start_ff, sizeof(start_ff));
  return (nRet == sizeof(start_ff));
}

int VistaHIDDriverForceFeedbackAspect::GetNumInputDOF() const {
  return 0;
}

int VistaHIDDriverForceFeedbackAspect::GetNumOutputDOF() const {
  return 0;
}

bool VistaHIDDriverForceFeedbackAspect::SetForcesEnabled(bool bEnabled) {
  return false;
}

bool VistaHIDDriverForceFeedbackAspect::GetForcesEnabled() const {
  return false;
}

float VistaHIDDriverForceFeedbackAspect::GetMaximumStiffness() const {
  return 0.0f;
}

float VistaHIDDriverForceFeedbackAspect::GetMaximumForce() const {
  return 0.0f;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif
