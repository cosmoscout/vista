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


#ifndef __VISTADIRECTXGAMEPADSENSORMEASURES_H
#define __VISTADIRECTXGAMEPADSENSORMEASURES_H

#if defined(WIN32)
#include <Windows.h>
#endif

namespace VistaDirectXGamepadSensorMeasures
{
	//Unfortunately we do need these information here ...
	typedef struct
	{
		long    lX;                     /* x-axis position              */
		long    lY;                     /* y-axis position              */
		long    lZ;                     /* z-axis position              */
		long    lRx;                    /* x-axis rotation              */
		long    lRy;                    /* y-axis rotation              */
		long    lRz;                    /* z-axis rotation              */
		long    rglSlider[2];           /* extra axes positions         */
		unsigned long   rgdwPOV[4];             /* POV directions               */
		unsigned char    rgbButtons[128];        /* 128 buttons                  */
		long    lVX;                    /* x-axis velocity              */
		long    lVY;                    /* y-axis velocity              */
		long    lVZ;                    /* z-axis velocity              */
		long    lVRx;                   /* x-axis angular velocity      */
		long    lVRy;                   /* y-axis angular velocity      */
		long    lVRz;                   /* z-axis angular velocity      */
		long    rglVSlider[2];          /* extra axes velocities        */
		long    lAX;                    /* x-axis acceleration          */
		long    lAY;                    /* y-axis acceleration          */
		long    lAZ;                    /* z-axis acceleration          */
		long    lARx;                   /* x-axis angular acceleration  */
		long    lARy;                   /* y-axis angular acceleration  */
		long    lARz;                   /* z-axis angular acceleration  */
		long    rglASlider[2];          /* extra axes accelerations     */
		long    lFX;                    /* x-axis force                 */
		long    lFY;                    /* y-axis force                 */
		long    lFZ;                    /* z-axis force                 */
		long    lFRx;                   /* x-axis torque                */
		long    lFRy;                   /* y-axis torque                */
		long    lFRz;                   /* z-axis torque                */
		long    rglFSlider[2];          /* extra axes forces            */
	} VISTADIRECTXJOYSTATE2;

	typedef struct
	{
		VISTADIRECTXJOYSTATE2 m_pDirectXJoystate2;
		long	m_iButtonCount;
	} VistaDirectXGamepadSensorMeasure;

	enum
	{
		AXIS_X1 = 0,
		AXIS_Y1,
		AXIS_Z1,
		AXIS_Z2,
		AXIS_X2,
		AXIS_Y2,
		AXIS_POV = 6
	};
}




#endif //__VISTADIRECTXGAMEPADSENSORMEASURES_H

