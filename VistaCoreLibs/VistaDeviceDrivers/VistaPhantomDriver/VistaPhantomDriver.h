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


#ifndef __VISTAPHANTOMDRIVER_H
#define __VISTAPHANTOMDRIVER_H

// Windows DLL build
#if defined(WIN32) && !defined(VISTAPHANTOMDRIVER_STATIC) 
#ifdef VISTAPHANTOMDRIVER_EXPORTS
#define VISTAPHANTOMDRIVERAPI __declspec(dllexport)
#else
#define VISTAPHANTOMDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAPHANTOMDRIVERAPI
#endif

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaPhantomCommonShare.h"

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
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a driver for the Phantom haptic device. It uses the OpenHaptics library
 * and supports multiple phantoms on the same machine. The driver collects
 * data at the full data rate and offers advanced force feedback using the
 * force feedback API and LLDx.
 */
class VISTAPHANTOMDRIVERAPI VistaPhantomDriver : public IVistaDeviceDriver
{
	friend class VistaPhantomForceFeedbackAspect;
public:

	VistaPhantomDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaPhantomDriver();

	class VISTAPHANTOMDRIVERAPI VistaPhantomForceFeedbackAspect : public IVistaDriverForceFeedbackAspect
	{
		friend class VistaPhantomDriver;
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
		VistaPhantomForceFeedbackAspect( VistaPhantomDriver *pDriver );
		virtual ~VistaPhantomForceFeedbackAspect();
		VistaPhantomDriver *m_pParent;

		int m_nInputDOF,
			m_nOutputDOF;
        float m_nMaxForce,
              m_nMaxStiffness;

		VistaVector3D   m_v3Force;
		VistaVector3D   m_v3AngularForce;
	};

	std::string GetDeviceString() const;
	void SetDeviceString(const std::string &strDevice);
	bool GetIsWarningOnHapticUpdateRateExceed() const { return m_bIsWarningOnHapticUpdateRateExceed; }
	void SetIsWarningOnHapticUpdateRateExceed(bool val) { m_bIsWarningOnHapticUpdateRateExceed = val; }
protected:
	/**
	 * Overloaded from IVistaDeviceDriver.
	 * Tries to open the phantom device that was specified in the ProtocolAspect's name
	 * field. Revision is ignored.
	 */
	bool DoConnect();
	bool DoDisconnect();
	bool         PhysicalEnable(bool bEnable);
	virtual bool DoSensorUpdate(VistaType::microtime dTs);
private:
	VistaDriverWorkspaceAspect *m_pWorkSpace;
	IVistaDriverProtocolAspect  *m_pProtocol;
	VistaDriverInfoAspect      *m_pInfo;
	VistaDeviceIdentificationAspect *m_pIdentification;
	VistaPhantomForceFeedbackAspect *m_pForceFeedBack;
	bool m_bIsWarningOnHapticUpdateRateExceed;
	

	struct _sPhantomPrivate;
	_sPhantomPrivate *m_pPrivate;
};

class VISTAPHANTOMDRIVERAPI VistaPhantomDriverFactory : public IVistaDriverCreationMethod
{
public:

	VistaPhantomDriverFactory(IVistaTranscoderFactoryFactory *metaFac)
		: IVistaDriverCreationMethod(metaFac)
	{
		RegisterSensorType( "", sizeof(VistaPhantomMeasures::sPhantomMeasure), 1000,
			metaFac->CreateFactoryForType("VistaPhantomDriverMeasureTranscode"));
	}

	virtual IVistaDeviceDriver *CreateDriver()
	{
		return new VistaPhantomDriver(this);
	}
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //__VISTAPHANTOMDRIVER_H

