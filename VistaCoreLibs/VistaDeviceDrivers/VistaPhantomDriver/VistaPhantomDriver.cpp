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


#include "VistaPhantomDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDeviceIdentificationAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaAspects/VistaReferenceCountable.h>

#include <HD/hd.h>
#include <HD/hdDevice.h>

#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaPhantomDriverProtocolAspect : public IVistaDriverProtocolAspect
{
public:
	VistaPhantomDriverProtocolAspect( VistaPhantomDriver *pDriver )
		: IVistaDriverProtocolAspect(),
		  m_pParent(pDriver)
	{
	}

	virtual bool SetProtocol( const _cVersionTag &oTag )
	{
		m_pParent->SetDeviceString( oTag.m_strProtocolName );
		return true;
	}
private:
	VistaPhantomDriver *m_pParent;
};
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

class _CSchedulerHlp : public IVistaReferenceCountable
{
public:
	_CSchedulerHlp()
	{
		hdStartScheduler();
	}

	~_CSchedulerHlp()
	{
		hdStopScheduler();
		m_pScheduler = NULL;
	}

	static _CSchedulerHlp *m_pScheduler;
};

_CSchedulerHlp *_CSchedulerHlp::m_pScheduler = NULL;

struct VistaPhantomDriver::_sPhantomPrivate
{
	_sPhantomPrivate()
		: m_hHD(~0),
		  m_effectID(0)
	{
	}

	HHD				m_hHD;   // haptic device
	std::string     m_strDeviceString;
	int				m_effectID;
};

namespace
{
	HDCallbackCode HDCALLBACK LoopCallback(void *data)
	{
		((VistaPhantomDriver*) data)->Update();
		//((VistaPhantomDriver*) data)->m_effect.CalcEffectForce();
		return HD_CALLBACK_CONTINUE;
	}
}

typedef TVistaDriverEnableAspect< VistaPhantomDriver > VistaPhantomDriverEnableAspect;

VistaPhantomDriver::VistaPhantomDriver(IVistaDriverCreationMethod *crm)
: IVistaDeviceDriver(crm),
  m_pWorkSpace( new VistaDriverWorkspaceAspect ),
  m_pInfo( new VistaDriverInfoAspect ),
  m_pIdentification( new VistaDeviceIdentificationAspect ),
  m_pProtocol( NULL ),
  m_pPrivate( new _sPhantomPrivate ),
  m_pForceFeedBack(NULL),
  m_bIsWarningOnHapticUpdateRateExceed( false )
{

	SetUpdateType(IVistaDeviceDriver::UPDATE_CUSTOM_THREADED);

	// create and register a sensor for the phantom
	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	pSensor->SetTypeHint( "" );

	AddDeviceSensor(pSensor);

	pSensor->SetMeasureTranscode( GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder() );

	m_pProtocol = new VistaPhantomDriverProtocolAspect(this);
	RegisterAspect( m_pProtocol );

	RegisterAspect( m_pWorkSpace );
	RegisterAspect( m_pInfo );
	RegisterAspect( m_pIdentification );

	RegisterAspect( new VistaPhantomDriverEnableAspect( this, &VistaPhantomDriver::PhysicalEnable ) );
}

VistaPhantomDriver::~VistaPhantomDriver()
{
	if(m_pPrivate->m_hHD != ~0u)
	{
		hdMakeCurrentDevice(m_pPrivate->m_hHD);
		if( _CSchedulerHlp::m_pScheduler )
			_CSchedulerHlp::refdown( _CSchedulerHlp::m_pScheduler );

		if(m_pPrivate->m_effectID != 0)
		{
			hdUnschedule(m_pPrivate->m_effectID);
			m_pPrivate->m_effectID = 0;
		}

		hdDisableDevice(m_pPrivate->m_hHD);
		m_pPrivate->m_hHD = ~0;
	}

	VistaDeviceSensor *pSensor = GetSensorByIndex(0);
	RemDeviceSensor(pSensor);
	IVistaMeasureTranscode *pTC = pSensor->GetMeasureTranscode();
	pSensor->SetMeasureTranscode(NULL);
	delete pSensor;
	delete pTC;

	UnregisterAspect( GetAspectById( VistaDriverWorkspaceAspect::GetAspectId() ) );
	//UnregisterAspect( GetAspectById( IVistaDriverProtocolAspect::GetAspectId() ) );
	UnregisterAspect( GetAspectById( VistaDriverInfoAspect::GetAspectId() ) );
	UnregisterAspect( GetAspectById( IVistaDriverForceFeedbackAspect::GetAspectId() ) );
	UnregisterAspect( GetAspectById( VistaDeviceIdentificationAspect::GetAspectId() ) );

	VistaPhantomDriverEnableAspect *enabler = GetAspectAs< VistaPhantomDriverEnableAspect >( VistaDriverEnableAspect::GetAspectId() );
	UnregisterAspect( enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete enabler;

	delete m_pPrivate;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaPhantomDriver::DoDisconnect()
{
	hdDisableDevice(m_pPrivate->m_hHD);
	m_pPrivate->m_hHD = ~0;
	return true;
}

bool VistaPhantomDriver::DoConnect()
{
	HDErrorInfo error;

	m_pPrivate->m_strDeviceString = m_pIdentification->GetDeviceName();

	m_pPrivate->m_hHD = hdInitDevice(m_pPrivate->m_strDeviceString.c_str());
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		std::cerr << "Failed to initialize haptic device. Error code: " << error.errorCode << std::endl;
		std::cerr << "[PhantomDriver]: Device string given: ["
			<< (m_pPrivate->m_strDeviceString.empty() ? "DEFAULT_DEVICE" : m_pPrivate->m_strDeviceString)
			<< std::string("]") << std::endl;
		m_pPrivate->m_hHD = ~0; // should be 0 anyways...?
		return false;
	}
	else
	{
		VistaPropertyList &oInfo = m_pInfo->GetInfoPropsWrite();

		hdMakeCurrentDevice(m_pPrivate->m_hHD);
		HDstring deviceVendor = hdGetString(HD_DEVICE_VENDOR);
		HDstring modelType = hdGetString(HD_DEVICE_MODEL_TYPE);
		HDstring driverVersion = hdGetString(HD_DEVICE_DRIVER_VERSION);
		HDstring serial = hdGetString(HD_DEVICE_SERIAL_NUMBER);

		HDdouble nFirmware = 0;
		hdGetDoublev( HD_DEVICE_FIRMWARE_VERSION, &nFirmware );

		oInfo.SetValue<double>( "FIRMWARE_REVISION", nFirmware );
		oInfo.SetValue( "VENDOR", std::string( (deviceVendor ? deviceVendor : "<none>") ) );
		oInfo.SetValue( "MODEL", std::string( (modelType ? modelType : "<none>") ) );
		oInfo.SetValue( "DRIVERVERSION", std::string( (driverVersion ? driverVersion : "<none>") ) );
		oInfo.SetValue( "SERIALNUMBER", std::string( (serial ? serial : "<none>") ) );

		HDint nCalStyle = 0;
		//HDint calibrationStyle = 0;

		hdGetIntegerv( HD_CALIBRATION_STYLE, &nCalStyle );

		std::string sCalStyle;
		if(nCalStyle & HD_CALIBRATION_AUTO)
			sCalStyle = "AUTO ";
		if(nCalStyle & HD_CALIBRATION_ENCODER_RESET)
			sCalStyle += "ENCODER_RESET ";
		if(nCalStyle & HD_CALIBRATION_INKWELL )
			sCalStyle += "INKWELL ";

		oInfo.SetValue("CALSTYLES", sCalStyle);
		oInfo.SetValue<int>( "CALFLAGS", nCalStyle );

		if(hdCheckCalibration()==HD_CALIBRATION_NEEDS_UPDATE)
		{
			hdUpdateCalibration(nCalStyle);
		}

		HDfloat nTableOffset = 0;
		hdGetFloatv( HD_TABLETOP_OFFSET, &nTableOffset );

		oInfo.SetValue<double>( "TABLETOP_OFFSET", nTableOffset );

		float maxStiffness, maxForce, maxDamping;

		hdGetFloatv(HD_NOMINAL_MAX_STIFFNESS, &maxStiffness);
		hdGetFloatv(HD_NOMINAL_MAX_FORCE, &maxForce);
        hdGetFloatv(HD_NOMINAL_MAX_DAMPING, &maxDamping);

		oInfo.SetValue( "MAXSTIFFNESS", maxStiffness );
		oInfo.SetValue( "MAXFORCE", maxForce );

		float maxWorkspace[6],
			  maxUsableWorkspace[6];

		hdGetFloatv(HD_MAX_WORKSPACE_DIMENSIONS, maxWorkspace);
		hdGetFloatv(HD_USABLE_WORKSPACE_DIMENSIONS, maxUsableWorkspace);

		m_pWorkSpace->SetWorkspace( "MAXWORKSPACE", VistaBoundingBox( &maxWorkspace[0], &maxWorkspace[3] ) );
		m_pWorkSpace->SetWorkspace( "USABLEWORKSPACE", VistaBoundingBox( &maxUsableWorkspace[0], &maxUsableWorkspace[3] ) );

		// register update function
		if(m_pPrivate->m_effectID == 0)
			m_pPrivate->m_effectID = hdScheduleAsynchronous( LoopCallback, this, HD_DEFAULT_SCHEDULER_PRIORITY);

		if (HD_DEVICE_ERROR(error = hdGetError()))
		{
			vstr::err () << "[VistaPhantomDriver] error code: " << error.errorCode << std::endl;
		}

		int nInputDOF, nOutputDOF;

		hdGetIntegerv(HD_INPUT_DOF,	 &nInputDOF);
		hdGetIntegerv(HD_OUTPUT_DOF, &nOutputDOF);

		if(nOutputDOF > 0)
		{
			m_pForceFeedBack = new VistaPhantomForceFeedbackAspect(this);
			m_pForceFeedBack->m_nInputDOF = nInputDOF;
			m_pForceFeedBack->m_nOutputDOF = nOutputDOF;
            m_pForceFeedBack->m_nMaxStiffness = maxStiffness;
            m_pForceFeedBack->m_nMaxForce = maxForce;
			RegisterAspect( m_pForceFeedBack );
		}

		return true;
	}
}

bool VistaPhantomDriver::DoSensorUpdate(VistaType::microtime nTs)
{
	VistaDeviceSensor *pSensor = GetSensorByIndex(0);

	MeasureStart(0, nTs);
	// get the current place for the decoding for sensor 0
	VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);

	// for the sake of readability: interpret the memory blob
	// as a place to store a sample for the Phantom
	VistaPhantomMeasures::sPhantomMeasure *s = reinterpret_cast<VistaPhantomMeasures::sPhantomMeasure*>(&( *pM ).m_vecMeasures[0]);

	hdBeginFrame( m_pPrivate->m_hHD );

	hdGetFloatv(HD_CURRENT_POSITION,           s->m_afPosition);
    hdGetFloatv(HD_CURRENT_VELOCITY,           s->m_afVelocity);
	hdGetFloatv(HD_CURRENT_ANGULAR_VELOCITY,   s->m_afAngularVelocity);
	hdGetFloatv(HD_CURRENT_TRANSFORM,          s->m_afRotMatrix);

#ifdef PHANTOM_USES_VISTA_UNITS
	for (int i =0; i<3;i++)
	{
		s->m_afPosition[i]*=0.001;
		s->m_afVelocity[i]*=0.001;
	};
#endif

	if(m_pForceFeedBack->GetForcesEnabled())
	{
		VistaVector3D v3Force;
		VistaVector3D v3Torque;

		IVistaDriverForceFeedbackAspect::IForceAlgorithm *pMd
            = m_pForceFeedBack->GetForceAlgorithm();
		if(pMd)
		{
			VistaVector3D pos( s->m_afPosition[0], s->m_afPosition[1], s->m_afPosition[2] );
            VistaVector3D vel( s->m_afVelocity[0], s->m_afVelocity[1], s->m_afVelocity[2] );
			//VistaQuaternion qAngVel(VistaVector3D(0,0,1),
			//		                 VistaVector3D(s->m_afAngularVelocity[0],
			//										s->m_afAngularVelocity[1],
			//										s->m_afAngularVelocity[2]) );
			VistaTransformMatrix t (
				float(s->m_afRotMatrix[0]), float(s->m_afRotMatrix[1]), float(s->m_afRotMatrix[2]), 0,
				float(s->m_afRotMatrix[4]), float(s->m_afRotMatrix[5]), float(s->m_afRotMatrix[6]), 0,
				float(s->m_afRotMatrix[8]), float(s->m_afRotMatrix[9]), float(s->m_afRotMatrix[10]), 0,
				0,                          0,                          0, 1);

			VistaQuaternion qRot = -VistaQuaternion(t);
			
			//mapping of the quaternion into the positive hemisphere
			//otherwise the quaternion will numerically jump in some situations which 
			//which can cause problems (e.g. when using a 6 dof virtual coupling)
			if (qRot[Vista::W] < 0.0)
			{
				qRot *= -1;
			}
			// calc update force from that information
			// @todo: think about the timestamps
			pMd->UpdateForce( double(nTs), pos, vel, qRot, v3Force, v3Torque );
		}
		else
		{
			v3Force = m_pForceFeedBack->m_v3Force;
			v3Torque = m_pForceFeedBack->m_v3AngularForce;
		}

		if(m_pForceFeedBack->m_nOutputDOF >= 3)
		{
			hdSetFloatv( HD_CURRENT_FORCE, &v3Force[0] );
		}

		if(m_pForceFeedBack->m_nOutputDOF == 6)
			hdSetFloatv( HD_CURRENT_TORQUE, &v3Torque[0] );
	}

	hdGetIntegerv(HD_CURRENT_BUTTONS,         &s->m_nButtonState);
	hdGetFloatv(HD_MOTOR_TEMPERATURE,          s->m_afOverheatState);
	hdGetFloatv(HD_INSTANTANEOUS_UPDATE_RATE, &s->m_nUpdateRate);
	hdGetFloatv(HD_CURRENT_JOINT_ANGLES,       s->m_afJointAngles);
	hdGetFloatv(HD_CURRENT_GIMBAL_ANGLES,      s->m_afGimbalAngles);
	hdGetLongv(HD_CURRENT_ENCODER_VALUES,      s->m_nEncoderValues);
	HDboolean bSwitch = 0;

	hdGetBooleanv(HD_CURRENT_INKWELL_SWITCH,  &bSwitch);
	s->m_bInkwellSwitch = bSwitch ? true : false;

	hdGetFloatv(HD_CURRENT_FORCE,              s->m_afForce);
	hdGetFloatv(HD_CURRENT_VELOCITY,           s->m_afVelocity);

	hdEndFrame( m_pPrivate->m_hHD );


 	if (m_bIsWarningOnHapticUpdateRateExceed && hdGetSchedulerTimeStamp() > .001)
 	{
 		std::cout << "Scheduler has exceeded 1ms.\n";
 	}

    HDErrorInfo error;
    /* Check if an error occurred while attempting to render the force */
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
		vstr::err () << "[VistaPhantomDriver] error code: " << error.errorCode << std::endl;
	}

	// we are done. Indicate that to the history
	MeasureStop(0);

	return true;
}

