/*============================================================================*/
/*                    ViSTA VR toolkit - OpenAL1.1 driver                     */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/


#ifndef __VISTAOPENALRECORDDRIVER_H
#define __VISTAOPENALRECORDDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>

#include "OpenALSoundContext.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTAOPENALRECORDDRIVER_STATIC) 
#ifdef VISTAOPENALRECORDDRIVER_EXPORTS
#define VISTAOPENALRECORDDRIVERAPI __declspec(dllexport)
#else
#define VISTAOPENALRECORDDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAOPENALRECORDDRIVERAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverInfoAspect;
class VistaDriverThreadAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAOPENALRECORDDRIVERAPI VistaOpenALRecordDriver : public IVistaDeviceDriver
{
public:
	struct _audiotype
	{
		_audiotype()
		: nFormat(0)
		, nFreq(0)
		, nMeasureSize(0)
		{
		}

		int   nFormat;
		int   nFreq;
		int   nMeasureSize;
	};

	VistaOpenALRecordDriver(IVistaDriverCreationMethod *);
	virtual ~VistaOpenALRecordDriver();


	class Parameters : public VistaDriverGenericParameterAspect::IParameterContainer
	{
		REFL_DECLARE
	public:
		Parameters( VistaOpenALRecordDriver* pDriver );

		enum
		{
			MSG_RECORDINGFREQ_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
			MSG_FORMAT_CHG,
			MSG_DEVICENAME_CHG,
			MSG_BUFFERTIME_CHG,
			MSG_BUFFERSIZE_CHG,
			MSG_MINBUFFERTIME_CHG,
			MSG_LAST
		};

		std::string GetDeviceName() const;
		bool SetDeviceName( const std::string & );

		int GetRecordingFrequency() const;
		bool SetRecordingFrequency( int );


		int GetRecordingFormat() const;
		std::string GetRecordingFormatString() const;
		bool SetRecordingFormat(int);
		bool SetRecordingFormat( const std::string &);

		int GetBufferTime() const;
		bool SetBufferTime( int );

		int GetBufferSize() const;
		bool SetBufferSize( int nSize );

		int GetMinimumBufferTime() const;
		bool SetMinimumBufferTime( int );

//		static std::string GetFormatStringFor( int format );
//		static int  GetNumberOfBytesPerSample(int format);

		int GetNumberOfBytesPerSample() const;
		int GetNumberOfSamples() const;
		bool UpdateBufferSize();

		virtual bool TurnDef( bool def );
		virtual bool Apply();
	private:
		VistaOpenALRecordDriver *m_parent;



		std::string m_strDeviceName;
		int    m_recordingFrequency; /**< use one of the usual suspects for audio recording as int */
		int    m_format; /**< see OpenAL constants for capture formats to set this int */
		int    m_bufferSize;
		int    m_bufferTime;
		int    m_minimumBufferTime;

		bool m_isDef;
	};

	void SignalPropertyChanged(int msg);
protected:

	virtual bool PhysicalEnable(bool bEnable);
	bool DoSensorUpdate(VistaType::microtime nTs);
	bool DoConnect();
	bool DoDisconnect();
private:

	void OnChangeRecordingParameters();
	void OnUpdateMeasureSize();

	VistaDriverInfoAspect       *m_pInfo;
	VistaDriverThreadAspect     *m_pThread;
	OpenALSoundContext          *m_pContext;
	OpenALSoundContext::OpenALCaptureContext *m_pCapture;

	VistaDriverGenericParameterAspect *m_pParams;
	int   nToGo, samples, numBytesPerSample;
};


class VISTAOPENALRECORDDRIVERAPI VistaOpenALRecordDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaOpenALRecordDriverCreationMethod(IVistaTranscoderFactoryFactory *fac);
	virtual IVistaDeviceDriver *CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif //__VISTAOPENALRECORD_H

