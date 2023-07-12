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

#ifndef _VISTASIXENSECOMMONSHARE_H
#define _VISTASIXENSECOMMONSHARE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaBaseTypes.h>

namespace VistaSixenseMeasures {
/**
 * memory layout of the global type sensor
 */
struct Measure {
  float          m_a3fPosition[3];
  float          m_a3x3fRotationMatrix[3][3];
  float          m_nJoystickX;
  float          m_nJoystickY;
  float          m_nTrigger;
  unsigned int   m_nButtons;
  unsigned char  m_nSequenceNumber;
  float          a4fRotationQuaternion[4];
  unsigned short m_nFirmwareVersion;
  unsigned short m_nHardwareRevision;
  unsigned short m_nPacketType;
  unsigned short m_nMagneticFrequency;
  int            m_nEnabled;
  int            m_nControllerIndex;
  unsigned char  m_cIsDocked;
  unsigned char  m_cWhichHand;
  unsigned char  m_cHemisphereTrackingEnabled;
};

// button mask definitions
static const int g_nButton1Mask        = (0x01 << 5);
static const int g_nButton2Mask        = (0x01 << 6);
static const int g_nButton3Mask        = (0x01 << 3);
static const int g_nButton4Mask        = (0x01 << 4);
static const int g_nButtonBumperMask   = (0x01 << 7);
static const int g_nButtonJoystickMask = (0x01 << 8);
static const int g_nButtonStartMask    = (0x01 << 0);
} // namespace VistaSixenseMeasures

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASIXENSECOMMONSHARE_H
