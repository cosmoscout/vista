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
#include "VistaViveDriverConfig.h"

namespace
{

	class VistaViveHeadTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaViveHeadTranscode()
		{
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "VistaViveHeadTranscode"; }
		REFL_INLINEIMP(VistaViveHeadTranscode, IVistaMeasureTranscode);
	};

	class VistaViveHeadOrientationTranscode : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaViveHeadOrientationTranscode()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			VistaViveHeadTranscode::GetTypeString(),
			"Vive head's 3D orientation") {}

		virtual VistaQuaternion    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_head_type *m = (VistaViveConfig::VISTA_vive_head_type*)&(*pMeasure).m_vecMeasures[0];
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

	class VistaViveHeadPosTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaViveHeadPosTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION",
			VistaViveHeadTranscode::GetTypeString(),
			"Vive head's 3D position") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_head_type *m = (VistaViveConfig::VISTA_vive_head_type*)&(*pMeasure).m_vecMeasures[0];

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

	class VistaViveHeadPoseTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >
	{
	public:
		VistaViveHeadPoseTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >("POSE",
			VistaViveHeadTranscode::GetTypeString(),
			"Vive head's 3D pose") {}

		virtual VistaTransformMatrix    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_head_type *m = (VistaViveConfig::VISTA_vive_head_type*)&(*pMeasure).m_vecMeasures[0];

			return m->pose;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaTransformMatrix &mMat) const
		{
			mMat = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickTranscode : public IVistaMeasureTranscode
	{
	public:
		VistaViveStickTranscode()
		{
			m_nNumberOfScalars = 0;
		}

		static std::string GetTypeString() { return "VistaViveStickTranscode"; }
		REFL_INLINEIMP(VistaViveStickTranscode, IVistaMeasureTranscode);
	};

	class VistaViveStickOrientationTranscode : public IVistaMeasureTranscode::QuatGet
	{
	public:
		VistaViveStickOrientationTranscode()
			: IVistaMeasureTranscode::QuatGet("ORIENTATION",
			VistaViveStickTranscode::GetTypeString(),
			"Vive stick's 3D orientation") {}

		virtual VistaQuaternion    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];
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

	class VistaViveStickPosTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaViveStickPosTranscode()
			: IVistaMeasureTranscode::V3Get("POSITION",
			VistaViveStickTranscode::GetTypeString(),
			"Vive stick's 3D position") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

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

	class VistaViveStickPoseTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >
	{
	public:
		VistaViveStickPoseTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< VistaTransformMatrix >("POSE",
			VistaViveStickTranscode::GetTypeString(),
			"Vive stick's 3D pose") {}

		virtual VistaTransformMatrix    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->pose;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaTransformMatrix &mMat) const
		{
			mMat = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickTriggerPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaViveStickTriggerPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("TRIGGER_PRESSED",
			VistaViveStickTranscode::GetTypeString(),
			"Vive stick's trigger button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trigger_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickTriggerTouchedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaViveStickTriggerTouchedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("TRIGGER_TOUCHED",
				VistaViveStickTranscode::GetTypeString(),
				"Vive stick's trigger button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];
			return m->trigger_touched;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickTriggerXTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< float >
	{
	public:
		VistaViveStickTriggerXTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< float >("TRIGGER_X",
				VistaViveStickTranscode::GetTypeString(),
				"vive stick trigger's x pos in comp (-1,1)") {}

		virtual float GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_x;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, float &axis) const
		{
			axis = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickGripPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaViveStickGripPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("GRIP_PRESSED",
				VistaViveStickTranscode::GetTypeString(),
				"Vive stick's grip button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->grip_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickTrackpadPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaViveStickTrackpadPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("TRACKPAD_PRESSED",
			VistaViveStickTranscode::GetTypeString(),
			"Vive stick's button trackpad") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			// maker and body structs have the same layout
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickTrackpadTouchedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaViveStickTrackpadTouchedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("TRACKPAD_TOUCHED",
				VistaViveStickTranscode::GetTypeString(),
				"Vive stick's button trackpad") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			// maker and body structs have the same layout
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_touched;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickTrackpadAxesTranscode : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaViveStickTrackpadAxesTranscode()
			: IVistaMeasureTranscode::V3Get("TRACKPADPAD_AXES",
				VistaViveStickTranscode::GetTypeString(),
				"vive stick trackpad's pos in comp (-1,1) -> (x,y)") {}

		virtual VistaVector3D    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return VistaVector3D(m->trackpad_x, m->trackpad_y, 0);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Pos) const
		{
			v3Pos = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickTrackpadXTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< float >
	{
	public:
		VistaViveStickTrackpadXTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< float >("TRACKPAD_X",
				VistaViveStickTranscode::GetTypeString(),
				"vive stick trackpad's x pos in comp (-1,1)") {}

		virtual float GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_x;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, float &axis) const
		{
			axis = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickTrackpadYTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< float >
	{
	public:
		VistaViveStickTrackpadYTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< float >("TRACKPAD_Y",
				VistaViveStickTranscode::GetTypeString(),
				"vive stick trackpad's y pos in comp (-1,1)") {}

		virtual float GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->trackpad_y;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, float &axis) const
		{
			axis = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickSystemButtonPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaViveStickSystemButtonPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("BUTTON_SYSTEM_PRESSED",
				VistaViveStickTranscode::GetTypeString(),
				"Vive stick's system button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->button_system_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	class VistaViveStickMenuButtonPressedTranscode : public IVistaMeasureTranscode::TTranscodeValueGet< bool >
	{
	public:
		VistaViveStickMenuButtonPressedTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet< bool >("BUTTON_MENU_PRESSED",
				VistaViveStickTranscode::GetTypeString(),
				"Vive stick's menu button") {}

		virtual bool    GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaViveConfig::VISTA_vive_stick_type *m = (VistaViveConfig::VISTA_vive_stick_type*)&(*pMeasure).m_vecMeasures[0];

			return m->button_menu_pressed;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, bool &bButton) const
		{
			bButton = GetValue(pMeasure);
			return true;
		}
	};

	IVistaPropertyGetFunctor *SapGetter[] =
	{
		new VistaViveHeadOrientationTranscode,
		new VistaViveHeadPosTranscode,
		new VistaViveHeadPoseTranscode,
		new VistaViveStickOrientationTranscode,
		new VistaViveStickPosTranscode,
		new VistaViveStickPoseTranscode,
		new VistaViveStickTriggerPressedTranscode,
		new VistaViveStickTriggerTouchedTranscode,
		new VistaViveStickTriggerXTranscode,
		new VistaViveStickGripPressedTranscode,
		new VistaViveStickTrackpadPressedTranscode,
		new VistaViveStickTrackpadTouchedTranscode,
		new VistaViveStickTrackpadAxesTranscode,
		new VistaViveStickTrackpadXTranscode,
		new VistaViveStickTrackpadYTranscode,
		new VistaViveStickSystemButtonPressedTranscode,
		new VistaViveStickMenuButtonPressedTranscode,
		NULL
	};

	class VistaViveStickTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaViveStickTranscode;
		}
	};

	class VistaViveHeadTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaViveHeadTranscode;
		}
	};

	class VistaViveDriverTranscoderFactoryFactory : public IVistaTranscoderFactoryFactory
	{
	public:
		VistaViveDriverTranscoderFactoryFactory()
		{
			CreateCreators(m_creators);
		}

		~VistaViveDriverTranscoderFactoryFactory()
		{
			CleanupCreators(m_creators);
		}

		typedef std::map<std::string,ICreateTranscoder*> CRMAP;
		CRMAP m_creators;


		static void CreateCreators(CRMAP &mp)
		{
			mp["STICK"]  = new TCreateTranscoder<VistaViveStickTranscode>;
			mp["HEAD"]   = new TCreateTranscoder<VistaViveHeadTranscode>;
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


#ifdef VistaViveDriverTranscoder_EXPORTS
DEFTRANSCODERPLUG_FUNC_EXPORTS( VistaViveDriverTranscoderFactoryFactory )
#else
DEFTRANSCODERPLUG_FUNC_IMPORTS( VistaViveDriverTranscoderFactoryFactory )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(VistaViveDriverTranscoderFactoryFactory)



