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


#ifndef __VISTACHAI3DHAPTICDEVICESDRIVER_H
#define __VISTACHAI3DHAPTICDEVICESDRIVER_H

// Windows DLL build
#if defined(WIN32) && !defined(VISTACHAI3DHAPTICDEVICESDRIVER_STATIC)
#ifdef VISTACHAI3DHAPTICDEVICESDRIVER_EXPORTS
#define VISTACHAI3DHAPTICDEVICESDRIVERAPI __declspec(dllexport)
#else
#define VISTACHAI3DHAPTICDEVICESDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTACHAI3DHAPTICDEVICESDRIVERAPI
#endif

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaCHAI3DHapticDevicesCommonShare.h"

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverForceFeedbackAspect.h>
#include <VistaBase/VistaVectorMath.h>

//CRM
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverWorkspaceAspect;
class IVistaDriverProtocolAspect;
class VistaDriverInfoAspect;
class VistaDeviceIdentificationAspect;
class VistaThreadLoop;

class cHapticDeviceHandler;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is a driver to use the CHAI3D Toolkit to support various haptic devices, i.e. a device simulator.
 * Latter should be started automatically if no other device is pluged into the computer (if not it can be started by hand .
 * The driver uses a VistaThreadLoop for a cyclic pull of data from the CHAI3D which
 * is then pushed into the history. As this driver was developed to be usable in exchange with the PhantomDriver
 * the interface is adapted to it. Therefore the data in the deviceMeasure is sometimes not filled with data from the device, as CHAI3D does not deliver them
 * Attention! The driver is only tested under Windows 32 bit, with the "VirtualDevice", Phantom Omni, Phantom Premium 1.5 6DOF, and Falcon.
 * It is possible to use it with multiple devices, e.g., virtual and falcon. But up to now it is not possible to use two Phantom Devices via this driver
 * in this case you have to use the phantom driver additionally.
 * Further information about setting up CHAI3D could be found in the drivers README.
 */
class VISTACHAI3DHAPTICDEVICESDRIVERAPI VistaCHAI3DHapticDevicesDriver : public IVistaDeviceDriver
{
	friend class VistaCHAI3DHapticDevicesForceFeedbackAspect;
public:

	VistaCHAI3DHapticDevicesDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaCHAI3DHapticDevicesDriver();

	/**
	 * Overloaded from IVistaDeviceDriver.
	 * Tries to open a CHAI3D device and get device information
	 */
	bool Connect();

	class VISTACHAI3DHAPTICDEVICESDRIVERAPI VistaCHAI3DHapticDevicesForceFeedbackAspect : public IVistaDriverForceFeedbackAspect
	{
		friend class VistaCHAI3DHapticDevicesDriver;
	public:
		virtual bool SetForce( const VistaVector3D   & force,
							   const VistaVector3D &angularForce );

		virtual bool SetForcesEnabled(bool bEnabled);
		virtual bool GetForcesEnabled() const;

		virtual int GetNumInputDOF() const;
		virtual int GetNumOutputDOF() const;

        virtual float GetMaximumStiffness() const;
        virtual float GetMaximumForce() const;

	private:
		VistaCHAI3DHapticDevicesForceFeedbackAspect( VistaCHAI3DHapticDevicesDriver *pDriver );
		virtual ~VistaCHAI3DHapticDevicesForceFeedbackAspect();
		VistaCHAI3DHapticDevicesDriver *m_pParent;

		int m_nInputDOF,
			m_nOutputDOF;
        float m_nMaxForce,
              m_nMaxStiffness;

		VistaVector3D   m_v3Force;
		VistaVector3D   m_v3AngularForce;
		bool			m_bEnabled;
	};

	std::string GetDeviceString() const;
	void SetDeviceString(const std::string &strDevice);
protected:
	bool         PhysicalEnable(bool bEnable);
	virtual bool DoSensorUpdate(VistaType::microtime dTs);
	/**
	 * Overloaded from IVistaDeviceDriver.
	 * Tries to open a CHAI3D device and get device information
	 */
	bool DoConnect();
	bool DoDisconnect();
private:

	//As CHAI3D does not deliver workspace information this is in here only for compatibility reason
	VistaDriverWorkspaceAspect *m_pWorkSpace;
	IVistaDriverProtocolAspect  *m_pProtocol;
	VistaDriverInfoAspect      *m_pInfo;
	VistaDeviceIdentificationAspect *m_pIdentification;
	VistaCHAI3DHapticDevicesForceFeedbackAspect *m_pForceFeedBack;

	// this thread triggers the update function of the driver
	VistaThreadLoop*	m_pUpdateLoop;

	struct _sCHAI3DHapticDevicesPrivate;
	_sCHAI3DHapticDevicesPrivate *m_pHapticDevicePrivate;
};

class VISTACHAI3DHAPTICDEVICESDRIVERAPI VistaCHAI3DHapticDevicesDriverFactory : public IVistaDriverCreationMethod
{
public:

	VistaCHAI3DHapticDevicesDriverFactory(IVistaTranscoderFactoryFactory *metaFac)
		: IVistaDriverCreationMethod(metaFac)
	{
		RegisterSensorType( "", sizeof(VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure), 1000,
			metaFac->CreateFactoryForType("VistaCHAI3DHapticDevicesDriverMeasureTranscode"));
	}

	virtual IVistaDeviceDriver *CreateDriver()
	{
		return new VistaCHAI3DHapticDevicesDriver(this);
	}
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

// a CHAI3D haptic device handler. it is defined globally as in case we initialize
// the driver twice only one handler is created and later reused as otherwise CHAI3D will crash!

cHapticDeviceHandler* m_pHapticDeviceHandler( NULL );
#endif //__VISTACHAI3DHapticDevicesDRIVER_H