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


#ifndef _VISTACPUINFO_H
#define _VISTACPUINFO_H

#if defined(WIN32)
#include <Windows.h>
#endif

#include "VistaToolsConfig.h"

#include <VistaBase/VistaBaseTypes.h>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTATOOLSAPI VistaCPUInfo
{
public:
	VistaCPUInfo();
	~VistaCPUInfo();

	int GetProcessorCount();

	 VistaType::uint64 GetSpeed() const;
	 unsigned int GetSpeedMHz() const;

	 std::string GetCPUName() const;
	 std::string GetVendorName() const;

	 short int GetCPUFamily() const;
	 short int GetCPUModel() const;
	 short int GetCPUStepping() const;

	 bool GetIsMMXSupported() const;
	 bool GetIsSSESupported() const;
	 bool GetIsSSE2Supported() const;
	 bool GetIs3DNowSupported() const;
protected:
private:
	class _CPU_STAT;
	_CPU_STAT *m_pStat;

	bool QueryCPUInfo();
	std::string GetCPUNameString() const;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACPUINFO_H


