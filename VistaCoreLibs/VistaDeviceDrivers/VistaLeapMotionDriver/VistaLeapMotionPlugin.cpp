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

#include "VistaLeapMotionDriver.h"

#if defined(WIN32) && !defined(VISTALEAPMOTIONDRIVERPLUGIN_STATIC)
#ifdef VISTALEAPMOTIONPLUGIN_EXPORTS
#define VISTALEAPMOTIONPLUGINAPI __declspec(dllexport)
#else
#define VISTALEAPMOTIONPLUGINAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTALEAPMOTIONPLUGINAPI
#endif

namespace {
VistaLeapMotionCreationMethod* SpFactory = NULL;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
extern "C" VISTALEAPMOTIONPLUGINAPI IVistaDeviceDriver* CreateDevice(
    IVistaDriverCreationMethod* crm) {
  return new VistaLeapMotionDriver(crm);
}

extern "C" VISTALEAPMOTIONPLUGINAPI IVistaDriverCreationMethod* GetCreationMethod(
    IVistaTranscoderFactoryFactory* fac) {
  if (SpFactory == NULL)
    SpFactory = new VistaLeapMotionCreationMethod(fac);

  IVistaReferenceCountable::refup(SpFactory);
  return SpFactory;
}

extern "C" VISTALEAPMOTIONPLUGINAPI void DisposeCreationMethod(IVistaDriverCreationMethod* crm) {
  if (SpFactory == crm) {
    delete SpFactory;
    SpFactory = NULL;
  } else
    delete crm;
}

extern "C" VISTALEAPMOTIONPLUGINAPI void UnloadCreationMethod(IVistaDriverCreationMethod* crm) {
  if (SpFactory != NULL) {
    if (IVistaReferenceCountable::refdown(SpFactory))
      SpFactory = NULL;
  }
}

extern "C" VISTALEAPMOTIONPLUGINAPI const char* GetDeviceClassName() {
  return "LEAPMOTION";
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
