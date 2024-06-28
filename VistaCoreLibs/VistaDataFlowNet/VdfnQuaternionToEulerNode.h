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

#ifndef _VDFNQUATERNIONTOEULERNODE_H
#define _VDFNQUATERNIONTOEULERNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Deconstruct a quaternion to a euler angles (give as vector 3D).
 *
 * @ingroup VdfnNodes
 * @inport{quaternion, VistaQuaternion ,VistaVector3D,mandatory}
 * @outport{out,VistaVector3D,the resulting eulerangles}
 */
class VISTADFNAPI VdfnQuaternionToEulerNode : public IVdfnNode {
 public:
  VdfnQuaternionToEulerNode();

  /**
   * @return quaternion is non-null
   */
  bool GetIsValid() const;

  /**
   * @return GetIsValid()
   */
  bool PrepareEvaluationRun();

 protected:
  /**
   * @return true
   */
  bool DoEvalNode();

 private:
  TVdfnPort<VistaQuaternion>*   m_pQuaternion;
  TVdfnPort<VistaVector3D>* m_pOut;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNQUATERNIONTOEULERNODE_H
