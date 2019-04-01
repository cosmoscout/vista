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


#ifndef _VISTASERIALPORTSTATEDESC_H
#define _VISTASERIALPORTSTATEDESC_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAINTERPROCCOMMAPI VistaSerialPortStateDesc
{
private:

	VistaSerialPortStateDesc( VistaSerialPortStateDesc &) {};

public:
	VistaSerialPortStateDesc() {};
	~VistaSerialPortStateDesc() {};
		/**
	 * defines the baud-rate for this serial-line.
	 * Note that ViSTA does only support 9600-115200 baud rates, although
	 * it might be possible to set different baud rates on the underlying
	 * hardware.
	 */
	enum VistaSpeed
	{
		CSSP_NONE      = 0,      /**< this is illegal and marks an uninitialized state */
		CSSP_9600      = 9600,   /**< this is default speed 9600 baud */
		CSSP_19200     = 19200,  /**< 19200 baud */
		CSSP_38400     = 38400,  /**< 38400 baud */
		CSSP_57600     = 57600,  /**< 57600 baud */
		CSSP_115200    = 115200  /**< 115200 baud */
	};

	/**
	 * ViSTA assumes that there are only up to four different serial-line-plugs on a
	 * single machine. The driver does not react on more than four lines in a single
	 * machine. The ennumeration scheme corresponds to the hardware layout on the
	 * machine, the first port in the system is flagged as CSP_1 etc.
	 * Note that the driver does build a list of names that are used for each index.
	 * In case you do not have a port with the name for that index, you can not use
	 * SetPortByIndex() but instead use SetPortByName() with the proper name.
	 */
	enum VistaPortIndex
	{
		CSP_1   = 1,  /**< the first port on the machine */
		CSP_2   = 2,  /**< the second port on the machine */
		CSP_3   = 3,  /**< the third port on the machine */
		CSP_4   = 4,  /**< the fourth port on the machine */
		CSP_NA  = 0,  /**< this is a marking for not-available */
		CSP_MAX = 5,  /**< this is a virtual mark for the internal datastructures */
		CSP_USER= 6   /**< this is a special mark for a user-defined name of the serial port */
	};


	/**
	 * ViSTA supports none, even and odd parity
	 */
	enum VistaParity
	{
		CSPA_NONE = 0, /**< marks "no-parity" mode */
		CSPA_EVEN = 2, /**< marks "even-parity" mode */
		CSPA_ODD  = 1  /**< marks "odd-parity" mode */
	};


	/**
	 * ViSTA supports no-stopbit, one, two and 1.5 stop-bit mode.
	 * Note that 1.5 is not always supported (e.g. POSIX does not seem to support it)
	 */
	enum VistaStopBits
	{
		CSST_NONE=0, /**< no stopbit */
		CSST_1=1,    /**< one stopbit */
		CSST_2=2,    /**< two stopbits */
		CSST_15=15   /**< 1.5 stopbits, which is not always supported */
	};


	/**
	 * This does indicate the length of a single information unit in bits.
	 * ViSTA does support 5,6,7 and 8 bit units, a NONE-value does reflect an
	 * illegal state and should be treated as error.
	 */
	enum VistaDataBits
	{
		CSD_NONE = 0,  /**< this is an illegal state that does reflect an error */
		CSD_5 = 5,     /**< 5 bits for one information unit */
		CSD_6 = 6,     /**< 6 bits for one information unit */
		CSD_7 = 7,     /**< 7 bits for one information unit */
		CSD_8 = 8      /**< 8 bits for one information unit (default) */
	};
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

