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


#ifndef _VISTA3DCSPACENAVIGATORDRIVER_H
#define _VISTA3DCSPACENAVIGATORDRIVER_H

/**
 * @page SpaceNavigatorPage 3DConnexion SpaceNavigator
 *
 * The 3D SpaceNavigator is a cheap Magellan and a nice desktop device.
 * It is connected over an USB bus to the host machine.
 * However, the windows port uses a COM driver which works without
 * the notion of a USB device and by supporting user profiles.
 * The <b>precondition</b> for this driver to function is that
 *
 * - the application implements a message pump (on windows).
 *   This is ok for ViSTA applications. If you plan to use this driver standalone,
 *   check details on windows message pumps.
 * - the shipped driver is installed on the host machine
 * - only the first found SpaceNavigator can be used. The first address is hardcoded to the code
 * - ViSTA opens a profile called „ViSTA“ on windows.
 *   See manual of the SpaceNavigator for more details.
 *   The ViSTA profile should map the buttons to „Button 1“ and „Button 2“ in order to work properly.
 *   Special keys are not supported (though this seems to be trivial).
 * - It does only support the SpaceNavigator and no other device of the 3D Connexion series, and only the first two buttons
 *
 * Note that the driver simply reports the values <b>as reported by the driver</b>.
 * That means: if you alter the values to output by the driver application setting,
 * this is what you will get!! So basically, there are two choices for forcing a specific input pattern:
 * - define the driver settings to report as raw as possible, write code that can handle that
 * - use stoopit code and set your desired values in the panel settings using tht installed driver.
 *
 * @section SpaceNavigatorSubSectionTypes Sensor Types
 *
 * The 3DCSpaceNavigator driver only exports a single sensor,
 * indexed 0 with anonymous type (the empty type string ““).
 * The update rate of the SpaceNavigator is approximately 20Hz (should be less).
 *
 * @section SpaceNavigatorSubSectionExportedAspects Exported Aspects
 * - @ref DriverHistoryPage
 * @todo workspace aspects KNOB
 *
 * @section SpaceNavigatorSubSectionPropertiesBySensor Properties exported by device sensors
 * <table>
 * 	<tr><th>Name</th><th>Type</th><th>Remark</th></tr>
 * 	<tr><td>POSITION</td><td>VistaVector3D</td>
 * 		<td>The position of the cap on the device.
 *      	Raw as output by the COM broker of the driver
 *      	(so it might be dependent on the calibration of the profile used)</td>
 *  </tr>
 * 	<tr><td>ROTATION</td><td>VistaQuaternion</td>
 * 		<td>The rotation defined by the rotation of the cap as standard in ViSTA (positive zero-axis)</td>
 *  </tr>
 * 	<tr><td>AXIS</td><td>VistaAxisAndAngle</td>
 * 		<td>The original values from the driver (rotation axis and angle</td>
 *  </tr>
 * 	<tr><td>BUTTON_1</td><td>bool</td>
 * 		<td>the state of the „button1“ mapping (by default this is the left one)</td>
 *  </tr>
 * 	<tr><td>BUTTON_2</td><td>bool</td>
 * 		<td>the state of the „button2“ mapping (by default this is the right one)</td>
 *  </tr>
 * </table>
 *
 * @section SpaceNavigatorSubSectionIniKeys Ini Keys
 * <table>
 * 	<tr>
 * 		<th>Name</th><th>Type</th><th>Remark</th>
 * </tr>
 * 	<tr>
 * 		<td>NAME</td><td>string</td><td>the name of the device driver</td>
 * 	</tr>
 * 	<tr>
 * 		<td>TYPE</td><td>string</td><td>must be <span class="search_hit">SPACENAVIGATOR</span></td>
 * 	</tr>
 * 	<tr>
 * 		<td>HISTORY</td><td>int</td><td>the number of slots to be access during a read phase</td>
 * 	</tr>
 * 	<tr>
 * 		<td>SENSORS</td><td>list of sections (string)</td><td>the sensors that are exported by the driver</td>
 * 	</tr>
 * </table>
 *
 * @section SpaceNavigatorSubSectionExample Example Configuration
 * @verbatim
[SPACENAV]
NAME = SPACEMOUSE
TYPE = SPACENAVIGATOR
HISTORY = 10
SENSORS = NAV_MAIN

[NAV_MAIN]
RAWID = 0
@endverbatim
 *
 * @section SpaceNavigatorSubSectionOS OS Specific remarks
 * @subsection SpaceNavigatorSubSectionWin Windows
 * - implements the COM example as given by the SDK
 * - in order to compile it, you have to have Microsoft's ATL/MFC tools installed.
 *   This is an option to give once out are installing the visual-C.
 *   The ATL/MFC can be installed afterwards and it should work.
 * - The reported update rate of the device is 17Hz at max when using the 3DC shipped COM deamon.
 * - It was observed that the device can send invalid rotation and angle values (NAN values).
 *   The ViSTA driver maps that out and reports a 0 rotation then.
 *
 * @subsection SpaceNavigatorSubSectionLinux Linux
 * - we are using the HID event layer on linux.
 *   The SpaceNavigator reports „relative“ values, while in truth, it reports absolute values.
 *   As a consequence, the „0“ postition is not reported. So use the data as „relative“ on linux only,
 *   or use a non-linear scaling, or twiddle the head slowly on linux ;)
 * - we are not scaling the positional values of the head knob on linux (range: -400; +400)
 * - we are converting the axis reported to a right handed system to be used for OpenGL processing.
 * - In case your spacenavigator controls the local mouse, use 'xinput set-int-prop „3Dconnexion SpaceNavigator“ „Device Enabled“ 32 0 '
 * - alternatively you can add a policy to the hardware abstraction layer:
 *
 * @verbatim
<deviceinfo version=„0.2“>
 <match key="input.product" string="3Dconnexion SpaceNavigator">
	 <remove key="input.x11_driver" />
 </match>
</deviceinfo>
@endverbatim
 * - to install the spacenavigator on a new machine look at /etc/udev/rules.d and check rights in /dev/input:
 *
 * @verbatim
KERNEL=="event*", ATTRS{product}=="SpaceNavigator", MODE="0666"
@endverbatim
 */


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

