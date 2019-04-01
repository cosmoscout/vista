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
#include "VistaOpenCVFaceTrackDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/
namespace
{
	class VistaOpenCVFaceTrackFaceTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP( VistaOpenCVFaceTrackFaceTranscoder, IVistaMeasureTranscode );
	public:
		VistaOpenCVFaceTrackFaceTranscoder()
		{
			// inherited as protected member
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "VistaOpenCVFaceTrackFaceTranscoder"; }
	};

	class VistaFacePositionTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<VistaVector3D>
	{
	public:
		VistaFacePositionTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<VistaVector3D>( "POSITION",
							VistaOpenCVFaceTrackFaceTranscoder::GetTypeString(),
							"returns position of the eye center" )
		{
		}

		virtual VistaVector3D GetValue( const VistaSensorMeasure* pMeasure ) const
		{
			const VistaOpenCVFaceTrackDriver::FacePoseMeasure* pFaceMeasure
					= pMeasure->getRead<VistaOpenCVFaceTrackDriver::FacePoseMeasure>();
			return VistaVector3D( pFaceMeasure->m_a3fPosition );
		}

		virtual bool GetValue( const VistaSensorMeasure *pMeasure, VistaVector3D& v3Value ) const
		{
			v3Value = GetValue( pMeasure );
			return true;
		}
	};


	static IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaFacePositionTranscode,
		NULL
	};


	// ###############################################################################################
	// FACTORYFACTORY
	// ###############################################################################################

	class VistaOpenCVFaceTrackTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory
	{
	public:
		virtual IVistaMeasureTranscoderFactory *CreateFactoryForType( const std::string &strTypeName )
		{
			if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals( strTypeName, "FACE" ) )
			{
				return new TDefaultTranscoderFactory<VistaOpenCVFaceTrackFaceTranscoder>( VistaOpenCVFaceTrackFaceTranscoder::GetTypeString() );
			}
			else
				return NULL;
		}

		virtual void DestroyTranscoderFactory( IVistaMeasureTranscoderFactory *fac )
		{
			delete fac;
		}

		static void OnUnload()
		{
			TDefaultTranscoderFactory<VistaOpenCVFaceTrackFaceTranscoder>  oFac( VistaOpenCVFaceTrackFaceTranscoder::GetTypeString() );
			oFac.OnUnload();
		}

	};
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifdef VISTAOPENCVFACETRACKTRANSCODER_EXPORTS
	DEFTRANSCODERPLUG_FUNC_EXPORTS( VistaOpenCVFaceTrackTranscoderFactoryFactory )
#else
	DEFTRANSCODERPLUG_FUNC_IMPORTS( VistaOpenCVFaceTrackTranscoderFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP( VistaOpenCVFaceTrackTranscoderFactoryFactory )
