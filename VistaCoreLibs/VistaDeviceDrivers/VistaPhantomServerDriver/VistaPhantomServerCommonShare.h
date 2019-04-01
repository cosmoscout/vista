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


#ifndef _VISTAPHANTOMDRIVERCONFIG_H
#define _VISTAPHANTOMDRIVERCONFIG_H

/**
 * the memory layout of a phantom measure
 */

namespace VistaPhantomServerMeasures
{
	struct sPhantomServerMeasure
	{

		float m_afPosition[3],
			m_afRotMatrix[9],
			m_afPosSCP[3],
			m_afVelocity[3],
			m_afForce[3];
		int   m_nButtonState;
		float m_nUpdateRate,
			m_afOverheatState[6];
	};
}

#endif //_VISTAPHANTOMDRIVERCONFIG_H

