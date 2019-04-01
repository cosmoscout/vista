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


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>


// ****************************************************************************
// PARAMETER API IMPLEMENTATION
// ****************************************************************************

namespace
{

	class VistaSpaceNavigatorTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaSpaceNavigatorTranscode()
		{
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "VistaSpaceNavigatorTranscode"; }
		REFL_INLINEIMP(VistaSpaceNavigatorTranscode, IVistaMeasureTranscode);
	};

	class VistaSpaceNavigatorTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaSpaceNavigatorTranscode;
		}
		virtual void DestroyTranscoder( IVistaMeasureTranscode *transcoder )
		{
			delete transcoder;
		}
		virtual std::string GetTranscoderName() const { return "VistaSpaceNavigatorTranscodeFactory"; }


	};


}

// ############################################################################

#ifdef VISTASPACENAVIGATORTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<VistaSpaceNavigatorTranscodeFactory> )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<VistaSpaceNavigatorTranscodeFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaSpaceNavigatorTranscodeFactory>)


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*                                               */
/*============================================================================*/


