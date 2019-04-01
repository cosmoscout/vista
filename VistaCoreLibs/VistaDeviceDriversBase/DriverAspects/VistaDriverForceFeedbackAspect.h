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


#ifndef _VISTADRIVERFORCEFEEDBACKASPECT_H
#define _VISTADRIVERFORCEFEEDBACKASPECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

#include <VistaBase/VistaVectorMath.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaMutex;
class VistaDeviceSensor;
class VistaDriverMeasureHistoryAspect;
class VistaSensorReadState;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * As the name says, the ''ForceFeedbackAspect'' enables the use of
 * force feedback in a rather transparent way. If a device exports a
 * ''ForceFeedbackAspect'', it is supposed to support force feedback,
 * else it should not export such an aspect.
 * The ''ForceFeedbackAspect'' is a base class for more specific FF aspects,
 * as the capabilities and interfaces of some FF devices may be very
 * different. Subclasses of this aspect should NOT shadow the static
 * GetAspectId()/SetAspectId() APIs, as one must do when defining a new
 * aspect and should NOT try to register the aspect id with a global aspect id
 * mapping (such as the VistaNewInteractionManager).
 */
class VISTADEVICEDRIVERSAPI IVistaDriverForceFeedbackAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	IVistaDriverForceFeedbackAspect();
	virtual ~IVistaDriverForceFeedbackAspect();


	/**
	 * Low-level API to apply forces, this should be called
	 * only in very simple cases, for example vibrating joysticks. For more advanced
     * support, check the SetForceConstraintModel() API
     * @see SetForceConstraintModel()
	 * @param v3Force the force vector to set on the device API (dir+length)
	 * @param qTorque the angular force to apply, not all devices may support this
	 */
	virtual bool SetForce( const VistaVector3D   & v3Force,
						   const VistaVector3D &v3Torque )=0;

	/**
	 * Macro-method, calls SetForce(v3,q) with q=(0,0,0,1)
	 * @see SetForce()
	 */
	virtual bool SetForce( const VistaVector3D &v3Force );

    /**
     * @return the number of DOF for force input (0 for none)
     */
	virtual int GetNumInputDOF() const  = 0;

    /**
     * @return the number of DOF for force output (0 for none)
     */
	virtual int GetNumOutputDOF() const = 0;

    /**
     * Returns the maximum stiffness of this device.
     * @return 0 when this parameter is not present, > 0 else
     */
    virtual float GetMaximumStiffness() const = 0;

    /**
     * Returns the maximal force to be output by the device.
     * @return 0 for not applicable, > 0 else
     */
    virtual float GetMaximumForce() const = 0;

    /**
     * Enable / Disable the force output. Calls to SetForce() can
     * still be done in state disabled, but have no effect.
     */
	virtual bool SetForcesEnabled(bool bEnabled) = 0;

    /**
     * @return true when forces are enabled for this aspect, false else.
     * @see SetForcesEnabled()
     */
	virtual bool GetForcesEnabled() const = 0;


    /**
     * Base class for the definition of a force constraint. This is to
     * give a better level of defining the force feedback algorithm by
     * an application. The interface knows two sides:
     * - the ''writer'' of the force parameters (i.e. the application).
         the writer is constantly, but maybe at a lower frequency, updating
         the internal parameters of the force calculation algorithm.
         The interface for this is to be defined by subclasses.
         Once all parameters are set, the user code is to call Publish()
         which will transfer the new parameter set to the ''reader'', i.e.,
         the force feedback device.
     * - the ''reader'' which is in this case the force feedback device.
         the device will call the UpdateForce() method frequently in order
         to retrieve new forces and torque values. Zero values are allowed
         here to indicate that no force/torque is to be rendered by the device.
     * Subclasses of the IForceAlgorithm thus have to define two things:
     * - the interface to define the force algorithm parameters
     * - the calculation of the force effect based on these parameters inside
         of the UpdateForce() method. This ensures that the force calculation
         is performed with the frequency of the force feedback device.
     */
	class VISTADEVICEDRIVERSAPI IForceAlgorithm
	{
	public:
		virtual ~IForceAlgorithm();

		/**
		 *  this API is called by the applicator of the resulting force,
		 *  e.g., the driver. The coordinates are defined in the coordinate
         *  system of the force-feedback device!
         * @param dTs the current time stamp in seconds.
                  may be useful for interpolation.
         * @param v3CurrentPos the current position of the force effector (may be 0)
         * @param qCurrentOrientation the current orientation of the force effector (may be 0)
         * @param v3ResultingForce store the resulting force for the driver to apply here
         * @param qResultingTorque store the resulting torque for the driver to apply here
         * @param v3CurrentVelocity the current velocity vector in device frame
         * @return true iff a new force/torque was calculated, false else
                   a driver may decide to keep applying the current force/torque values
		 */
		virtual bool UpdateForce( double dTs, 
								  const VistaVector3D &v3CurrentPos,
								  const VistaVector3D &v3CurrentVelocity,
								  const VistaQuaternion &qCurrentOrientation,
								  VistaVector3D &v3ResultingForce,
								  VistaVector3D &v3ResultingTorque) = 0;

		/**
		 * alternative API to UpdateForce() from above, which takes a
		 * VistaTransformMatrix instread of a decomposed pos/ori.
		 * @see UpdateForce()
		 */
		virtual bool UpdateForce( double dTs, 
								  const VistaTransformMatrix &,
								  const VistaVector3D &v3Velocity,
				                  VistaVector3D &v3ResultingForce,
				                  VistaVector3D &v3ResultingTorque );
	protected:
		/**
		 * @param nStorageSize defines the size in bytes to store
                  for a single parameter set
		 */
		IForceAlgorithm( unsigned int nStorageSize );

		/**
		 *  call publish once done with the update of the writeout.
         * @return true iff the data was published to the ''writer''
		 */
		bool Publish();

		VistaDriverMeasureHistoryAspect *m_pHistoryAspect; /**< low level access to the parameter storage */
		VistaSensorReadState* m_pReadState; /**<ReadState for History>**/
		VistaDeviceSensor *m_pSensor; /**< low level access to the parameter storage */
	private:

	};

    /**
     * Sets the force constraint model. Is using locks to ensure a thread safety for the
     * reader of the force constraint model. However, if an old pointer is to be exchanged,
     * it can happen that this one is still used by the driver. Users should not delete
     * an old pointer, unless the driver is currently not updating or have waited another
     * update cycle.
     * @return always true
     */
	virtual bool SetForceAlgorithm( IForceAlgorithm *pModel );

    /**
     * Locks the internal pointer on access to ensure a proper pointer, just in case
     * a ''writer'' updates the force constraint model.
     * @see SetForceConstraintModel()
     * @return NULL iff no constraint model was set before
     */
	IForceAlgorithm *GetForceAlgorithm() const;

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:

