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


#ifndef _VISTASERIALPORT_H
#define _VISTASERIALPORT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <string>

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaBase/VistaBaseTypes.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSerialPortImp;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


/**
 * This is the user-level class for SerialPort-direct access. It does the job
 * of hiding implementation details (e.g. the used platform to the user).
 */
class VISTAINTERPROCCOMMAPI VistaSerialPort
{
private:

	/**
	 * A pointer to an implementation instance for real serial port access.
	 * this is never null during the lifetime of an instance of VistaSerialPort.
	 */
	VistaSerialPortImp *m_pImp;
protected:
public:

	/**
	 * Does create the correct implementation for the current platform.
	 */
	VistaSerialPort();

	/**
	 * Does create the correct implementation for the current platform, but
	 * in addition to that is directly sets the specified port
	 * as given in sPortName as port name. Thus it is possible to simply
	 * call VistaSerialPort::OpenSerialPort() without a call to VistaSerialPort::SetPortName()
	 * beforehand. This is for convinience, as you do not necessarily need to have
	 * all information right at hand when using VistaSerialPorts.
	 * @param sPortName the name to give to VistaSerialPort::SetPortName()
	 * @see SetPortName()
	 */
	VistaSerialPort(const std::string &sPortName);

	/**
	 * Closes serial port. It is safe to close a port more than once (closed ports
	 * simply stay closed).
	 */
	virtual ~VistaSerialPort();


	/**
	 * Opens the serialport. Note that either the portname or the portindex MUST be set
	 * BEFORE calling OpenSerialPort(). Otherwise this method will return false.
	 * A port can be opened when
	 * <ul>
	 *  <li>there is a physical port with the given name (either through index or by name)
	 *  <li>this port is not assigned by other applications or devices in this application
	 * </ul>
	 * Note that, depending on device and OS, you can even open a port although THERE IS
	 * NO DEVICE ATTACHED! If this port opens in such a case, the first try to communicate
	 * with the assumed device WILL fail. Note that other OSes might react differently, it
	 * sometimes even seems to depend on the used UART.
	 * It is safe to open a port more than once (an open port will stay open)
	 * @return true iff the port was openend.
	 */
	virtual bool OpenSerialPort();

	/**
	 * Closes the serialport. This will succeed on non-opened ports (alas... closed ports
	 * will stay closed). Calls to Send and Receive will fail on a closed port.
	 * @return true iff the port could be closed or was closed already
	 */
	virtual bool CloseSerialPort();

	/**
	 * Receives a number of bytes from the serialport and store them in a buffer. In case
	 * there are not as many bytes as given as length parameter and a timeout unqeual to
	 * zero is given, the method will return after the timeout has passed, the number
	 * of read bytes is returned as a result then. Note that the behaviour of this method
	 * is determined by the general blocking behaviour of the serial-port! In case there
	 * is a special request or the general setting have to be overruled, one can use
	 * the iTimout-Parameter in conjunction with this call.
	 * @param buffer the place to store the incoming data to. must be: |buffer| >= length
	 * @param length the number of bytes to read at maximum
	 * @param iTimeout the number of microseconds to wait for |length|-sized data, 0 for no timeout at all
	 * @return -1 on failure, the number of bytes read otherwise
	 */
	virtual int Receive(void *buffer, const int length, int iTimeout = 0);

	/**
	 * Sends the number of length bytes beginning at buffer. Note that |buffer| >= length
	 * is a must, although not critical (depends on the mercy of the OS).
	 * This method returns the number of bytes that could be sent.
	 * @param buffer the data to send
	 * @param length the number of bytes to send
	 * @return -1 on failure else the number of bytes that could be sent successfully
	 */
	virtual int Send(const void *buffer, const int length);


	/**
	 * Sets the port name that is used in the try to OpenSerialPort().
	 * It has either to be called beforhand to the call to OpenSerialPort() or it will
	 * be called in one of the present constructors. In case the given name matches with
	 * one of the names that is present as a default for this driver, the appropriate
	 * index will be set automatically. In case the name does not match, the index will be
	 * set to VistaSerialPortDesc::CSP_USER value.
	 * @see OpenSerialPort()
	 * @see GetPortByIndex()
	 * @see SetPortByIndex()
	 * @return true iff the name could be set as the name to use on open
	 */
	bool          SetPortByName(const std::string &sPortName);

	/**
	 * Returns the name that was set by SetPortName() or that was determined after setting
	 * the appropriate index that resolved a specific default name for the current machine.
	 * @return the name that will be used at the call to OpenSerialPort()
	 * @see OpenSerialPort()
	 */
	std::string   GetPortName() const;

	/**
	 * Sets the proper index for this serial port. Note that usually any os will have an
	 * ennumeration of SerialPorts in its system. There are seldom more than 4, so valid
	 * entries for this method will be in between 1 and 4. There are only default names
	 * for any systems in between 1 and 4. Any index above that REQUIRES the correct name
	 * to be set by SetPortName().
	 * @param iIndex the index to set for this port, this better be in between 1 and 4, but iff you have more, give that one
	 * @return true iff the index could be resolved as a default port and internal values are set correctly
	 */
	bool          SetPortByIndex(int iIndex);

	/**
	 * Gets the set port-index for this port. You can call this after you used SetPortName() in order
	 * to resolve the index for this port as given in the list of default names for the current OS.
	 * @return the index for this port (as enumerated by the OS)
	 */
	int           GetPortIndex() const;

	/**
	 * Set the speed for this serial port. The usual values are supported. Try
	 * <ul>
	 *  <li>9600</li>
	 *  <li>19200</li>
	 *  <li>28800</li>
	 *  <li>38400</li>
	 *  <li>57600</li>
	 *  <li>115200</li>
	 *  </ul>
	 * though it is not always guaranteed that you will be able to fetch the incoming
	 * data fast enough in order to avoid buffer-overflows. You can always set the speed of a
	 * port, any change will be propagated to the hardware directly after that. (most of the times
	 * you do not really wnat this to happen, but you could...).
	 * @param eSpeed a numerical value to set the speed of this device to. Has to be one of the listed above.
	 * @return true iff the speed-value was noted sucessfully (meaning that it was one of the above listed)
	 */
	bool          SetSpeed( int eSpeed);

	/**
	 * Returns the currently set speed of this serialport. This is one of the following.
	 * <ul>
	 *  <li>9600</li>
	 *  <li>19200</li>
	 *  <li>28800</li>
	 *  <li>38400</li>
	 *  <li>57600</li>
	 *  <li>115200</li>
	 *  </ul>
	 * Note that it is tried to gain the set speed directly from the hardware, so a call pair like
	 * SetSpeed(115200);  GetSpeed() == 115200 ? true: false; can give FALSE (in case the speed could
	 * not be set, as the hardware did not support it).
	 * @return the set baud for this serialport.
	 */
	int           GetSpeed() const;

	/**
	 * returns the theoretical maximum speed for this serial device. Note that this is implementation dependant,
	 * and is not queried from the hardware.
	 * @return 115200 currently.
	 */
	unsigned long GetMaximumSpeed () const;


	/**
	 * Sets the parity value for this device. Not all implementations support all possible parity
	 * values. A call to this method is forwarded directly to the hardware and might fail if the
	 * value could not be set. This is the case when the port is not yet opened. The value set will
	 * be safed and used at the call to OpenSerialPort() so do not worry about failures of this method
	 * on non-open ports.
	 * @param eParam 0 for none, 1 for odd, 2 for even.
	 * @return false iff the parity value could not be set to the underlying hardware (e.g. when the serialport is not open yet)
	 */
	bool          SetParity ( int eParam );

	/**
	 * returns the currently set parity.
	 * @return 0 for none, 1 for odf, 2 for even parity.
	 */
	int           GetParity () const;


	/**
	 * Sets the number of databits that is used for each informational entity.
	 * You can use any value in between 5 and 8. A call to this method is forwarded directly to the hardware and might fail if the
	 * value could not be set. This is the case when the port is not yet opened. The value set will
	 * be safed and used at the call to OpenSerialPort() so do not worry about failures of this method
	 * on non-open ports.
	 * @param eParam either 5,6,7 or 8
	 * @return false iff the value could not be set to the underlying hardware (e.g. when the serial port is not opened, yet)
	 */
	bool          SetDataBits ( int eParam );

	/**
	 * Returns the number of databits that are used for each information entity that is transported over this
	 * serialport. This will be either 5,6,7 or 8
	 * @return either 5,6,7 or 8
	 */
	int           GetDataBits () const;


	/**
	 * Sets the number of stopbits that are used for transmission. Note that not all implementations support
	 * any possible value here. WIN32 supports 1,2 and 1.5 stopbits (do not ask me what 1.5 stopbits means...),
	 * whereas POSIX does only support 1 or none stopbit at all. Note that the argument is a float value, which
	 * means that you have to give 1.5 stopbits as (float)1.5. THIS MIGHT CHANGE. A call to this method is forwarded directly to the hardware and might fail if the
	 * value could not be set. This is the case when the port is not yet opened. The value set will
	 * be safed and used at the call to OpenSerialPort() so do not worry about failures of this method
	 * on non-open ports.
	 * @param eParam either 0, 1, 2 or 1.5
	 * @return true iff the value could be set to the underlying hardware.
	 */
	bool          SetStopBits ( float eParam );

	/**
	 * Returns the number of stopbits that are set to use for this serialport. See notes at the SetStopBits() for
	 * hints to the possible return values.
	 * @see SetStopBits()
	 * @return either 0, 1, 2 or 1.5
	 */
	float           GetStopBits () const;


	/**
	 * Enabled hardwareflow for this serialport. Note that improper settings here are mostly the cause for a
	 * non-funtionig communication. Iff you have a blocking device or something reacts awfully SLOW, this might
	 * be caused by different handshaking (HW-flow) settings on either host or device.
	 * @return true iff hardware flow was enabled, false else
	 * @param bInHandshaking true iff hardwareflow is to be enabled, false else
	 */
	bool          SetHardwareFlow ( bool bInHandshaking );

	/**
	 * Returns whether hardwareflow is enabled for this serialport.
	 * @return true iff hardwareflow is enabled for this serialport, false else
	 */
	bool          GetIsHardwareFlow() const;


	/**
	 * Sets softwareflow to "on" for this serialport. Note that the Xon and Xoff chars are set by the
	 * driver as default values. In order to change this, please contact one of the authors of ViSTA.
	 * @return true iff software-flow could be enabled, false else
	 * @param bParam true iff software-flow is to be enabled to this serialport, false else.
	 */
	bool          SetSoftwareFlow ( bool bParam );

	/**
	 * Returns whether softwareflow is enabled for this serialport.
	 * @return true iff software flow in on, false else
	 */
	bool          GetIsSoftwareFlow () const;

	/**
	 * Better leave this method alone. It is likely to change in the near future.
	 */
	bool          SetBlockingMode ( unsigned long inReadInterval, unsigned long inReadMultiplyer, unsigned long inReadConstant );


	/**
	 * This method puts this serialport into sleep-mode until either data arrives at its port
	 * or a timeout occurs. While in sleep-mode, it will not occupy any cpu-performance. This call does
	 * return before the number of given milliseconds if more than 0 characters are ready to be fetched.
	 * @param timeout the number of milliseconds to wait for data before this call returns
	 * @return the number of bytes that can be fetched from the device.
	 */
	virtual unsigned long WaitForIncomingData(int timeout=0);


	/**
	 * Returns the number of bytes that can be fetched from this serialport. This can be a quite
	 * expensive operation, so try to avoid calling it frequently.
	 * @return the number of bytes that can be read by a call to Receive() in a single block
	 */
	virtual unsigned long PendingDataSize() const;


	/**
	 * Sets this serialport to total blocking mode. Iff this serialport is blocking, any call to send or
	 * receive will only return when the specified number of bytes could be transmitted successfully.
	 * Note that SetIsBlocking(true) will set default timeout parameters for the timeout behaviour that
	 * are defined on driver level
	 * @param bBlocking true iff calls to Send/Receive shall block the caller until the request succeeded.
	 */
	virtual void          SetIsBlocking(bool bBlocking);

	/**
	 * returns whether this SerialPort ist in BlockingMode. If it is in blocking mode, any call to
	 * send/receive will block the caller until the request is successfully fullfilled.
	 */
	virtual bool          GetIsBlocking() const;

	virtual HANDLE           GetDescriptor() const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASERIALPORT_H

