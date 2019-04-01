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


#ifndef _VISTAGLUTMOUSEDRIVER_H
#define _VISTAGLUTMOUSEDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaDeviceDriversBase/Drivers/VistaMouseDriver.h>
#include <VistaInterProcComm/Concurrency/VistaMutex.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTAGLUTMOUSEDRIVER_STATIC) 
#ifdef VISTAGLUTMOUSEDRIVER_EXPORTS
#define VISTAGLUTMOUSEDRIVERAPI __declspec(dllexport)
#else
#define VISTAGLUTMOUSEDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAGLUTMOUSEDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaDriverCreationMethod;
class VistaDriverAbstractWindowAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAGLUTMOUSEDRIVERAPI VistaGlutMouseDriver : public IVistaMouseDriver
{
public:
	VistaGlutMouseDriver(IVistaDriverCreationMethod *crm);
	virtual ~VistaGlutMouseDriver();

	static void MouseFunction ( int iButton, int iState, int iX, int iY);
	static void MotionFunction( int iX, int iY );
	static void MouseWheelFunction( int nWheelNumber, int nDirection, int nX, int nY);
	
	bool GetMouseWarpPending() const;
	void SetMouseWarpPending( const bool& oValue );
	bool GetGrabCursorChanged() const;
	void SetGrabCursorChanged( const bool& oValue );

protected:
	virtual bool DoSensorUpdate(VistaType::microtime dTs);
	virtual bool DoConnect();
	virtual bool DoDisconnect();
private:

	struct _state
	{
		_state(int nGlutWinId,
			   int nX = 0,
			   int nY = 0,
			   int Bs0 = 0,
			   int Bs1 = 0,
			   int Bs2 = 0,
			   int nWheelNumber = -1,
			   int nWheelDirection = 0)
			: m_nWinId(nGlutWinId),
			  m_nX(nX),
			  m_nY(nY),
			  m_nWheelNumber(nWheelNumber),
			  m_nWheelDirection(nWheelDirection)
		{
			m_nButtonStates[0] = Bs0;
			m_nButtonStates[1] = Bs1;
			m_nButtonStates[2] = Bs2;
		}

		int m_nButtonStates[3];
		int m_nX, m_nY;
		int m_nWheelNumber,
			m_nWheelDirection,
			m_nWinId;
	};

	void Receive( const _state &s );


	std::vector<_state> m_vecUpdates;
	int m_nButtonStates[3];
	int m_nWheelState,
		m_nWheelDirState;
	VistaDriverAbstractWindowAspect *m_pWindowAspect;

	bool m_bMouseWarpPending;
	// @TODO: set this if the grab mode is switched during runtime
	bool m_bGrabCursorChanged;
	

	bool m_bConnected;

	VistaMutex m_update_vec_mutex;	
};


class VISTAGLUTMOUSEDRIVERAPI VistaGlutMouseDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaGlutMouseDriverCreationMethod(IVistaTranscoderFactoryFactory *fac);
	virtual IVistaDeviceDriver *CreateDriver();
protected:
private:
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif
