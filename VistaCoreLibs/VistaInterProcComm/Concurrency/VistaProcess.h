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


#ifndef _VISTAPROCESS_H
#define _VISTAPROCESS_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

// project includes
#include "VistaFork.h"
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <string>

class IVistaProcessImp;
class VistaPriority;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


/**
 * @todo is this needed anywhere?
 */
class VISTAINTERPROCCOMMAPI VistaProcess : public VistaFork
{
public:
	VistaProcess ();
	virtual ~VistaProcess ();

	bool Run( const std::string & inCommand );
	bool Suspend();
	bool Resume();
	bool Join();
	bool Abort();

	bool SetPriority( const VistaPriority & );
	void GetPriority( VistaPriority & ) const;

	virtual void PreRun();
	virtual void PostRun();

	/**
	 * A suspended process IS running (but not executing)
	 * A non-RUN process is NOT running (and not executing)
	 */
	int GetIsRunning() const;

	std::string GetProcessCommand() const;

	virtual bool GetIsFinished() const;

private:
	/**
	 * Defines the platform specific implementation for this process
	 */
	IVistaProcessImp     *m_pProcessImp;
	std::string m_sProcessCommand;

	bool m_bIsFinished;
	bool m_bRunning;
};

#endif // _VISTAPROCESS_H
