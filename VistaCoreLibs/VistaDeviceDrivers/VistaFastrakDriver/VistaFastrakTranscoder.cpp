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



#include <VistaDeviceDriversBase/VistaDriverPlugDev.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>
#include "VistaFastrakCommonShare.h"

#include <cstring>
#include <cstdio>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace
{
	class VistaFastrakBodyTranscoder : public IVistaMeasureTranscode
	{
	public:
		VistaFastrakBodyTranscoder()
			: IVistaMeasureTranscode()
		{
		}

		static std::string GetTypeString() { return "VistaFastrakBodyTranscoder"; }

		REFL_INLINEIMP(VistaFastrakBodyTranscoder, IVistaMeasureTranscode);
	};

	class VistaFastrakWandTranscoder : public VistaFastrakBodyTranscoder
	{
	public:
		VistaFastrakWandTranscoder()
			: VistaFastrakBodyTranscoder()
		{
		}

		static std::string GetTypeString() { return "VistaFastrakWandTranscoder"; }

		REFL_INLINEIMP(VistaFastrakWandTranscoder, VistaFastrakBodyTranscoder);
	};


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	class VistaFastrakWandFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaFastrakWandTranscoder;
		}
	};

	class VistaFastrakBodyFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaFastrakBodyTranscoder;
		}
	};

	class VistaFastrackPosGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaFastrackPosGet()
			: IVistaMeasureTranscode::V3Get( "POSITION",
			VistaFastrakBodyTranscoder::GetTypeString(),
			"the sensor position in ViSTA reference frame (v3)" )
		{
		}

		virtual VistaVector3D GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			VistaVector3D v3;
			GetValue(pMeasure, v3);
			return v3;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			const VistaFastrakMeasures::sFastrakSample *m = pMeasure->getRead< VistaFastrakMeasures::sFastrakSample >();

			v3Pos[0] = m->m_vPos[0];
			v3Pos[1] = m->m_vPos[1];
			v3Pos[2] = m->m_vPos[2];

			return true;
		}
	};

	class VistaFastrackOriGet : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaFastrackOriGet()
			: IVistaMeasureTranscode::QuatGet( "ORIENTATION",
			VistaFastrakBodyTranscoder::GetTypeString(),
			"the sensor orientation in ViSTA reference frame (q)" )
		{
		}

		virtual VistaQuaternion GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			VistaQuaternion q;
			GetValue(pMeasure, q);
			return q;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &qOri) const
		{
			const VistaFastrakMeasures::sFastrakSample *m = pMeasure->getRead< VistaFastrakMeasures::sFastrakSample >();

			qOri[0] = m->m_vOri[0];
			qOri[1] = m->m_vOri[1];
			qOri[2] = m->m_vOri[2];
			qOri[3] = m->m_vOri[3];
			return true;
		}
	};


	// getter for button states
	class VistaFastrackButtonGet : public IVistaMeasureTranscode::TTranscodeValueGet<bool>
	{
	public:
		VistaFastrackButtonGet()
			: IVistaMeasureTranscode::TTranscodeValueGet<bool>( "BUTTON_1",
			VistaFastrakWandTranscoder::GetTypeString(),
			"the single button of a possible fastrak wand (bool)")
		{
		}

		virtual bool GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			bool bRet = false;
			GetValue(pMeasure, bRet);
			return bRet;
		}

		virtual bool GetValue( const VistaSensorMeasure *pMeasure, bool &bRet ) const
		{
			const VistaFastrakMeasures::sFastrakButtonSample *m = pMeasure->getRead< VistaFastrakMeasures::sFastrakButtonSample >();
			bRet = m->m_bBtPress;
			return true;
		}
	};


	class VistaFastrackStatsGet : public IVistaMeasureTranscode::TTranscodeValueGet<int>
	{
	public:
		enum eMd
		{
			MD_DISTORTION_LEVEL = 0,
			MD_TIMESTAMP,
			MD_FRAMECOUNT
		};

		VistaFastrackStatsGet(eMd md, const std::string &strPropName,
			const std::string &strPropHelp )
			: IVistaMeasureTranscode::TTranscodeValueGet<int>( strPropName,
			VistaFastrakBodyTranscoder::GetTypeString(), strPropHelp),
			m_md(md)
		{
		}

		virtual int GetValue( const VistaSensorMeasure *pMeasure ) const
		{
			int nRet = 0;
			GetValue(pMeasure, nRet);
			return nRet;
		}

		virtual bool GetValue( const VistaSensorMeasure *pMeasure, int &nRet ) const
		{
			const VistaFastrakMeasures::sFastrakSample *m = pMeasure->getRead< VistaFastrakMeasures::sFastrakSample >();

			switch(m_md)
			{
			case MD_DISTORTION_LEVEL:
				{
					nRet = m->m_nDistortionLevel;
					return true;
				}
			case MD_TIMESTAMP:
				{
					nRet = m->m_nTimeStamp;
					return true;
				}
			case MD_FRAMECOUNT:
				{
					nRet = m->m_nFrameCount;
					return true;
				}
			default:
				return false;
			}
		}
	private:
		eMd m_md;
	};
	// #############################################################################
	// #############################################################################

	// static initialization of get-functors
	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaFastrackPosGet,
		new VistaFastrackOriGet,
		new VistaFastrackButtonGet,
		new VistaFastrackStatsGet( VistaFastrackStatsGet::MD_DISTORTION_LEVEL, "DISTORTIONLEVEL", "distortion level" ),
		new VistaFastrackStatsGet( VistaFastrackStatsGet::MD_DISTORTION_LEVEL, "TIMESTAMP", "timestamp" ),
		new VistaFastrackStatsGet( VistaFastrackStatsGet::MD_DISTORTION_LEVEL, "FRAMECOUNT", "frame count" ),
		NULL
	};

	class VistaFastrakDriverTranscodeFactoryFactory : public IVistaTranscoderFactoryFactory
	{
	public:
		VistaFastrakDriverTranscodeFactoryFactory()
		{
			CreateCreators(m_creators);
		}

		~VistaFastrakDriverTranscodeFactoryFactory()
		{
			CleanupCreators(m_creators);
		}

		typedef std::map<std::string,ICreateTranscoder*> CRMAP;
		CRMAP m_creators;


		static void CreateCreators(CRMAP &mp)
		{
			mp["WAND"]  = new TCreateTranscoder<VistaFastrakWandTranscoder>;
			mp["BODY"]  = new TCreateTranscoder<VistaFastrakBodyTranscoder>;
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

#ifdef VISTAFASTRAKTRANSCODER_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( VistaFastrakDriverTranscodeFactoryFactory )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( VistaFastrakDriverTranscodeFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaFastrakDriverTranscodeFactoryFactory)