bool VistaPhantomDriver::PhysicalEnable(bool bEnable)
{
	if(bEnable)
	{
		hdMakeCurrentDevice(m_pPrivate->m_hHD);
		if(_CSchedulerHlp::m_pScheduler == NULL )
			_CSchedulerHlp::m_pScheduler = new _CSchedulerHlp;

		_CSchedulerHlp::refup( _CSchedulerHlp::m_pScheduler );

		return true;
	}
	else
	{
		if(m_pPrivate->m_hHD == ~0u)
			return true; // disabled and not initialized stays disabled
		hdMakeCurrentDevice(m_pPrivate->m_hHD);
		if(_CSchedulerHlp::m_pScheduler)
			_CSchedulerHlp::refdown( _CSchedulerHlp::m_pScheduler );

		return true;
	}
}

std::string VistaPhantomDriver::GetDeviceString() const
{
	return m_pPrivate->m_strDeviceString;
}

void VistaPhantomDriver::SetDeviceString(const std::string &strDevice)
{
	if(m_pPrivate->m_hHD == ~0u)
		m_pPrivate->m_strDeviceString = strDevice;
}

// #############################################################################

VistaPhantomDriver::VistaPhantomForceFeedbackAspect::VistaPhantomForceFeedbackAspect( VistaPhantomDriver *pDriver )
: IVistaDriverForceFeedbackAspect(),
  m_pParent(pDriver),
  m_nInputDOF(0),
  m_nOutputDOF(0),
  m_nMaxForce(0),
  m_nMaxStiffness(0)
{
}

