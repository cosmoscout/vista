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


#ifndef _VISTADRIVERABSTRACTWINDOWASPECT_H
#define _VISTADRIVERABSTRACTWINDOWASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

#include <list>
#include <set>

#if defined(WIN32)
#include <Windows.h>
#endif
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
 * Some drivers may have the need to communicate with the system by using
 * a window handle (e.g., in order to use the process message port for system
 * communication. The mapping from and to windows (VistaWindow) can be done
 * with the WindowAspect. Windows may be attached and detached, as such attach
 * and detach sequences can be defined on a per-aspect basis. This aspect has
 * the name "WINDOW".
 */
class VISTADEVICEDRIVERSAPI VistaDriverAbstractWindowAspect
				   : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	class VISTADEVICEDRIVERSAPI IWindowHandle
	{
	public:
		virtual ~IWindowHandle() {}


		virtual bool operator< ( const IWindowHandle &other ) = 0;
		virtual bool operator==( const IWindowHandle &other ) = 0;

	protected:
		IWindowHandle() {}
	};


	class VISTADEVICEDRIVERSAPI NativeWindowHandle : public IWindowHandle
	{
	public:
#if defined(WIN32)
		typedef HWND OSHANDLE; /*<< wrap windows handles HWND */
#else // this goes for all the others as void*
		typedef void* OSHANDLE; /*<< wrap generic handles */
#endif
		NativeWindowHandle() {};

		virtual OSHANDLE GetOSHandle() const = 0;
		virtual int GetID() const = 0;

		operator int() const { return GetID(); };
		operator OSHANDLE() const { return GetOSHandle(); };


		bool operator< ( const IWindowHandle& oOther ) 
		{
			const NativeWindowHandle* pOtherNative = dynamic_cast<const NativeWindowHandle *>( &oOther );
			if( pOtherNative == NULL ) 
				return false;

			if( GetID() == pOtherNative->GetID() )
				return ( GetOSHandle() < pOtherNative->GetOSHandle() );
			else
				return ( GetID() < pOtherNative->GetID() );
		}
		bool operator== ( const IWindowHandle& oOther ) 
		{
			const NativeWindowHandle* pOtherNative = dynamic_cast<const NativeWindowHandle *>( &oOther );
			if( pOtherNative == NULL ) 
				return false;

			return( GetID() == pOtherNative->GetID() && GetOSHandle() == pOtherNative->GetOSHandle() );
		}
	};

	class VISTADEVICEDRIVERSAPI WindowLookup
	{
	public:
		WindowLookup();
		~WindowLookup();

		//IWindowHandle *GetWindowById( int id ) const;
		//IWindowHandle *GetWindowByHandle( IWindowHandle::OSHANDLE handle ) const;

		void RegisterWindow( IWindowHandle *window );
		bool UnregisterWindow( IWindowHandle *window );
	protected:
		std::set<IWindowHandle*> m_windows;
	};

	class VISTADEVICEDRIVERSAPI WindowLookupCollection
	{
	public:
		WindowLookupCollection();

		const WindowLookup *GetWindowLookupBySystemTag( const std::string &tag ) const;
		bool RegisterWindowLookup( const std::string &tag, WindowLookup * );
		WindowLookup *UnregisterWindowLookup( const std::string &tag );
	protected:
		std::map<std::string, WindowLookup *> m_tagMap;
	};


	/**
	 * Registers an aspect class with name "WINDOW" iff no such class was
	 * registered before.
	 */
	VistaDriverAbstractWindowAspect();
	virtual ~VistaDriverAbstractWindowAspect();

	/**
	 * This method is called by the application / environment <b>every time</b>
	 * a window is to be attached to this aspect. The IWindowHandle is assumed to
	 * be valid as long as this aspect lives (it is copied to the handle list).
	 * This API is non-virtual on purpose, it calls
	 * IVistaDriverAbstractWindowTouchSequence::AttachSequence()
	 * passing the IWindowHandle. So be sure to register this touch sequence by the driver
	 * before using the AttachToWindow API. The construction allows to <b>give</b> window
	 * implementations to drivers without the need for sub-classing (driver code does not
	 * subclass the aspect, it specializes the TouchSequence!)
	 * @param a non NULL IWindowHandle wrapper to read the platform specific window handle from
	 * @return true if this call went through good, false else
	 * @see IVistaDriverAbstractWindowTouchSequence()
	 */
	bool AttachToWindow( IWindowHandle * oWindow );
	bool DetachFromWindow( IWindowHandle * oWindow );

	/**
	 * return a list of window handles to which the aspect is attached to.
	 */
	std::list<IWindowHandle*> GetWindowList() const;

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

	class VISTADEVICEDRIVERSAPI IVistaDriverAbstractWindowTouchSequence
	{
	public :
		virtual ~IVistaDriverAbstractWindowTouchSequence() {}
		virtual bool AttachSequence( IWindowHandle * oWindow ) = 0;
		virtual bool DetachSequence( IWindowHandle * oWindow ) = 0;
		virtual std::list<IWindowHandle*> GetWindowList() const = 0;
	};

	/**
	 * sets the callback to call every time AttachToWindow() is called by a toolkit shell.
	 * The touch sequence is expected to live as long as this aspect lives. Passing NULL
	 * erases the current setting.
	 * @param pAtSeq the sequence to hold onto
	 */
	bool SetTouchSequence(IVistaDriverAbstractWindowTouchSequence *pAtSeq);

	/**
	 * returns the current touch sequence for this aspect. NULL iff none was set
	 * by driver code.
	 */
	IVistaDriverAbstractWindowTouchSequence *GetTouchSequence() const;

protected:
private:
	IVistaDriverAbstractWindowTouchSequence *m_pTouchSeq;

	static int m_nAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERWINDOWASPECT_H
