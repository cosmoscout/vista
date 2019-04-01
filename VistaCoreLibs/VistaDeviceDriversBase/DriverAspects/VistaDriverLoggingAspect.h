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


#ifndef _VISTADRIVERLOGGINGASPECT_H
#define _VISTADRIVERLOGGINGASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaDLVTypes.h>
#include <list>
#include <string>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaDeviceDriver;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This is currently the only aspect that can be given to a driver for logging
 * purposes. Not all drivers may support this. In case you want to use logging
 * for your driver, register this aspect and use it during processing.
 * @todo re-work this... seems strange to me.
 */
class VISTADEVICEDRIVERSAPI VistaDriverLoggingAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	VistaDriverLoggingAspect();
	virtual ~VistaDriverLoggingAspect();

	/**
	 * call to log a string
	 * @param nTimestamp the timestamp for logging
	 * @param pDriver the driver to log for
	 * @param strMsg the message to log (as-is)
	 * @param nLogLevel the log-level mask (-1 for non-maskable)
	 */
	void Log( VistaType::microtime nTimestamp,
				const IVistaDeviceDriver *pDriver,
				const std::string &strMsg,
				int nLogLevel = -1 );

	/**
	 * get the current log level
	 */
	int  GetLogLevel() const;

	/**
	 * set the current log level
	 */
	void SetLogLevel(int nLevel);

	/**
	 * get enable flag
	 */
	bool GetEnabled() const;

	/**
	 * set enable flag
	 */
	void SetEnabled(bool bEnabled);

	/**
	 * get prefix of 'mnemonic' which is prefixed in front of every string that
	 * is logged, can be the driver name, for example.
	 * @see SetMnemonic()
	 */
	std::string GetMnemonic() const;

	/**
	 * sets the prefix to prepend on each log string, can be the driver name for
	 * example.
	 * @see GetMnemonic()
	 */
	void SetMnemonic(const std::string &strMnemonic);

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:
	/**
	 * overwrite in subclasses to define your own custom logging...
	 * @todo re-think this!
	 */
	virtual bool DoLog( VistaType::microtime nTimestamp,
		const IVistaDeviceDriver *pDriver,
		const std::string &strMsg, int nLogLevel);
private:
	int        m_nLogLevel;
	bool       m_bEnabled;
	std::string m_strMnemonic;
	static int m_nAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERLOGGINGASPECT_H