private:
	static int m_nAspectId;
	VistaMutex           *m_pModelLock;
	IForceAlgorithm *m_pConstraintModel;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/**
 * Implements a plane constraint for force calculation on the plane.
 * The writer just calls SetContstraintPlane() whenever the constraint changes.
 * The FF device will, by itself concurrently or serial, the UpdateForce() method.
 */
class VISTADEVICEDRIVERSAPI VistaPlaneConstraint
: public IVistaDriverForceFeedbackAspect::IForceAlgorithm
{
public:
	VistaPlaneConstraint();
    virtual ~VistaPlaneConstraint();

	/**
	 * Is called by the driver to calculate the current force behavior.
	 */
	virtual bool UpdateForce( double dTs, 
							  const VistaVector3D &v3CurrentPos,
							  const VistaVector3D &v3CurrentVelocity,
							  const VistaQuaternion &qCurrentOrientation,
							  VistaVector3D &v3ResultingForce,
							  VistaVector3D &v3ResultingTorque);
	// using base class' function to make alternate version of UpdateForce available
	using IVistaDriverForceFeedbackAspect::IForceAlgorithm::UpdateForce;


	/**
	 * Is called by the application / modifier algorithm to parameterize the
	 * force-calculation during the UpdateForce. The call to this method is thread safe by
	 * design, no need for locking. However, only one thread should call it at the same time,
	 * as it is not reentrant.
	 * @param v3ToOrigin a vector to the plane origin, must be in the reference frame of the FF device
	 * @param v3Normal the normal vector of the plane, defines ABOVE/BELOW of the plane, should be in the
	                   refernence frame of the FF device
	 * @param nStiffness the stiffness of the constraint point
	 * @param v3InternalForce an internal force at the point, which is added to the force that is calculated.
	 */
	bool SetConstraintPlane( const VistaVector3D &v3ToOrigin,
			                 const VistaVector3D &v3Normal,
			                 float nStiffness,
							 float nDamping,
			                 const VistaVector3D &v3InternalForce = VistaVector3D() );

private:
	// small helper to structure the memory layout of the history.
	struct _sPlane
	{
		float m_afPlaneOrigin[4],
		      m_afPlaneNormal[4];
		float m_afInternalForce[4];
		float m_nStiffness;
		float m_nDamping;
	};
};
#endif //_VISTADRIVERFORCEFEEDBACKASPECT_H
