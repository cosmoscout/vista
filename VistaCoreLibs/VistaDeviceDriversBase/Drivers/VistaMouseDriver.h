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


#ifndef _VISTAMOUSEDRIVER_H
#define _VISTAMOUSEDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a basic class for mouse drivers. subclass when encountering a new ui toolkit.
 * this driver defines some common basics about the ViSTA mouse handling. There
 * is no one on earth that can stop you from developing your own mouse code
 * or driver. But then again, you can use this one as a start :)
 * Mice, as well as keyboards, need to be attached to a window. As you may notice
 * this IVistaMouseDriver does not talk about windows, as it only encapsulates
 * logic, fields to record and not more. Subclasses have to decide about
 * how to handle window attachments.
 */
class VISTADEVICEDRIVERSAPI IVistaMouseDriver : public IVistaDeviceDriver
{
public:
	class VISTADEVICEDRIVERSAPI MouseDriverParameters : public VistaDriverGenericParameterAspect::IParameterContainer
	{
		REFL_DECLARE
	public:
		MouseDriverParameters( IVistaMouseDriver* pDriver );

		enum
		{
			MSG_CAPTURE_CURSOR_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
			MSG_REPORT_DELTA_CHG,			
			MSG_LAST
		};

		bool GetCaptureCursor() const;
		bool SetCaptureCursor( bool bCapture );

	private:
		bool m_bCaptureCursor;
	};
public:
	virtual ~IVistaMouseDriver();

	enum eBt
	{
		BT_LEFT=0,
		BT_MIDDLE,
		BT_RIGHT,
		BT_WHEEL_STATE, /**< integrated wheel state */
		BT_WHEEL_DIR,   /**< mouse wheel direction, 0 (not moved at all),
		                    -1 or 1 for up/down, depending on your mouse driver */
		BT_LAST
	};

	/**
	 * memory layout of a mouse measure. use this for
	 * casting the slot pointer to a meaningful scheme.
	 */
	struct _sMouseMeasure
	{
		int m_nButtons[IVistaMouseDriver::BT_LAST];
		int m_nX,
			m_nY;
	};


	/**
	 * adds a new sensor to this device, in case it does not have a transcoder
	 * attached, a transcoder will be created and attached.
	 * @return the physical index of the sensor
	 */
	virtual unsigned int AddDeviceSensor(VistaDeviceSensor *pSensor,
										size_t pos = ANYPOS, 
		                                AllocMemoryFunctor *amf = NULL,
										ClaimMemoryFunctor *cmf = NULL );

	/**
	 * in case you want to add your own transcoders for mouse measures, this
	 * token can be handy. Register this with the functor reflectionable type,
	 * and things should work as expected.
	 * @return the transcoder type string used for registering the get-functors
	 */
	static std::string GetMouseTranscodeName();



protected:
	IVistaMouseDriver(IVistaDriverCreationMethod *crm);

	/**
	 * subclass API, call upon a new mouse position for sensor nIdx.
	 * the reference frame of the x and y values is not defined (may
	 * be root or may be window... depending heavily on the mouse implementation
	 * for the real mouse
	 * @param nIdx the sensor (physical) index to update.
	 * @param x x coord
	 * @param y y coord
	 * @return falls if new measure slot was available (history not set?)
	 */
	bool UpdateMousePosition(unsigned int nIdx, int x, int y);

	/**
	 * subclass API, call upon a new mouse button event for sensor nIdx
	 * @todo check: why is nPressed a double?
	 * @param nIdx the sensor index to update
	 * @param nMouseBt the button state to report
	 * @param nPressed is converted to int and simply set
	 * @return nMouseBt is greater than BT_LAST, or no slot was available (history size?)
	 */
	bool UpdateMouseButton(unsigned int nIdx, eBt nMouseBt,
						   double nPressed);

	MouseDriverParameters* GetParameters();

private:
	VistaDriverGenericParameterAspect* m_pParams;
};

/**
 * used by subclasses to create transcoders co-responding to the measure type
 * defined above.
 */
class VISTADEVICEDRIVERSAPI VistaMouseDriverTranscodeFactory : public IVistaMeasureTranscoderFactory
{
public:
	virtual IVistaMeasureTranscode *CreateTranscoder();
	virtual void DestroyTranscoder( IVistaMeasureTranscode * );
};
namespace VistaMouseDriverUtil
{
	void ReleaseProperties();
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMOUSEDRIVER_H
