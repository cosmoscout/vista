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

#include "SwitchShadowCallback.h"
#include "ShadowDemo.h"

#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernelOpenSGExt/VistaOpenSGShadow.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

SwitchShadowCallback::SwitchShadowCallback(ShadowDemo* pAppl, int iDiff)
    : m_pShadowDemo(pAppl)
    , m_iDiff(iDiff) {
}

SwitchShadowCallback::~SwitchShadowCallback() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool SwitchShadowCallback::Do() {
  // parses the Parameters again
  // use modulo to stay in legal scope
  int iNextMode =
      (m_pShadowDemo->GetActiveShadowMode() + m_iDiff) % (VistaOpenSGShadow::VOSGSHADOW_LAST + 1);
  while (iNextMode < 0)
    iNextMode += VistaOpenSGShadow::VOSGSHADOW_LAST + 1;
  m_pShadowDemo->SetActiveShadowMode(VistaOpenSGShadow::eShadowMode(iNextMode));
  std::cout << "ShadowMode is now "
            << ShadowDemo::GetShadowModeName(m_pShadowDemo->GetActiveShadowMode()) << std::endl;
  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "MyDemoAppl.cpp"                                              */
/*============================================================================*/
