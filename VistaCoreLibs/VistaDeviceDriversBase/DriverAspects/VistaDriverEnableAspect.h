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


#ifndef _VISTADRIVERENABLEASPECT_H
#define _VISTADRIVERENABLEASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

#include <string>
#include <list>
#include <map>



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
 * Aspect that drivers can register to support explict enabling and disabling 
 * of a driver.
 * Specific driver code inherits from this aspect, and only overloads the 
 * SetEnabled() and GetIsEnabled() API.
 * 
 * A disabled driver (GetIsEnabled() == false) is not transmitting any data to
 * the measure history.
 * 
 * Registers an aspect with token type "ENABLE".
 */
class VISTADEVICEDRIVERSAPI VistaDriverEnableAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	VistaDriverEnableAspect();
	virtual ~VistaDriverEnableAspect();


	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	virtual bool SetIsEnabled( bool bEnable ) = 0;
	virtual bool GetIsEnabled() const = 0;


	// #########################################
	// DO NOT SHADOW IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:
private:
	static int m_nAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


/**
 * shorthand template to create an enable/disable aspect based on an existing method
 * in the driver context.
 * 
 * Typical usage via typedef:
 * \begincode
 * typedef TVistaDriverEnableAspect< MyDriver > MyDriverEnableAspect;
 * \endcode
 * Register as normal aspect and use it to callback on the passed in EnableFunction.
 * 
 */
template<class DriverType>
class TVistaDriverEnableAspect : public VistaDriverEnableAspect
{
public:
	//! pointer to method that takes one bool as argument, and returns a bool
	//! @param enabled_flag [in] true for enabled, false for disabled
	//! @return bool indicating if the state-change succeeded
	typedef bool (DriverType::*EnableFunction)( bool bEnabledFlag );

	TVistaDriverEnableAspect( DriverType *pParent, EnableFunction fnEnableFunction )
		: VistaDriverEnableAspect()
		, m_pParent( pParent )
		, m_bIsEnabled(false)
		, m_fnEnableFunc( fnEnableFunction ) {}


	virtual bool SetIsEnabled( bool bEnable )
	{
		if( bEnable == GetIsEnabled() )
			return true;
		bool bSuccess = ((*m_pParent).*m_fnEnableFunc)( bEnable );
		if( bSuccess )
			m_bIsEnabled = bEnable;
		return bSuccess;
	}

	virtual bool GetIsEnabled() const
	{
		return m_bIsEnabled;
	}

	DriverType    *m_pParent;
	bool           m_bIsEnabled;
	EnableFunction m_fnEnableFunc;
};

#endif //_VISTADRIVERENABLEASPECT_H