// Windows DLL build
#if defined(WIN32) && !defined(VISTA3DCSPACENAVIGATORDRIVER_STATIC) 
#ifdef VISTA3DCSPACENAVIGATORDRIVER_EXPORTS
#define VISTA3DCSPACENAVIGATORAPI __declspec(dllexport)
#else
#define VISTA3DCSPACENAVIGATORAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTA3DCSPACENAVIGATORAPI
#endif

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverWorkspaceAspect;
class IVistaDriverCreationMethod;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * this is a driver for the simple SpaceNav device vom 3DC. It uses the HID
 * event layer on Linux and the 3DC specific driver shipped with windows.
 * On windows, the driver raises a dependency to the installed DCOM drivers,
 * even on compile time.
 * There is currently only support for a two-button device having one puck
 * and no device backwards communication.
 *
 */
class VISTA3DCSPACENAVIGATORAPI Vista3DCSpaceNavigator : public IVistaDeviceDriver
{
public:
	Vista3DCSpaceNavigator(IVistaDriverCreationMethod *);
	virtual ~Vista3DCSpaceNavigator();

	struct VISTA3DCSPACENAVIGATORAPI _sMeasure
	{
		_sMeasure()
			: m_nRotationX(0),
				m_nRotationY(0),
				m_nRotationZ(0),
				m_nRotationAngle(0),

				m_nPositionX(0),
				m_nPositionY(0),
				m_nPositionZ(0),
				m_nLength(0),
				m_NumberOfButtons(2)
		{
			m_nKeys[0] = m_nKeys[1] = 0;
		}

		double m_nRotationX,
			   m_nRotationY,
			   m_nRotationZ,
			   m_nRotationAngle;

		double m_nPositionX,
			   m_nPositionY,
			   m_nPositionZ,
			   m_nLength;

		long   m_NumberOfButtons;
		long   m_nKeys[2];
	};

	//class Parameters : public TParameterContainer<Vista3DCSpaceNavigator>
	//{
	//public:
	//	REFL_DECLARE;

	//	Parameters( Vista3DCSpaceNavigator *parent );
	//	virtual ~Parameters();

	//	enum 
	//	{
	//		MSG_LED_MASK_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
	//		MSG_LAST
	//	};

	//	int  GetLEDMask() const;
	//	bool SetLEDMask( int mask );

	//	bool CanBeep() const;
	//	bool LetBeep( bool beep );

	//private:
	//	enum Beep
	//	{
	//		CAN_BEEP,
	//		CAN_NOT_BEEP,
	//		BEEP_STATE_UNKNOWN
	//	};

	//	Beep m_can_beep;
	//	unsigned int m_led_mask;
	//};

protected:
	virtual bool DoSensorUpdate(VistaType::microtime dTs);

	bool DoConnect();
	bool DoDisconnect();
	bool PhysicalEnable( bool enable_flag );

public:
	struct _sPrivate;
private:
	friend class Parameters;
	_sPrivate *m_pPrivate;
};


class VISTA3DCSPACENAVIGATORAPI Vista3DCSpaceNavigatorCreateMethod : public IVistaDriverCreationMethod
{
public:
	Vista3DCSpaceNavigatorCreateMethod( IVistaTranscoderFactoryFactory * );
	virtual IVistaDeviceDriver *CreateDriver();
};


#endif //__VISTA3DCSPACENAVIGATORDRIVER_H
