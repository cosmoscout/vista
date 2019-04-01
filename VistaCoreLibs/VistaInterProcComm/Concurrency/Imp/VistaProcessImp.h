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


#ifndef _VISTAPROCESSIMP_H
#define _VISTAPROCESSIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaPriority;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI IVistaProcessImp
{
public:
	virtual ~IVistaProcessImp();

	virtual bool     Run         ( const std::string & inCommand ) = 0;
	virtual bool     Suspend          () = 0;
	virtual bool     Resume      () = 0;
	virtual bool     Join        () = 0;
	virtual bool     Abort        () = 0;

	virtual bool     SetPriority   ( const VistaPriority & ) = 0;
	virtual void GetPriority   (VistaPriority &) const = 0;

	static IVistaProcessImp *CreateProcessImp();

	/**
	 * Method that is to be performed BEFORE departed fork starts execution
	 */
	virtual void PreRun() ;

	/**
	 * Method that is to be performed AFTER forked work is done
	 */
	virtual void PostRun();

protected:
	IVistaProcessImp();
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

