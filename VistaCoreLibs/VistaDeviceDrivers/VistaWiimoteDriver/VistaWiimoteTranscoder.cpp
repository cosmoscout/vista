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
#include "VistaWiimoteCommonShare.h"
#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>

#ifdef WIN32
#include <float.h>
#define myisnan _isnan
#else
#define myisnan std::isnan
#endif

// ****************************************************************************
// PARAMETER API IMPLEMENTATION
// ****************************************************************************

namespace
{

	class VistaWiimoteTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP(VistaWiimoteTranscoder, IVistaMeasureTranscode);
	public:
		VistaWiimoteTranscoder()
		{
			// inherited as protected member
			m_nNumberOfScalars = 13;
		}

		static std::string GetTypeString() { return "VistaWiimoteTranscode"; }
	};

	class VistaWiimoteTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaWiimoteTranscoder;
		}

		virtual void DestroyTranscoder( IVistaMeasureTranscode *transcoder )
		{
			delete transcoder;
		}

		virtual std::string GetTranscoderName() const { return "VistaWiimoteTranscodeFactory"; }
	};

	// ############################################################################

	// create wiimote getters.

	class VistaWiimoteIndexedButtonGet : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaWiimoteIndexedButtonGet()
			: IVistaMeasureTranscode::ScalarDoubleGet( "BUTTON",
			VistaWiimoteTranscoder::GetTypeString(),
			"retrieve button states (0-11)")
		{}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar,
			unsigned int nIndex ) const
		{
			if(!pMeasure || nIndex >= 13)
				return false;

			const VistaWiimoteDriverCommonShare::MeasureType *m
				= pMeasure->getRead<VistaWiimoteDriverCommonShare::MeasureType>();

			switch(nIndex)
			{
			case 0:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_ONE) ? 1:0);
					break;
				}
			case 1:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_TWO) ? 1:0);
					break;
				}
			case 2:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_B) ? 1:0);
					break;
				}
			case 3:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_MINUS) ? 1:0);
					break;
				}
			case 4:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_HOME) ? 1:0);
					break;
				}
			case 5:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_PLUS) ? 1:0);
					break;
				}
			case 6:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_A) ? 1:0);
					break;
				}
			case 7:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_LEFT) ? 1:0);
					break;
				}
			case 8:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_RIGHT) ? 1:0);
					break;
				}
			case 9:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_UP) ? 1:0);
					break;
				}
			case 10:
				{
					dScalar = ((m->m_buttons & WIIMOTE_BUTTON_DOWN) ? 1:0);
					break;
				}

			default:
				break;
			}

			//		if(dScalar)
			//			std::cout << "dScalar(" << nIndex << ") = " << dScalar << std::endl;

			return true;
		}
	};

	class VistaWiimoteAccelerationGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaWiimoteAccelerationGet()
			: IVistaMeasureTranscode::V3Get("ACCELERATION",
			VistaWiimoteTranscoder::GetTypeString(),
			"retrieve acceleration values from wiimote device"  )
		{

		}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaVector3D v3;
			GetValue( pMeasure, v3 );
			return v3;
		}


		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
		{
			if(!pMeasure)
				return false;

			const VistaWiimoteDriverCommonShare::MeasureType *m
				= pMeasure->getRead<VistaWiimoteDriverCommonShare::MeasureType>();


			v3[Vista::X] = m->m_Acceleration.x;
			v3[Vista::Y] = m->m_Acceleration.y;
			v3[Vista::Z] = m->m_Acceleration.z;
			return true;
		}
	};


	class VistaWiimotePosGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaWiimotePosGet()
			: IVistaMeasureTranscode::V3Get( "IRPOSITION",
			VistaWiimoteTranscoder::GetTypeString(),
			"retrieve the IR reported position as VistaVec3D(x,y,0)")
		{}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaVector3D v3;
			GetValue( pMeasure, v3 );
			return v3;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
		{
			if(!pMeasure)
				return false;

			const VistaWiimoteDriverCommonShare::MeasureType *m
				= pMeasure->getRead<VistaWiimoteDriverCommonShare::MeasureType>();


			v3[Vista::X] = (float)m->m_IR.ax;
			v3[Vista::Y] = (float)m->m_IR.ay;
			v3[Vista::Z] = 0.0f;
			return true;
		}
	};


	class VistaWiimoteSingleButtonGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
	{
	public:
		VistaWiimoteSingleButtonGet( unsigned short nIndex, const std::string &sName, const std::string &strDesc)
			: IVistaMeasureTranscode::TTranscodeValueGet<bool>( sName,
			VistaWiimoteTranscoder::GetTypeString(),
			strDesc ),
			m_nIndex(nIndex)
		{
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			bool bRet = false;
			GetValue( pMeasure, bRet );
			return bRet;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &nBtn) const
		{
			if(!pMeasure)
				return false;

			const VistaWiimoteDriverCommonShare::MeasureType *m
				= pMeasure->getRead<VistaWiimoteDriverCommonShare::MeasureType>();


			// this is the same as the IS_PRESSED() macro from wiiuse,
			// but works direcly on the button, not on a wiimote struct

			nBtn = ( m->m_buttons & m_nIndex ? true:false);
			//		std::cout << nBtn << " " << m_nIndex << " " << m->m_buttons << std::endl;
			return true;
		}

		unsigned short m_nIndex;
	};

	class VistaWiimoteIRMarkerGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaVector3D>
	{
	public:
		VistaWiimoteIRMarkerGet( unsigned short nIndex, const std::string &sName, const std::string &strDesc)
			: IVistaMeasureTranscode::TTranscodeValueGet<VistaVector3D>( sName,
			VistaWiimoteTranscoder::GetTypeString(),
			strDesc ),
			m_nIndex(nIndex)
		{
		}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaVector3D v3Result;
			GetValue( pMeasure, v3Result );
			return v3Result;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			if(!pMeasure)
				return false;

			const VistaWiimoteDriverCommonShare::MeasureType *m
				= pMeasure->getRead<VistaWiimoteDriverCommonShare::MeasureType>();


			const ir_dot_t& oDot = m->m_IR.dot[m_nIndex];
			if( oDot.visible )
			{
				v3Pos[0] = oDot.rx;
				v3Pos[1] = oDot.ry;
				v3Pos[2] = oDot.size;
			}
			else
				v3Pos[2] = -1;

			return true;
		}

		unsigned short m_nIndex;
	};


	// ############################################################################
	// NUNCHUK
	// ############################################################################

	class VistaNunchukTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP(VistaNunchukTranscoder, IVistaMeasureTranscode);
	public:
		VistaNunchukTranscoder()
		{
			// inherited as protected member
			m_nNumberOfScalars = 4;
		}

		static std::string GetTypeString() { return "VistaNunchukTranscode"; }
	};

	class VistaNunchukTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaNunchukTranscoder;
		}
	};


	class VistaNunchukPaddleGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaNunchukPaddleGet()
			: IVistaMeasureTranscode::V3Get( "PADDLE",
			VistaNunchukTranscoder::GetTypeString(),
			"getter for the nunchuk paddle / analog joystick as v3(x,y,0)"
			)
		{
		}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaVector3D v3;
			GetValue( pMeasure, v3 );
			return v3;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
		{
			if(!pMeasure)
				return false;

			const nunchuk_t *m =
					pMeasure->getRead<nunchuk_t>();

			// ok the documentation on the wiiuse does not seem to be correct,
			// or the nunchuk I have here is... broken...
			// in case the paddle is centered, I still measure 90deg and a magnitude
			// of 0.01 absolute (constant).
			// so here I mask out all measures that show a magnitude below 0.01f
			// or (as documented) the angle isnan.
			if( std::fabs(m->js.mag - 0.01f) < std::numeric_limits<float>::epsilon() || myisnan(m->js.ang) )
			{
				// joystick is "straight up"
				// interaction code should expect a NULL vector!
				v3.SetToZeroVector();
				//v3.SetNull();
				return true;
			}


			// in contrast to the documentation on the wiiuse lib (0.12 that is)
			// m->js.ang is in degrees: 0 up, 90 right, 180 down, 270 left

			// for the output, construct a rotation that will rotate 0,0,1 ('forward')
			// around the absolute Y-axis by angle angle
			VistaQuaternion q(VistaAxisAndAngle( VistaVector3D(0,1,0), Vista::DegToRad(m->js.ang) ) );

			// rotate the null ('forward') using q, scale by mag
			v3 = m->js.mag * ( q.Rotate(VistaVector3D(0,0,1) ) );

			return true;
		}
	};

	class VistaNunchukButtonGet : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaNunchukButtonGet()
			: IVistaMeasureTranscode::ScalarDoubleGet( "BUTTON",
			VistaNunchukTranscoder::GetTypeString(),
			"get nunchuk buttons by index")
		{

		}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar,
			unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			const nunchuk_t *m =
					pMeasure->getRead<nunchuk_t>();

			dScalar = ( m->btns & (1<<nIndex) ? 1:0 );
			return true;
		}
	};


	class VistaNunchukAccelerationGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaNunchukAccelerationGet()
			: IVistaMeasureTranscode::V3Get( "ACCELERATION",
			VistaNunchukTranscoder::GetTypeString(),
			"get nunchuk acceleration values"
			)
		{
		}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaVector3D v3;
			GetValue( pMeasure, v3 );
			return v3;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
		{
			if(!pMeasure)
				return false;

			const nunchuk_t *m =
					pMeasure->getRead<nunchuk_t>();

			v3[Vista::X] = m->accel.x;
			v3[Vista::Y] = m->accel.y;
			v3[Vista::Z] = m->accel.z;

			return true;
		}
	};

	class VistaNunchukOrientationGet : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaNunchukOrientationGet()
			: IVistaMeasureTranscode::QuatGet( "ORIENTATION",
			VistaNunchukTranscoder::GetTypeString(),
			"get nunchuk orientation value (wiiuse-lib)"
			)
		{
		}

		virtual VistaQuaternion GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaQuaternion q;
			GetValue( pMeasure, q );
			return q;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &q) const
		{
			if(!pMeasure)
				return false;

			const nunchuk_t *m =
					pMeasure->getRead<nunchuk_t>();

			VistaTransformMatrix mt;
			mt.SetToRotationMatrix(VistaQuaternion(VistaEulerAngles(m->orient.yaw, m->orient.roll, m->orient.pitch)));
			//mt.Rotation( m->orient.yaw, m->orient.roll, m->orient.pitch );
			q = VistaQuaternion(mt);

			return true;
		}
	};

	class VistaNunchukGravityGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaNunchukGravityGet()
			: IVistaMeasureTranscode::V3Get( "GRAVITY",
			VistaNunchukTranscoder::GetTypeString(),
			"get nunchuk gravity report values"
			)
		{
		}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaVector3D v3;
			GetValue( pMeasure, v3 );
			return v3;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
		{
			if(!pMeasure)
				return false;

			const nunchuk_t *m =
					pMeasure->getRead<nunchuk_t>();

			v3[Vista::X] = m->gforce.x;
			v3[Vista::Y] = m->gforce.y;
			v3[Vista::Z] = m->gforce.z;

			return true;
		}
	};


	class VistaNunchukSingleButtonGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
	{
	public:
		VistaNunchukSingleButtonGet( int nIndex, const std::string &sName, const std::string &strDesc)
			: IVistaMeasureTranscode::TTranscodeValueGet<bool>( sName,
			VistaNunchukTranscoder::GetTypeString(),
			strDesc ),
			m_nIndex(nIndex)
		{
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			bool bRet = false;
			GetValue( pMeasure, bRet );
			return bRet;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &nBtn) const
		{
			if(!pMeasure)
				return false;

			const nunchuk_t *m =
					pMeasure->getRead<nunchuk_t>();

			nBtn = ( m->btns & (1<<m_nIndex) ? true:false);

			return true;
		}

		int m_nIndex;
	};


	// ############################################################################
	// WIIMOTE CLASSIC CONTROLLER
	// ############################################################################


	class VistaClassicTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP(VistaClassicTranscoder, IVistaMeasureTranscode);
	public:
		VistaClassicTranscoder()
		{
			// inherited as protected member
			m_nNumberOfScalars = 20;
		}

		static std::string GetTypeString() { return "VistaClassicTranscode"; }
	};

	class VistaClassicTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaClassicTranscoder;
		}
	};


	class VistaClassicPaddleGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		enum ePad
		{
			PAD_LEFT = 0,
			PAD_RIGHT = 1,
		};

		VistaClassicPaddleGet( ePad nIndex, const std::string &strName, const std::string &strDesc )
			: IVistaMeasureTranscode::V3Get( strName,
			VistaClassicTranscoder::GetTypeString(),
			strDesc )
			, m_nIndex(nIndex)
		{
		}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaVector3D v3;
			GetValue( pMeasure, v3 );
			return v3;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
		{
			if(!pMeasure)
				return false;

			const classic_ctrl_t *m =
					pMeasure->getRead<classic_ctrl_t>();

			switch(m_nIndex)
			{
			case PAD_LEFT:
				{
					return GetPaddle( m->ljs, v3 );
				}
			case PAD_RIGHT:
				{
					return GetPaddle( m->rjs, v3 );
				}
			}

			return false;
		}

		bool GetPaddle(const joystick_t &js, VistaVector3D &v3) const
		{
			// ok the documentation on the wiiuse does not seem to be correct,
			// or the nunchuk I have here is... broken...
			// in case the paddle is centered, I still measure 90deg and a magnitude
			// of 0.01 absolute (constant).
			// so here I mask out all measures that show a magnitude below 0.01f
			// or (as documented) the angle isnan.
			if( std::fabs(js.mag - 0.01f) < std::numeric_limits<float>::epsilon() || myisnan(js.ang) )
			{
				// joystick is "straight up"
				// interaction code should expect a NULL vector!
				v3.SetToZeroVector();
				return true;
			}


			// in contrast to the documentation on the wiiuse lib (0.12 that is)
			// m->js.ang is in degrees: 0 up, 90 right, 180 down, 270 left

			// for the output, construct a rotation that will rotate 0,0,1 ('forward')
			// around the absolute Y-axis by angle angle
			VistaQuaternion q(VistaAxisAndAngle( VistaVector3D(0,1,0), Vista::DegToRad(js.ang) ) );

			// rotate the null ('forward') using q, scale by mag
			v3 = js.mag * ( q.Rotate(VistaVector3D(0,0,1) ) );
			return true;
		}

		ePad m_nIndex;
	};


	class VistaClassicButtonGet : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaClassicButtonGet()
			: IVistaMeasureTranscode::ScalarDoubleGet( "BUTTON",
			VistaClassicTranscoder::GetTypeString(),
			"get classic control buttons by index")
		{

		}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar,
			unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			const classic_ctrl_t *m =
					pMeasure->getRead<classic_ctrl_t>();

			if(nIndex >= 0 && nIndex <= 15)
				dScalar = m->btns & (1<<nIndex);
			else
			{
				// shoulder keys
				switch( nIndex )
				{
				case 16:
					dScalar = double(m->l_shoulder);
					return true;
				case 17:
					dScalar = double(m->r_shoulder);
					return true;
				default:
					return false;
				}
			}
			return true;
		}
	};

	class VistaClassicShoulderGet : public IVistaMeasureTranscode::TTranscodeValueGet<float>
	{
	public:
		enum ePad
		{
			SHOULDER_LEFT = 0,
			SHOULDER_RIGHT = 1,
		};

		VistaClassicShoulderGet( ePad pd, const std::string &strName, const std::string &strDesc )
			: IVistaMeasureTranscode::TTranscodeValueGet<float>( strName,
			VistaClassicTranscoder::GetTypeString(),
			strDesc ),
			m_ePd(pd)
		{

		}

		virtual float GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			float v3;
			GetValue( pMeasure, v3 );
			return v3;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, float &v3) const
		{
			switch(m_ePd)
			{
			case SHOULDER_LEFT:
				break;
			case SHOULDER_RIGHT:
				break;
			default:
				break;
			}
			return false;
		}

		ePad m_ePd;
	};



	// ############################################################################
	// WIIMOTE STATUS
	// ############################################################################


	class VistaStatusTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP(VistaStatusTranscoder, IVistaMeasureTranscode);
	public:
		VistaStatusTranscoder()
		{
			// inherited as protected member
			m_nNumberOfScalars = 20;
		}

		static std::string GetTypeString() { return "VistaStatusTranscode"; }
	};

	class VistaStatusTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaStatusTranscoder;
		}
	};

	class VistaStatusBatteryGet : public IVistaMeasureTranscode::TTranscodeValueGet<float>
	{

	public:
		VistaStatusBatteryGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<float>( "BATTERY",
			VistaStatusTranscoder::GetTypeString(),
			"retrieve the battery state of the wiimote [0..1]" )
		{
		}

		float GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			float nRet = 0;
			GetValue( pMeasure, nRet );
			return nRet;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure, float &nValue ) const
		{
			if(!pMeasure)
				return false;

			const VistaWiimoteDriverCommonShare::StateMeasureType *m
				= pMeasure->getRead<VistaWiimoteDriverCommonShare::StateMeasureType>();

			nValue = m->m_nBatteryLevel;

			return true;
		}
	};

	class VistaStatusExpansionStateGet : public IVistaMeasureTranscode::TTranscodeValueGet<int>
	{

	public:
		VistaStatusExpansionStateGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<int>( "EXPANSIONSTATE",
			VistaStatusTranscoder::GetTypeString(),
			"retrieve the current expansion state as bitmask" )
		{
		}

		int GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			int nRet = 0;
			GetValue( pMeasure, nRet );
			return nRet;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure, int &nValue ) const
		{
			if(!pMeasure)
				return false;

			const VistaWiimoteDriverCommonShare::StateMeasureType *m
				= pMeasure->getRead<VistaWiimoteDriverCommonShare::StateMeasureType>();

			nValue = m->m_nExpansionState;

			return true;
		}
	};

	// ############################################################################
	// GUITAR CONTROLLER
	// ############################################################################


	class VistaGuitarTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP(VistaGuitarTranscoder, IVistaMeasureTranscode);
	public:
		VistaGuitarTranscoder()
		{
			// inherited as protected member
			// @todo check guitar stuff
			m_nNumberOfScalars = 10;
		}

		static std::string GetTypeString() { return "VistaGuitarTranscode"; }
	};

	class VistaGuitarTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaGuitarTranscoder;
		}
	};

	class VistaGuitarPaddleGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaGuitarPaddleGet()
			: IVistaMeasureTranscode::V3Get( "PADDLE",
			VistaGuitarTranscoder::GetTypeString(),
			"getter for the guitar paddle / analog joystick as v3(x,y,0)"
			)
		{
		}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaVector3D v3;
			GetValue( pMeasure, v3 );
			return v3;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3) const
		{
			if(!pMeasure)
				return false;
			const guitar_hero_3_t *m =
					pMeasure->getRead<guitar_hero_3_t>();

			// ok the documentation on the wiiuse does not seem to be correct,
			// or the nunchuk I have here is... broken...
			// in case the paddle is centered, I still measure 90deg and a magnitude
			// of 0.01 absolute (constant).
			// so here I mask out all measures that show a magnitude below 0.01f
			// or (as documented) the angle isnan.
			if( std::fabs(m->js.mag - 0.01f) < std::numeric_limits<float>::epsilon() || myisnan(m->js.ang) )
			{
				// joystick is "straight up"
				// interaction code should expect a NULL vector!
				//v3.SetNull();
				v3.SetToZeroVector();
				return true;
			}


			// in contrast to the documentation on the wiiuse lib (0.12 that is)
			// m->js.ang is in degrees: 0 up, 90 right, 180 down, 270 left

			// for the output, construct a rotation that will rotate 0,0,1 ('forward')
			// around the absolute Y-axis by angle angle
			VistaQuaternion q(VistaAxisAndAngle( VistaVector3D(0,1,0), Vista::DegToRad(m->js.ang) ) );

			// rotate the null ('forward') using q, scale by mag
			v3 = m->js.mag * ( q.Rotate(VistaVector3D(0,0,1) ) );

			return true;
		}
	};

	class VistaGuitarButtonGet : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaGuitarButtonGet()
			: IVistaMeasureTranscode::ScalarDoubleGet( "BUTTON",
			VistaGuitarTranscoder::GetTypeString(),
			"get guitar control buttons by index [0-8] btns [10] whammy bar")
		{

		}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar,
			unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			const guitar_hero_3_t *m =
					pMeasure->getRead<guitar_hero_3_t>();

			if(nIndex == 9)
			{
				dScalar = m->whammy_bar;
			}
			else
				dScalar = m->btns & (1<<nIndex);
			return true;
		}
	};

	class VistaGuitarWhammyGet : public IVistaMeasureTranscode::TTranscodeValueGet<float>
	{

	public:
		VistaGuitarWhammyGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<float>( "WHAMMY",
			VistaGuitarTranscoder::GetTypeString(),
			"retrieve the state of the whammy bar" )
		{
		}

		float GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			float nRet = 0;
			GetValue( pMeasure, nRet );
			return nRet;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure, float &nValue ) const
		{
			if(!pMeasure)
				return false;

			const guitar_hero_3_t *m =
					pMeasure->getRead<guitar_hero_3_t>();

			nValue = m->whammy_bar;

			return true;
		}
	};

	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaWiimoteIndexedButtonGet,
		new VistaWiimoteAccelerationGet,
		new VistaWiimotePosGet,
		new VistaWiimoteIRMarkerGet( 0, "IR_MARKER_0", "position of marker 0 as (x,y,size) [or(0,0,-1) if invisible]" ),
		new VistaWiimoteIRMarkerGet( 1, "IR_MARKER_1", "position of marker 1 as (x,y,size) [or(0,0,-1) if invisible]" ),
		new VistaWiimoteIRMarkerGet( 2, "IR_MARKER_2", "position of marker 2 as (x,y,size) [or(0,0,-1) if invisible]" ),
		new VistaWiimoteIRMarkerGet( 3, "IR_MARKER_3", "position of marker 3 as (x,y,size) [or(0,0,-1) if invisible]" ),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_ONE, "BTN_1", "state of button labeled '1'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_TWO, "BTN_2", "state of button labeled '2'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_MINUS, "BTN_MINUS", "state of button labeled '-'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_PLUS, "BTN_PLUS", "state of button labeled '+'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_ONE, "BTN_HOME", "state of button labeled 'home'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_A, "BTN_A", "state of button labeled 'A'"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_B, "BTN_B", "state of button labeled 'B' (fire)"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_UP, "BTN_UP", "state of button on cross direction up"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_LEFT, "BTN_LEFT", "state of button on cross direction left"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_RIGHT, "BTN_RIGHT", "state of button on cross direction right"),
		new VistaWiimoteSingleButtonGet( WIIMOTE_BUTTON_DOWN, "BTN_DOWN", "state of button on cross direction down"),
		new VistaNunchukPaddleGet,
		new VistaNunchukAccelerationGet,
		new VistaNunchukGravityGet,
		new VistaNunchukButtonGet,
		new VistaNunchukSingleButtonGet(0, "BTN_Z", "retrieve button C"),
		new VistaNunchukSingleButtonGet(1, "BTN_C", "retrieve button Z"),
		new VistaNunchukOrientationGet,
		new VistaClassicPaddleGet( VistaClassicPaddleGet::PAD_LEFT,
		"LPADDLE", "retrieve left paddle as v3"),
		new VistaClassicPaddleGet( VistaClassicPaddleGet::PAD_RIGHT,
		"RPADDLE", "retrieve right paddle as v3"),
		new VistaClassicButtonGet,
		new VistaClassicShoulderGet( VistaClassicShoulderGet::SHOULDER_LEFT, "L_SHOULDER",
		"retrieve button state of left shoulder button"),
		new VistaClassicShoulderGet( VistaClassicShoulderGet::SHOULDER_RIGHT, "R_SHOULDER",
		"retrieve button state of right shoulder button"),
		new VistaStatusBatteryGet,
		new VistaStatusExpansionStateGet,

		new VistaGuitarPaddleGet,
		new VistaGuitarButtonGet,
		new VistaGuitarWhammyGet,

		NULL
	};


	class VistaWiimoteDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory
	{
	public:
		VistaWiimoteDriverTranscoderFactoryFactory()
		{
			CreateCreators(m_creators);
		}

		~VistaWiimoteDriverTranscoderFactoryFactory()
		{
			CleanupCreators(m_creators);
		}

		typedef std::map<std::string,ICreateTranscoder*> CRMAP;
		CRMAP m_creators;


		static void CreateCreators(CRMAP &mp)
		{
			mp["WIIMOTE"]  = new TCreateTranscoder<VistaWiimoteTranscoder>;
			mp["GUITAR"]   = new TCreateTranscoder<VistaGuitarTranscoder>;
			mp["NUNCHUK"]  = new TCreateTranscoder<VistaNunchukTranscoder>;
			mp["CLASSIC"]  = new TCreateTranscoder<VistaClassicTranscoder>;
			mp["STATUS"]   = new TCreateTranscoder<VistaStatusTranscoder>;
		}

		static void CleanupCreators( CRMAP &mp )
		{
			for( CRMAP::iterator it = mp.begin(); it != mp.end(); ++it )
				delete (*it).second;

			mp.clear();
		}

		virtual IVistaMeasureTranscoderFactory *CreateFactoryForType( const std::string &strTypeName )
		{
			CRMAP::const_iterator it = m_creators.find( strTypeName );
			if( it == m_creators.end() )
				return NULL;
			return (*it).second->Create();
		}


		virtual void DestroyTranscoderFactory( IVistaMeasureTranscoderFactory *fac )
		{
			delete fac;
		}

		static void OnUnload()
		{
			CRMAP mp;
			CreateCreators(mp);
			for( CRMAP::iterator it = mp.begin(); it != mp.end(); ++it )
				(*it).second->OnUnload();
			CleanupCreators(mp);
		}
	};

}


// ############################################################################

#ifdef VISTAWIIMOTETRANSCODER_EXPORTS
	DEFTRANSCODERPLUG_FUNC_EXPORTS( VistaWiimoteDriverTranscoderFactoryFactory )
#else
	DEFTRANSCODERPLUG_FUNC_IMPORTS( VistaWiimoteDriverTranscoderFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaWiimoteDriverTranscoderFactoryFactory)


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*                                               */
/*============================================================================*/


