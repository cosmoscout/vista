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

#ifndef _VDFNQUATERNIONSLERPNODE_H
#define _VDFNQUATERNIONSLERPNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"
#include "VdfnNodeFactory.h"
#include "VdfnObjectRegistry.h"

#include <VistaBase/VistaVectorMath.h>

#include <string>

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
 * VdfnQuaternionSlerpNode<br>
 * This DFN-Node calculates the slerp between two quaternions using a fraction
 * value.<br>
 * 
 * @ingroup VdfnNodes
 * @inport{ first, VistaQuaternion, optional, First quaternion (default = 0\,
 * 0\, 0\, 1) }
 * @inport{ second, VistaQuaternion, optional, Second quaternion (default = 
 * 0\, 0\, 0\, 1) }
 * @inport{ fraction, float, optional, Fraction value for interpolation
 * between quaternions (default = 0) }
 *
 * @outport{ out, VistaQuaternion, The resulting VistaQuaternion }
 */
class VISTADFNAPI VdfnQuaternionSlerpNode : public IVdfnNode
{
public:
	VdfnQuaternionSlerpNode();
   virtual ~VdfnQuaternionSlerpNode();
   
   virtual bool GetIsValid() const;
   virtual bool PrepareEvaluationRun();
   
   static const std::string S_sFirstQuaternionInPortName;
   static const std::string S_sSecondQuaternionInPortName;
   static const std::string S_sFractionInPortName;

   static const std::string S_sQuaternionOutPortName;

protected:
   virtual bool DoEvalNode();

private:
   TVdfnPort< VistaQuaternion > *m_pInFirstQuaternion;
   TVdfnPort< VistaQuaternion > *m_pInSecondQuaternion;
   TVdfnPort< float > *m_pInFraction;
   
   TVdfnPort< VistaQuaternion > *m_pOutQuaternion;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif //_VDFNQUATERNIONSLERPNODE_H
