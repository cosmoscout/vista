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


#if !defined(VISTA_THREADING_WIN32)

#ifndef _VISTAPOSIXPROCESSEVENTIMPL_H
#define _VISTAPOSIXPROCESSEVENTIMPL_H


#include "VistaProcessEventImp.h"

#include <sys/select.h>

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaPosixProcessEventImp : public IVistaProcessEventImp
{
public:

	VistaPosixProcessEventImp( const std::string& sEventName );
	VistaPosixProcessEventImp( const std::string& sEventName,
							   const int nWaitTimeout );
	virtual ~VistaPosixProcessEventImp();
	
	bool GetIsValid() const;

	bool SignalEvent();
	bool WaitForEvent(bool bBlock);
	bool WaitForEvent(int iBlockTime);

	bool ResetThisEvent();

private:
	bool m_bManagesFiles;
	int m_nFifo;
	fd_set* m_oReadSet;
};


/*============================================================================*/

#endif // _VISTAITERATIONTHREAD_H

#endif // VISTA_THREADING_WIN32

