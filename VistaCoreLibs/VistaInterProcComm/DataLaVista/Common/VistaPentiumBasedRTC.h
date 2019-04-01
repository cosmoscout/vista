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


#if defined(WIN32) || defined(LINUX) && !defined(ARM)
#define DLV_IX86
#endif


#if defined(DLV_IX86)

#ifndef IDLVISTAPENTIUMBASEDRTC_H
#define IDLVISTAPENTIUMBASEDRTC_H

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

class VISTAINTERPROCCOMMAPI DLVistaPentiumBasedRTC : public IDLVistaRTC
{
public:
	DLVistaPentiumBasedRTC();

	virtual ~DLVistaPentiumBasedRTC();

	virtual DLV_INT64 GetTickCount() const;

	virtual DLV_INT32 GetTimeStamp() const;

	virtual double GetTickToSecond( DLV_INT64 nTs ) const;

	virtual double GetSystemTime() const;

	//virtual const char *GetPrintfFormatSpecifier();

	void SetClockFrequency(DLV_INT64 nFreq);
	DLV_INT64 GetClockFrequency() const;
protected:
	DLV_INT64 GetPentiumCounter() const;
private:
	/**
	 * We prohibit copying
	 */
	DLVistaPentiumBasedRTC(const DLVistaPentiumBasedRTC &);
	DLVistaPentiumBasedRTC& operator= (const DLVistaPentiumBasedRTC&);

	DLV_INT64 m_nFrequency;

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //IDLVISTAFILTER_H
#endif // IX86

