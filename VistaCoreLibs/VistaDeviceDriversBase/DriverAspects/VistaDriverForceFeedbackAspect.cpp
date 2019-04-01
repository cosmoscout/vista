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


#include "VistaDriverForceFeedbackAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"

#include "VistaDriverMeasureHistoryAspect.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaSensorReadState.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <VistaMath/VistaGeometries.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>

#include <assert.h>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int IVistaDriverForceFeedbackAspect::m_nAspectId = -1;

int  IVistaDriverForceFeedbackAspect::GetAspectId()
{
	return IVistaDriverForceFeedbackAspect::m_nAspectId;
}

void IVistaDriverForceFeedbackAspect::SetAspectId(int nId)
{
	assert(m_nAspectId == -1);
	m_nAspectId = nId;
}


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaDriverForceFeedbackAspect::IVistaDriverForceFeedbackAspect()
	: IVistaDeviceDriver::IVistaDeviceDriverAspect(),
	  m_pModelLock( new VistaMutex ),
	  m_pConstraintModel(NULL)
{
	if(IVistaDriverForceFeedbackAspect::GetAspectId() == -1) // unregistered
		IVistaDriverForceFeedbackAspect::SetAspectId(
		VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("FORCEFEEDBACK"));

	SetId(IVistaDriverForceFeedbackAspect::GetAspectId());

}

IVistaDriverForceFeedbackAspect::~IVistaDriverForceFeedbackAspect()
{
	delete m_pModelLock;
	delete m_pConstraintModel; // good idea?
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool IVistaDriverForceFeedbackAspect::SetForce( const VistaVector3D &v3Force )
{
	return SetForce( v3Force, VistaVector3D() );
}

bool IVistaDriverForceFeedbackAspect::SetForceAlgorithm( IForceAlgorithm *pModel )
{
    // using a simple lock to ensure that an exchange of the pointer
    // takes place without severe problems. Note that however, there
    // might be a race when the "old" pointer of the m_pConstraintModel
    // is still used (for example inside a driver), so users should not
    // delete an old pointer immediately after retrieval.
	VistaMutexLock l(*m_pModelLock);
	m_pConstraintModel = pModel;
	return true;
}

IVistaDriverForceFeedbackAspect::IForceAlgorithm *IVistaDriverForceFeedbackAspect::GetForceAlgorithm() const
{
    // is accessed frequently, but should almost always be a free lock
    // as the ''writer'' is probably not going to exchange the constraint
    // model (SetForceConstraintModel()) with a high frequency.
    VistaMutexLock l(*m_pModelLock);
	return m_pConstraintModel;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
IVistaDriverForceFeedbackAspect::IForceAlgorithm::IForceAlgorithm( unsigned int nStorageSize )
: m_pSensor( new VistaDeviceSensor ),
  m_pHistoryAspect( new VistaDriverMeasureHistoryAspect(~0) )
{
	m_pReadState = new VistaSensorReadState( m_pSensor, false );
	m_pHistoryAspect->RegisterSensor( m_pSensor );

	// ok, this is kindof a wild guess
	// at the time we can write 1 constraint plane, a driver may be allowed to
	// read 16 slots, this is an assumption based on:
	// write with 60Hz / read with 1000Hz; we give a paranoia factor of 2, just in case
	// that means: write every 16ms, in that time we can read 16 times (just one value)
	/** @todo think about this */
	m_pHistoryAspect->SetHistorySize( m_pSensor, 32, 1, nStorageSize );
}

IVistaDriverForceFeedbackAspect::IForceAlgorithm::~IForceAlgorithm()
{
	m_pHistoryAspect->UnregisterSensor( m_pSensor );
	delete m_pSensor;
	delete m_pReadState;
	delete m_pHistoryAspect;
}


bool IVistaDriverForceFeedbackAspect::IForceAlgorithm::Publish()
{
	// adjust read head
	return m_pReadState->SetToLatestSample();
}

bool IVistaDriverForceFeedbackAspect::IForceAlgorithm::UpdateForce( 
	double dTs, 
	const VistaTransformMatrix &m,
	const VistaVector3D &v3CurrentVelocity,
	VistaVector3D &v3ResultingForce,
	VistaVector3D &v3ResultingTorque )
{
	VistaVector3D v3Trans;
	m.GetTranslation(v3Trans);
 	VistaQuaternion q(m);
	return UpdateForce(dTs, v3Trans, v3CurrentVelocity, q, v3ResultingForce, v3ResultingTorque );
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

VistaPlaneConstraint::VistaPlaneConstraint()
: IVistaDriverForceFeedbackAspect::IForceAlgorithm( sizeof( _sPlane ) )
{

}

VistaPlaneConstraint::~VistaPlaneConstraint()
{

}
bool VistaPlaneConstraint::UpdateForce( double dTs, const VistaVector3D &v3CurrentPos,
		          const VistaVector3D &v3CurrentVelocity,
		          const VistaQuaternion &qCurrentOrientation,
		          VistaVector3D   &v3ResultingForce,
		          VistaVector3D &v3ResultingTorque)
{
	const VistaSensorMeasure *pM    = m_pReadState->GetCurrentMeasure();
	if(pM == NULL)
		return false; // no new constraint, no forces
	const _sPlane *pPlane = pM->getRead< _sPlane >();

	VistaPlane oPlane;
	oPlane.SetOrigin( VistaVector3D(pPlane->m_afPlaneOrigin ) );
	oPlane.SetNormVector( VistaVector3D(pPlane->m_afPlaneNormal ) );


	// calc distance to plane for current point
	float nDistance = oPlane.CalcDistance( v3CurrentPos );

	if( std::abs(nDistance) < 1.e-4f) // make a small tolerance around for points within the plane
	{
		// point on plane
		v3ResultingForce = oPlane.GetNormVector()
						 * pPlane->m_nStiffness
		                 + VistaVector3D( pPlane->m_afInternalForce );

		v3ResultingTorque = VistaVector3D(0,0,0,1);
		return true;
	}
	else if(nDistance > 0)
	{
		v3ResultingForce = VistaVector3D(0,0,0);
		v3ResultingTorque = VistaVector3D(0,0,0,1);
		return true;
	}
	else
	{
        v3ResultingForce = -(oPlane.GetNormVector() * pPlane->m_nStiffness
        		           * nDistance
						   + pPlane->m_nDamping * v3CurrentVelocity);
		v3ResultingTorque = VistaVector3D(0,0,0,1);
		return true;
	}
}


bool VistaPlaneConstraint::SetConstraintPlane(  const VistaVector3D &v3ToOrigin,
												 const VistaVector3D &v3Normal,
												 float nStiffness,
												 float nDamping,
												 const VistaVector3D &v3InternalForce )
{
	VistaType::microtime dTimeStamp = VistaTimeUtils::GetStandardTimer().GetMicroTime();

	// write the new plane to the driver using the sensor for that
	m_pHistoryAspect->MeasureStart( m_pSensor, dTimeStamp );
	// get the current place for the decoding for sensor 0
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(m_pSensor);
	if(pM == NULL)
	{
		m_pHistoryAspect->MeasureStop(m_pSensor); // mark stop
		return false; // can happen when there is a memory problem,
	                  // or when working on uninitialized history
	}

	// convert to a proper memory layout
	_sPlane *pPlane = pM->getWrite< _sPlane >();

	// the GetValues() reads from the vista vector and writes to the argument
	// that's why it is 4 values for the origin / normal / internal force... pure lazyness ;)
	v3ToOrigin.GetValues( pPlane->m_afPlaneOrigin );
    VistaVector3D v3NormalizedNormal = v3Normal.GetNormalized();
	v3NormalizedNormal.GetValues(pPlane->m_afPlaneNormal);
	v3InternalForce.GetValues(pPlane->m_afInternalForce);
	pPlane->m_nStiffness = nStiffness;
	pPlane->m_nDamping = nDamping;

	m_pHistoryAspect->MeasureStop(m_pSensor); // mark stop

	// ok, we are done, let's publish to the driver immediately!
	// this is ok, as with the ff-aspect and constraint model, we have a 1:1 reader / writer
	// so we do not need to wait for other conditions, besides... no one else can access the
	// driver / history and stuff (by design: this method is not reentrant)
	return Publish();
}

