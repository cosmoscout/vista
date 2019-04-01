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
#include "Vista3DCSpaceNavigatorDriver.h"

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
	class Vista3DCTranscode : public IVistaMeasureTranscode
	{
	public:
		Vista3DCTranscode()
		{
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "Vista3DCTranscode"; }
		REFL_INLINEIMP(Vista3DCTranscode, IVistaMeasureTranscode);
	};

	class Vista3DCSNPosTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		Vista3DCSNPosTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION",
			Vista3DCTranscode::GetTypeString(),
									"space navigator 3D position values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			const Vista3DCSpaceNavigator::_sMeasure *m = pMeasure->getRead< Vista3DCSpaceNavigator::_sMeasure >();

			return VistaVector3D(
						float( m->m_nPositionX),
						float( m->m_nPositionY),
						float( m->m_nPositionZ));
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class Vista3DCSNAxisTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<VistaAxisAndAngle>
	{
	public:
		Vista3DCSNAxisTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<VistaAxisAndAngle>("AXIS",
									Vista3DCTranscode::GetTypeString(),
									"space navigator 3D axis and angle value")
		{}

		virtual VistaAxisAndAngle GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			const Vista3DCSpaceNavigator::_sMeasure *m = pMeasure->getRead< Vista3DCSpaceNavigator::_sMeasure >();

			VistaAxisAndAngle aaa(
						VistaVector3D(float( m->m_nRotationX),
									   float( m->m_nRotationY),
									   float( m->m_nRotationZ)),
									   float(Vista::DegToRad(float(m->m_nRotationAngle))));

			if(_isnan(aaa.m_v3Axis[0]) || _isnan(aaa.m_v3Axis[1]) || _isnan(aaa.m_v3Axis[2]) )
				return VistaAxisAndAngle( VistaVector3D(0,0,-1), 0 );

			return aaa;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaAxisAndAngle &aaa) const
		{
			aaa = GetValue(pMeasure);
			return true;
		}
	};

	class Vista3DCSNOrientationTranscode : public IVistaMeasureTranscode::QuatGet
	{
	public:
		Vista3DCSNOrientationTranscode()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			Vista3DCTranscode::GetTypeString(),
									"space navigator 3D orientation (right-handed)") {}

		virtual VistaQuaternion    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			const Vista3DCSpaceNavigator::_sMeasure *m = pMeasure->getRead< Vista3DCSpaceNavigator::_sMeasure >();

			VistaAxisAndAngle aaa(VistaVector3D(float( m->m_nRotationX),
												  float( m->m_nRotationY),
												  float( m->m_nRotationZ)),
												  Vista::DegToRad(float(m->m_nRotationAngle)));

			if(_isnan(aaa.m_v3Axis[0]) || _isnan(aaa.m_v3Axis[1]) || _isnan(aaa.m_v3Axis[2]) )
				return VistaQuaternion();

			if(aaa.m_v3Axis.GetLength() < Vista::Epsilon)
				return VistaQuaternion();

			VistaQuaternion q(aaa);
			q.Normalize();

			return q;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &qRot) const
		{
			qRot = GetValue(pMeasure);
			return true;
		}
	};

	class Vista3DCSNButtonTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
	{
	public:
		Vista3DCSNButtonTranscode(int nBt,
			const std::string &strLabel,
			const std::string &strDesc)
			: IVistaMeasureTranscode::TTranscodeValueGet<bool>(strLabel,
			Vista3DCTranscode::GetTypeString(), strDesc),
			m_nBt(nBt)
		{}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			const Vista3DCSpaceNavigator::_sMeasure *m = pMeasure->getRead< Vista3DCSpaceNavigator::_sMeasure >();

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
		new Vista3DCSNPosTranscode,
		new Vista3DCSNAxisTranscode,
		new Vista3DCSNOrientationTranscode,
		new Vista3DCSNButtonTranscode(0, "BUTTON_1", "Left button of 3DC SpaceNavigator"),
		new Vista3DCSNButtonTranscode(1, "BUTTON_2", "Right button of 3DC SpaceNavigator"),
		NULL
	};

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	class Vista3DCSpaceNavigatorTranscodeFactory : public TDefaultTranscoderFactory<Vista3DCTranscode>
	{
	public:
		Vista3DCSpaceNavigatorTranscodeFactory()
		: TDefaultTranscoderFactory<Vista3DCTranscode>(Vista3DCTranscode::GetTypeString())
		{}
	};
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


#ifdef VISTA3DCSPACENAVIGATORTRANSCODER_EXPORTS
	DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<Vista3DCSpaceNavigatorTranscodeFactory> )
#else
	DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<Vista3DCSpaceNavigatorTranscodeFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<Vista3DCSpaceNavigatorTranscodeFactory>)


