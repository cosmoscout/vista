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

#include "VdfnInvertNode.h"
#include <VistaBase/VistaVectorMath.h>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
template <>
VISTADFNAPI bool TVdfnInvertNode<float>::DoEvalNode() {
  m_pOut->SetValue(1.0f / m_pIn->GetValue(), GetUpdateTimeStamp());
  return true;
}

template <>
VISTADFNAPI bool TVdfnInvertNode<double>::DoEvalNode() {
  m_pOut->SetValue(1.0 / m_pIn->GetValue(), GetUpdateTimeStamp());
  return true;
}

template <>
VISTADFNAPI bool TVdfnInvertNode<VistaQuaternion>::DoEvalNode() {
  m_pOut->SetValue(m_pIn->GetValue().GetInverted(), GetUpdateTimeStamp());
  return true;
}

template <>
VISTADFNAPI bool TVdfnInvertNode<VistaTransformMatrix>::DoEvalNode() {
  VistaTransformMatrix m;
  m_pIn->GetValueConstRef().GetInverted(m);
  m_pOut->SetValue(m, GetUpdateTimeStamp());
  return true;
}

template <>
VISTADFNAPI bool TVdfnInvertNode<bool>::DoEvalNode() {
  m_pOut->SetValue(!m_pIn->GetValue(), GetUpdateTimeStamp());
  return true;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
