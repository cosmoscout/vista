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


#ifndef _VISTAOPENSGGLUTJOYSTICKDRIVER_H
#define _VISTAOPENSGGLUTJOYSTICKDRIVER_H

#if defined(WIN32)
#pragma warning(disable: 4786)
#endif

// Windows DLL build
#if defined(WIN32) && !defined(VISTAGLUTJOYSTICKDRIVER_STATIC)
	#ifdef VISTAGLUTJOYSTICKDRIVER_EXPORTS
		#define VISTAGLUTJOYSTICKDRIVERAPI __declspec(dllexport)
	#else
		#define VISTAGLUTJOYSTICKDRIVERAPI __declspec(dllimport)
	#endif
#else // no Windows or static build
	#define VISTAGLUTJOYSTICKDRIVERAPI
#endif

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <map>
#include <vector>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDriverAbstractWindowAspect;
class IVistaDriverCreationMethod;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAGLUTJOYSTICKDRIVERAPI VistaGlutJoystickDriver : public IVistaDeviceDriver
{
public:
	struct _sJoyMeasure
	{
		double m_nVals[4];
	};

	VistaGlutJoystickDriver( IVistaDriverCreationMethod *);
	virtual ~VistaGlutJoystickDriver();

	static void JoystickFunction(unsigned int, int, int, int );
protected:
	virtual bool DoConnect();
	virtual bool DoDisconnect();

	virtual bool DoSensorUpdate(VistaType::microtime dTs);

private:
	friend class CVistaJoystickScalarTranscode;
	class _sJoySample
	{
		public:
			_sJoySample()
			: m_nBtMask(0),
			m_nAxis1(0),
			m_nAxis2(1),
			m_nAxis3(1) {}

		_sJoySample(unsigned int nBtMask,
					int nAxis1, int nAxis2, int nAxis3)
					: m_nBtMask(nBtMask),
					  m_nAxis1(nAxis1),
					  m_nAxis2(nAxis2),
					  m_nAxis3(nAxis3) {}

		unsigned int m_nBtMask;
		int          m_nAxis1;
		int          m_nAxis2;
		int          m_nAxis3;

		bool operator==(const _sJoySample &oOther) const;
		_sJoySample &operator=(const _sJoySample &oOther);
	};
	std::vector< _sJoySample > m_vecJoyVec;

	VistaDriverAbstractWindowAspect *m_pWindowAspect;
};


class VISTAGLUTJOYSTICKDRIVERAPI VistaGlutJoystickDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaGlutJoystickDriverCreationMethod( IVistaTranscoderFactoryFactory *fac );
	virtual IVistaDeviceDriver *CreateDriver();
protected:
private:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //_VISTAOPENSGGLUTKEYBOARDDRIVER_H
