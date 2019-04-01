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


#ifndef _VISTATHREADEVENTIMP_H
#define _VISTATHREADEVENTIMP_H


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <VistaBase/VistaBaseTypes.h>

class VISTAINTERPROCCOMMAPI IVistaThreadEventImp
{
public:
	IVistaThreadEventImp( );
	virtual ~IVistaThreadEventImp();

	virtual void SignalEvent() = 0;

	virtual bool WaitForEvent(bool bBlock) = 0;
	virtual bool WaitForEvent(int iTimeoutMSecs) = 0;

	virtual HANDLE GetEventSignalHandle() const = 0;
	virtual HANDLE GetEventWaitHandle() const = 0;

	enum ResetBehavior
	{
		RESET_ALL_EVENTS = 0,
		RESET_JUST_ONE_EVENT
	};
	virtual bool ResetThisEvent( ResetBehavior reset_behavior ) = 0;

	static IVistaThreadEventImp *CreateThreadEventImp(bool bUsePipes=false);
};


/*============================================================================*/

#endif // _VISTAITERATIONTHREAD_H
