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


#ifndef _VDFNAXISROTATENODE_H
#define _VDFNAXISROTATENODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnSerializer.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
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
 * Constructs a quaternion from a given axis and angle (give as float and
 * vector 3D), or can be used to convert a given VistaAxisAndAngle type to
 * a VistaQuaternion. The axis and angle will be used to construct an
 * VistaAxisAndAngle object, which in turn is converted to a VistaQuaternion.
 * If an VistaAxisAndAngle is directly specified, this node will call the
 * conversion constructor to deliver a coresponding VistaQuaternion.
 *
 *
 * @ingroup VdfnNodes
 * @inport{angle,float,mandatory with axis inport,the angle to rotate the axis (radians)}
 * @inport{axis,VistaVector3D,mandatory with angle inport,the axis to use for rotation}
 * @inport{axisandangle,VistaAxisAndAngle,optional (dominates axis and angle inports),
 *         an VistaAxisAndAngle value to convert}
 * @outport{out,VistaQuaternion,the resulting quat}
 */
class VISTADFNAPI VdfnAxisRotateNode : public IVdfnNode
{
public:
	VdfnAxisRotateNode();

	/**
	 * @return either axisandangle is non-null and (angle and axis) are null OR
	           axisandangle is null and (angle and axis) are non-NULL
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
	TVdfnPort<VistaAxisAndAngle> *m_pAxisAndAngle;
	TVdfnPort<float>            *m_pAngle;
	TVdfnPort<VistaVector3D>   *m_pAxis;
	TVdfnPort<VistaQuaternion> *m_pOut;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNAXISROTATENODE_H

