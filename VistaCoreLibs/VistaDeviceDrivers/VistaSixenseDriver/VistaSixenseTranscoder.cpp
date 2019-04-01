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


#include "VistaSixenseCommonShare.h"

#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>


#include <cstring>
#include <cstdio>

#include <string>

namespace
{
	class VistaSixenseTranscode : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP( VistaSixenseTranscode, IVistaMeasureTranscode );
	public:
		VistaSixenseTranscode() {}

		static std::string GetTypeString() { return "VistaSixenseTranscode"; }
	};

	class VistaSixensePosGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaSixensePosGet( const std::string& sTranscodeName )
		: IVistaMeasureTranscode::V3Get( "POSITION", sTranscodeName, "position" )
		{
		}

		VistaVector3D GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			const VistaSixenseMeasures::Measure* pRealMeasure = pMeasure->getRead<VistaSixenseMeasures::Measure>();
			return VistaVector3D( pRealMeasure->m_a3fPosition );
		}

		bool GetValue( const VistaSensorMeasure* pMeasure, VistaVector3D& v3Value ) const
		{
			if(!pMeasure)
				return false;

			const VistaSixenseMeasures::Measure* pRealMeasure = pMeasure->getRead<VistaSixenseMeasures::Measure>();
			v3Value.SetValues( pRealMeasure->m_a3fPosition );
			return true;
		}
	};

	class VistaSixenseOriGet : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaSixenseOriGet( const std::string& sTranscodeName )
		: IVistaMeasureTranscode::QuatGet( "ORIENTATION", sTranscodeName, "orientation" ) {}

		VistaQuaternion GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			VistaQuaternion qOri;
			GetValue( pMeasure, qOri );
			return qOri;
		};

		bool GetValue(const VistaSensorMeasure * pMeasure, VistaQuaternion &qQuat) const
		{
			if(!pMeasure)
				return false;

			const VistaSixenseMeasures::Measure* pRealMeasure = pMeasure->getRead<VistaSixenseMeasures::Measure>();
			qQuat = VistaQuaternion( pRealMeasure->a4fRotationQuaternion ); // @todo: check order
			return true;
		}
	};

	class VistaSixenseRotMatrixGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>
	{
	public:
		VistaSixenseRotMatrixGet( const std::string& sTranscodeName )
		: IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>( "ROTMATRIX",
				sTranscodeName, "orientation as matrix")
		{}

		VistaTransformMatrix GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			VistaTransformMatrix matRot;
			GetValue( pMeasure, matRot );
			return matRot;
		};

		bool GetValue( const VistaSensorMeasure* pMeasure, VistaTransformMatrix& matRot ) const
		{
			if( !pMeasure )
				return false;

			const VistaSixenseMeasures::Measure* pRealMeasure = pMeasure->getRead<VistaSixenseMeasures::Measure>();
			for( int i = 0; i < 3; ++i )
			{
				for( int j = 0; j < 3; ++j )
					matRot[i][j] = pRealMeasure->m_a3x3fRotationMatrix[j][i]; // @todo check order
			}
			return true;
		}
	};

	class VistaSixensePoseGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>
	{
	public:
		VistaSixensePoseGet( const std::string& sTranscodeName )
			: IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>( "POSEMATRIX",
				sTranscodeName, "pose (trans+rot) as matrix")
		{}

		VistaTransformMatrix GetValue(const VistaSensorMeasure *pMeasure) const
		{
			VistaTransformMatrix matPose;
			GetValue( pMeasure, matPose );
			return matPose;
		};

		bool GetValue(const VistaSensorMeasure* pMeasure, VistaTransformMatrix& matPose ) const
		{
			if(!pMeasure)
				return false;

			const VistaSixenseMeasures::Measure* pRealMeasure = pMeasure->getRead<VistaSixenseMeasures::Measure>();
			for( int i = 0; i < 3; ++i )
			{
				for( int j = 0; j < 3; ++j )
					matPose[i][j] = pRealMeasure->m_a3x3fRotationMatrix[j][i]; // @todo check order
			}
			matPose.SetTranslation( pRealMeasure->m_a3fPosition );

			return true;
		}
	};

	class VistaSixenseButtonGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
	{
	public:
		VistaSixenseButtonGet( const std::string& sTranscodeName,
							const std::string& sGetterName,
							const std::string& sDescription,
							const int nButtonMask )
		: IVistaMeasureTranscode::TTranscodeValueGet<bool>( sGetterName, sTranscodeName, sDescription )
		, m_nButtonMask( nButtonMask )
		{
		}

		bool GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			bool bTmp = false;
			GetValue( pMeasure, bTmp );
			return bTmp;
		};

		bool GetValue( const VistaSensorMeasure* pMeasure, bool& bValue ) const
		{
			if( pMeasure == NULL )
				return false;

			const VistaSixenseMeasures::Measure* pRealMeasure = pMeasure->getRead<VistaSixenseMeasures::Measure>();
			bValue = ( ( pRealMeasure->m_nButtons & m_nButtonMask ) != 0 );

			return true;
		}

	private:
		const int m_nButtonMask;
	};

	class VistaSixenseJoyXGet : public IVistaMeasureTranscode::TTranscodeValueGet<float>
	{
	public:
		VistaSixenseJoyXGet( const std::string& sTranscodeName )
		: IVistaMeasureTranscode::TTranscodeValueGet<float>( "JOYSTICK_X", sTranscodeName, "horizontal joystick axis")
		{
		}

		float GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			float nValue;
			GetValue( pMeasure, nValue );
			return nValue;
		};

		bool GetValue( const VistaSensorMeasure* pMeasure, float& nValue ) const
		{
			if( pMeasure == NULL )
				return false;

			const VistaSixenseMeasures::Measure* pRealMeasure = pMeasure->getRead<VistaSixenseMeasures::Measure>();
			nValue = pRealMeasure->m_nJoystickX;
			return true;
		}
	};

	class VistaSixenseJoyYGet : public IVistaMeasureTranscode::TTranscodeValueGet<float>
	{
	public:
		VistaSixenseJoyYGet( const std::string& sTranscodeName )
		: IVistaMeasureTranscode::TTranscodeValueGet<float>( "JOYSTICK_Y", sTranscodeName, "vertical joystick axis")
		{
		}

		float GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			float nValue;
			GetValue( pMeasure, nValue );
			return nValue;
		};

		bool GetValue( const VistaSensorMeasure* pMeasure, float& nValue ) const
		{
			if( pMeasure == NULL )
				return false;

			const VistaSixenseMeasures::Measure* pRealMeasure = pMeasure->getRead<VistaSixenseMeasures::Measure>();
			nValue = pRealMeasure->m_nJoystickY;
			return true;
		}
	};

	class VistaSixenseTriggerGet : public IVistaMeasureTranscode::TTranscodeValueGet<float>
	{
	public:
		VistaSixenseTriggerGet( const std::string& sTranscodeName )
		: IVistaMeasureTranscode::TTranscodeValueGet<float>( "TRIGGER", sTranscodeName, "analog trigger value")
		{
		}

		float GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			float nValue;
			GetValue( pMeasure, nValue );
			return nValue;
		};

		bool GetValue( const VistaSensorMeasure* pMeasure, float& nValue ) const
		{
			if( pMeasure == NULL )
				return false;

			const VistaSixenseMeasures::Measure* pRealMeasure = pMeasure->getRead<VistaSixenseMeasures::Measure>();
			nValue = pRealMeasure->m_nTrigger;
			return true;
		}
	};


	static IVistaPropertyGetFunctor *SaBodyGet[] =
	{
		new VistaSixensePosGet( "VistaSixenseTranscode" ),
		new VistaSixenseOriGet( "VistaSixenseTranscode" ),
		new VistaSixenseRotMatrixGet( "VistaSixenseTranscode" ),
		new VistaSixensePoseGet( "VistaSixenseTranscode" ),
		new VistaSixenseTriggerGet( "VistaSixenseTranscode" ),
		new VistaSixenseJoyXGet( "VistaSixenseTranscode" ),
		new VistaSixenseJoyYGet( "VistaSixenseTranscode" ),
		new VistaSixenseButtonGet( "VistaSixenseTranscode", "BUTTON_1", "button 1", VistaSixenseMeasures::g_nButton1Mask ),
		new VistaSixenseButtonGet( "VistaSixenseTranscode", "BUTTON_2", "button 2", VistaSixenseMeasures::g_nButton2Mask ),
		new VistaSixenseButtonGet( "VistaSixenseTranscode", "BUTTON_3", "button 3", VistaSixenseMeasures::g_nButton3Mask ),
		new VistaSixenseButtonGet( "VistaSixenseTranscode", "BUTTON_4", "button 4", VistaSixenseMeasures::g_nButton4Mask ),
		new VistaSixenseButtonGet( "VistaSixenseTranscode", "BUTTON_START", "start button", VistaSixenseMeasures::g_nButtonStartMask ),
		new VistaSixenseButtonGet( "VistaSixenseTranscode", "BUTTON_BUMPER", "bumper button", VistaSixenseMeasures::g_nButtonBumperMask ),
		new VistaSixenseButtonGet( "VistaSixenseTranscode", "BUTTON_JOYSTICK", "joystick button", VistaSixenseMeasures::g_nButtonJoystickMask ),
		NULL
	};


	class VistaSixenseDriverTranscodeFactoryFactory : public IVistaTranscoderFactoryFactory
	{
	public:
		virtual IVistaMeasureTranscoderFactory* CreateFactoryForType( const std::string& sTypeName )
		{
			if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( sTypeName, "" ) )
			{
				return new TDefaultTranscoderFactory<VistaSixenseTranscode>( VistaSixenseTranscode::GetTypeString() );
			}
			else
				return NULL;
		}

		virtual void DestroyTranscoderFactory( IVistaMeasureTranscoderFactory* pFac )
		{
			delete pFac;
		}

		static void OnUnload()
		{
			TDefaultTranscoderFactory<VistaSixenseTranscode> oFac( VistaSixenseTranscode::GetTypeString() );
			oFac.OnUnload();
		}

	};
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifdef VISTASIXENSETRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( VistaSixenseDriverTranscodeFactoryFactory )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( VistaSixenseDriverTranscodeFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP( VistaSixenseDriverTranscodeFactoryFactory )

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

