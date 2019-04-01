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


#include "VistaGlutKeyboardDriver.h"
#include <map>
#include <algorithm>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverAbstractWindowAspect.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

#ifdef WIN32
// disable warnings from unreferenced functions in glut.h
// sadly, C4505 cannot be disabled locally with push/pop
#pragma warning( disable : 4505 )
#endif
#if defined(USE_NATIVE_GLUT)
  #if defined(DARWIN) // we use the mac os GLUT framework on darwin
    #include <GLUT/glut.h>
  #else
    #include <GL/glut.h>
  #endif
#else
    #include <GL/freeglut.h>
	// since freeglut doesn't provide a proper mechanism to detect its version, we
	// have to work around it by checking for a define that only appears in
	// freeglut 2.8+
	#ifdef GLUT_KEY_SHIFT_L
		#define VISTA_FREEGLUT_VERSION_ATLEAST_2_8
	#endif
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

VistaGlutKeyboardDriverCreationMethod::VistaGlutKeyboardDriverCreationMethod(IVistaTranscoderFactoryFactory *fac)
: IVistaDriverCreationMethod(fac)
{
	RegisterSensorType( "",
		sizeof( IVistaKeyboardDriver::_sKeyboardMeasure ),
		20,
		fac->CreateFactoryForType("") );
}

IVistaDeviceDriver *VistaGlutKeyboardDriverCreationMethod::CreateDriver()
{
	return new VistaGlutKeyboardDriver(this);
}


namespace
{
	class UVistaKeyboardMap
	{
	public:
		typedef std::map<int, VistaGlutKeyboardDriver *> DEVMAP;
		DEVMAP m_mapDevices;

		VistaGlutKeyboardDriver *RetrieveKeyboardFromWindowId(int nWindow)
		{
			DEVMAP::const_iterator cit = m_mapDevices.find(nWindow);
			if(cit == m_mapDevices.end())
				return NULL; // ?

			return (*cit).second;
		}

		bool RegisterKeyboardWithWindow(int nWindowId, VistaGlutKeyboardDriver *pAddDriver)
		{
			VistaGlutKeyboardDriver *pDriver = RetrieveKeyboardFromWindowId(nWindowId);
			if(pDriver)
				return (pDriver == pAddDriver);


			m_mapDevices[nWindowId] = pAddDriver;

			return true;
		}

		bool UnregisterKeyboardFromWindow(int nWindowId, VistaGlutKeyboardDriver *pAddDriver)
		{
			DEVMAP::iterator it = m_mapDevices.find(nWindowId);
			if(it == m_mapDevices.end())
				return true;

			if((*it).second != pAddDriver)
				return false;

			m_mapDevices.erase(it);

			return true;
		}
	};

	UVistaKeyboardMap S_mapKeyboardMap;
}

// do translation of key presses here
void VistaGlutKeyboardDriver::SetKeyValue(VistaGlutKeyboardDriver *pKeyboard,
												 unsigned char nKeyValue,
												 bool bIsUp,
												 int nModifier)
{
	int nCurrentKey = int(nKeyValue);
	switch(nKeyValue)
	{
		case 27: //ASCII Escape
			nCurrentKey = VISTA_KEY_ESC;
			break;
		case 13: //ASCII Return
			nCurrentKey = VISTA_KEY_ENTER;
			break;
		case 9: //ASCII Tab ???
			nCurrentKey = VISTA_KEY_TAB;
			break;
		case 8: //ASCII Backspace
			nCurrentKey = VISTA_KEY_BACKSPACE;
			break;
		default:
			nCurrentKey = int(nKeyValue);
			break;
	}
	pKeyboard->Receive( _sKeyHlp(nCurrentKey, nModifier, bIsUp ) );
}

