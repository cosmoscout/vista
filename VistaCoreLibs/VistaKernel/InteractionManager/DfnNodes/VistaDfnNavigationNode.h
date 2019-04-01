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


#ifndef _VISTADFNNAVIGATIONNODE_H
#define _VISTADFNNAVIGATIONNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaDataFlowNet/VdfnNodeFactory.h>

#include <VistaBase/VistaVectorMath.h>

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
 * Node to apply different navigation operations
 *
 * @ingroup VdfnNodes
 * @inport{translation,VistaVector3D,semi-mandatory,translation to be applied}
 * @inport{rotation,VistaQuaternion,semi-mandatory,rotation to be applied}
 * @inport{pivot,VistaTransformMatrix,optional,center of rotation}
 * @inport{dt,double,mandatory,time delta to last evaluation}
 * @inport{transform,VistaTransformMatrix,optional,original matrix to be transformed}
 * @inport{navigation_mode,int,optional,navigation mode (0\, 1\, 2}
 * @inport{linear_velocity,float,optional,angular velocity in meters per second}
 * @inport{angular_velocity,float,optional,angular velocity in rad per soceond}
 * @outport{transform,VistaTransformMatrix,transformed ourput matrix}
 *
 * The Navigation Node allows a transform with input translation and rotation.
 * To be valid, the node needs at least the dt input and either translation or
 * rotation.
 * Linear and Angular Velocities specify the speed of the navigation and are
 * reached when the input translation has a magnitude of 1, and the input rotation
 * has a magnitude (rotation angle) of pi.
 * There are three different navigation modes available, which determine how
 * the rotation is applied:
 * mode 0 - Unconstrained rotation - rotation is not modified
 * mode 1 - Helicopter Mode - rotation is constrained to yaw, i.e. one only
 *          rotates around the y axis
 * mode 2 - LookAround Mode - Only Yaw and Pitch components are retained, and 
 *          all roll (rotation around x-axis) is removed. Thus, one can look
 *          around and up and down, but not "tilt" your head.
 *          Careful: This node removes all roll from the full rotation, i.e.
 *          it also removes any roll from the input transform. This is necessary
 *          To prevent roll sneaking in by accumulation of smaller rotations.
 *          This mode also prevents gimbal locks by constraining the maximum
 *          angle one can look up and down.
 * The navigation mode can be set dynamically via the inports. If the inports
 * are not set, default values will be used. These can be specified in the
 * node's parameters using "default_navigation_mode" (default is 0),
 * "default_linear_velocity" (default is 1.0) and "default_angular_velocity"
 * (default is pi).
 */
class VISTAKERNELAPI VistaDfnNavigationNode : public IVdfnNode
{
public:
	VistaDfnNavigationNode( const int iDefaultNavigationMode = 0,
							const float fDefaultLinearSpeed = 1.0f,
							const float fDefaultAngularSpeed = 3.14159f,
							const float fLinearAcceleration = 0,
							const float fLinearDecceleration = 0,
							const float fAngularAcceleration = 0,
							const float fAngularDecceleration = 0 );
	virtual ~VistaDfnNavigationNode();

	virtual bool PrepareEvaluationRun();
protected:
	virtual bool DoEvalNode();
	virtual bool GetIsValid() const;

	/**
	 * Translates matTransform with the offset given by the translation
	 * inport.
	 */
	void ApplyTranslation( VistaTransformMatrix& matTransform );

	void UpdateVelocities();

	/**
	 * Helper functions that take the matrix matTransform and rotate it
	 * by the rotation on the inport, scaled by angular velocity
	 * and time difference, and applies the rotation to the input
	 * matrix. The rotation is constraint differently in
	 * the different functions.
	 */
	void ApplyRotationFull( VistaTransformMatrix& matTransform );
	void ApplyRotationYawOnly( VistaTransformMatrix& matTransform );
	void ApplyRotationNoRoll( VistaTransformMatrix& matTransform );

private:
	TVdfnPort<VistaTransformMatrix>*	m_pTransform;
	TVdfnPort<VistaTransformMatrix>*	m_pOut;
	TVdfnPort<VistaVector3D>*		m_pTranslation;
	TVdfnPort<double>*				m_pDeltaTime;
	TVdfnPort<VistaQuaternion>*		m_pRotation;
	TVdfnPort<VistaVector3D>*		m_pRotationPivot;
	TVdfnPort<int>*					m_pNavigationMode;
	TVdfnPort<float>*				m_pLinearSpeed;
	TVdfnPort<float>*				m_pAngularSpeed;

	float				m_fTargetAngularSpeed;
	float				m_fCurrentAngularSpeed;
	VistaAxisAndAngle	m_aaCurrentAngularVelocity;
	float				m_fAngularAcceleration;
	float				m_fAngularDecceleration;

	float				m_fDefaultLinearSpeed;
	VistaVector3D		m_v3TargetLinearVelocity;
	VistaVector3D		m_v3CurrentLinearVelocity;
	float				m_fLinearAcceleration;
	float				m_fLinearDeceleration;
	int					m_iNavigationMode;

	float				m_fDeltaTime;

	// saves the revision of the delta-time inport from the last evaluation
	// to see if the time-delta has really been updated.
	unsigned int m_nLastDTUpateCount;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif
