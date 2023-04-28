/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*            Copyright (c) 2016-2019 German Aerospace Center (DLR)           */
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
#include "VistaOpenVRDriverConfig.h"

namespace
{

	class VistaOpenVRHeadTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaOpenVRHeadTranscode()
		{
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "VistaOpenVRHeadTranscode"; }
		REFL_INLINEIMP(VistaOpenVRHeadTranscode, IVistaMeasureTranscode);
	};

	class VistaOpenVRHeadOrientationTranscode : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaOpenVRHeadOrientationTranscode()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			VistaOpenVRHeadTranscode::GetTypeString(),
			"OpenVR head's 3D orientation") {}

		virtual VistaQuaternion    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_head_type *m = (VistaOpenVRConfig::VISTA_openvr_head_type*)&(*pMeasure).m_vecMeasures[0];
			// VistaTransformMatrix mt (float(m->rot[0]), float(m->rot[3]), float(m->rot[6]), 0,
			// 	float(m->rot[1]), float(m->rot[4]), float(m->rot[7]), 0,
			// 	float(m->rot[2]), float(m->rot[5]), float(m->rot[8]), 0,
			// 	0    , 0    , 0    , 1);
			// VistaQuaternion q(mt);
			// q.Normalize();
			// return q;
			return m->orientation;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &qRot) const
		{
			qRot = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRHeadPosTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaOpenVRHeadPosTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION",
			VistaOpenVRHeadTranscode::GetTypeString(),
			"OpenVR head's 3D position") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_head_type *m = (VistaOpenVRConfig::VISTA_openvr_head_type*)&(*pMeasure).m_vecMeasures[0];

			return VistaVector3D(
				float( m->loc[0]),
				float( m->loc[1]),
				float( m->loc[2]));
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRHeadPoseTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >
	{
	public:
		VistaOpenVRHeadPoseTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >("POSE",
			VistaOpenVRHeadTranscode::GetTypeString(),
			"OpenVR head's 3D pose") {}

		virtual VistaTransformMatrix    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_head_type *m = (VistaOpenVRConfig::VISTA_openvr_head_type*)&(*pMeasure).m_vecMeasures[0];

			return m->pose;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaTransformMatrix &mMat) const
		{
			mMat = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaOpenVRStickTranscode()
		{
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "VistaOpenVRStickTranscode"; }
		REFL_INLINEIMP(VistaOpenVRStickTranscode, IVistaMeasureTranscode);
	};

	class VistaOpenVRStickOrientationTranscode : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaOpenVRStickOrientationTranscode()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			VistaOpenVRStickTranscode::GetTypeString(),
			"OpenVR stick's 3D orientation") {}

		virtual VistaQuaternion    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];
			// VistaTransformMatrix mt (float(m->rot[0]), float(m->rot[3]), float(m->rot[6]), 0,
			// 	float(m->rot[1]), float(m->rot[4]), float(m->rot[7]), 0,
			// 	float(m->rot[2]), float(m->rot[5]), float(m->rot[8]), 0,
			// 	0    , 0    , 0    , 1);
			// VistaQuaternion q(mt);
			// q.Normalize();
			// return q;
			return m->orientation;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaQuaternion &qRot) const
		{
			qRot = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickPosTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaOpenVRStickPosTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION",
			VistaOpenVRStickTranscode::GetTypeString(),
			"OpenVR stick's 3D position") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return VistaVector3D(
				float( m->loc[0]),
				float( m->loc[1]),
				float( m->loc[2]));
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickPoseTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >
	{
	public:
		VistaOpenVRStickPoseTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >("POSE",
			VistaOpenVRStickTranscode::GetTypeString(),
			"OpenVR stick's 3D pose") {}

		virtual VistaTransformMatrix    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->pose;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaTransformMatrix &mMat) const
		{
			mMat = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickTriggerPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaOpenVRStickTriggerPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("TRIGGER_PRESSED",
			VistaOpenVRStickTranscode::GetTypeString(),
			"OpenVR stick's trigger button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trigger_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickTriggerTouchedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaOpenVRStickTriggerTouchedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("TRIGGER_TOUCHED",
				VistaOpenVRStickTranscode::GetTypeString(),
				"OpenVR stick's trigger button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];
			return m->trigger_touched;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickTriggerXTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< float >
	{
	public:
		VistaOpenVRStickTriggerXTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< float >("TRIGGER_X",
				VistaOpenVRStickTranscode::GetTypeString(),
				"openvr stick trigger's x pos in comp (-1,1)") {}

		virtual float GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_x;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, float &axis) const
		{
			axis = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickGripPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaOpenVRStickGripPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("GRIP_PRESSED",
				VistaOpenVRStickTranscode::GetTypeString(),
				"OpenVR stick's grip button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->grip_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickTrackpadPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaOpenVRStickTrackpadPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("TRACKPAD_PRESSED",
			VistaOpenVRStickTranscode::GetTypeString(),
			"OpenVR stick's button trackpad") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			// maker and body structs have the same layout
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickTrackpadTouchedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaOpenVRStickTrackpadTouchedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("TRACKPAD_TOUCHED",
				VistaOpenVRStickTranscode::GetTypeString(),
				"OpenVR stick's button trackpad") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			// maker and body structs have the same layout
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_touched;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickTrackpadAxesTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaOpenVRStickTrackpadAxesTranscode()
			: IVistaMeasureTranscode::V3Get("TRACKPADPAD_AXES",
				VistaOpenVRStickTranscode::GetTypeString(),
				"openvr stick trackpad's pos in comp (-1,1) -> (x,y)") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return VistaVector3D(m->trackpad_x, m->trackpad_y, 0);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickTrackpadXTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< float >
	{
	public:
		VistaOpenVRStickTrackpadXTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< float >("TRACKPAD_X",
				VistaOpenVRStickTranscode::GetTypeString(),
				"openvr stick trackpad's x pos in comp (-1,1)") {}

		virtual float GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_x;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, float &axis) const
		{
			axis = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickTrackpadYTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< float >
	{
	public:
		VistaOpenVRStickTrackpadYTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< float >("TRACKPAD_Y",
				VistaOpenVRStickTranscode::GetTypeString(),
				"openvr stick trackpad's y pos in comp (-1,1)") {}

		virtual float GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_y;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, float &axis) const
		{
			axis = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickSystemButtonPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaOpenVRStickSystemButtonPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("BUTTON_SYSTEM_PRESSED",
				VistaOpenVRStickTranscode::GetTypeString(),
				"OpenVR stick's system button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->button_system_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickMenuButtonPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaOpenVRStickMenuButtonPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("BUTTON_MENU_PRESSED",
				VistaOpenVRStickTranscode::GetTypeString(),
				"OpenVR stick's menu button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->button_menu_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaOpenVRStickButtonAPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaOpenVRStickButtonAPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("BUTTON_A_PRESSED",
				VistaOpenVRStickTranscode::GetTypeString(),
				"OpenVR stick's A button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaOpenVRConfig::VISTA_openvr_stick_type *m = (VistaOpenVRConfig::VISTA_openvr_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->button_a_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	IVistaPropertyGetFunctor *SapGetter[] =
	{
		new VistaOpenVRHeadOrientationTranscode,
		new VistaOpenVRHeadPosTranscode,
		new VistaOpenVRHeadPoseTranscode,
		new VistaOpenVRStickOrientationTranscode,
		new VistaOpenVRStickPosTranscode,
		new VistaOpenVRStickPoseTranscode,
		new VistaOpenVRStickTriggerPressedTranscode,
		new VistaOpenVRStickTriggerTouchedTranscode,
		new VistaOpenVRStickTriggerXTranscode,
		new VistaOpenVRStickGripPressedTranscode,
		new VistaOpenVRStickTrackpadPressedTranscode,
		new VistaOpenVRStickTrackpadTouchedTranscode,
		new VistaOpenVRStickTrackpadAxesTranscode,
		new VistaOpenVRStickTrackpadXTranscode,
		new VistaOpenVRStickTrackpadYTranscode,
		new VistaOpenVRStickSystemButtonPressedTranscode,
		new VistaOpenVRStickMenuButtonPressedTranscode,
		new VistaOpenVRStickButtonAPressedTranscode,
		NULL
	};

	class VistaOpenVRStickTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaOpenVRStickTranscode;
		}
	};

	class VistaOpenVRHeadTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaOpenVRHeadTranscode;
		}
	};

	class VistaOpenVRDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory
	{
	public:
		VistaOpenVRDriverTranscoderFactoryFactory()
		{
			CreateCreators(m_creators);
		}

		~VistaOpenVRDriverTranscoderFactoryFactory()
		{
			CleanupCreators(m_creators);
		}

		typedef std::map<std::string,ICreateTranscoder*> CRMAP;
		CRMAP m_creators;


		static void CreateCreators(CRMAP &mp)
		{
			mp["STICK"]  = new TCreateTranscoder<VistaOpenVRStickTranscode>;
			mp["HEAD"]   = new TCreateTranscoder<VistaOpenVRHeadTranscode>;
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


#ifdef VistaOpenVRDriverTranscoder_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( VistaOpenVRDriverTranscoderFactoryFactory )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( VistaOpenVRDriverTranscoderFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaOpenVRDriverTranscoderFactoryFactory)