void VistaGlutKeyboardDriver::SetSpecialKeyValue(VistaGlutKeyboardDriver *pKeyboard,
														int nKeyValue,
														bool bIsUp,
														int nModifier)
{
	int nCurrentKey = 0;
	switch(nKeyValue)
	{
	case GLUT_KEY_F1:
		nCurrentKey = VISTA_KEY_F1;
		break;
	case GLUT_KEY_F2:
		nCurrentKey = VISTA_KEY_F2;
		break;
	case GLUT_KEY_F3:
		nCurrentKey = VISTA_KEY_F3;
		break;
	case GLUT_KEY_F4:
		nCurrentKey = VISTA_KEY_F4;
		break;
	case GLUT_KEY_F5:
		nCurrentKey = VISTA_KEY_F5;
		break;
	case GLUT_KEY_F6:
		nCurrentKey = VISTA_KEY_F6;
		break;
	case GLUT_KEY_F7:
		nCurrentKey = VISTA_KEY_F7;
		break;
	case GLUT_KEY_F8:
		nCurrentKey = VISTA_KEY_F8;
		break;
	case GLUT_KEY_F9:
		nCurrentKey = VISTA_KEY_F9;
		break;
	case GLUT_KEY_F10:
		nCurrentKey = VISTA_KEY_F10;
		break;
	case GLUT_KEY_F11:
		nCurrentKey = VISTA_KEY_F11;
		break;
	case GLUT_KEY_F12:
		nCurrentKey = VISTA_KEY_F12;
		break;
	case GLUT_KEY_LEFT:
		nCurrentKey = VISTA_KEY_LEFTARROW;
		break;
	case GLUT_KEY_RIGHT:
		nCurrentKey = VISTA_KEY_RIGHTARROW;
		break;
	case GLUT_KEY_UP:
		nCurrentKey = VISTA_KEY_UPARROW;
		break;
	case GLUT_KEY_DOWN:
		nCurrentKey = VISTA_KEY_DOWNARROW;
		break;
	case GLUT_KEY_PAGE_UP:
		nCurrentKey = VISTA_KEY_PAGEUP;
		break;
	case GLUT_KEY_PAGE_DOWN:
		nCurrentKey = VISTA_KEY_PAGEDOWN;
		break;
	case GLUT_KEY_HOME:
		nCurrentKey = VISTA_KEY_HOME;
		break;
	case GLUT_KEY_END:
		nCurrentKey = VISTA_KEY_END;
		break;

#ifdef VISTA_FREEGLUT_VERSION_ATLEAST_2_8
	case GLUT_KEY_NUM_LOCK:
		nCurrentKey = VISTA_KEY_END;
		break;
	case GLUT_KEY_BEGIN:
		// not mapped yet
		break;
	case GLUT_KEY_DELETE:
		nCurrentKey = VISTA_KEY_DELETE;
		break;
	case GLUT_KEY_SHIFT_L:
		nCurrentKey = VISTA_KEY_SHIFT_LEFT;
		break;
	case GLUT_KEY_SHIFT_R:
		nCurrentKey = VISTA_KEY_SHIFT_RIGHT;
		break;
	case GLUT_KEY_CTRL_L:
		nCurrentKey = VISTA_KEY_CTRL_LEFT;
		break;
	case GLUT_KEY_CTRL_R:
		nCurrentKey = VISTA_KEY_CTRL_RIGHT;
		break;
	case GLUT_KEY_ALT_L:
		nCurrentKey = VISTA_KEY_ALT_LEFT;
		break;
	case GLUT_KEY_ALT_R:
		nCurrentKey = VISTA_KEY_ALT_RIGHT;
		break;
#endif

	default:
		nCurrentKey = nKeyValue;
		break;
	}
	pKeyboard->Receive( _sKeyHlp(nCurrentKey, nModifier, bIsUp ) );
}


void VistaGlutKeyboardDriver::KeyDownFunction(unsigned char ucKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return;

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	SetKeyValue(pKeyboard, ucKey, false, glutGetModifiers());
}

void VistaGlutKeyboardDriver::KeyUpFunction(unsigned char ucKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return;

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	SetKeyValue(pKeyboard, ucKey, true, glutGetModifiers());
}

void VistaGlutKeyboardDriver::SpecialKeyDownFunction(int nCurrentKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return;

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	SetSpecialKeyValue(pKeyboard, nCurrentKey, false, glutGetModifiers());
}

void VistaGlutKeyboardDriver::SpecialKeyUpFunction(int nCurrentKey, int, int)
{
	int nWindow = glutGetWindow();
	if(nWindow == 0)
		return;

	VistaGlutKeyboardDriver *pKeyboard = S_mapKeyboardMap.RetrieveKeyboardFromWindowId(nWindow);
	if(!pKeyboard)
		return;

	SetSpecialKeyValue(pKeyboard, nCurrentKey, true, glutGetModifiers());
}


/*============================================================================*/
/*============================================================================*/