VistaPhantomDriver::VistaPhantomForceFeedbackAspect::~VistaPhantomForceFeedbackAspect()
{
}

bool VistaPhantomDriver::VistaPhantomForceFeedbackAspect::SetForce( const VistaVector3D   & v3Force,
																	  const VistaVector3D & v3AngularForce)
{
	// check thread safety here!
	m_v3Force       = v3Force;
	m_v3AngularForce = v3AngularForce;
	return true;
}

bool VistaPhantomDriver::VistaPhantomForceFeedbackAspect::SetForcesEnabled(bool bEnabled)
{
	hdMakeCurrentDevice( m_pParent->m_pPrivate->m_hHD );
	if(bEnabled)
		hdEnable( HD_FORCE_OUTPUT );
	else
		hdDisable( HD_FORCE_OUTPUT );

	return (GetForcesEnabled() == bEnabled);
}

bool VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetForcesEnabled() const
{
	hdMakeCurrentDevice( m_pParent->m_pPrivate->m_hHD );
	return (hdIsEnabled(HD_FORCE_OUTPUT) ? true : false);
}

int VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetNumInputDOF() const
{
	return m_nInputDOF;
}

int VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetNumOutputDOF() const
{
	return m_nOutputDOF;
}

float VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetMaximumStiffness() const
{
    return m_nMaxStiffness;
}

float VistaPhantomDriver::VistaPhantomForceFeedbackAspect::GetMaximumForce() const
{
    return m_nMaxForce;
}