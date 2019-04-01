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


#ifndef _VISTALEAPMOTIONDRIVER_H
#define _VISTALEAPMOTIONDRIVER_H



/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include "VistaLeapMotionCommonShare.h"

//Creation Method
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
#include <vector>
#include <map>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
//Windows DLL build
#if defined(WIN32) && !defined(VISTALEAPMOTIONDRIVER_STATIC) 
	#ifdef VISTALEAPMOTIONDRIVER_EXPORTS
		#define VISTALEAPMOTIONDRIVERAPI __declspec(dllexport)
	#else
		#define VISTALEAPMOTIONDRIVERAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTALEAPMOTIONDRIVERAPI
#endif
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverSensorMappingAspect;
class VistaDriverGenericParameterAspect;
class VistaDriverThreadAspect;

namespace Leap
{
	class Controller;
	class Gesture;
	class Frame;
}
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTALEAPMOTIONDRIVERAPI VistaLeapMotionDriver : public IVistaDeviceDriver
{
public:
	class LeapListener;

	VistaLeapMotionDriver( IVistaDriverCreationMethod* pCreationMethod );
	virtual ~VistaLeapMotionDriver();

	class Parameters : public VistaDriverGenericParameterAspect::IParameterContainer
	{
		REFL_DECLARE
	public:
		Parameters( VistaLeapMotionDriver* pDriver );

		enum 
		{
			MSG_POLICY_FLAGS_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
			MSG_GESTURE_FLAGS_CHG,
			MSG_CONNECTION_TIMEOUT_CHG,
			MSG_CONNECTION_STATE_CHG,
			MSG_LAST
		};


		enum GestureFlags
		{
			NO_GESTURE_ENABLED = 0,
			CIRCLE_ENABLED     = ( 1 << 0 ),
			SWIPE_ENABLED      = ( 1 << 1 ),
			KEY_TAP_ENABLED    = ( 1 << 2 ),
			SCREEN_TAP_ENABLED = ( 1 << 3 ),
			ALL_ENABLED        = CIRCLE_ENABLED | SWIPE_ENABLED | KEY_TAP_ENABLED | SCREEN_TAP_ENABLED,
		};

		GestureFlags GetGestureFlags() const;
		bool SetGestureFlags( GestureFlags flags );

		enum PolicyFlags
		{
			POLICY_DEFAULT = 0,
	        POLICY_BACKGROUND_FRAMES = (1 << 0),
            POLICY_IMAGES = (1 << 1),
	        POLICY_OPTIMIZE_HMD = (1 << 2)
		};

		PolicyFlags GetPolicyFlags() const;
		bool SetPolicyFlags( PolicyFlags flags );


		/*
		* Gesture.Circle.MinRadius              float      5.0           mm
		* Gesture.Circle.MinArc                 float      1.5 * pi      radians
		* Gesture.Swipe.MinLength               float      150           mm
		* Gesture.Swipe.MinVelocity             float      1000          mm/s
		* Gesture.KeyTap.MinDownVelocity        float      50            mm/s
		* Gesture.KeyTap.HistorySeconds         float      0.1           s
		* Gesture.KeyTap.MinDistance            float      3.0           mm
		* Gesture.ScreenTap.MinForwardVelocity  float      50            mm/s
		* Gesture.ScreenTap.HistorySeconds      float      0.1           s
		* Gesture.ScreenTap.MinDistance         float      5.0           mm
		*/

		float GetGestureProp( const std::string &prop_name ) const;
		bool  SetGestureProp( const std::string &prop_name, float value );

		VistaType::microtime GetConnectionTimeout() const;
		bool SetConnectionTimeout( VistaType::microtime timout_in_s );

		
		bool GetListenerConnectionState() const;

	private:
		friend class LeapListener;
		friend class VistaLeapMotionDriver;

		void ApplyGestureConfiguration();
		void ApplyPolicyFlags();

		bool SetListenerConnectionState( bool connection_state );

		VistaLeapMotionDriver* m_parent;

		GestureFlags m_gesture_flags;
		PolicyFlags  m_policy_flags;

		VistaType::microtime m_connection_timeout;

		std::map< std::string, float > m_param_cache;

		bool m_listener_connection_state;
	};


	Parameters *GetParameters() const;

	Leap::Controller *GetLeapController() const;
protected:
	virtual bool DoSensorUpdate( VistaType::microtime dTs );
	virtual bool PhysicalEnable( bool bEnable );

	virtual bool DoConnect();
	virtual bool DoDisconnect();

	void ProcessFrame( const Leap::Frame& oFrame, VistaType::microtime dTs );
	void ProcessGesture( const int nSensorTypeId, const Leap::Gesture& oGesture, const VistaType::microtime nTime, VistaType::uint64 driver_time_stamp );

private:
	VistaDriverSensorMappingAspect*		m_pSensors;	
	VistaDriverGenericParameterAspect*  m_pConfigAspect;
	VistaDriverThreadAspect*            m_pThreadAspect;


	LeapListener*     m_pListener;
	Leap::Controller* m_pLeapController;

	bool m_bListenerIsRegistered;

	unsigned int m_nHandsSensorId;
	unsigned int m_nFingerSensorId;
	unsigned int m_nToolsSensorId;
	unsigned int m_nTapGestureSensorId;
	unsigned int m_nGestureSensorId;
	unsigned int m_nCircleGestureSensorId;
	unsigned int m_nScreenTapGestureSensorId;
	unsigned int m_nKeyTapGestureSensorId;
	unsigned int m_nSwipeGestureSensorId;	
	unsigned int m_nImageSensorId;

	VistaType::uint64 m_nLastFrameId;

	Parameters* m_pParameters;

	VistaMutex m_oConnectionGuardMutex;	
};


//CREATION METHOD

class VISTALEAPMOTIONDRIVERAPI VistaLeapMotionCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaLeapMotionCreationMethod(IVistaTranscoderFactoryFactory *metaFac);
	virtual IVistaDeviceDriver *CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTALEAPMOTIONDRIVER_H
