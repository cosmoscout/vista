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
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include "VistaPhantomServerCommonShare.h"
#include "VistaPhantomServerCommonShare.h"


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace
{
	class VistaPhantomServerDriverMeasureTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaPhantomServerDriverMeasureTranscode()
		{
			// inherited as protected member
			m_nNumberOfScalars = 13;
		}

		virtual ~VistaPhantomServerDriverMeasureTranscode() {}
		static std::string GetTypeString() { return "VistaPhantomServerDriverMeasureTranscode"; }
		REFL_INLINEIMP(VistaPhantomServerDriverMeasureTranscode, IVistaMeasureTranscode);
	};

	class VistaPhantomServerPosTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaPhantomServerPosTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION",
			VistaPhantomServerDriverMeasureTranscode::GetTypeString(),
			"phantom 3D position values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			const VistaPhantomServerMeasures::sPhantomServerMeasure *m = pMeasure->getRead< VistaPhantomServerMeasures::sPhantomServerMeasure >();

			return VistaVector3D(m->m_afPosition[0],
				m->m_afPosition[1],
				m->m_afPosition[2]);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaPhantomServerPosSCPTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaPhantomServerPosSCPTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION_SCP",
			VistaPhantomServerDriverMeasureTranscode::GetTypeString(),
			"phantom 3D position surface contact point values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			const VistaPhantomServerMeasures::sPhantomServerMeasure *m = pMeasure->getRead< VistaPhantomServerMeasures::sPhantomServerMeasure >();

			return VistaVector3D(m->m_afPosSCP[0],
				m->m_afPosSCP[1],
				m->m_afPosSCP[2]);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaPhantomServerVelocityTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaPhantomServerVelocityTranscode()
			: IVistaMeasureTranscode::V3Get("VELOCITY",
			VistaPhantomServerDriverMeasureTranscode::GetTypeString(),
			"phantom 3D position surface contact point values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			const VistaPhantomServerMeasures::sPhantomServerMeasure *m = pMeasure->getRead< VistaPhantomServerMeasures::sPhantomServerMeasure >();

			return VistaVector3D(m->m_afVelocity[0],
				m->m_afVelocity[1],
				m->m_afVelocity[2]);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaPhantomServerForceReadTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaPhantomServerForceReadTranscode()
			: IVistaMeasureTranscode::V3Get("FORCE",
			VistaPhantomServerDriverMeasureTranscode::GetTypeString(),
			"phantom 3D force values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			const VistaPhantomServerMeasures::sPhantomServerMeasure *m = pMeasure->getRead< VistaPhantomServerMeasures::sPhantomServerMeasure >();

			return VistaVector3D(m->m_afForce[0],
				m->m_afForce[1],
				m->m_afForce[2]);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaPhantomServerScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaPhantomServerScalarTranscode()
			: IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
			VistaPhantomServerDriverMeasureTranscode::GetTypeString(),
			"Phantom scalar values") {}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar,
			unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			const VistaPhantomServerMeasures::sPhantomServerMeasure *m = pMeasure->getRead< VistaPhantomServerMeasures::sPhantomServerMeasure >();

			switch(nIndex)
			{
			case 0:
			case 1:
				dScalar = (m->m_nButtonState == (int)nIndex + 1) ? 1.0 : 0.0;
				break;
			case 2:
				{
					float dVal = m->m_afOverheatState[0];

					for(int i=1; i<6; i++)
						if(dVal < m->m_afOverheatState[i])
							dVal = m->m_afOverheatState[i];
					dScalar = dVal;
					break;
				}
			case 3:
				{
					dScalar = m->m_afForce[0];
					break;
				}
			case 4:
				{
					dScalar = m->m_afForce[1];
					break;
				}
			case 5:
				{
					dScalar = m->m_afForce[2];
					break;
				}
			case 6:
				{
					dScalar = m->m_afVelocity[0];
					break;
				}
			case 7:
				{
					dScalar = m->m_afVelocity[1];
					break;
				}
			case 8:
				{
					dScalar = m->m_afVelocity[2];
					break;
				}
			case 9:
				{
					dScalar = m->m_afPosSCP[0];
					break;
				}
			case 10:
				{
					dScalar = m->m_afPosSCP[1];
					break;
				}
			case 11:
				{
					dScalar = m->m_afPosSCP[2];
					break;
				}
			case 12:
				dScalar = m->m_nUpdateRate;
				break;

			default:
				return false;
			}
			return true;
		}
	};

	class VistaPhantomServerRotationGet : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaPhantomServerRotationGet()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			VistaPhantomServerDriverMeasureTranscode::GetTypeString(),
			"phantom body orientation") {}

		VistaQuaternion GetValue(const VistaSensorMeasure *pMeasure) const
		{
			VistaQuaternion q;
			GetValue(pMeasure, q);
			return q;
		}

		bool GetValue(const VistaSensorMeasure * pMeasure, VistaQuaternion &qQuat) const
		{
			if(!pMeasure)
				return false;

			const VistaPhantomServerMeasures::sPhantomServerMeasure *m = pMeasure->getRead< VistaPhantomServerMeasures::sPhantomServerMeasure >();
			VistaTransformMatrix t (
				float(m->m_afRotMatrix[0]), float(m->m_afRotMatrix[1]), float(m->m_afRotMatrix[2]), 0,
				float(m->m_afRotMatrix[3]), float(m->m_afRotMatrix[4]), float(m->m_afRotMatrix[5]), 0,
				float(m->m_afRotMatrix[6]), float(m->m_afRotMatrix[7]), float(m->m_afRotMatrix[8]), 0,
				0,                          0,                          0, 1);

			qQuat = -VistaQuaternion(t);
			return true;
		}
	};

	static IVistaPropertyGetFunctor *SapGetter[] =
	{
		new VistaPhantomServerPosTranscode,
		new VistaPhantomServerForceReadTranscode,
		new VistaPhantomServerScalarTranscode,
		new VistaPhantomServerPosSCPTranscode,
		new VistaPhantomServerVelocityTranscode,
		new VistaPhantomServerRotationGet,
		NULL
	};
}

/*
class VistaPhantomServerTranscoderFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaPhantomServerDriverMeasureTranscode;
	}
};
*/

class VistaPhantomServerTranscoderFactory : public TDefaultTranscoderFactory<VistaPhantomServerDriverMeasureTranscode>
{
public:
	VistaPhantomServerTranscoderFactory()
		: TDefaultTranscoderFactory<VistaPhantomServerDriverMeasureTranscode>(VistaPhantomServerDriverMeasureTranscode::GetTypeString())
	{}
};

#ifdef VISTAPHANTOMSERVERTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<VistaPhantomServerTranscoderFactory> )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<VistaPhantomServerTranscoderFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaPhantomServerTranscoderFactory>)
