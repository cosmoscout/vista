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


#ifndef _VISTADUMMYRTC_H
#define _VISTADUMMYRTC_H

 /*============================================================================*/
 /* MACROS AND DEFINES                                                         */
 /*============================================================================*/

 /*============================================================================*/
 /* INCLUDES                                                                   */
 /*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
 #include <VistaInterProcComm/DataLaVista/Base/VistaRTC.h>

 /*============================================================================*/
 /* FORWARD DECLARATIONS                                                       */
 /*============================================================================*/


 /*============================================================================*/
 /* CLASS DEFINITIONS                                                          */
 /*============================================================================*/

class DLVistaDummyRTC : public IDLVistaRTC
{
private:
	/**
	 * We prohibit copying
	 */
	DLVistaDummyRTC	(DLVistaDummyRTC &) {}

public:

						DLVistaDummyRTC	() {}
	 virtual			~DLVistaDummyRTC	() {}

	 virtual DLV_INT64	GetTickCount				() const {return 0;}
	 virtual DLV_INT32	GetTimeStamp				() const {return 0;}
	 virtual double		GetSystemTime				() const {return 0.0;}
	 virtual double GetTickToSecond( DLV_INT64 nTs ) const { return 0.0; }
};


#endif //_VISTADUMMYRTC_H

