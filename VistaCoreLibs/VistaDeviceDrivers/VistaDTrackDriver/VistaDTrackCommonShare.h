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


#ifndef _VISTADTRACKCOMMONSHARE_H
#define _VISTADTRACKCOMMONSHARE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaBaseTypes.h>

namespace VistaDTrackMeasures
{
	/**
	 * memory layout of the global type sensor
	 */
	struct sGlobalMeasure
	{
		int   m_nFrameCount;
		float m_nTimeStamp;
	};


	/**
	 * memory layout of the typical body measure
	 */
	struct sBodyMeasure
	{
		double m_nId;
		double m_nQuality;
		double m_anPosition[3];
		double m_anEuler[3];
		double m_anRotation[9];
	};


	/**
	 * memory layout of the stick measure (protocol 1 flystick)
	 */
	struct sStickMeasure
	{
		double m_nId;
		double m_nQuality;
		double m_nButtonState;
		double m_anPosition[3];
		double m_anEuler[3];
		double m_anRotation[9];
	};

	
	/**
	 * memory layout of the stick measure (protocol 1 flystick)
	 */
	struct sStick2Measure
	{
		double	m_nId;
		double	m_nQuality;
		double	m_nNumberButtonValues;
		double	m_nNumberControllerValues;
		double	m_anPosition[3];
		double	m_anRotation[9];
		VistaType::sint32	m_nButtonState;		// limits number of buttons to 32
		double	m_anControllers[8]; //@todo: max number of controllers?
	};


	/**
	 *  memory layout of the marker measure
	 */
	struct sMarkerMeasure
	{
		double m_nId;
		double m_nQuality;
		double m_anPosition[3];
	};

	/**
	 * memory layout of the measure device's measure
	 */
	struct sMeasureMeasure
	{
		double m_nId;
		double m_nQuality;
		double m_nButtonState;
		double m_anPosition[3];
		double m_anRotation[9];
	};

	/**
	 * memory layout of the base measure
	 */
	struct sBaseMasure
	{
		double m_nId;
		double m_nQuality;
	};

	/**
	 * this is a dummy structure to read off the values in a generic
	 * way. the other structures are simple access functions for the
	 * transcode getters!
	 */
	struct sGenericMeasure
	{
		double m_anField[18];
	};

	/**
	 * memory layout of the typical finger measure
	 */
	struct sHandMeasure
	{
		enum
		{
			THUMB = 0,
			INDEX_FINGER,
			MIDDLE_FINGER,
			RING_FINGER,
			LITTLE_FINGER,
		};
		struct Finger
		{
			double m_anPosition[3];
			double m_anRotation[9];
			double m_nRadius;
			double m_nOuterPhalanxLength;
			double m_nOuterToMiddleAngle;
			double m_nMiddlePhalanxLength;
			double m_nMiddleToInnerAngle;
			double m_nInnerPhalanxLength;			
		};
		double m_nId;
		double m_nQuality;
		double m_nIsRightHand;
		double m_nNumberOfFingers;
		double m_anPosition[3];
		double m_anRotation[9];
		Finger m_aFingers[5];	
	};

}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADTRACKCOMMONSHARE_H
