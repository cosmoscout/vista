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
#include "VistaJswJoystickDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS                                  */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace
{
	class VistaJswJoystickTranscoder : public IVistaMeasureTranscode
	{
		  REFL_INLINEIMP(VistaJswJoystickTranscoder, IVistaMeasureTranscode);
	public:
		VistaJswJoystickTranscoder()
		{
			// inherited as protected member
			m_nNumberOfScalars = 17;
		}

		  static std::string GetTypeString() { return "VistaJswJoystickTranscoder"; }
	};

	class VistaJswJoystickTranscodeFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaJswJoystickTranscoder;
		}
	};


	// TRANSCODERS
	class VistaAxisTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int>
	{
		public:
			VistaAxisTranscode( const std::string &strAxis,
											  const std::string &strHelp,
											  unsigned int nAxis)
		: IVistaMeasureTranscode::TTranscodeValueGet<int>(strAxis,
										VistaJswJoystickTranscoder::GetTypeString(),
										 strHelp),
										 m_nAxis(nAxis)
		{
		}

		virtual int GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaJswJoystickDriver::_sJoyMeasure *m
				= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
			if(m_nAxis < 16)
				return m->m_nAxes[m_nAxis];
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nValue) const
		{
			nValue = GetValue(pMeasure);
			return true;
		}

		private:
			unsigned int m_nAxis;
	};

	class VistaButtonMaskTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<int>
	{
		public:
			VistaButtonMaskTranscode()
		: IVistaMeasureTranscode::TTranscodeValueGet<int>("BUTTONMASK",
										VistaJswJoystickTranscoder::GetTypeString(),
										 "button mask as int (32bit)")
		{

		}

		virtual int GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaJswJoystickDriver::_sJoyMeasure *m
				= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
			int nMask=0;
			for(unsigned int n=0; n < 32; ++n)
			{
				if(m->m_nButtons[n] == true)
					nMask += 1 << n;

	//			if(m->m_nButtons[n] == true)
	//				std::cout << "n="<< n << " : " <<  (m->m_nButtons[n] ? "true":"false")<< std::endl;
			}
			return nMask;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, int &nValue) const
		{
			nValue = GetValue(pMeasure);
			return true;
		}

		private:
			unsigned int m_nAxis;
	};


	class VistaJoystickAxisScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaJoystickAxisScalarTranscode()
			: IVistaMeasureTranscode::ScalarDoubleGet("AXIS",
					VistaJswJoystickTranscoder::GetTypeString(),
									"axis scalar get (0-16)") {}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
									 double &dScalar,
									 unsigned int nIndex ) const
		{
			if(!pMeasure || nIndex >= 16)
				return false;

			VistaJswJoystickDriver::_sJoyMeasure *m
				= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
			dScalar = m->m_nAxes[nIndex];
			return true;
		}
	};

	class VistaJoystickAxisTsScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaJoystickAxisTsScalarTranscode()
			: IVistaMeasureTranscode::ScalarDoubleGet("AXIS_TS",
					VistaJswJoystickTranscoder::GetTypeString(),
									"axis timestamp scalar get (0-16)") {}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
									 double &dScalar,
									 unsigned int nIndex ) const
		{
			if(!pMeasure || nIndex >= 16)
				return false;

			VistaJswJoystickDriver::_sJoyMeasure *m
				= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
			dScalar = double(m->m_nAxesTs[nIndex]);
			return true;
		}
	};

	class VistaJoystickButtonScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaJoystickButtonScalarTranscode()
			: IVistaMeasureTranscode::ScalarDoubleGet("BUTTON",
					VistaJswJoystickTranscoder::GetTypeString(),
									"button scalar get (0-32)") {}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
									 double &dScalar,
									 unsigned int nIndex ) const
		{
			if(!pMeasure || nIndex >= 32)
				return false;

			VistaJswJoystickDriver::_sJoyMeasure *m
				= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
			dScalar = double(m->m_nButtons[nIndex]);
			return true;
		}
	};

	class VistaJoystickButtonTsScalarTranscode : public IVistaMeasureTranscode::ScalarDoubleGet
	{
	public:
		VistaJoystickButtonTsScalarTranscode()
			: IVistaMeasureTranscode::ScalarDoubleGet("BUTTON_TS",
					VistaJswJoystickTranscoder::GetTypeString(),
									"button timestamp scalar get (0-32)") {}

		virtual bool GetValueIndexed(const VistaSensorMeasure *pMeasure,
									 double &dScalar,
									 unsigned int nIndex ) const
		{
			if(!pMeasure || nIndex >= 32)
				return false;

			VistaJswJoystickDriver::_sJoyMeasure *m
				= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];
			dScalar = double(m->m_nButtonsTs[nIndex]);
			return true;
		}
	};


	class VistaJoystickAxisVecGet : public IVistaMeasureTranscode::V3Get
	{
	public:
		VistaJoystickAxisVecGet( const std::string &strAxis,
								  const std::string &strDesc,
								  unsigned int nFirst, unsigned int nSecond, unsigned int nThird = ~0 )
		: IVistaMeasureTranscode::V3Get( strAxis, VistaJswJoystickTranscoder::GetTypeString(), strDesc ),
		  m_nFirst(nFirst), m_nSecond(nSecond), m_nThird(nThird)
		{
		}

		virtual VistaVector3D GetValue(const VistaSensorMeasure *pMeasure)    const
		{
			VistaJswJoystickDriver::_sJoyMeasure *m
				= (VistaJswJoystickDriver::_sJoyMeasure*)&(*pMeasure).m_vecMeasures[0];

			VistaVector3D vec;

			if(m_nFirst != static_cast<unsigned int>(~0) && m_nFirst < 16)
				vec[0] = m->m_nAxes[m_nFirst];

			if(m_nSecond != static_cast<unsigned int>(~0) && m_nSecond < 16)
				vec[1] = m->m_nAxes[m_nSecond];

			if(m_nThird != static_cast<unsigned int>(~0) && m_nThird < 16)
				vec[2] = m->m_nAxes[m_nThird];


			return vec;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure, VistaVector3D &v3Value) const
		{
			v3Value = GetValue(pMeasure);
			return true;
		}

	private:
		unsigned int m_nFirst, m_nSecond, m_nThird;
	};

	static IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaJoystickAxisScalarTranscode,
		new VistaJoystickAxisTsScalarTranscode,
		new VistaJoystickButtonScalarTranscode,
		new VistaJoystickButtonTsScalarTranscode,
		new VistaButtonMaskTranscode,
		new VistaJoystickAxisVecGet("AXIS_1", "axis 1/2 as vec3d", 0, 1),
		new VistaJoystickAxisVecGet("AXIS_2", "axis 3/4 as vec3d", 2, 3),
		new VistaJoystickAxisVecGet("AXIS_3", "axis 5/6 as vec3d", 4, 5),
		NULL
	};

	//// FACTORY
	class VistaJswJoystickTranscoderFactory : public IVistaMeasureTranscoderFactory
	{
	public:
		virtual IVistaMeasureTranscode *CreateTranscoder()
		{
			return new VistaJswJoystickTranscoder;
		}

		virtual void DestroyTranscoder( IVistaMeasureTranscode *transc )
		{
			delete transc;
		}

		virtual std::string GetTranscoderName() const { return VistaJswJoystickTranscoder::GetTypeString(); }

	protected:
	};

} // namespace

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

#ifdef VISTAJSWJOYSTICKDRIVERTRANSCODER_EXPORTS
	DEFTRANSCODERPLUG_FUNC_EXPORTS( TSimpleTranscoderFactoryFactory<VistaJswJoystickTranscoderFactory> )
#else
	DEFTRANSCODERPLUG_FUNC_IMPORTS( TSimpleTranscoderFactoryFactory<VistaJswJoystickTranscoderFactory> )
#endif

DEFTRANSCODERPLUG_CLEANUP;
IMPTRANSCODERPLUG_CLEANUP(TSimpleTranscoderFactoryFactory<VistaJswJoystickTranscoderFactory>)
