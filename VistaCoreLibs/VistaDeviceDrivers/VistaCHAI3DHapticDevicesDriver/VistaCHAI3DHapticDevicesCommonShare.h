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


#ifndef _VISTACHAI3DHapticDevicesDRIVERCONFIG_H
#define _VISTACHAI3DHapticDevicesDRIVERCONFIG_H

namespace VistaCHAI3DHapticDevicesMeasures
{
	struct sCHAI3DHapticDevicesMeasure
	{
		float m_afPosition[3],        /**< in mm */
			m_afRotMatrix[16],      /**< column major */
			m_afVelocity[3],        /**< in mm/s */
			m_afForce[3],           /**< in N */
			m_afAngularVelocity[3], /**< the gimbal's, in rad/s */
			m_afJointAngles[3],     /**< radians */
			m_afGimbalAngles[3],    /**< radians */
			m_afTorque[3],          /**< in Nm */
			m_nUpdateRate,          /**< in Hz */
			m_afOverheatState[6];   /**< degrees Celsius? */
		long  m_nEncoderValues[6];    /**< raw values */
		int   m_nButtonState;         /**< button mask */
		bool  m_bInkwellSwitch;
	};
}

#endif //_VISTACHAI3DHapticDevicesDRIVERCONFIG_H