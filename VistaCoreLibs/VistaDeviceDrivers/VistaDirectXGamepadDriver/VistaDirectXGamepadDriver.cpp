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


// include header here

#if defined(WIN32)

#include "VistaDirectXGamepadDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <iostream>
#include <algorithm>



namespace
{
	class GamepadProtocolAspect : public IVistaDriverProtocolAspect
	{
	public:
		GamepadProtocolAspect( VistaDirectXGamepad *pParent )
			: m_pParent(pParent)
		{
		}

		virtual bool SetProtocol( const _cVersionTag &oTag )
		{
			if(oTag.m_strProtocolName == "DIRECTX")
			{
				m_pParent->SetJoystickName( oTag.m_strProtocolRevision );
				return true;
			}
			return false;
		}

	private:
		VistaDirectXGamepad *m_pParent;
	};
}



class GamepadAttach : public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence
{
public:

	GamepadAttach(VistaDirectXGamepad *pParent)
		: m_nHwnd(0),
		  m_pParent(pParent)
	{
	}

	virtual bool AttachSequence( VistaDriverAbstractWindowAspect::IWindowHandle* oHandle)
	{
		if(m_nHwnd != NULL)
			return false;

		VistaDriverAbstractWindowAspect::NativeWindowHandle *oWindow 
			= dynamic_cast<VistaDriverAbstractWindowAspect::NativeWindowHandle *>( oHandle );
		if( oWindow == 0 )
			return false;

		// unfortunately, there seems to be now way to
		// retrieve the window id in a reliable way from ViSTA
		// as GLUT, for example, does not allow to give out
		// the window handle "as-raw" handle.
		// so we have to lookup the "global-active-top-level" window
		// from windows itself.
		m_nHwnd = oWindow->GetOSHandle();

		if(m_nHwnd != NULL)
		{
			m_liWindows.push_back(oWindow);
			return true;
		}
		return false;
	}

	virtual bool DetachSequence( VistaDriverAbstractWindowAspect::IWindowHandle* oHandle)
	{
		if(m_nHwnd == NULL)
			return true;
		VistaDriverAbstractWindowAspect::NativeWindowHandle *oWindow 
			= dynamic_cast<VistaDriverAbstractWindowAspect::NativeWindowHandle *>( oHandle );
		
		if( oWindow == 0 )
			return false;
		
		m_liWindows.remove( oWindow );
		m_nHwnd = NULL;
		return true;
	}

	virtual std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> GetWindowList() const
	{
		return m_liWindows;
	}


	HWND m_nHwnd;
	std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> m_liWindows;
	VistaDirectXGamepad *m_pParent;
};

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/



/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDirectXGamepad::VistaDirectXGamepad(IVistaDriverCreationMethod *crm)
: IVistaDeviceDriver(crm),
  m_eDriver(TP_UNKNOWN),
  m_pJoystick(NULL),
  m_nNumberOfButtons(0),
  m_pForceAspect(NULL),
  m_pWindowAspect(NULL),
  m_pProtocol(NULL)
{
	SetUpdateType( IVistaDeviceDriver::UPDATE_EXPLICIT_POLL );
	DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION,
							 IID_IDirectInput8, (VOID**)&m_pDI, NULL );

	m_pProtocol = new GamepadProtocolAspect(this);
	m_pProtocol->RegisterProtocol( IVistaDriverProtocolAspect::_cVersionTag( "DIRECTX", "" ) );
	m_pProtocol->RegisterProtocol( IVistaDriverProtocolAspect::_cVersionTag( "XINPUT", "" ) );

	memset(&m_oldState, 0, sizeof(DIJOYSTATE2));
}

