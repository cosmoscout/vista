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


#ifndef _VISTAPRIORITY_H
#define _VISTAPRIORITY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include "VistaIpcThreadModel.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaPriorityImp;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaPriority
{
public:

	enum PRIORITY_RANGE
	{
		VISTA_MIN_PRIORITY  =     0,
		VISTA_MID_PRIORITY  = 16383,
		VISTA_MAX_PRIORITY  = 32767
	};


	VistaPriority(int iPrio=VISTA_MID_PRIORITY);
	VistaPriority(const VistaPriority &);
	virtual ~VistaPriority();


	int GetVistaPriority() const;
	void SetVistaPriority(int);


	int GetSystemPriority() const;

	int GetVistaPriorityForSystemPriority( int iSysPrio ) const;

	VistaPriority &operator=(const VistaPriority &);

private:
	IVistaPriorityImp *m_pPriorityImp;
	int m_iVistaPriority;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAPRIORITY_H

