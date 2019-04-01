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


#ifndef _VISTADRIVERPROTOCOLASPECT_H
#define _VISTADRIVERPROTOCOLASPECT_H


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
 * The protocol aspect is still under development. So far:
 * The idea is to have an API that
   - is capable of telling the user what protocols are available
     when seen from the driver
   - is capable of telling the driver what protocol to use on behalf
     of the user
 * These two sides are currently the drawback of the protocol aspect design,
 * as it is not quite clear which one is used on a per-driver level.
 * @todo think about this
 */
class VISTADEVICEDRIVERSAPI IVistaDriverProtocolAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	IVistaDriverProtocolAspect();
	virtual ~IVistaDriverProtocolAspect();


	class _cVersionTag
	{
	public:
		_cVersionTag() {}
		_cVersionTag( const std::string &strName,
			const std::string &strRevision )
			: m_strProtocolName(strName),
			  m_strProtocolRevision(strRevision)
		{
		}

		bool operator==(const _cVersionTag &oOther) const
		{
			return (m_strProtocolName == oOther.m_strProtocolName)
				&& (m_strProtocolRevision == oOther.m_strProtocolRevision);
		}

		std::string m_strProtocolName;
		std::string m_strProtocolRevision;
	};

	bool RegisterProtocol( const _cVersionTag & );
	bool GetHasProtocol( const _cVersionTag & ) const;
	bool UnregisterProtocol( const _cVersionTag & );

	/**
	 * override in subclass iff you want to intercept a protocol set
	 * e.g., for setting a specific protocol handler or actor
	 */
	virtual bool SetProtocol( const _cVersionTag & );
	bool GetProtocol(_cVersionTag &oTag) const;
	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:

private:
	std::list<_cVersionTag> m_liProtocols;
	std::list<_cVersionTag>::iterator m_oCurrent;
	static int m_nAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERLOGGINGASPECT_H