VistaDirectXGamepad::~VistaDirectXGamepad()
{
	
	VistaDeviceSensor *pSen = GetSensorByIndex(0);
	if( pSen )
	{


		IVistaMeasureTranscode *pTr = pSen->GetMeasureTranscode();

		pSen->SetMeasureTranscode( NULL );
		GetFactory()->GetTranscoderFactoryForSensor("")->DestroyTranscoder(pTr);

		RemDeviceSensor(pSen);
	}
	

	if(m_pProtocol)
	{
		UnregisterAspect(m_pProtocol, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
		delete m_pProtocol;
	}

	if(m_pWindowAspect)
	{
		UnregisterAspect(m_pWindowAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
		delete m_pWindowAspect;
	}

	if(m_pForceAspect)
		UnregisterAspect(m_pForceAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pForceAspect;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaDirectXGamepad::DoConnect()
{
	if(InitInput())
	{
		// create and attach a single sensor
		VistaDeviceSensor *pSensor = new VistaDeviceSensor;
		//pSensor->SetTypeHint( VistaDirectXGamepad::GetDriverFactoryMethod()->GetTypeFor("") );
		pSensor->SetTypeHint("");
		pSensor->SetMeasureTranscode( GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder() );


		// this call will allocate the default number of history slots
		// (hopefully set for by the user)
		AddDeviceSensor(pSensor);

		// set the correct button number in the transcoder
		IVistaMeasureTranscode *pTrans = pSensor->GetMeasureTranscode();

		pTrans->SetNumberOfScalars( 7 + m_nNumberOfButtons );
		
        return true;
	}
    return false;
}

bool VistaDirectXGamepad::DoDisconnect()
{
	m_pJoystick->Release();
	m_pDI->Release();

	m_pJoystick = 0;
	m_pDI = 0;

	return true;
}

unsigned int VistaDirectXGamepad::GetSensorMeasureSize() const
{
	return sizeof(VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure);
}


bool VistaDirectXGamepad::DoSensorUpdate(VistaType::microtime dTs)
{
	if(m_pJoystick)
	{
		return UpdateDirectXInput(dTs);
	}
	return false;
}

bool VistaDirectXGamepad::UpdateDirectXInput(VistaType::microtime dTs)
{
	HRESULT hr = 0;

	// Poll the device to read the current state
	hr = m_pJoystick->Poll();

	// DI_NOEFFECT can happen, as we do not always have a force-feedback
	// state without which we do not really need a poll().
	if(hr != S_OK && (hr!=DI_NOEFFECT))
	{
		switch(hr)
		{
		case DIERR_INPUTLOST:
			{
				std::cout << "DIERR_INPUTLOST\n";
				break;
			}
		case DIERR_NOTACQUIRED:
			{
				std::cout << "DIERR_NOTACQUIRED\n";
				break;
			}
		case DIERR_NOTINITIALIZED:
			{
				std::cout << "DIERR_NOTINITIALIZED\n";
				break;
			}
		default:
			std::cout << "ERROR?\n";
			break;
		}
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = m_pJoystick->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = m_pJoystick->Acquire();

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of
		// switching, so just try again later
	   return false;
	}

	// the rational for the update routine is as follows:

	// we claim a new state from the device
	// then we will compare the state with the last one and see iff
	// there is a difference (using memcmp). Iff not, we will skip this
	// update and return false (no update happened), else we will
	// copy the newstate as a new compare state (m_oldState)
	// AND we will copy the new state to the measure history!
	// the latter is necessary as we do not know how long the history is
	// and we will not affect old readings / writings
	DIJOYSTATE2 newjs;
	// Get the input's device state
	hr = m_pJoystick->GetDeviceState( sizeof(DIJOYSTATE2), &newjs );
	if( hr != S_OK )
	{
		std::cout<<"\nGetDeviceState failed!\n";
		memset(&newjs, 0, sizeof(DIJOYSTATE2));
	}
	else
	{
		if( memcmp( &m_oldState, &newjs, sizeof(DIJOYSTATE2) ) != 0 )
		{
			// copy over old block
			memcpy( &m_oldState, &newjs, sizeof(DIJOYSTATE2) );

			// pass to buffer

			// pre-cache the sensor instance to update
			VistaDeviceSensor *pSen = GetSensorByIndex(0);
			MeasureStart(0, dTs);
			VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSen);
					
			VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure *p = (VistaDirectXGamepadSensorMeasures::VistaDirectXGamepadSensorMeasure*)&(*pM).m_vecMeasures[0];
			VistaDirectXGamepadSensorMeasures::VISTADIRECTXJOYSTATE2 pDIJoy = p->m_pDirectXJoystate2;
			memcpy( &pDIJoy, &newjs, sizeof(DIJOYSTATE2) );
			p->m_iButtonCount = m_nNumberOfButtons;
			p->m_pDirectXJoystate2 = pDIJoy;
			MeasureStop(0); // advance write cursor

			return true;
		}
	}
	return false;
}

struct DI_ENUM_CONTEXT
{
	DI_ENUM_CONTEXT()
		: pPreferredJoyCfg(NULL),
		m_pJoystick(NULL),
		bPreferredJoyCfgValid(false),
		m_pDI(NULL),
		m_nCount(0)
	{}


	DIJOYCONFIG*         pPreferredJoyCfg;
	LPDIRECTINPUTDEVICE8 m_pJoystick;
	bool                 bPreferredJoyCfgValid;
	bool                 bFilterOutXinputDevices;
	LPDIRECTINPUT8       m_pDI;
	int                  m_nCount;
	std::string          m_strJoyName;
};


static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
										   VOID* pContext )
{
	DI_ENUM_CONTEXT* pEnumContext = (DI_ENUM_CONTEXT*) pContext;
	HRESULT hr;

	pEnumContext->m_nCount++; // increase joystick count

	// Skip anything other than the perferred Joystick device as defined by the control panel.
	// Instead you could store all the enumerated Joysticks and let the user pick.
	if( pEnumContext->bPreferredJoyCfgValid &&
		!IsEqualGUID( pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance ) )
		return DIENUM_CONTINUE;

	// Obtain an interface to the enumerated Joystick.
	hr = pEnumContext->m_pDI->CreateDevice( pdidInstance->guidInstance,
		&pEnumContext->m_pJoystick, NULL );

	// query joy name!
	DIDEVICEINSTANCE DeviceInfo;
	DeviceInfo.dwSize=sizeof(DIDEVICEINSTANCE);
	hr = pEnumContext->m_pJoystick->GetDeviceInfo(&DeviceInfo);
	if(hr != S_OK)
		return DIENUM_CONTINUE; // can not use this joystick!

	pEnumContext->m_strJoyName = DeviceInfo.tszInstanceName;


	// If it failed, then we can't use this Joystick. (Maybe the user unplugged
	// it while we were in the middle of enumerating it.)
	if( FAILED(hr) )
		return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

static BOOL CALLBACK FindJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
										   VOID* pContext )
{
	DI_ENUM_CONTEXT* pEnumContext = (DI_ENUM_CONTEXT*) pContext;
	HRESULT hr;

	pEnumContext->m_nCount++; // increase joystick count

	// Skip anything other than the perferred Joystick device as defined by the control panel.
	// Instead you could store all the enumerated Joysticks and let the user pick.
	if( pEnumContext->bPreferredJoyCfgValid &&
		!IsEqualGUID( pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance ) )
		return DIENUM_CONTINUE;

	if(!pEnumContext->m_strJoyName.empty())
	{
		DIDEVICEINSTANCE DeviceInfo;
		DeviceInfo.dwSize=sizeof(DIDEVICEINSTANCE);
		hr = pEnumContext->m_pJoystick->GetDeviceInfo(&DeviceInfo);
		if(hr != S_OK)
			return DIENUM_CONTINUE;
		if( pEnumContext->m_strJoyName == std::string(DeviceInfo.tszInstanceName) )
			hr = pEnumContext->m_pDI->CreateDevice( pdidInstance->guidInstance,
			&pEnumContext->m_pJoystick, NULL );

		// Obtain an interface to the enumerated Joystick.

		// If it failed, then we can't use this Joystick. (Maybe the user unplugged
		// it while we were in the middle of enumerating it.)
		if( FAILED(hr) )
			return DIENUM_CONTINUE;
	}

	return DIENUM_STOP;
}

//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a
//       joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------

struct _sEnumJoystickContext
{
	_sEnumJoystickContext(LPDIRECTINPUTDEVICE8 pJoystick,
		int nAxesRangeMin = -1000,
		int nAxesRangeMax = +1000)
		: m_nBtNum(0),
		m_nAxesRangeMin(nAxesRangeMin),
		m_nAxesRangeMax(nAxesRangeMax),
		m_pJoystick(pJoystick),
		m_nForceFeedbackAxes(0)
	{}

	int m_nBtNum,
		m_nAxesRangeMin,
		m_nAxesRangeMax,
		m_nForceFeedbackAxes;

	LPDIRECTINPUTDEVICE8 m_pJoystick;
};

BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
								  VOID* pContext )
{
	_sEnumJoystickContext *ctx=(_sEnumJoystickContext*)pContext;

	// For axes that are returned, set the DIPROP_RANGE property for the
	// enumerated axis in order to scale min/max values.
	if( pdidoi->dwType & DIDFT_AXIS )
	{
		DIPROPRANGE diprg;
		diprg.diph.dwSize       = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow        = DIPH_BYID;
		diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
		diprg.lMin              = ctx->m_nAxesRangeMin;
		diprg.lMax              = ctx->m_nAxesRangeMax;
		if( (pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0 )
			ctx->m_nForceFeedbackAxes++;

		// Set the range for the axis
		if( FAILED( ctx->m_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
			return DIENUM_STOP;


	}

	if( pdidoi->dwType & DIDFT_BUTTON )
	{
		ctx->m_nBtNum++;
	}

	return DIENUM_CONTINUE;
}

bool VistaDirectXGamepad::InitInput()
{
	switch(m_eDriver)
	{
	case TP_DIRECTX:
		{
			return InitDirectXInput();
		}
	case TP_UNKNOWN:
		{
			if(InitDirectXInput())
				return true;
			else
			{
				// try to init xinput
				if(InitXInput())
					return true;
			}
		}
	case TP_XINPUT:
		{
			return InitXInput();
		}
	default:
		break;
	}

	return false;
}


bool VistaDirectXGamepad::InitDirectXInput()
{

	// get window handle from touch sequence
	GamepadAttach *pAttach = dynamic_cast<GamepadAttach*>(m_pWindowAspect->GetTouchSequence());
	if(pAttach == NULL || pAttach->m_nHwnd == 0L)
		return false;


	DIJOYCONFIG PreferredJoyCfg = {0};
	DI_ENUM_CONTEXT enumContext;
	memset(&enumContext, 0, sizeof(DI_ENUM_CONTEXT));

	enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
	enumContext.bPreferredJoyCfgValid = false;
	enumContext.m_pDI = m_pDI;
	enumContext.m_pJoystick = m_pJoystick;

	IDirectInputJoyConfig8* pJoyConfig = NULL;
	HRESULT hr;

	// claim a joystick config structure
	hr = m_pDI->QueryInterface( IID_IDirectInputJoyConfig8, (void **) &pJoyConfig );

	// fill the config structure with information from the
	// windows system database
	PreferredJoyCfg.dwSize = sizeof(PreferredJoyCfg);

	// simply try to claim config for joystick 0
	// This function is expected to fail if no joystick is attached
	// as index 0 is not present
	if( SUCCEEDED( pJoyConfig->GetConfig(0, &PreferredJoyCfg, DIJC_GUIDINSTANCE ) ) )
		enumContext.bPreferredJoyCfgValid = true;

	// give away this structure
	if(pJoyConfig)
		pJoyConfig->Release();

	// Look for a  joystick
	if(m_strJoyName.empty())
	{
		hr = m_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL,
											 EnumJoysticksCallback, &enumContext,
											 DIEDFL_ATTACHEDONLY );
	}
	else
	{
		enumContext.m_strJoyName = m_strJoyName;
		enumContext.bPreferredJoyCfgValid = false;

		hr = m_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL,
											 FindJoysticksCallback, &enumContext,
											 DIEDFL_ATTACHEDONLY );
	}

	if( hr == S_OK  && enumContext.m_pJoystick )
	{
		// copy the joystick allocation routine to
		// our member pointer
		m_pJoystick = enumContext.m_pJoystick;

		DIDEVICEINSTANCE DeviceInfo;
		DeviceInfo.dwSize=sizeof(DIDEVICEINSTANCE);
		if( FAILED( hr = m_pJoystick->GetDeviceInfo(&DeviceInfo))){
			std::cerr<<"GetDeviceInfo  failed!\n";
			return false;
		}

		std::cout << "VistaDirectXGamepad::InitDirextX() -- found ["
			<< DeviceInfo.tszInstanceName << "] controller\n";

		// set the desired data-format
		// c_dfDIJoystick2 is a global enum from directX
		hr = m_pJoystick->SetDataFormat( &c_dfDIJoystick2 );
		if(hr != S_OK )
		{
			std::cerr << "Could not set DataFormat for Joystick.\n";
		}
		// Set the cooperative level to let DInput know how this device should
		// interact with the system and with other DInput applications.

		hr = m_pJoystick->SetCooperativeLevel( pAttach->m_nHwnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND );
		if( hr != S_OK )
		{
			std::cerr << "Could not set cooperative level\n";
			return false;
		}

		// normalize axes and determine number of buttons
		_sEnumJoystickContext s(m_pJoystick);
		hr = m_pJoystick->EnumObjects( EnumObjectsCallback,
									  (VOID*)&s , DIDFT_AXIS | DIDFT_BUTTON );
		if( hr != S_OK )
			std::cout<<"\nEnumObjects  failed (no axis or buttons?)!";

		m_nNumberOfButtons = s.m_nBtNum;
		std::cout << "This controller has [" << s.m_nBtNum << "] buttons.\n";

		DIPROPDWORD  dipdw;
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		dipdw.dwData            = DIPROPAXISMODE_ABS;  //DIPROPAXISMODE_ABS or DIPROPAXISMODE_REL
		hr = m_pJoystick->SetProperty(DIPROP_AXISMODE , &dipdw.diph);
		if(hr != S_OK)
			std::cerr << "Could not set axis mode\n";

		dipdw.dwData = 100; // = 1%
		hr = m_pJoystick->SetProperty(DIPROP_DEADZONE   , &dipdw.diph);
		if(hr != S_OK)
			std::cerr << "Could not set DEADZONE\n";

		dipdw.dwData = 10000; // = 100%
		hr = m_pJoystick->SetProperty(DIPROP_SATURATION    , &dipdw.diph);
		if(hr != S_OK)
			std::cerr << "Could not set SATURATION\n";

		hr = m_pJoystick->Acquire();
		if( hr != S_OK )
			std::cerr << "Could not acquire initially!\n";

		m_eDriver = TP_DIRECTX;


		// try to figure out whether this driver has a force feedback support or not
		if(s.m_nForceFeedbackAxes != 0)
		{
			// yes!
			VistaDirectXForceFeedbackAspect *pForce = new VistaDirectXForceFeedbackAspect(this);

			// configure force aspect
			// assign and register as aspect
			m_pForceAspect = pForce;
			RegisterAspect( m_pForceAspect );
		}
		return true;
	}
	else
	{
		std::cerr << "VistaDirectXGamepad::InitDirectXInput() "
				  << "-- FAILED to enumerate a joystick"
				  << std::endl;
		if(enumContext.m_nCount == 0)
			std::cerr << "VistaDirectXGamepad::InitDirectXInput() -- "
					  << "Maybe no joystick attached?\n";
	}

	return false;
}

unsigned int VistaDirectXGamepad::GetNumberOfButtons() const
{
	return m_nNumberOfButtons;
}

bool VistaDirectXGamepad::GetRanges( int &xMin, int &yMin, int &zMin,
				int &xMax, int &yMax, int &zMax) const
{
	if(m_pJoystick == NULL)
		return false;

	xMin = yMin = zMin = -1000;
	xMax = yMax = zMax =  1000;

	return true;
}

void VistaDirectXGamepad::SetJoystickName( const std::string &strJoyName )
{
	if(m_pJoystick)
		return;
	m_strJoyName = strJoyName;
}

std::string VistaDirectXGamepad::GetJoystickName() const
{
	return m_strJoyName;
}


bool VistaDirectXGamepad::InitXInput()
{
	return false;
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect *
	   VistaDirectXGamepad::GetDirectXForceFeedbackAspect() const
{
	return dynamic_cast<VistaDirectXForceFeedbackAspect*>(m_pForceAspect);
}

bool VistaDirectXGamepad::RegisterAspect(IVistaDeviceDriverAspect *pAspect)
{
	bool bRet = IVistaDeviceDriver::RegisterAspect(pAspect);
	if( bRet && (pAspect->GetId() == VistaDriverAbstractWindowAspect::GetAspectId()) )
	{
		// register window aspect
		m_pWindowAspect = dynamic_cast<VistaDriverAbstractWindowAspect*>(pAspect);
		m_pWindowAspect->SetTouchSequence( new GamepadAttach(this) );
	}

	return bRet;
}

bool VistaDirectXGamepad::UnregisterAspect(IVistaDeviceDriverAspect *pAspect, eDeleteAspectBehavior bDelete, eUnregistrationBehavior bForce )
{
	if( m_pWindowAspect && (pAspect->GetAspectId() == VistaDriverAbstractWindowAspect::GetAspectId()) )
	{
		std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> liWds = m_pWindowAspect->GetWindowList();
		for(std::list<VistaDriverAbstractWindowAspect::IWindowHandle*>::iterator it = liWds.begin(); it != liWds.end(); ++it)
			m_pWindowAspect->DetachFromWindow(*it);

		VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence *pTouch
			= m_pWindowAspect->GetTouchSequence();
		m_pWindowAspect->SetTouchSequence(NULL);
		delete pTouch;

		m_pWindowAspect = NULL;
	}

	return IVistaDeviceDriver::UnregisterAspect(pAspect, bDelete, bForce );
}

// #############################################################################
// FORCE FEEDBACK ASPECT
// #############################################################################

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::VistaDirectXForceFeedbackAspect(
	VistaDirectXGamepad *pPad)
: IVistaDriverForceFeedbackAspect(),
  m_pCurrentEffect(NULL),
  m_pParent(pPad)
{
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::~VistaDirectXForceFeedbackAspect()
{
	// unregister all uploaded effects
	for(std::vector<Effect*>::iterator it = m_vecEffects.begin();
		it != m_vecEffects.end(); ++it)
	{
		(*it)->m_pEffect->Unload();
		delete (*it);
	}
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Effect*
	VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::CreateEffect(REFGUID effectType)
{
	// we can assume that a valid joy configuration is present,
	// otherwise this aspect would not be present

	Effect *pEffect = new Effect;

	LONG rglDirection[2] = { 0,0 };
	DWORD  rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };

	DICONSTANTFORCE cf = { 0 };
	DIEFFECT eff;
	memset(&eff, 0, sizeof(eff));

	eff.dwSize                = sizeof(DIEFFECT);
	eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.cAxes                 = 2;
	eff.rglDirection          = rglDirection;
	eff.lpEnvelope            = 0;
	eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams = &cf;
	eff.dwStartDelay            = 0;
	eff.rgdwAxes                = rgdwAxes;
	eff.dwDuration              = INFINITE;
	eff.dwSamplePeriod          = 0;
	eff.dwGain                  = DI_FFNOMINALMAX;
	eff.dwTriggerButton         = DIEB_NOTRIGGER;
	eff.dwTriggerRepeatInterval = 0;

	HRESULT hr = m_pParent->m_pJoystick->CreateEffect( effectType, &eff, &pEffect->m_pEffect, NULL );
	if( hr != S_OK )
	{
		delete pEffect;
		return NULL;
	}

	m_vecEffects.push_back(pEffect);

	return pEffect;
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::DeleteEffect(Effect *pEffect)
{
	pEffect->m_pEffect->Unload();

	// remove from internal structures
	std::vector<Effect*>::iterator it = std::remove(m_vecEffects.begin(),
													 m_vecEffects.end(), pEffect);
	m_vecEffects.erase(it, m_vecEffects.end());

	delete pEffect;

	return true;
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::SetForce( const VistaVector3D& v3Force,
																	   const VistaVector3D& )
{
	if(!m_pCurrentEffect)
		return false;

	DIEFFECT eff;
	memset(&eff, 0, sizeof(eff));

	// interpret the force vector as a projection
	// on the x-z plane
	VistaVector3D norm(v3Force);
	norm[1] = 0;
	norm.Normalize();

	LONG rglDirection[2] = { LONG(norm[0]), LONG(norm[2]) };
	DWORD  rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };

	DICONSTANTFORCE cf;
	cf.lMagnitude = (LONG)v3Force.GetLength();

	eff.dwSize                = sizeof(DIEFFECT);
	eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.cAxes                 = 2;
	eff.rglDirection          = rglDirection;
	eff.lpEnvelope            = 0;
	eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams = &cf;
	eff.dwStartDelay            = 0;
	eff.rgdwAxes                = rgdwAxes;
	eff.dwDuration              = INFINITE;
	eff.dwSamplePeriod          = 0;
	eff.dwGain                  = DI_FFNOMINALMAX;
	eff.dwTriggerButton         = DIEB_NOTRIGGER;
	eff.dwTriggerRepeatInterval = 0;

	m_pParent->m_pJoystick->Acquire();
	if(m_pCurrentEffect->SetParameters(eff))
		return Start(m_pCurrentEffect, 1);
	return false;
}


bool     VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::SetCurrentEffect( Effect *pEffect )
{
	m_pCurrentEffect = pEffect;
	return true;
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Effect *
	 VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetCurrentEffect() const
{
	return m_pCurrentEffect;
}


bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Effect::SetParameters(DIEFFECT &params)
{
	return (m_pEffect->SetParameters( &params, DIEP_DIRECTION | DIEP_TYPESPECIFICPARAMS ) == S_OK);
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Effect::GetParameters( DIEFFECT &params )
{
	return (m_pEffect->GetParameters( &params, 0 ) == S_OK);
}


bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Start(Effect *pEffect, int nIterations)
{
	m_pParent->m_pJoystick->Acquire();
	return (pEffect->m_pEffect->Start(nIterations, 0) == S_OK);
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Stop(Effect *pEffect)
{
	m_pParent->m_pJoystick->Acquire();
	return (pEffect->m_pEffect->Stop() == S_OK);
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Effect::Effect()
: m_pEffect(NULL)
{
}

VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::Effect::~Effect()
{

}


int VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetNumInputDOF() const
{
	return 0;
}

int VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetNumOutputDOF() const
{
	return 3; // force-vector
}


bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::SetForcesEnabled(bool bEnabled)
{
	return false;
}

bool VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetForcesEnabled() const
{
	return false;
}


float VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetMaximumStiffness() const
{
    return 1.0f;
}

float VistaDirectXGamepad::VistaDirectXForceFeedbackAspect::GetMaximumForce() const
{
    return 1.0f;
}

#endif // WIN32


CreateProxy::CreateProxy( IVistaTranscoderFactoryFactory *fac ) : IVistaDriverCreationMethod(fac)
{
	m_pOrig = new VistaDirectXGamepadCreationMethod(fac);

	m_pOrig->RegisterSensorType( "", sizeof(VistaDirectXGamepadSensorMeasures::VISTADIRECTXJOYSTATE2),
		20,
		fac->CreateFactoryForType("") );
}

IVistaDeviceDriver * CreateProxy::CreateDriver()
{
	IVistaDeviceDriver *pDriver = (*m_pOrig).CreateDriver(); // create driver

	// sanity check...
	VistaDirectXGamepad *pGp = dynamic_cast<VistaDirectXGamepad*>(pDriver);
	if(!pGp)
	{
		delete pDriver;
		return NULL;
	}

	// this is the important change: inject a driver window aspect
	// (this one comes as a dependency from the kernel!)
	pGp->RegisterAspect( new VistaDriverAbstractWindowAspect() );

	return pGp;
}

unsigned int CreateProxy::RegisterSensorType( const std::string &strName, unsigned int nMeasureSize, unsigned int nUpdateEstimator, IVistaMeasureTranscoderFactory *pFac, const std::string &strTranscoderTypeString )
{
	return m_pOrig->RegisterSensorType(strName, nMeasureSize,
		nUpdateEstimator, pFac);
}

bool CreateProxy::UnregisterType( const std::string &strType, bool bDeleteFactories /*= false*/ )
{
	return m_pOrig->UnregisterType(strType, bDeleteFactories);
}

unsigned int CreateProxy::GetTypeFor( const std::string &strType ) const
{
	return m_pOrig->GetTypeFor(strType);
}

bool CreateProxy::GetTypeNameFor( unsigned int nTypeId, std::string &strDest ) const
{
	return m_pOrig->GetTypeNameFor(nTypeId, strDest);
}

unsigned int CreateProxy::GetMeasureSizeFor( unsigned int nType ) const
{
	return m_pOrig->GetMeasureSizeFor(nType);
}

unsigned int CreateProxy::GetUpdateEstimatorFor( unsigned int nType ) const
{
	return m_pOrig->GetUpdateEstimatorFor(nType);
}

std::list<std::string> CreateProxy::GetTypeNames() const
{
	return m_pOrig->GetTypeNames();
}

IVistaMeasureTranscoderFactory * CreateProxy::GetTranscoderFactoryForSensor( unsigned int nType ) const
{
	return m_pOrig->GetTranscoderFactoryForSensor(nType);
}

IVistaMeasureTranscoderFactory * CreateProxy::GetTranscoderFactoryForSensor( const std::string &strTypeName ) const
{
	return m_pOrig->GetTranscoderFactoryForSensor(strTypeName);
}

std::list<unsigned int> CreateProxy::GetTypes() const
{
	return m_pOrig->GetTypes();
}

CreateProxy::~CreateProxy()
{
	UnregisterType("", false);
}
