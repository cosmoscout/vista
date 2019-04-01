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


#include "VistaDTrackCommonShare.h"

#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>


#include <cstring>
#include <cstdio>

#include <string>

namespace
{
	VistaTransformMatrix GetTransformFromDTrack( const double a9fRot[9] )
	{
		return VistaTransformMatrix( float(a9fRot[0]), float(a9fRot[3]), float(a9fRot[6]), 0,
									 float(a9fRot[1]), float(a9fRot[4]), float(a9fRot[7]), 0,
									 float(a9fRot[2]), float(a9fRot[5]), float(a9fRot[8]), 0,
									 0               , 0               , 0               , 1 );
	}

	// abstract Template Transcodes for 6DOF Bodies and similar
	template<typename TMeasure>
	class TVistaDTrack6DOFPosGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		TVistaDTrack6DOFPosGet( const std::string& sTranscodeName )
		: IVistaMeasureTranscode::V3Get( "POSITION", sTranscodeName, "dtrack position" )
		{
		}

		VistaVector3D GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			const TMeasure* pRealMeasure = pMeasure->getRead<TMeasure>();
			return VistaVector3D( pRealMeasure->m_anPosition );
		}

		bool GetValue( const VistaSensorMeasure* pMeasure, VistaVector3D& v3Value ) const
		{
			if(!pMeasure)
				return false;

			const TMeasure* pRealMeasure = pMeasure->getRead<TMeasure>();
			v3Value.SetValues( pRealMeasure->m_anPosition );
			return true;
		}
	};

	template<typename TMeasure>
	class TVistaDTrack6DOFOriGet : public IVistaMeasureTranscode::QuatGet
	{
	public:
		TVistaDTrack6DOFOriGet( const std::string& sTranscodeName )
		: IVistaMeasureTranscode::QuatGet( "ORIENTATION", sTranscodeName, "dtrack orientation" ) {}

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

			const TMeasure* pRealMeasure = pMeasure->getRead<TMeasure>();
			VistaTransformMatrix matRot = GetTransformFromDTrack( pRealMeasure->m_anRotation );
			qQuat = VistaQuaternion( matRot );
			return true;
		}
	};

	template<typename TMeasure>
	class TVistaDTrack6DOFMatrixGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>
	{
	public:
		TVistaDTrack6DOFMatrixGet( const std::string& sTranscodeName )
		: IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>( "ROTMATRIX",
				sTranscodeName, "dtrack orientation as matrix")
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

			const TMeasure* pRealMeasure = pMeasure->getRead<TMeasure>();
			matRot = GetTransformFromDTrack( pRealMeasure->m_anRotation );

			return true;
		}
	};

	template<typename TMeasure>
	class TVistaDTrack6DOFPoseGet : public IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>
	{
	public:
		TVistaDTrack6DOFPoseGet( const std::string& sTranscodeName )
			: IVistaMeasureTranscode::TTranscodeValueGet<VistaTransformMatrix>( "POSEMATRIX",
				sTranscodeName, "dtrack pose (trans+rot) as matrix")
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

			const TMeasure* pRealMeasure = pMeasure->getRead<TMeasure>();
			matPose = GetTransformFromDTrack( pRealMeasure->m_anRotation );
			matPose.SetTranslation( VistaVector3D( pRealMeasure->m_anPosition ) );

			return true;
		}
	};

	
	template<typename TMeasure>
	class TVistaDTrackEulerScalarGet : public IVistaMeasureTranscode::DoubleGet
	{
	public:
		enum EulerType
		{
			EULER_X,
			EULER_Y,
			EULER_Z,
		};
		std::string GetGetterName( const EulerType nType )
		{
			switch( nType )
			{
				case EULER_X:
					return "EULER_X";
				case EULER_Y:
					return "EULER_Y";
				case EULER_Z:
					return "EULER_Z";
			}
			return "";
		}
		std::string GetGetterDesc( const EulerType nType )
		{
			switch( nType )
			{
				case EULER_X:
					return "dtrack x angle of euler angles (in xyz format)";
				case EULER_Y:
					return "dtrack y angle of euler angles (in xyz format)";
				case EULER_Z:
					return "dtrack z angle of euler angles (in xyz format)";
			}
			return "";
		}
		TVistaDTrackEulerScalarGet( const std::string& sTranscodeName,
								const EulerType nType )
		: IVistaMeasureTranscode::DoubleGet ( GetGetterName( nType ),
							sTranscodeName,	GetGetterDesc( nType ) )
		, m_nType( nType )
		{
		}

		double GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			double nVal = 0.0;
			GetValue( pMeasure, nVal );
			return nVal;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure, double &dVal ) const
		{
			if( pMeasure == NULL )
				return false;

			const TMeasure* pRealMeasure = pMeasure->getRead<TMeasure>();
			dVal = pRealMeasure->m_anEuler[m_nType];
			return true;
		}
	private:
		EulerType m_nType;
	};

	template<typename TMeasure>
	class TVistaDTrackGeneralScalarGet : public IVistaMeasureTranscode::DoubleGet
	{
	public:
		enum ValueType
		{
			TYPE_ID,
			TYPE_QUALITY,
		};
		std::string GetGetterName( const ValueType nType )
		{
			switch( nType )
			{
				case TYPE_ID:
					return "ID";
				case TYPE_QUALITY:
					return "QUALITY";
			}
			return "";
		}
		std::string GetGetterDesc( const ValueType nType )
		{
			switch( nType )
			{
				case TYPE_ID:
					return "dtrack sensor index";
				case TYPE_QUALITY:
					return "dtrack quality of the sensor";
			}
			return "";
		}
		TVistaDTrackGeneralScalarGet( const std::string& sTranscodeName,
								const ValueType nType )
		: IVistaMeasureTranscode::DoubleGet ( GetGetterName( nType ),
							sTranscodeName,	GetGetterDesc( nType ) )
		, m_nType( nType )
		{
		}

		double GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			double nVal = 0.0;
			GetValue( pMeasure, nVal );
			return nVal;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure, double &dVal ) const
		{
			if( pMeasure == NULL )
				return false;

			const TMeasure* pRealMeasure = pMeasure->getRead<TMeasure>();
			switch( m_nType )
			{
				case TYPE_ID:
					dVal = pRealMeasure->m_nId;
					break;
				case TYPE_QUALITY:
					dVal = pRealMeasure->m_nQuality;
					break;
				default:
					return false;
			}
			return true;
		}
	private:
		ValueType m_nType;
	};

	template<typename TMeasure>
	class TVistaDTrackGeneralButtonMaskGet : public IVistaMeasureTranscode::UIntGet
	{
	public:
		TVistaDTrackGeneralButtonMaskGet( const std::string& sTrancodeName )
		: IVistaMeasureTranscode::UIntGet( "BTMASK", sTrancodeName, "button mask of the device" )
		{
		}


		unsigned int GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			unsigned int nVal = 0;
			GetValue( pMeasure, nVal );
			return nVal;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure,
						unsigned int &dVal ) const
		{
			if(!pMeasure)
				return false;

			const TMeasure* pStickMeasure = pMeasure->getRead<TMeasure>();

			dVal = (unsigned int)( pStickMeasure->m_nButtonState );
			return true;
		}
	};

	

	// ############################################################################
	// TRANSCODERS
	// ############################################################################

	class VistaDTrackGlobalsTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaDTrackGlobalsTranscode()
		{
			// inherited
			m_nNumberOfScalars = 2;
		}

		static std::string GetTypeString() { return "VistaDTrackGlobalsTranscode"; }

		REFL_INLINEIMP(VistaDTrackGlobalsTranscode, IVistaMeasureTranscode);
	};

	class VistaDTrackGlobalsScalarGet : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaDTrackGlobalsScalarGet()
			: IVistaMeasureTranscode::ScalarDoubleGet("DSCALAR",
			"VistaDTrackGlobalsTranscode", "get global dtrack measures (frame cnt/ts)")
		{}


		bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar,
			unsigned int nIdx) const
		{
			if(!pMeasure || nIdx > 2)
				return false;

			const VistaDTrackMeasures::sGlobalMeasure *m = pMeasure->getRead< VistaDTrackMeasures::sGlobalMeasure >();
			if(nIdx == 0)
				dScalar = m->m_nFrameCount;
			else
				dScalar = m->m_nTimeStamp;

			return true;
		}
	};

	static IVistaPropertyGetFunctor *SaGlobalGet[] =
	{
		new VistaDTrackGlobalsScalarGet,
		NULL
	};

	// #########################################################################

	class VistaDTrackBodyTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaDTrackBodyTranscode()
		{
			m_nNumberOfScalars = 5;
		}

		virtual ~VistaDTrackBodyTranscode() {}

		static std::string GetTypeString() { return "VistaDTrackBodyTranscode"; }

	protected:
	private:

		REFL_INLINEIMP(VistaDTrackBodyTranscode, IVistaMeasureTranscode);
	};


	class VistaDTrackBodyScalarGet : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaDTrackBodyScalarGet()
			: IVistaMeasureTranscode::ScalarDoubleGet ("DSCALAR",
			"VistaDTrackBodyTranscode", "dtrack body scalar get") {}

		VistaDTrackBodyScalarGet(const std::string &strPropName,
			const std::string &strClassName, const std::string &strDesc)
			: IVistaMeasureTranscode::ScalarDoubleGet (strPropName,
			strClassName, strDesc) {}

		bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar, unsigned int nIndex) const
		{
			if(!pMeasure || nIndex >= 5)
				return false;

			const VistaDTrackMeasures::sBodyMeasure* pRealMeasure = pMeasure->getRead<VistaDTrackMeasures::sBodyMeasure>();

			switch(nIndex)
			{
			case 0:
				dScalar = pRealMeasure->m_nId;
				break;
			case 1:
				dScalar = pRealMeasure->m_nQuality;
				break;
			case 2: // ex
			case 3: // ey
			case 4: // ez
				dScalar = pRealMeasure->m_anEuler[ nIndex - 2 ]; // shift back by 2
				break;
			}

			return true;
		}
	};


	static IVistaPropertyGetFunctor *SaBodyGet[] =
	{
		new TVistaDTrack6DOFPosGet<VistaDTrackMeasures::sBodyMeasure>( "VistaDTrackBodyTranscode" ),
		new TVistaDTrack6DOFOriGet<VistaDTrackMeasures::sBodyMeasure>( "VistaDTrackBodyTranscode" ),
		new TVistaDTrack6DOFMatrixGet<VistaDTrackMeasures::sBodyMeasure>( "VistaDTrackBodyTranscode" ),
		new TVistaDTrack6DOFPoseGet<VistaDTrackMeasures::sBodyMeasure>( "VistaDTrackBodyTranscode" ),
		new TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sBodyMeasure>( "VistaDTrackBodyTranscode",
						TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sBodyMeasure>::EULER_X ),
		new TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sBodyMeasure>( "VistaDTrackBodyTranscode",
						TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sBodyMeasure>::EULER_Y ),
		new TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sBodyMeasure>( "VistaDTrackBodyTranscode",
						TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sBodyMeasure>::EULER_Z ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sBodyMeasure>( "VistaDTrackBodyTranscode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sBodyMeasure>::TYPE_ID ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sBodyMeasure>( "VistaDTrackBodyTranscode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sBodyMeasure>::TYPE_QUALITY ),

		new VistaDTrackBodyScalarGet,	

		NULL
	};

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VistaDTrackStickTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaDTrackStickTranscode()
		: IVistaMeasureTranscode()
		{
			m_nNumberOfScalars = 8 + 5;
		}

		static std::string GetTypeString() { return "VistaDTrackStickTranscode"; }

		REFL_INLINEIMP( VistaDTrackStickTranscode, IVistaMeasureTranscode );
	};


	class VistaDTrackStickScalarGet : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaDTrackStickScalarGet()
			: IVistaMeasureTranscode::ScalarDoubleGet( "DSCALAR",
			"VistaDTrackStickTranscode", "dtrack stick dscalar get" ) {}


		bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
			double &dScalar, unsigned int nIndex) const
		{
			if(!pMeasure)
				return false;

			// normalize index
			const VistaDTrackMeasures::sStickMeasure* pRealMeasure = pMeasure->getRead<VistaDTrackMeasures::sStickMeasure>();
			if( nIndex < 8)
			{
				dScalar = ( int(pRealMeasure->m_nButtonState) & ( 1 << nIndex ) ) ? 1.0 : 0.0;
				return true;
			}
			else
			{
				switch(nIndex)
				{
				case 8:
					dScalar = pRealMeasure->m_nId;
					return true;
				case 9:
					dScalar = pRealMeasure->m_nQuality;
					return true;
				case 10:
				case 11:
				case 12:
					dScalar = pRealMeasure->m_anEuler[nIndex - 10];
					return true;
				default:
					break;
				}
			}
			return false;
		}
	};

	class VistaDTrackStickButtonGet : public IVistaMeasureTranscode::TTranscodeIndexedGet<bool>
	{
	public:
		VistaDTrackStickButtonGet()
			: IVistaMeasureTranscode::TTranscodeIndexedGet<bool>( "BUTTONS",
				"VistaDTrackStickTranscode", "Flystick buttons")
		{
		}

		virtual unsigned int GetNumberOfIndices() const { return 8; }

		bool GetValueIndexed( const VistaSensorMeasure *pMeasure,
							bool &bScalar, unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			if( nIndex >= 8 )
				return false;

			const VistaDTrackMeasures::sStickMeasure* pStickMeasure
						= pMeasure->getRead<VistaDTrackMeasures::sStickMeasure>();

			bScalar = ( (int)pStickMeasure->m_nButtonState & ( 1 << nIndex ) ) != 0;
			return true;
		}
	};
	
	


	static IVistaPropertyGetFunctor *SaStickGet[] =
	{
		new TVistaDTrack6DOFPosGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode" ),
		new TVistaDTrack6DOFOriGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode" ),
		new TVistaDTrack6DOFMatrixGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode" ),
		new TVistaDTrack6DOFPoseGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode" ),
		new TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode",
						TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sStickMeasure>::EULER_X ),
		new TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode",
						TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sStickMeasure>::EULER_Y ),
		new TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode",
						TVistaDTrackEulerScalarGet<VistaDTrackMeasures::sStickMeasure>::EULER_Z ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sStickMeasure>::TYPE_ID ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sStickMeasure>::TYPE_QUALITY ),
		new TVistaDTrackGeneralButtonMaskGet<VistaDTrackMeasures::sStickMeasure>( "VistaDTrackStickTranscode" ),

		new VistaDTrackStickScalarGet,	
		new VistaDTrackStickButtonGet,

		
		NULL
	};


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VistaDTrackStick2Transcode : public IVistaMeasureTranscode
	{
	public:
		VistaDTrackStick2Transcode()
		: IVistaMeasureTranscode()
		{
			// 5 info, 8 buttons, 8 controllers, 
			m_nNumberOfScalars = 8 + 8 + 2;
		}

		static std::string GetTypeString() { return "VistaDTrackStick2Transcode"; }

		REFL_INLINEIMP( VistaDTrackStick2Transcode, IVistaMeasureTranscode );
	};


	class VistaDTrackStick2ScalarGet : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaDTrackStick2ScalarGet()
		: IVistaMeasureTranscode::ScalarDoubleGet( "DSCALAR",
				"VistaDTrackStick2Transcode", "dtrack Stick2 dscalar get" )
		{
		}

		bool GetValueIndexed( const VistaSensorMeasure *pMeasure,
								double& dScalar, unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			// normalize index
			const VistaDTrackMeasures::sStick2Measure* pStick2Measure =
							pMeasure->getRead<VistaDTrackMeasures::sStick2Measure>();
			if( nIndex < 8 )
			{
				dScalar =  ( pStick2Measure->m_nButtonState & ( 1 << nIndex ) ) ? 1.0 : 0.0;
				return true;
			}
			else if( nIndex < 16 )
			{
				dScalar = pStick2Measure->m_anControllers[nIndex - 8];
				return true;
			}
			else
			{
				switch(nIndex)
				{
				case 16:
					dScalar = pStick2Measure->m_nId;
					return true;
				case 17:
					dScalar = pStick2Measure->m_nQuality;
					return true;
				default:
					break;
				}
			}
			return false;
		}
	};

	class VistaDTrackStick2ButtonGet : public IVistaMeasureTranscode::TTranscodeIndexedGet<bool>
	{
	public:
		VistaDTrackStick2ButtonGet()
			: IVistaMeasureTranscode::TTranscodeIndexedGet<bool>("BUTTONS",
				"VistaDTrackStick2Transcode", "Flystick2 buttons")
		{
		}

		virtual unsigned int GetNumberOfIndices() const { return 32; }

		bool GetValueIndexed( const VistaSensorMeasure *pMeasure,
							bool &bScalar, unsigned int nIndex) const
		{
			if(!pMeasure)
				return false;

			const VistaDTrackMeasures::sStick2Measure* pStickMeasure
						= pMeasure->getRead<VistaDTrackMeasures::sStick2Measure>();

			if( nIndex >= pStickMeasure->m_nNumberButtonValues )
				return false;

			bScalar = ( pStickMeasure->m_nButtonState & ( 1 << nIndex ) ) != 0;
			return true;
		}
	};

	class VistaDTrackStick2ControllerGet : public IVistaMeasureTranscode::TTranscodeIndexedGet<float>
	{
	public:
		VistaDTrackStick2ControllerGet()
			: IVistaMeasureTranscode::TTranscodeIndexedGet<float>( "CONTROLLER",
				"VistaDTrackStick2Transcode", "Flystick2 controller (i.e. axes)")
		{
		}

		virtual unsigned int GetNumberOfIndices() const { return 8; }

		bool GetValueIndexed( const VistaSensorMeasure *pMeasure,
							float& dScalar, unsigned int nIndex ) const
		{
			if(!pMeasure)
				return false;

			const VistaDTrackMeasures::sStick2Measure* pStickMeasure
						= pMeasure->getRead<VistaDTrackMeasures::sStick2Measure>();

			if( nIndex >= pStickMeasure->m_nNumberControllerValues )
				return false;

			dScalar = pStickMeasure->m_anControllers[nIndex];
			return true;
		}
	};


	static IVistaPropertyGetFunctor *SaStick2Get[] =
	{
		new TVistaDTrack6DOFPosGet<VistaDTrackMeasures::sStick2Measure>( "VistaDTrackStick2Transcode" ),
		new TVistaDTrack6DOFOriGet<VistaDTrackMeasures::sStick2Measure>( "VistaDTrackStick2Transcode" ),
		new TVistaDTrack6DOFMatrixGet<VistaDTrackMeasures::sStick2Measure>( "VistaDTrackStick2Transcode" ),
		new TVistaDTrack6DOFPoseGet<VistaDTrackMeasures::sStick2Measure>( "VistaDTrackStick2Transcode" ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sStick2Measure>( "VistaDTrackStick2Transcode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sStick2Measure>::TYPE_ID ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sStick2Measure>( "VistaDTrackStick2Transcode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sStick2Measure>::TYPE_QUALITY ),
		new TVistaDTrackGeneralButtonMaskGet<VistaDTrackMeasures::sStick2Measure>( "VistaDTrackStick2Transcode" ),

		new VistaDTrackStick2ScalarGet,
		new VistaDTrackStick2ButtonGet,
		new VistaDTrackStick2ControllerGet,
		NULL
	};


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VistaDTrackHandTranscode : public VistaDTrackBodyTranscode
	{
	public:
		VistaDTrackHandTranscode()
			: VistaDTrackBodyTranscode()
		{
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "VistaDTrackHandTranscode"; }

		REFL_INLINEIMP(VistaDTrackHandTranscode, VistaDTrackBodyTranscode);
	};


	class VistaDTrackHandScalarGet : public VistaDTrackBodyScalarGet
	{
	public:
		VistaDTrackHandScalarGet()
			: VistaDTrackBodyScalarGet("DSCALAR",
			"VistaDTrackHandTranscode", "dtrack hand dscalar get") {}


		bool GetValueIndexed( const VistaSensorMeasure *pMeasure,
			double &dScalar, unsigned int nIndex) const
		{
			if(!pMeasure)
				return false;

			//// normalize index
			//const VistaDTrackMeasures::sHandMeasure* pHandMeasure =
			//			reinterpret_cast<const VistaDTrackMeasures::sHandMeasure*>( &(*pMeasure).m_vecMeasures[0] );
			//if( nIndex < 8)
			//{
			//	// trying to retrieve virtual button state
			//	// note that we map the virtual scalar index 5 .. 5+nNumberOfButtons
			//	// from a single bit mask which is stored in m_vecMeasures[2]

			//	dScalar =  ( pHandMeasure->m_nButtonMask & ( 1 << nIndex ) ) ? 1.0 : 0.0;
			//	return true;
			//}
			//else if( nIndex < 16 )
			//{
			//	dScalar = pHandMeasure->m_anControllers[nIndex - 8];
			//	return true;
			//}
			//else
			//{
			//	switch(nIndex)
			//	{
			//	case 16:
			//		dScalar = pHandMeasure->m_nId;
			//		return true;
			//	case 17:
			//		dScalar = pHandMeasure->m_nQuality;
			//		return true;
			//	default:
			//		break;
			//	}
			//}
			return false;
		}
	};

	class VistaDTrackHandFingerPositionGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaDTrackHandFingerPositionGet( int nFingerIndex,
								const std::string &strName,
								const std::string &strClass,
								const std::string &strDesc)
			: IVistaMeasureTranscode::V3Get( strName, strClass, strDesc )
			, m_nFingerIndex( nFingerIndex )
		{
		}


		VistaVector3D GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			VistaVector3D v3Pos;
			GetValue( pMeasure, v3Pos );
			return v3Pos;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure,
							VistaVector3D& v3Val ) const
		{
			bool bRet = true;
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			if( m_nFingerIndex >= pHandMeasure->m_nNumberOfFingers )
				return false;

			const VistaDTrackMeasures::sHandMeasure::Finger& oFinger = pHandMeasure->m_aFingers[m_nFingerIndex];
			v3Val.SetValues( oFinger.m_anPosition );
			return bRet;
		}
	private:
		int m_nFingerIndex;
	};

	class VistaDTrackHandFingerOrientationGet : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaDTrackHandFingerOrientationGet( int nFingerIndex,
								const std::string &strName,
								const std::string &strClass,
								const std::string &strDesc)
			: IVistaMeasureTranscode::QuatGet( strName, strClass, strDesc )
			, m_nFingerIndex( nFingerIndex )
		{
		}


		VistaQuaternion GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			VistaQuaternion v3Pos;
			GetValue( pMeasure, v3Pos );
			return v3Pos;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure,
							VistaQuaternion& v3Val ) const
		{
			bool bRet = true;
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			if( m_nFingerIndex >= pHandMeasure->m_nNumberOfFingers )
				return false;

			const VistaDTrackMeasures::sHandMeasure::Finger& oFinger = pHandMeasure->m_aFingers[m_nFingerIndex];
			VistaTransformMatrix matRot;
			matRot.SetBasisMatrix( oFinger.m_anRotation );
			v3Val = VistaQuaternion( matRot );
			return bRet;
		}
	private:
		int m_nFingerIndex;
	};

	class VistaDTrackHandFingerDataGet : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<float> >
	{
	public:
		VistaDTrackHandFingerDataGet( int nFingerIndex,
								const std::string &strName,
								const std::string &strClass,
								const std::string &strDesc)
			: IVistaMeasureTranscode::TTranscodeValueGet<std::vector<float> >( strName, strClass, strDesc )
			, m_nFingerIndex( nFingerIndex )
		{
		}


		std::vector<float> GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			std::vector<float> vecData;
			GetValue( pMeasure, vecData );
			return vecData;
		}

		bool GetValue( const VistaSensorMeasure *pMeasure,
							std::vector<float>& vecData ) const
		{
			if( pMeasure == NULL )
				return false;

			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();

			const VistaDTrackMeasures::sHandMeasure::Finger& oFinger = pHandMeasure->m_aFingers[m_nFingerIndex];

			vecData.resize( 6 );
			vecData[0] = (float)oFinger.m_nRadius;
			vecData[1] = (float)oFinger.m_nOuterPhalanxLength;
			vecData[2] = (float)oFinger.m_nOuterToMiddleAngle;
			vecData[3] = (float)oFinger.m_nMiddlePhalanxLength;
			vecData[4] = (float)oFinger.m_nMiddleToInnerAngle;
			vecData[5] = (float)oFinger.m_nInnerPhalanxLength;

			return true;
		}
	private:
		int m_nFingerIndex;
	};


	class VistaDTrackHandAllFingerPositionsGet : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaVector3D> >
	{
	public:
		VistaDTrackHandAllFingerPositionsGet()
		: IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaVector3D> >
				( "FINGER_POSITIONS", "VistaDTrackHandTranscode",
				   "vector with positions of fingers")
		{
		}
		virtual std::vector<VistaVector3D> GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			std::vector<VistaVector3D> vecPositions;
			GetValue( pMeasure, vecPositions );
			return vecPositions;
		}
		virtual bool GetValue( const VistaSensorMeasure* pMeasure, std::vector<VistaVector3D>& vecOut ) const
		{
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			vecOut.resize( (std::size_t)pHandMeasure->m_nNumberOfFingers );
			for( int i = 0; i < pHandMeasure->m_nNumberOfFingers; ++i )
				vecOut[i] = VistaVector3D( pHandMeasure->m_aFingers[i].m_anPosition );
			return true;
		}
	};

	class VistaDTrackHandAllFingerOrientationsGet : public IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaQuaternion> >
	{
	public:
		VistaDTrackHandAllFingerOrientationsGet()
		: IVistaMeasureTranscode::TTranscodeValueGet<std::vector<VistaQuaternion> >
				( "FINGER_ORIENTATIONS", "VistaDTrackHandTranscode",
				   "vector with positions of fingers")
		{
		}
		virtual std::vector<VistaQuaternion> GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			std::vector<VistaQuaternion> vecPositions;
			GetValue( pMeasure, vecPositions );
			return vecPositions;
		}
		virtual bool GetValue( const VistaSensorMeasure* pMeasure, std::vector<VistaQuaternion>& vecOut ) const
		{
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			vecOut.resize( (std::size_t)pHandMeasure->m_nNumberOfFingers );
			for( int i = 0; i < pHandMeasure->m_nNumberOfFingers; ++i )
			{
				VistaTransformMatrix matTransform;
				matTransform.SetBasisMatrix( pHandMeasure->m_aFingers[i].m_anRotation );
				vecOut[i] = VistaQuaternion( matTransform );
			}
			return true;
		}
	};

	class VistaDTrackHandIsRightGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
	{
	public:
		VistaDTrackHandIsRightGet()
		: IVistaMeasureTranscode::TTranscodeValueGet<bool>
				( "RIGHT_HAND", "VistaDTrackHandTranscode",
				   "true if the data represents the right hand, otherwise its the left")
		{
		}
		virtual bool GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			bool bRight;
			GetValue( pMeasure, bRight );
			return bRight;
		}
		virtual bool GetValue( const VistaSensorMeasure* pMeasure, bool& bIsRight ) const
		{
			if( pMeasure == NULL )
				return false;
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			bIsRight = pHandMeasure->m_nIsRightHand != 0;
			return true;
		}
	};

	class VistaDTrackHandNumFingersGet : public IVistaMeasureTranscode::UIntGet
	{
	public:
		VistaDTrackHandNumFingersGet()
		: IVistaMeasureTranscode::UIntGet( "NUM_FINGERS", "VistaDTrackHandTranscode",
									 "number of visible fingers" )
		{
		}
		virtual unsigned int GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			unsigned int nNum;
			GetValue( pMeasure, nNum );
			return nNum;
		}
		virtual bool GetValue( const VistaSensorMeasure* pMeasure, unsigned int& nNum ) const
		{
			if( pMeasure == NULL )
				return false;
			const VistaDTrackMeasures::sHandMeasure* pHandMeasure = (*pMeasure).getRead<VistaDTrackMeasures::sHandMeasure>();
			nNum = (unsigned int)pHandMeasure->m_nNumberOfFingers;
			return true;
		}
	};

	static IVistaPropertyGetFunctor *SaHandGet[] =
	{
		new TVistaDTrack6DOFPosGet<VistaDTrackMeasures::sHandMeasure>( "VistaDTrackHandTranscode" ),
		new TVistaDTrack6DOFOriGet<VistaDTrackMeasures::sHandMeasure>( "VistaDTrackHandTranscode" ),
		new TVistaDTrack6DOFMatrixGet<VistaDTrackMeasures::sHandMeasure>( "VistaDTrackHandTranscode" ),
		new TVistaDTrack6DOFPoseGet<VistaDTrackMeasures::sHandMeasure>( "VistaDTrackHandTranscode" ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sHandMeasure>( "VistaDTrackHandTranscode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sHandMeasure>::TYPE_ID ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sHandMeasure>( "VistaDTrackHandTranscode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sHandMeasure>::TYPE_QUALITY ),

		new VistaDTrackHandScalarGet,
		new VistaDTrackHandAllFingerPositionsGet,
		new VistaDTrackHandAllFingerOrientationsGet,
		new VistaDTrackHandIsRightGet,
		new VistaDTrackHandNumFingersGet,

		new VistaDTrackHandFingerPositionGet( 0, "FINGER1_POSITION", "VistaDTrackHandTranscode", "Position of thumb" ),
		new VistaDTrackHandFingerPositionGet( 1, "FINGER2_POSITION", "VistaDTrackHandTranscode", "Position of index finger" ),
		new VistaDTrackHandFingerPositionGet( 2, "FINGER3_POSITION", "VistaDTrackHandTranscode", "Position of middle finger" ),
		new VistaDTrackHandFingerPositionGet( 3, "FINGER4_POSITION", "VistaDTrackHandTranscode", "Position of ring finger" ),
		new VistaDTrackHandFingerPositionGet( 4, "FINGER5_POSITION", "VistaDTrackHandTranscode", "Position of little finger" ),
		new VistaDTrackHandFingerOrientationGet( 0, "FINGER1_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of thumb" ),
		new VistaDTrackHandFingerOrientationGet( 1, "FINGER2_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of index finger" ),
		new VistaDTrackHandFingerOrientationGet( 2, "FINGER3_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of middle finger" ),
		new VistaDTrackHandFingerOrientationGet( 3, "FINGER4_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of ring finger" ),
		new VistaDTrackHandFingerOrientationGet( 4, "FINGER5_ORIENTATION", "VistaDTrackHandTranscode", "Orientation of little finger" ),
		new VistaDTrackHandFingerDataGet( 0, "FINGER1_DATA", "VistaDTrackHandTranscode", "Extended Data of thumb" ),
		new VistaDTrackHandFingerDataGet( 1, "FINGER2_DATA", "VistaDTrackHandTranscode", "Extended Data of index finger" ),
		new VistaDTrackHandFingerDataGet( 2, "FINGER3_DATA", "VistaDTrackHandTranscode", "Extended Data of middle finger" ),
		new VistaDTrackHandFingerDataGet( 3, "FINGER4_DATA", "VistaDTrackHandTranscode", "Extended Data of ring finger" ),
		new VistaDTrackHandFingerDataGet( 4, "FINGER5_DATA", "VistaDTrackHandTranscode", "Extended Data of little finger" ),
		NULL
	};

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VistaDTrackMarkerTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaDTrackMarkerTranscode()
		{
			// inherited
			m_nNumberOfScalars = 2;
		}

		virtual ~VistaDTrackMarkerTranscode() {}

		static std::string GetTypeString() { return "VistaDTrackMarkerTranscode"; }

		REFL_INLINEIMP(VistaDTrackMarkerTranscode, IVistaMeasureTranscode);
	};	

	static IVistaPropertyGetFunctor *SaMarkerGet[] =
	{
		new TVistaDTrack6DOFPosGet<VistaDTrackMeasures::sMarkerMeasure>( "VistaDTrackBodyTranscode" ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sMarkerMeasure>( "VistaDTrackBodyTranscode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sMarkerMeasure>::TYPE_ID ),
		new TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sMarkerMeasure>( "VistaDTrackBodyTranscode",
						TVistaDTrackGeneralScalarGet<VistaDTrackMeasures::sMarkerMeasure>::TYPE_QUALITY ),

		NULL
	};

	class VistaDTrackDriverTranscodeFactoryFactory : public IVistaTranscoderFactoryFactory
	{
	public:
		VistaDTrackDriverTranscodeFactoryFactory()
		{
			CreateCreators(m_creators);
		}

		~VistaDTrackDriverTranscodeFactoryFactory()
		{
			CleanupCreators(m_creators);
		}

		typedef std::map<std::string,ICreateTranscoder*> CRMAP;
		CRMAP m_creators;

		static void CreateCreators(CRMAP &mp)
		{
			mp["MARKER"]  = new TCreateTranscoder<VistaDTrackMarkerTranscode>;
			mp["STICK"]  = new TCreateTranscoder<VistaDTrackStickTranscode>;
			mp["STICK2"]  = new TCreateTranscoder<VistaDTrackStick2Transcode>;
			mp["HAND"]  = new TCreateTranscoder<VistaDTrackHandTranscode>;
			mp["BODY"]  = new TCreateTranscoder<VistaDTrackBodyTranscode>;
			mp["GLOBAL"]  = new TCreateTranscoder<VistaDTrackGlobalsTranscode>;
			//mp["MEASURE"]  = new TCreateTranscoder<VistaDTrackMeasureTranscode>;
			mp["MEASURE"]  = NULL;
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
			{
				if( (*it).second )
					(*it).second->OnUnload();
			}
			CleanupCreators(mp);
		}
	};

}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifdef VISTADTRACKTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( VistaDTrackDriverTranscodeFactoryFactory )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( VistaDTrackDriverTranscodeFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaDTrackDriverTranscodeFactoryFactory)

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

