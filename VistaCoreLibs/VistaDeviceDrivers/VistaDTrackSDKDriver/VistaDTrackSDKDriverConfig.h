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

#ifndef __VISTADTRACKSDKDRIVERCONFIG_H
#define __VISTADTRACKSDKDRIVERCONFIG_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(WIN32)
#pragma warning(disable : 4786)
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

// Single marker data (3DOF):
namespace VistaDTrackSDKConfig {
typedef struct {
  int   id;      // id number (starting with 1)
  float quality; // quality (0 <= qu <= 1)

  float loc[3]; // location (in mm)
} VISTA_dtrack_marker_type;

// Standard body data (6DOF):
//  - currently not tracked bodies are getting a quality of -1

typedef struct {
  int   id;      // id number (starting with 0)
  float quality; // quality (0 <= qu <= 1, no tracking if -1)

  float loc[3]; // location (in mm)
  float rot[9]; // rotation matrix (column-wise)
} VISTA_dtrack_body_type;
} // namespace VistaDTrackSDKConfig

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__VISTADTRACKSDKDRIVERCONFIG_H
