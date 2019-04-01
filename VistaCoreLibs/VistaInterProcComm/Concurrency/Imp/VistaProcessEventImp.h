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


#ifndef _VISTAPROCESSEVENTIMP_H
#define _VISTAPROCESSEVENTIMP_H


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaBase/VistaBaseTypes.h>
#include <string>

class VISTAINTERPROCCOMMAPI IVistaProcessEventImp
{
public:
	// signaller ctor
	IVistaProcessEventImp( const std::string& sEventName );
	// receiver ctor
	IVistaProcessEventImp( const std::string& sEventName,
						   const int nMaxWaitForSignaller );
	virtual ~IVistaProcessEventImp();

	std::string GetEventName() const;
	bool GetIsSignaller() const;

	virtual bool GetIsValid() const = 0;
 
	virtual bool SignalEvent() = 0;
	virtual bool WaitForEvent( bool bBlock ) = 0;
	virtual bool WaitForEvent( int iBlockTime ) = 0;

	static IVistaProcessEventImp* CreateProcessEventSignallerImp( const std::string& sEventName );
	static IVistaProcessEventImp* CreateProcessEventReceiverImp( const std::string& sEventName,
																	const int nMaxWaitForSignaller );
private:
	std::string m_sEventName;
	bool m_bIsSignaller;
};


/*============================================================================*/

#endif // _VISTAITERATIONTHREAD_H