class VistaGlutKeybTouchSequence :
	public VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence
{

public:
	VistaGlutKeybTouchSequence(VistaGlutKeyboardDriver *pDriver)
		: VistaDriverAbstractWindowAspect::IVistaDriverAbstractWindowTouchSequence(),
		  m_pKeyboardDriver(pDriver)
	{
	}

	bool AttachSequence( VistaDriverAbstractWindowAspect::IWindowHandle *oHandle)
	{
		VistaDriverAbstractWindowAspect::NativeWindowHandle *oWindow 
			= dynamic_cast<VistaDriverAbstractWindowAspect::NativeWindowHandle *>( oHandle );
		if( oWindow == 0 )
			return false;

		// check whether this driver is already registered with the window
		WINMAP::const_iterator cit = m_mapWindows.find(oWindow);
		if(cit == m_mapWindows.end())
		{
			// ugly workaround
			int windowId = oWindow->GetID();

			// ok, register with the window is in the statics
			// section
			if(S_mapKeyboardMap.RegisterKeyboardWithWindow(windowId, m_pKeyboardDriver))
			{
				// register this window with the instance variable
				m_mapWindows[oWindow] = windowId;

				// set the "current" window in glut
				int nCurWindow = glutGetWindow();
				glutSetWindow( windowId );

				// we register the static callbacks functions every time a keyboard
				// is attached to a window (so this may happen multiple times, even
				// when there is only one instance of keyboard. The important thing
				// is that we always set the same static function, so nothing really
				// terrible should happen.
				glutKeyboardFunc(&VistaGlutKeyboardDriver::KeyDownFunction);
				glutSpecialFunc(&VistaGlutKeyboardDriver::SpecialKeyDownFunction);
				glutKeyboardUpFunc(&VistaGlutKeyboardDriver::KeyUpFunction);
				glutSpecialUpFunc(&VistaGlutKeyboardDriver::SpecialKeyUpFunction);

				glutSetWindow(nCurWindow); // reset cur window id
				return true;
			}
		}
		return false;
	}

	bool DetachSequence( VistaDriverAbstractWindowAspect::IWindowHandle * oHandle)
	{
		VistaDriverAbstractWindowAspect::NativeWindowHandle *oWindow 
			= dynamic_cast<VistaDriverAbstractWindowAspect::NativeWindowHandle *>( oHandle );
		if( oWindow == 0 )
			return false;

		WINMAP::iterator cit = m_mapWindows.find(oWindow);
		if(cit != m_mapWindows.end())
		{
			// erase from map
			m_mapWindows.erase(cit);
			return S_mapKeyboardMap.UnregisterKeyboardFromWindow( oWindow->GetID(), m_pKeyboardDriver );
		}
		return false;
	}

	typedef std::map<VistaDriverAbstractWindowAspect::IWindowHandle*, int> WINMAP;

	class _copyIn : public std::unary_function< const WINMAP::value_type &, void>
	{
	public:
		_copyIn( std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> &list )
		: m_list(list) {}

		std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> &m_list;

		void operator()( const WINMAP::value_type &p )
		{
			m_list.push_back( p.first );
		}
	};

	virtual std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> GetWindowList() const
	{
		std::list<VistaDriverAbstractWindowAspect::IWindowHandle*> list;

		std::for_each( m_mapWindows.begin(), m_mapWindows.end(), _copyIn(list) );
		return list;
	}

	VistaGlutKeyboardDriver *m_pKeyboardDriver;

	WINMAP  m_mapWindows;
};


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaGlutKeyboardDriver::VistaGlutKeyboardDriver(IVistaDriverCreationMethod *crm)
: IVistaKeyboardDriver(crm)
, m_bLastFrameValue(false)
, m_pWindowAspect(new VistaDriverAbstractWindowAspect)
, m_bConnected(false)
, m_update_vec_lock()
{
	RegisterAspect(m_pWindowAspect);
	// is deleted by aspect, later on
	m_pWindowAspect->SetTouchSequence(new VistaGlutKeybTouchSequence(this));
}

VistaGlutKeyboardDriver::~VistaGlutKeyboardDriver()
{
	UnregisterAspect(m_pWindowAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pWindowAspect;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaGlutKeyboardDriver::DoSensorUpdate(VistaType::microtime dTs)
{
	if( !m_bConnected )
		return true;

	std::vector< _sKeyHlp > processing_vec;
	{
		VistaMutexLock l( m_update_vec_lock );
		processing_vec.swap( m_vecKeyVec );
	}

	if(processing_vec.empty()) // no key pressed
	{
		if(m_bLastFrameValue) // last frame had a value, so set back to 0
		{
			// the behavior we want is to get the outvalue on KEY to be 0
			// after an up-key, that is we want a single "-value" in the history
			// followed by a "0"
			m_bLastFrameValue = false;

			MeasureStart(dTs);
			UpdateKey( 0, 0 );
			MeasureStop();


			return true;
		}
		return false;
	}

	std::vector< _sKeyHlp >::const_iterator begin = processing_vec.begin();
	std::vector< _sKeyHlp >::const_iterator end   = processing_vec.end();
	for(std::vector< _sKeyHlp >::const_iterator cit = begin; cit != end; ++cit)
	{
		MeasureStart(dTs);
		UpdateKey(( (*cit).m_bUpKey == true ? -(*cit).m_nKey : (*cit).m_nKey ), (*cit).m_nModifier);
		MeasureStop();

		// toggle to true only last-neg-frame value
		// forces a "-value" is followed by "0" iff no other
		// key is pressed afterwards.
		m_bLastFrameValue |= ((*cit).m_bUpKey == true);

	}

	m_vecKeyVec.clear();


	return true;
};


bool VistaGlutKeyboardDriver::DoConnect()
{
	m_bConnected = true;
	return true;
}

bool VistaGlutKeyboardDriver::DoDisconnect()
{
	m_bConnected = false;
	return true;
}

void VistaGlutKeyboardDriver::Receive( const _sKeyHlp &v )
{
	VistaMutexLock l( m_update_vec_lock );
	m_vecKeyVec.push_back( v );
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
