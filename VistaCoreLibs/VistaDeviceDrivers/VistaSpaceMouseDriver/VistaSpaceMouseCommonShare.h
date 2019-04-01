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


#ifndef _VISTASPACEMOUSEDRIVERCONFIG_H
#define _VISTASPACEMOUSEDRIVERCONFIG_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

namespace VistaSpaceMouseMeasures
{
	/**
	 * the memory layout of a space mouse measure.
	 */
	struct sSpaceMouseMeasure
	{
		float  m_nPos[4],  // make it 4, that's easier to convert for the vista
						   // vector
			   m_nOri[4];
		float  m_nRotAx[4]; // raw values for the rotation axis

		double m_nButtonStates[9];
	};
}

#endif // _VISTASPACEMOUSEDRIVERCONFIG_H
