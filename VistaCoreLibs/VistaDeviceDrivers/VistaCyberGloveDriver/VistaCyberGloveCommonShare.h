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


#ifndef _VISTACYBERGLOVECOMMONSHARE_H__
#define _VISTACYBERGLOVECOMMONSHARE_H__


namespace VistaCyberGloveCommonShare
{
	enum VCG_ERROR_CODE
	{
		VCG_ERROR_NONE = 0,		// no error

		VCG_ERROR_UNKNOWN,		// unknown error
		VCG_ERROR_GLOVE,		// error code 'eg': Glove not plugged in.
		VCG_ERROR_SAMPLING		// error code 'es': Sampling rate too fast.
	};

	struct sCyberGloveSample
	{
		char m_cRecord[35];

		int m_nSensorsInSample;
		int m_nSensorMask;

		bool m_bIncStatusByte;
		bool m_bIncTimestamp;

		VCG_ERROR_CODE m_eErrorCode;
	};
}


#endif //_VISTACYBERGLOVECOMMONSHARE_H__
