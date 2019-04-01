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
#include "VistaDepthSenseDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/
namespace
{
	const std::string g_sColorTranscoderName = "VistaDepthSenseColorTranscoder";
	const std::string g_sDepthTranscoderName = "VistaDepthSenseDepthTranscoder";
	const std::string g_sUVMapTranscoderName = "VistaDepthSenseUVMapTranscoder";

	class VistaDepthSenseColorTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP(VistaDepthSenseColorTranscoder,
					   IVistaMeasureTranscode);
	public:
		VistaDepthSenseColorTranscoder()
		{
			// inherited as protected member
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return g_sColorTranscoderName; }
	};

	class VistaDepthSenseColorFrameGet :
		public IVistaMeasureTranscode::TTranscodeValueGet<const unsigned char*>
	{
	public:
		VistaDepthSenseColorFrameGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<const unsigned char*>(
				"COLOR_FRAME",
				g_sColorTranscoderName,
				"Color frame, 320x240 RGB888")
		{}

		const unsigned char* GetValue(
			const VistaSensorMeasure* pMeasure ) const
		{
			const unsigned char* pColorFrame;
			GetValue( pMeasure, pColorFrame );
			return pColorFrame;
		};

		bool GetValue( const VistaSensorMeasure* pMeasure,
					   const unsigned char*& pColorFrame ) const {

			const VistaDepthSenseDriver::ColorMeasure *pData =
				pMeasure->getRead<VistaDepthSenseDriver::ColorMeasure>();
			pColorFrame = pData->frame;

			return true;
		}
	};

	class VistaDepthSenseDepthTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP(VistaDepthSenseDepthTranscoder,
					   IVistaMeasureTranscode);
	public:
		VistaDepthSenseDepthTranscoder()
		{
			// inherited as protected member
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return g_sDepthTranscoderName; }
	};

	class VistaDepthSenseDepthFrameGet :
		public IVistaMeasureTranscode::TTranscodeValueGet<const unsigned short*>
	{
	public:
		VistaDepthSenseDepthFrameGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<const unsigned short*>(
				"DEPTH_FRAME",
				g_sDepthTranscoderName,
				"Depth frame, 320x240 16bit")
		{}

		const unsigned short* GetValue(
			const VistaSensorMeasure* pMeasure ) const
		{
			const unsigned short* pDepthFrame;
			GetValue( pMeasure, pDepthFrame );
			return pDepthFrame;
		};

		bool GetValue( const VistaSensorMeasure* pMeasure,
					   const unsigned short*& pDepthFrame ) const {

			const VistaDepthSenseDriver::DepthMeasure *pData =
				pMeasure->getRead<VistaDepthSenseDriver::DepthMeasure>();
			pDepthFrame = pData->frame;

			return true;
		}
	};

	class VistaDepthSenseUVMapTranscoder : public IVistaMeasureTranscode
	{
		REFL_INLINEIMP(VistaDepthSenseUVMapTranscoder,
					   IVistaMeasureTranscode);
	public:
		VistaDepthSenseUVMapTranscoder()
		{
			// inherited as protected member
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return g_sUVMapTranscoderName; }
	};
	
	class VistaDepthSenseUVMapFrameGet :
		public IVistaMeasureTranscode::TTranscodeValueGet<const float*>	{
	public:
		VistaDepthSenseUVMapFrameGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<const float*>(
				"UVMAP_FRAME",
				g_sUVMapTranscoderName,
				"UV map frame, 320x240 normalized float u,v coordinates")
		{}

		const float* GetValue(const VistaSensorMeasure* pMeasure) const {
			const float* pUVMap;
			GetValue( pMeasure, pUVMap );
			return pUVMap;
		};

		bool GetValue( const VistaSensorMeasure* pMeasure,
					   const float*& pUVMap ) const {

			const VistaDepthSenseDriver::UVMapMeasure *pData =
				pMeasure->getRead<VistaDepthSenseDriver::UVMapMeasure>();
			pUVMap = pData->frame;

			return true;
		}
	};

	
	static IVistaPropertyGetFunctor *SaGetter[] = {
		new VistaDepthSenseColorFrameGet,
		new VistaDepthSenseDepthFrameGet,
		new VistaDepthSenseUVMapFrameGet,
		NULL
	};


	// #########################################################################
	// FACTORYFACTORY
	// #########################################################################

	class VistaDepthSenseDriverTranscoderFactoryFactory :
		public IVistaTranscoderFactoryFactory
	{
	public:
		virtual IVistaMeasureTranscoderFactory *CreateFactoryForType(
			const std::string &strTypeName )
		{
			if(VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
				   strTypeName, "COLOR" ) ) {
				return new TDefaultTranscoderFactory<
					VistaDepthSenseColorTranscoder>(
						VistaDepthSenseColorTranscoder::GetTypeString() );
			}
			else if( VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
						 strTypeName, "DEPTH" ) ) {
				return new TDefaultTranscoderFactory<
					VistaDepthSenseDepthTranscoder>(
						VistaDepthSenseDepthTranscoder::GetTypeString() );
			}
			else if(VistaAspectsComparisonStuff::StringCaseInsensitiveEquals(
						strTypeName, "UVMAP" ) ) {
				return new TDefaultTranscoderFactory<
					VistaDepthSenseUVMapTranscoder>(
						VistaDepthSenseUVMapTranscoder::GetTypeString() );
			}
			else
				return NULL;
		}

		virtual void DestroyTranscoderFactory(
			IVistaMeasureTranscoderFactory *fac ) {
			delete fac;
		}

		static void OnUnload() {
			TDefaultTranscoderFactory<VistaDepthSenseColorTranscoder>
				a( VistaDepthSenseColorTranscoder::GetTypeString() );
			a.OnUnload();
			TDefaultTranscoderFactory<VistaDepthSenseDepthTranscoder>
				b( VistaDepthSenseDepthTranscoder::GetTypeString() );
			b.OnUnload();
			TDefaultTranscoderFactory<VistaDepthSenseUVMapTranscoder>
				c( VistaDepthSenseUVMapTranscoder::GetTypeString() );
			c.OnUnload();
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

#ifdef VISTADEPTHSENSETRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS(
	VistaDepthSenseDriverTranscoderFactoryFactory )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS(
	VistaDepthSenseDriverTranscoderFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaDepthSenseDriverTranscoderFactoryFactory)
