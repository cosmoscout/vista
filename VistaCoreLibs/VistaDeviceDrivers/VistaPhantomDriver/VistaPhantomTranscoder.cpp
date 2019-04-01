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
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include "VistaPhantomCommonShare.h"

namespace
{
	class VistaPhantomDriverMeasureTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaPhantomDriverMeasureTranscode()
		{
			// inherited as protected member
			m_nNumberOfScalars = 13;
		}

		virtual ~VistaPhantomDriverMeasureTranscode() {}
		static std::string GetTypeString() { return "VistaPhantomDriverMeasureTranscode"; }
		REFL_INLINEIMP(VistaPhantomDriverMeasureTranscode, IVistaMeasureTranscode);
	};

	class VistaPhantomPosTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaPhantomPosTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION",
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			"phantom 3D position values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaPhantomMeasures::sPhantomMeasure *m =
				(VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

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


	class VistaPhantomVelocityTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaPhantomVelocityTranscode()
			: IVistaMeasureTranscode::V3Get("VELOCITY",
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			"phantom 3D position surface contact point values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaPhantomMeasures::sPhantomMeasure *m = (VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

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

	class VistaPhantomForceReadTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaPhantomForceReadTranscode()
			: IVistaMeasureTranscode::V3Get("FORCE",
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			"phantom 3D force values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaPhantomMeasures::sPhantomMeasure *m = (VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

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

	class VistaPhantomScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaPhantomScalarTranscode()
			: IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			"Phantom scalar values") {}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar,
			unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			VistaPhantomMeasures::sPhantomMeasure *m = (VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

			switch(nIndex)
			{
			case 0:
			case 1:
				dScalar = (m->m_nButtonState == nIndex+1) ? 1.0 : 0.0;
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
			case 12:
				dScalar = m->m_nUpdateRate;
				break;

			default:
				return false;
			}
			return true;
		}
	};

	class VistaPhantomRotationGet : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaPhantomRotationGet()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
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

			VistaPhantomMeasures::sPhantomMeasure *m = (VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
			VistaTransformMatrix t (
				float(m->m_afRotMatrix[0]), float(m->m_afRotMatrix[1]), float(m->m_afRotMatrix[2]), 0,
				float(m->m_afRotMatrix[4]), float(m->m_afRotMatrix[5]), float(m->m_afRotMatrix[6]), 0,
				float(m->m_afRotMatrix[8]), float(m->m_afRotMatrix[9]), float(m->m_afRotMatrix[10]), 0,
				0,                          0,                          0, 1);

			qQuat = -VistaQuaternion(t);
			return true;
		}
	};

	class VistaPhantomVecFieldGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		enum eField
		{
			FLD_ANG_VEL=0,
			FLD_JOINT_ANG,
			FLG_GIMBAL_ANG,
			FLD_TORQUE
		};


		VistaPhantomVecFieldGet(eField eFld,
			const std::string &strName, const std::string &strDesc)
			: IVistaMeasureTranscode::V3Get(strName,
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			strDesc),
			m_eFld(eFld)
		{}

		VistaVector3D GetValue(const VistaSensorMeasure *pMeasure) const
		{
			VistaVector3D v3;
			GetValue(pMeasure, v3);
			return v3;
		}

		bool GetValue(const VistaSensorMeasure * pMeasure, VistaVector3D &v3) const
		{
			VistaPhantomMeasures::sPhantomMeasure *m = (VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];

			switch( m_eFld )
			{
			case FLD_ANG_VEL:
				{
					v3[0] = m->m_afAngularVelocity[0];
					v3[1] = m->m_afAngularVelocity[1];
					v3[2] = m->m_afAngularVelocity[2];
					return true;
				}
			case FLD_JOINT_ANG:
				{
					v3[0] = m->m_afJointAngles[0];
					v3[1] = m->m_afJointAngles[1];
					v3[2] = m->m_afJointAngles[2];
					return true;
				}
			case FLG_GIMBAL_ANG:
				{
					v3[0] = m->m_afGimbalAngles[0];
					v3[1] = m->m_afGimbalAngles[1];
					v3[2] = m->m_afGimbalAngles[2];
					return true;
				}
			case FLD_TORQUE:
				{
					v3[0] = m->m_afTorque[0];
					v3[1] = m->m_afTorque[1];
					v3[2] = m->m_afTorque[2];
					return true;
				}
			default:
				break;
			}
			return false;
		}
	private:
		eField m_eFld;
	};


	template<class T>
	class VistaPhantomVectorFloatGet : public IVistaMeasureTranscode::TTranscodeValueGet< typename std::vector< T > >
	{
	public:
		enum eField
		{
			FLD_OVERHEATSTATE = 0,
			FLD_ENCODER_VALUES,
			FLD_ROTMATRIX
		};

		VistaPhantomVectorFloatGet( eField eFld,
			const std::string &strName, const std::string &strDesc )
			: IVistaMeasureTranscode::TTranscodeValueGet< std::vector< T > >(strName,
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			strDesc),
			m_eFld(eFld)
		{
		}

		std::vector<T> GetValue(const VistaSensorMeasure *pMeasure) const
		{
			std::vector<T> v3;
			GetValue(pMeasure, v3);
			return v3;
		}

		bool GetValue(const VistaSensorMeasure * pMeasure, std::vector<T> &v3) const
		{
			VistaPhantomMeasures::sPhantomMeasure *m = (VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
			switch(m_eFld)
			{
			case FLD_OVERHEATSTATE:
				{
					v3.resize(6);
					for(typename std::vector<T>::size_type n = 0; n<6; ++n)
						v3[n] = m->m_afOverheatState[n];
					return true;
				}
			case FLD_ENCODER_VALUES:
				{
					v3.resize(6);
					for(typename std::vector<T>::size_type n = 0; n<6; ++n)
						v3[n] = T(m->m_nEncoderValues[n]);
					return true;
				}
			case FLD_ROTMATRIX:
				{
					v3.resize(16);
					for(typename std::vector<T>::size_type n = 0; n<16; ++n)
						v3[n] = m->m_afRotMatrix[n];
					return true;
				}
			default:
				break;
			}

			return false;
		}
	private:
		eField m_eFld;

	};

	class VistaPhantomBtGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
	{
	public:
		enum eField
		{
			FLD_BT1=0,
			FLD_BT2,
			FLD_INKSWITCH
		};

		VistaPhantomBtGet( eField eFld,
			const std::string &strName, const std::string &strDesc )
			: IVistaMeasureTranscode::TTranscodeValueGet<bool>( strName,
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			strDesc ),
			m_eFld(eFld)
		{
		}

		bool GetValue(const VistaSensorMeasure *pMeasure) const
		{
			bool bRet;
			if(GetValue(pMeasure, bRet))
				return bRet;

			return false;
		}

		bool GetValue(const VistaSensorMeasure * pMeasure, bool &v3) const
		{
			VistaPhantomMeasures::sPhantomMeasure *m = (VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
			switch(m_eFld)
			{
			case FLD_BT1:
			case FLD_BT2:
				{
					v3 = ((m->m_nButtonState & int(m_eFld)+1) ? true : false);
					return true;
				}
			case FLD_INKSWITCH:
				{
					v3 = !m->m_bInkwellSwitch;
					return true;
				}
			default:
				break;
			}

			return false;
		}

	private:
		eField m_eFld;

	};

	class VistaPhantomTransformationGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>
	{
	public:
		VistaPhantomTransformationGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>( "TRANSFORMATION",
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			"the complete transformation as a vista transform matrix" )
		{
		}


		VistaTransformMatrix GetValue(const VistaSensorMeasure *pMeasure) const
		{
			VistaTransformMatrix m;
			GetValue(pMeasure, m);
			return m;
		}

		bool GetValue(const VistaSensorMeasure *pMeasure, VistaTransformMatrix &mt) const
		{
			VistaPhantomMeasures::sPhantomMeasure *m = (VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
			VistaTransformMatrix mt1(m->m_afRotMatrix);
			mt1.GetTranspose(mt); // phantom reports in column major, vista needs row-major
			return true;
		}
	};

	class VistaPhantomButtonMaskGet : public IVistaMeasureTranscode::TTranscodeValueGet<int>
	{
	public:
		VistaPhantomButtonMaskGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<int>( "BUTTONMASK",
			VistaPhantomDriverMeasureTranscode::GetTypeString(),
			"the buttonmask as an int" )
		{
		}

		int GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			VistaPhantomMeasures::sPhantomMeasure *m = (VistaPhantomMeasures::sPhantomMeasure*)&(*pMeasure).m_vecMeasures[0];
			return m->m_nButtonState;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure, int &value ) const
		{
			value = GetValue(pMeasure);
			return true;
		}
	};

	static IVistaPropertyGetFunctor *SapGetter[] =
	{
		new VistaPhantomPosTranscode,
		new VistaPhantomForceReadTranscode,
		new VistaPhantomScalarTranscode,
		new VistaPhantomVelocityTranscode,
		new VistaPhantomRotationGet,
		new VistaPhantomTransformationGet,
		new VistaPhantomVecFieldGet( VistaPhantomVecFieldGet::FLD_ANG_VEL, "ANGULAR_VELOCITY",
		"the angular velocity as reported by the phantom"),
		new VistaPhantomVecFieldGet( VistaPhantomVecFieldGet::FLD_JOINT_ANG, "JOINT_ANGLE",
		"the joint angle of the gimbal as reported by the phantom"),
		new VistaPhantomVecFieldGet( VistaPhantomVecFieldGet::FLG_GIMBAL_ANG, "GIMBAL_ANGLE",
		"the gimbal angle as reported by the phantom"),
		new VistaPhantomVecFieldGet( VistaPhantomVecFieldGet::FLD_TORQUE, "TORQUE",
		"the torque as reported by the phantom"),

		new VistaPhantomVectorFloatGet<float>( VistaPhantomVectorFloatGet<float>::FLD_OVERHEATSTATE,
		"OVERHEATSTATE", "the overheat state vector (6-float)"),
		new VistaPhantomVectorFloatGet<float>( VistaPhantomVectorFloatGet<float>::FLD_OVERHEATSTATE,
		"VROTMATRIX", "the rotation matrix as vector of (16-float)"),
		new VistaPhantomVectorFloatGet<float>( VistaPhantomVectorFloatGet<float>::FLD_ENCODER_VALUES,
		"ENCODER_VALUES", "the raw encoder value vector (6-long given as floats)"),
		new VistaPhantomBtGet( VistaPhantomBtGet::FLD_BT1, "BUTTON_1", "state of button 1 (front)" ),
		new VistaPhantomBtGet( VistaPhantomBtGet::FLD_BT2, "BUTTON_2", "state of button 2 (back)" ),
		new VistaPhantomBtGet( VistaPhantomBtGet::FLD_INKSWITCH, "INKWELLSWITCH", "state of the inkwell switch" ),
		new VistaPhantomButtonMaskGet,
		NULL
	};
}

/*
class VistaPhantomTranscoderFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaPhantomDriverMeasureTranscode;
	}
};
*/

class VistaPhantomTranscoderFactory : public TDefaultTranscoderFactory<VistaPhantomDriverMeasureTranscode>
{
public:
	VistaPhantomTranscoderFactory()
		: TDefaultTranscoderFactory<VistaPhantomDriverMeasureTranscode>(VistaPhantomDriverMeasureTranscode::GetTypeString())
	{}
};

#ifdef VISTAPHANTOMTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<VistaPhantomTranscoderFactory> )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<VistaPhantomTranscoderFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaPhantomTranscoderFactory>)


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


