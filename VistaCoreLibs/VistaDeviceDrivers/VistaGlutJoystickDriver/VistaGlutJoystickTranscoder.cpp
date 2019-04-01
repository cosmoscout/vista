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
#include "VistaGlutJoystickDriver.h"

#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/

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
	// ###########################################################################################
	// ###########################################################################################

	class VistaJoystickMeasureTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaJoystickMeasureTranscode()
		{

			// we have "virtually" 36 scalars
			// 32 btn + 1 btnMask + 3 Axes = 36 scalars
			m_nNumberOfScalars = 36; // inherited as protected member
		}

		virtual ~VistaJoystickMeasureTranscode() {}
		static std::string GetTypeString() { return "VistaJoystickMeasureTranscode"; }
		REFL_INLINEIMP(VistaJoystickMeasureTranscode, IVistaMeasureTranscode);
	};

	// ###########################################################################################
	// ###########################################################################################

	// FACTORY
	class VistaJoystickTranscoderFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaJoystickMeasureTranscode;
		}

		virtual void DestroyTranscoder( IVistaMeasureTranscode *transc )
		{
			delete transc;
		}

		virtual std::string GetTranscoderName() const { return VistaJoystickMeasureTranscode::GetTypeString(); }

	protected:
	};


	// ###########################################################################################
	// ###########################################################################################


	class VistaJoystickPositionTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaJoystickPositionTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION",
			VistaJoystickMeasureTranscode::GetTypeString(),
									"joystick glut axes position") {}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *msr)    const
		{
			VistaVector3D v3Ret;
			GetValue(msr, v3Ret);
			return v3Ret;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Value) const
		{
			if(!pMeasure)
				return false;

			const VistaGlutJoystickDriver::_sJoyMeasure *m
			        = (*pMeasure).getRead<VistaGlutJoystickDriver::_sJoyMeasure>();

			v3Value[0] = float(m->m_nVals[1]);
			v3Value[1] = float(m->m_nVals[2]);
			v3Value[2] = float(m->m_nVals[3]);
			return true;
		}
	};

	class VistaJoystickOrientationTranscode : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaJoystickOrientationTranscode()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			VistaJoystickMeasureTranscode::GetTypeString(),
									"joystick glut axes orientation") {}

		virtual VistaQuaternion GetValue(const VistaSensorMeasure *msr)    const
		{
			VistaQuaternion qRet;
			GetValue(msr, qRet);
			return qRet;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &qValue) const
		{
			if(!pMeasure)
				return false;

			const VistaGlutJoystickDriver::_sJoyMeasure *m
						= (*pMeasure).getRead<VistaGlutJoystickDriver::_sJoyMeasure>();

			qValue = VistaQuaternion( VistaVector3D(0,0,1),
									   VistaVector3D( float(m->m_nVals[1]),
									   float(m->m_nVals[2]),
									   float(m->m_nVals[3]) ) );

			return true;
		}
	};

	class VistaJoystickScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaJoystickScalarTranscode()
			: IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
			VistaJoystickMeasureTranscode::GetTypeString(),
									"joystick glut axes (btMask,Axis1,Axis2,Axis3)") {}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
									 double &dScalar,
									 unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			const VistaGlutJoystickDriver::_sJoyMeasure *m
					= (*pMeasure).getRead<VistaGlutJoystickDriver::_sJoyMeasure>();

			switch(nIndex)
			{
			case 32:
				{
					dScalar = m->m_nVals[0]; // btMask
					break;
				}
			case 33: // x
			case 34: // y
			case 35: // z
				{
					dScalar = m->m_nVals[nIndex - 32];
					break;
				}
			default:
				{
					if( nIndex < 32)
					{
						dScalar = ( int(m->m_nVals[0]) & (1 << nIndex) ? 1.0 : 0.0 );
						break;
					}
					break;
				}
			}
			return true;
		}
	};


	IVistaPropertyGetFunctor *SapGetter[] =
	{
		new VistaJoystickScalarTranscode,
		new VistaJoystickOrientationTranscode,
		new VistaJoystickPositionTranscode,
		NULL
	};
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////



#ifdef VISTAGLUTJOYSTICKTRANSCODER_EXPORTS
	DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<VistaJoystickTranscoderFactory> )
#else
	DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<VistaJoystickTranscoderFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaJoystickTranscoderFactory>)

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
