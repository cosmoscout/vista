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
#include "VistaCHAI3DHapticDevicesCommonShare.h"

namespace
{
	class VistaCHAI3DHapticDevicesDriverMeasureTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaCHAI3DHapticDevicesDriverMeasureTranscode()
		{
			// inherited as protected member
			m_nNumberOfScalars = 13;
		}

		virtual ~VistaCHAI3DHapticDevicesDriverMeasureTranscode() {}
		static std::string GetTypeString() { return "VistaCHAI3DHapticDevicesDriverMeasureTranscode"; }
		REFL_INLINEIMP(VistaCHAI3DHapticDevicesDriverMeasureTranscode, IVistaMeasureTranscode);
	};

	class VistaCHAI3DHapticDevicesPosTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaCHAI3DHapticDevicesPosTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION",
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
			"CHAI3DHapticDevices 3D position values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m =
				(VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];

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

	class VistaCHAI3DHapticDevicesVelocityTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaCHAI3DHapticDevicesVelocityTranscode()
			: IVistaMeasureTranscode::V3Get("VELOCITY",
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
			"CHAI3DHapticDevices 3D position surface contact point values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m = (VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];

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

	class VistaCHAI3DHapticDevicesForceReadTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaCHAI3DHapticDevicesForceReadTranscode()
			: IVistaMeasureTranscode::V3Get("FORCE",
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
			"CHAI3DHapticDevices 3D force values") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m = (VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];

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

	class VistaCHAI3DHapticDevicesScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaCHAI3DHapticDevicesScalarTranscode()
			: IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
			"CHAI3DHapticDevices scalar values") {}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar,
			unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m = (VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];

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

	class VistaCHAI3DHapticDevicesRotationGet : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaCHAI3DHapticDevicesRotationGet()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
			"CHAI3DHapticDevices body orientation") {}

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

			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m = (VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];
			VistaTransformMatrix t (
				float(m->m_afRotMatrix[0]), float(m->m_afRotMatrix[1]), float(m->m_afRotMatrix[2]), 0,
				float(m->m_afRotMatrix[4]), float(m->m_afRotMatrix[5]), float(m->m_afRotMatrix[6]), 0,
				float(m->m_afRotMatrix[8]), float(m->m_afRotMatrix[9]), float(m->m_afRotMatrix[10]), 0,
				0,                          0,                          0, 1);

			qQuat = -VistaQuaternion(t);
			return true;
		}
	};

	class VistaCHAI3DHapticDevicesVecFieldGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		enum eField
		{
			FLD_ANG_VEL=0,
			FLD_JOINT_ANG,
			FLG_GIMBAL_ANG,
			FLD_TORQUE
		};

		VistaCHAI3DHapticDevicesVecFieldGet(eField eFld,
			const std::string &strName, const std::string &strDesc)
			: IVistaMeasureTranscode::V3Get(strName,
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
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
			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m = (VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];

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
	class VistaCHAI3DHapticDevicesVectorFloatGet : public IVistaMeasureTranscode::TTranscodeValueGet< typename std::vector< T > >
	{
	public:
		enum eField
		{
			FLD_OVERHEATSTATE = 0,
			FLD_ENCODER_VALUES,
			FLD_ROTMATRIX
		};

		VistaCHAI3DHapticDevicesVectorFloatGet( eField eFld,
			const std::string &strName, const std::string &strDesc )
			: IVistaMeasureTranscode::TTranscodeValueGet< std::vector< T > >(strName,
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
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
			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m = (VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];
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

	class VistaCHAI3DHapticDevicesBtGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
	{
	public:
		enum eField
		{
			FLD_BT1=0,
			FLD_BT2,
			FLD_INKSWITCH
		};

		VistaCHAI3DHapticDevicesBtGet( eField eFld,
			const std::string &strName, const std::string &strDesc )
			: IVistaMeasureTranscode::TTranscodeValueGet<bool>( strName,
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
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
			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m = (VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];
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

	class VistaCHAI3DHapticDevicesTransformationGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>
	{
	public:
		VistaCHAI3DHapticDevicesTransformationGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>( "TRANSFORMATION",
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
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
			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m = (VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];
			VistaTransformMatrix mt1(m->m_afRotMatrix);
			mt1.GetTranspose(mt); // CHAI3DHapticDevices reports in column major, vista needs row-major
			return true;
		}
	};

	class VistaCHAI3DHapticDevicesButtonMaskGet : public IVistaMeasureTranscode::TTranscodeValueGet<int>
	{
	public:
		VistaCHAI3DHapticDevicesButtonMaskGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<int>( "BUTTONMASK",
			VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString(),
			"the buttonmask as an int" )
		{
		}

		int GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure *m = (VistaCHAI3DHapticDevicesMeasures::sCHAI3DHapticDevicesMeasure*)&(*pMeasure).m_vecMeasures[0];
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
		new VistaCHAI3DHapticDevicesPosTranscode,
		new VistaCHAI3DHapticDevicesForceReadTranscode,
		new VistaCHAI3DHapticDevicesScalarTranscode,
		new VistaCHAI3DHapticDevicesVelocityTranscode,
		new VistaCHAI3DHapticDevicesRotationGet,
		new VistaCHAI3DHapticDevicesTransformationGet,
		new VistaCHAI3DHapticDevicesVecFieldGet( VistaCHAI3DHapticDevicesVecFieldGet::FLD_ANG_VEL, "ANGULAR_VELOCITY",
		"the angular velocity as reported by the CHAI3DHapticDevices"),
		new VistaCHAI3DHapticDevicesVecFieldGet( VistaCHAI3DHapticDevicesVecFieldGet::FLD_JOINT_ANG, "JOINT_ANGLE",
		"the joint angle of the gimbal as reported by the CHAI3DHapticDevices"),
		new VistaCHAI3DHapticDevicesVecFieldGet( VistaCHAI3DHapticDevicesVecFieldGet::FLG_GIMBAL_ANG, "GIMBAL_ANGLE",
		"the gimbal angle as reported by the CHAI3DHapticDevices"),
		new VistaCHAI3DHapticDevicesVecFieldGet( VistaCHAI3DHapticDevicesVecFieldGet::FLD_TORQUE, "TORQUE",
		"the torque as reported by the CHAI3DHapticDevices"),

		new VistaCHAI3DHapticDevicesVectorFloatGet<float>( VistaCHAI3DHapticDevicesVectorFloatGet<float>::FLD_OVERHEATSTATE,
		"OVERHEATSTATE", "the overheat state vector (6-float)"),
		new VistaCHAI3DHapticDevicesVectorFloatGet<float>( VistaCHAI3DHapticDevicesVectorFloatGet<float>::FLD_OVERHEATSTATE,
		"VROTMATRIX", "the rotation matrix as vector of (16-float)"),
		new VistaCHAI3DHapticDevicesVectorFloatGet<float>( VistaCHAI3DHapticDevicesVectorFloatGet<float>::FLD_ENCODER_VALUES,
		"ENCODER_VALUES", "the raw encoder value vector (6-long given as floats)"),
		new VistaCHAI3DHapticDevicesBtGet( VistaCHAI3DHapticDevicesBtGet::FLD_BT1, "BUTTON_1", "state of button 1 (front)" ),
		new VistaCHAI3DHapticDevicesBtGet( VistaCHAI3DHapticDevicesBtGet::FLD_BT2, "BUTTON_2", "state of button 2 (back)" ),
		new VistaCHAI3DHapticDevicesBtGet( VistaCHAI3DHapticDevicesBtGet::FLD_INKSWITCH, "INKWELLSWITCH", "state of the inkwell switch" ),
		new VistaCHAI3DHapticDevicesButtonMaskGet,
		NULL
	};
}

/*
class VistaCHAI3DHapticDevicesTranscoderFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder()
	{
		return new VistaCHAI3DHapticDevicesDriverMeasureTranscode;
	}
};
*/

class VistaCHAI3DHapticDevicesTranscoderFactory : public TDefaultTranscoderFactory<VistaCHAI3DHapticDevicesDriverMeasureTranscode>
{
public:
	VistaCHAI3DHapticDevicesTranscoderFactory()
		: TDefaultTranscoderFactory<VistaCHAI3DHapticDevicesDriverMeasureTranscode>(VistaCHAI3DHapticDevicesDriverMeasureTranscode::GetTypeString())
	{}
};

#ifdef VISTACHAI3DHAPTICDEVICESTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<VistaCHAI3DHapticDevicesTranscoderFactory> )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<VistaCHAI3DHapticDevicesTranscoderFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaCHAI3DHapticDevicesTranscoderFactory>)

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/