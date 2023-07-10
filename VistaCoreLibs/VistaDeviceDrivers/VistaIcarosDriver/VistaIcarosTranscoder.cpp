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
#include "VistaIcarosDriver.h"
#include <VistaMath/VistaMathTools.h>

#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/

#if !defined(_isnan)
#define _isnan(A) (A != A)
#endif

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

namespace
{
	class VistaIcarosTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaIcarosTranscode()
		{
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "VistaIcarosTranscode"; }
		REFL_INLINEIMP(VistaIcarosTranscode, IVistaMeasureTranscode);
	};

	class VistaIcarosControllerTranscode : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaIcarosControllerTranscode()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			VistaIcarosTranscode::GetTypeString(),
									"icaros controller orientation") {}

		virtual VistaQuaternion GetValue(const VistaSensorMeasure *pMeasure) const
		{
			const VistaIcarosController::_sMeasure *m = pMeasure->getRead< VistaIcarosController::_sMeasure >();
			return VistaQuaternion(
						float( m->m_fQuat1),
						float( m->m_fQuat2),
						float( -m->m_fQuat3),
						float( m->m_fQuat4));
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &quat) const
		{
			quat = GetValue(pMeasure);
			return true;
		}
	};

	class VistaIcarosButtonTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
	{
	public:
		VistaIcarosButtonTranscode(int nBt,
			const std::string &strLabel,
			const std::string &strDesc)
			: IVistaMeasureTranscode::TTranscodeValueGet<bool>(strLabel,
			VistaIcarosTranscode::GetTypeString(), strDesc),
			m_nBt(nBt)
		{}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			const VistaIcarosController::_sMeasure *m = pMeasure->getRead< VistaIcarosController::_sMeasure >();

			return ((m->m_nKeys[m_nBt]==1) ? true : false);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &nState) const
		{
			nState = GetValue(pMeasure);
			return true;
		}

		int m_nBt;
	};


	IVistaPropertyGetFunctor *SapGetter[] =
	{
		new VistaIcarosControllerTranscode,
		new VistaIcarosButtonTranscode(0, "BUTTON_1", "First button of the icaros controller"),
		new VistaIcarosButtonTranscode(1, "BUTTON_2", "Second button of the icaros controller"),
		new VistaIcarosButtonTranscode(2, "BUTTON_3", "Third button of the icaros controller"),
		new VistaIcarosButtonTranscode(3, "BUTTON_4", "Fourth button of the icaros controller"),
		NULL
	};

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	class VistaIcarosTranscodeFactory : public TDefaultTranscoderFactory<VistaIcarosTranscode>
	{
	public:
		VistaIcarosTranscodeFactory()
		: TDefaultTranscoderFactory<VistaIcarosTranscode>(VistaIcarosTranscode::GetTypeString())
		{}
	};
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


#ifdef VISTAICAROSTRANSCODER_EXPORTS
	DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<VistaIcarosTranscodeFactory> )
#else
	DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<VistaIcarosTranscodeFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaIcarosTranscodeFactory>)


