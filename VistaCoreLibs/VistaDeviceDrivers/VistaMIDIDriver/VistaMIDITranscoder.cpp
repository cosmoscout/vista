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


#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include "VistaMIDICommonShare.h"

#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <cstring>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace
{
	class VistaMIDIDriverTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaMIDIDriverTranscode() {
			// inherited as protected member
			m_nNumberOfScalars = 4;
		}

		~VistaMIDIDriverTranscode() {}
		static std::string GetTypeString() { return "VistaMIDIDriverTranscode"; }

		REFL_INLINEIMP(VistaMIDIDriverTranscode, IVistaMeasureTranscode);
	};

	class VistaMIDIDriverTypeGet : public IVistaMeasureTranscode::IntGet
	{
	public:
		VistaMIDIDriverTypeGet()
			: IVistaMeasureTranscode::IntGet(
			"TYPE",
			VistaMIDIDriverTranscode::GetTypeString(),
			"The type of MIDI message (CC, PC, ...)" )
		{}

		virtual int GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			const VistaMIDIMeasures::sMIDIMeasure *m =
				pMeasure->getRead< VistaMIDIMeasures::sMIDIMeasure >();

			// the first byte of a midi message is the status byte
			// of which the first 4 bit contain the type of the message
			return (m->status & 0xF0) >> 4;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nType) const
		{
			const VistaMIDIMeasures::sMIDIMeasure *m =
				pMeasure->getRead< VistaMIDIMeasures::sMIDIMeasure >();
			nType = (m->status & 0xF0) >> 4;
			return true;
		}
	};

	class VistaMIDIDriverChannelGet : public IVistaMeasureTranscode::IntGet
	{
	public:
		VistaMIDIDriverChannelGet()
			: IVistaMeasureTranscode::IntGet(
			"CHANNEL",
			VistaMIDIDriverTranscode::GetTypeString(),
			"The MIDI channel on which this message was sent (1-16)" )
		{}

		virtual int GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			const VistaMIDIMeasures::sMIDIMeasure *m =
				pMeasure->getRead< VistaMIDIMeasures::sMIDIMeasure >();

			// the first byte of a midi message is the status byte
			// of which the last 4 bit contain the midi channel.
			// values are 0-15. midi channels are numbered 1-16, thus +1.
			return (m->status & 0x0F) + 1;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nChannel) const
		{
			const VistaMIDIMeasures::sMIDIMeasure *m =
				pMeasure->getRead< VistaMIDIMeasures::sMIDIMeasure >();
			nChannel = (m->status & 0x0F) + 1;
			return true;
		}
	};

	class VistaMIDIDriverControllerGet : public IVistaMeasureTranscode::IntGet
	{
	public:
		VistaMIDIDriverControllerGet()
			: IVistaMeasureTranscode::IntGet(
			"CONTROLLER",
			VistaMIDIDriverTranscode::GetTypeString(),
			"The CC controller number (0-119). The values 120-127 are reserved messages." )
		{}

		virtual int GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			const VistaMIDIMeasures::sMIDIMeasure *m =
				pMeasure->getRead< VistaMIDIMeasures::sMIDIMeasure >();

			// the lower 7 bit of the the first data byte (second message byte)
			// of a midi message contain the controller number for a CC message.
			return (m->data0 & 0x7F);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nController) const
		{
			const VistaMIDIMeasures::sMIDIMeasure *m =
				pMeasure->getRead< VistaMIDIMeasures::sMIDIMeasure >();
			nController = (m->data0 & 0x7F);
			return true;
		}
	};

	class VistaMIDIDriverValueGet : public IVistaMeasureTranscode::IntGet
	{
	public:
		VistaMIDIDriverValueGet()
			: IVistaMeasureTranscode::IntGet(
			"VALUE",
			VistaMIDIDriverTranscode::GetTypeString(),
			"The CC control value (0-127)." )
		{}

		virtual int GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			const VistaMIDIMeasures::sMIDIMeasure *m =
				pMeasure->getRead< VistaMIDIMeasures::sMIDIMeasure >();

			// the lower 7 bit of the the second data byte (third message byte)
			// of a midi message contain the control value for a CC message.
			return (m->data1 & 0x7F);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nValue) const
		{
			const VistaMIDIMeasures::sMIDIMeasure *m =
				pMeasure->getRead< VistaMIDIMeasures::sMIDIMeasure >();
			nValue = (m->data1 & 0x7F);
			return true;
		}
	};

	static IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaMIDIDriverTypeGet,
		new VistaMIDIDriverChannelGet,
		new VistaMIDIDriverControllerGet,
		new VistaMIDIDriverValueGet
	};

	class VistaMIDIDriverTranscodeFactory : public TDefaultTranscoderFactory<VistaMIDIDriverTranscode>
	{
	public:
		VistaMIDIDriverTranscodeFactory()
			: TDefaultTranscoderFactory<VistaMIDIDriverTranscode>(VistaMIDIDriverTranscode::GetTypeString())
		{}
	};
}

#ifdef VISTAMIDITRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<VistaMIDIDriverTranscodeFactory> )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<VistaMIDIDriverTranscodeFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaMIDIDriverTranscodeFactory>)


