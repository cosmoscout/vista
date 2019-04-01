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


#ifndef _VISTASERIALPORTIMP_H
#define _VISTASERIALPORTIMP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <VistaInterProcComm/Hardware/VistaSerialPortStateDesc.h>

#include <string>
#include <vector>


#include <VistaBase/VistaBaseTypes.h>
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
 * This is an implementation-interface for serial-line drivers under different OSes.
 * It should not be used by users directly, instead they should utilize the
 * ConnectionSerial-construct in order to establish serial-line communication.
 * In addition, this interface should not be shipped to clients. It is for internal
 * use, only.
 */
class VISTAINTERPROCCOMMAPI VistaSerialPortImp
{
public:

	/**
	 * Opens the serial port described here; it is needed to call this prior to Send/Receive calls.
	 * A call to this method will simply open the descriptor for the specified device and try
	 * to GET the current hardware-state. Is should be safe to call this on an already opened port, but
	 * it will return false then.
	 * <br>
	 * <ol>
	 *  <li>Open Port</li>
	 *  <li>Set configuration</li>
	 *  <li>Send/Receive data</li>
	 *  <li>Close Port</li>
	 * </ol><br> is a valid sequence. Note that you can change the configuration at any place in between
	 * Open and Close (but be sure to know what you are doing ;)
	 *
	 * @see GetHardwareState()
	 * @see Close()
	 * @return true iff the device could be opened, false else (including the case where it is called on an already state "open" port)
	 */
	virtual bool          OpenSerialPort() = 0;


	bool          IsOpen() const;

	/**
	 * Closes the serial port that is described by this instance.
	 * Note that it should be safe to call this on a port that is closed already.
	 * Calls to Send/Receive will fail to operate on a closed port. It is safe to
	 * call configuration methods on a closed port, as these will be set once the
	 * port is opened.
	 * @see OpenSerialPort()
	 * @return true iff the once opened port could be closed, false else (e.g. the part was closed already)
	 */
	virtual bool          CloseSerialPort() = 0;

	/**
	 * Receives a max number of bytes from the serial port.
	 * A call to receive will act upon the policy that is set by SetBlockingMode(). In case
	 * that this call will block the caller, it should be possible to override the timeout
	 * for this call to return by a non-zero parameter to iTimeout. In case you pass 0, the settings
	 * of SetBlockingMode() will be used. A call to this method will fail on a closed port and return -1.
	 * @see SetBlockingMode()
	 * @param buffer the place to store the incoming bytes to. Ensure that |buffer| >= length
	 * @param length the maximum length of characters to receive
	 * @param iTimeout the number of millisecs to wait for data until the call returns. 0 indicates "do not wait"
	 * @return the number of bytes that were actually read, or -1 on failure and timeout.
	 */
	virtual int           Receive(void *buffer, const int length, int iTimeout = 0) = 0;

	/**
	 * Sends a number length of bytes in buffer.
	 * A call to this method will return -1 iff the port is not opened.
	 * Note that this call may block, depending on the underlying system/OS
	 * @param buffer the byte-buffer to be sent (ensure |buffer| >= length)
	 * @param length the number of bytes to be sent
	 * @return the number of bytes that were actually sent, or -1 on failure.
	 */
	virtual int           Send(const void *buffer, const int length) = 0;


	/**
	 * Sets the string-name of this port. In case the name is in the list of default names
	 * this method will set the index properly. In case the name is NOT in the list of default
	 * names, this method will set the port-index to CSP_USER and simply copy the sPortName argument
	 * to the internal portname variable. This method will call SetHardwareState() after the
	 * name was set and return that method's answer. Note: even a return value of false will have the
	 * name set properly!
	 * @see SetPortByIndex()
	 * @see VistaPortIndex()
	 * @see GetPortByIndex()
	 * @param sPortName the port name to be used for opening the serial connection
	 * @return true iff HardwareState could be set (e.g. false iff this device is not opened, yet).
	 */
	bool          SetPortByName(const std::string &sPortName);

	/**
	 * Returns the currently set string for this serial-port
	 * @return the name that was set as a port-name.
	 */
	std::string   GetPortName() const;

	/**
	 * Sets the port via an index. The ennumeration scheme does correspond to the hardware layout
	 * on the specific machine. This method will try to figure our the name for this device that does
	 * fit to the index specified. You can set you own index and your own name in case you will give
	 * CSP_USER as an index and the proper name to SetPortByName() after that.
	 * In case you specify CSP_1 <= iIndex <= CSP_4, the corresponding name is matched by the default
	 * name-list that is set-up by the driver. This method will call SetHardwareState() after the
	 * index is set and return that method's return value. Note that the return value false does not
	 * indicate that the index was not set. It does only mean, that the hardware state could not be
	 * set (e.g. because the port is not opened, yet)
	 * @return true iff the hardware state could be set.
	 * @param iIndex the marking that corresponds to the values in VistaPortIndex()
	 */
	bool          SetPortByIndex(VistaSerialPortStateDesc::VistaPortIndex iIndex);

	/**
	 * Returns the index that is set for this device.
	 * @return the marking for the index as specified in VistaPortIndex(), a value of CSP_NA does indicate an uninitialized state
	 */
	VistaSerialPortStateDesc::VistaPortIndex GetPortIndex() const;


	virtual bool          SetSpeed( VistaSerialPortStateDesc::VistaSpeed eSpeed);
	virtual VistaSerialPortStateDesc::VistaSpeed    GetSpeed() const;
	virtual unsigned long GetMaximumSpeed () const;


	virtual bool          SetParity ( VistaSerialPortStateDesc::VistaParity eParam );
	virtual VistaSerialPortStateDesc::VistaParity   GetParity () const;


	virtual bool          SetDataBits ( VistaSerialPortStateDesc::VistaDataBits eParam );
	virtual VistaSerialPortStateDesc::VistaDataBits GetDataBits () const;


	virtual bool          SetStopBits ( VistaSerialPortStateDesc::VistaStopBits eParam );
	virtual VistaSerialPortStateDesc::VistaStopBits GetStopBits () const;


	virtual bool          SetHardwareFlow ( bool bInHandshaking );
	virtual bool          GetIsHardwareFlow() const;


	virtual bool          SetSoftwareFlow ( bool bParam );
	virtual bool          SetSoftwareFlowOnOffChars(char cXonChar,
													char cXoffChar);

	virtual char          GetXonChar() const;
	virtual char          GetXoffChar() const;

	virtual bool          SetCharForParityError(char cErrorChar);
	virtual char          GetCharForParityError() const;

	virtual bool          SetEofChar(char cEofChar);
	virtual char          GetEofChar() const;


	virtual bool          GetIsSoftwareFlow () const;


	virtual bool          SetBlockingMode ( unsigned long inReadInterval,
		unsigned long inReadMultiplyer, unsigned long inReadConstant ) = 0;


	virtual unsigned long WaitForIncomingData(int timeout=0) = 0;

	virtual unsigned long PendingDataSize() const = 0;

	virtual void          SetIsBlocking(bool bBlocking);
	virtual bool          GetIsBlocking() const;


	virtual HANDLE GetDescriptor() const = 0;

	/**
	 * Factory method to create an implementation of a serial port. It is a convinient way to
	 * let OS specific details about implementations be decided at compiletime.
	 * @return an implementation instance that holds this interface.
	 */
	static VistaSerialPortImp *CreateSerialPortImp();

	/**
	 * Calls delete on an implementation of a VistaSerialPort. If this imp
	 * is open, it will be closed first and then be deleted.
	 * @param pImp the pointer to an implementation to be closed and deleted
	 */
	static void                 DestroySerialPortImp(VistaSerialPortImp *pImp);

private:
	/**
	 * Every implementation can keep default names for port CSP_1 - CSP4 in this vector of
	 * names. Iff you set the port by index, the VistaConnectionSerialImp will lookup the
	 * proper name for this in case you set the port by index and vice versa it will set the
	 * proper index iff you set the port by name.
	 * The default-names are hardwired on every OS at compile-time. In case your machine
	 * layout does differ, use either SetPortByName() with the correct name or create symbolic
	 * links to the correct devices corresponding to the hardwired names.
	 */
	std::vector<std::string>    m_vePortNames;

	bool m_bIsOpen;

	bool m_bIsBlocking;

protected:

	/**
	 * The port name that has been set for this unit.
	 */
	std::string    m_sPortName;

	/**
	 * The proper index for this unit.
	 * @see VistaPortIndex()
	 */
	VistaSerialPortStateDesc::VistaPortIndex m_ePortIndex;

	/**
	 * The speed of this unit in terms of baud-flags.
	 * @see VistaSpeed()
	 */
	VistaSerialPortStateDesc::VistaSpeed     m_ePortSpeed;


	/**
	 * Marks the parity for this unit.
	 * @see VistaParity()
	 */
	VistaSerialPortStateDesc::VistaParity    m_ePortParity;

	/**
	 * Marks the number of bits for a single information unit
	 * @see VistaDataBits()
	 */
	VistaSerialPortStateDesc::VistaDataBits  m_ePortDataBits;

	/**
	 * Marks the number of StopBits for this unit.
	 * @see VistaStopBits()
	 */
	VistaSerialPortStateDesc::VistaStopBits  m_ePortStopBits;

	/**
	 * We use the term handshaking as an alias for hardware-flow.
	 * This bool indicates that this unit does do hardware-flow (true), or does not (false).
	 */
	bool           m_bIsHandshaking;

	/**
	 * We use xOnxOff as an alias for software-flow.
	 * This bool indicates that this unit does do software-flow (true), or does not (false).
	 */
	bool           m_bIsxOnxOff;

	char           m_cXonChar,   /**< sequence-begin marking for software-flow */
				   m_cXoffChar,  /**< sequence-end marking for software-flow */
				   m_cErrorChar, /**< transport error marking */
				   m_cEofChar;   /**< indicates end-of-file */


	/**
	 * Constructor, sets default values that should be valid for most devices.
	 * Sets 9600,8,n,1 for an undefined port (index = CSP_NA, port-name = "UNDEFINED")
	 */
	VistaSerialPortImp();

	/**
	 * The destructor, NOTE: this will NOT call close on the device.
	 * YOU HAVE TO CALL CLOSE YOURSELF AND YOU BETTER DO NOT FORGET THIS, BEFORE YOU DELETE
	 * AN INSTANCE OF VistaConnectionSerialImp.
	 */
	virtual ~VistaSerialPortImp();

	/**
	 * This method has to set the real hardware state by "parsing" the state of this instance.
	 * This method must be defined by sub-classes in order to make any sense. In case this
	 * method will fail to set the description properly, it MUST try to get the proper configuration
	 * back from the hardware that is currently set (e.g. via GetHardwareState()).
	 * @see GetHardwareState()
	 * @return true iff the the state that is outlined by this instance to the hardware, false else
	 */
	virtual bool SetHardwareState() = 0;

	/**
	 * This method will try to gain the current state of the hardware and match this to the
	 * description in this instance. This method is likely to fail iff the device could not be opened
	 * or has not been opened etc.
	 * @see SetHardwareState()
	 * @return true iff the state could be read from the hardware and matched on this description
	 */
	virtual bool GetHardwareState() = 0;


	/**
	 * Allows the delay of some microseconds. This is used as serial devices tend to be somewhat
	 * hard on timing constraints. It could be possible to use the VistaTimer::Sleep() for this,
	 * but this would bind the IPC to the VistaTools package.
	 */
	virtual void Delay(int iMsecs) const = 0;


	/**
	 * As for the restrictions in this class, we do not allow direct access to the list of ports,
	 * not even for subclasses. Subclasses may use this method in their constructor to fill up
	 * the list of available default-port names.
	 * The order in which the port-names are added is significant and corresponds to the index
	 * as described in VistaPort. Note that this method will not check for duplicates (you can
	 * add a single string more than one time).
	 * @param sPortName the name to add to the list of default-names for the ports.
	 */
	void    AddPortName(const std::string &sPortName);

	/**
	 * This accessor method allows subclasses to query whether a port-name is in the list of available
	 * default port-names.
	 * @param sPortName the name to look-up
	 * @return a marking as described in VistaPort, CSP_NA iff this name is not in the list of port-names
	 */
	VistaSerialPortStateDesc::VistaPortIndex     HasPortName(const std::string &sPortName) const;

	void SetIsOpen(bool bIsOpen);
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

