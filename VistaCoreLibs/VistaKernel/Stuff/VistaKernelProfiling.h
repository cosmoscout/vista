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

#ifndef _VISTAKERNELPROFILING_H
#define _VISTAKERNELPROFILING_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaTools/VistaBasicProfiler.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

#ifdef VISTAKERNEL_USE_PROFILING
#define VistaKernelProfileScope(sSectionName)                                                      \
  VistaBasicProfiler::ProfileScopeObject oLocalSection_sSectionName(sSectionName)
#define VistaKernelProfileStartSection(sSectionName)                                               \
  VistaBasicProfiler::GetSingleton()->StartSection(sSectionName)
#define VistaKernelProfileStopSection() VistaBasicProfiler::GetSingleton()->StopSection()
#define VistaKernelProfileStopNamedSection(sSectionName)                                           \
  VistaBasicProfiler::GetSingleton()->StopSection(sSectionName)
#define VistaKernelProfileNewFrame() VistaBasicProfiler::GetSingleton()->NewFrame()
#else
#define VistaKernelProfileScope(sSectionName)                                                      \
  {}
#define VistaKernelProfileStartSection(sSectionName)                                               \
  {}
#define VistaKernelProfileStopSection()                                                            \
  {}
#define VistaKernelProfileStopNamedSection(sSectionName)                                           \
  {}
#define VistaKernelProfileNewFrame()                                                               \
  {}
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAVIRTUALCONSOLE_H
