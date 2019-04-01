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


#ifndef _VISTADEVICEIDENTIFICATIONASPECT_H
#define _VISTADEVICEIDENTIFICATIONASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

#include <string>
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
 * Helper aspect for USB devices or all devices that follow the
 * vendor/product/serial identification scheme. In case your driver supports
 * this, feel free to implement it for your device.
 * Usually, the Set() methods are called from the driver whereas the Get()
 * methods are called from the user side.
 * The API should be self-explanatory.
 */
class VISTADEVICEDRIVERSAPI VistaDeviceIdentificationAspect
				   : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	VistaDeviceIdentificationAspect();
	virtual ~VistaDeviceIdentificationAspect();

	int GetVendorID() const;
	void SetVendorID(int);
	int GetProductID() const;
	void SetProductID(int);
	int GetSerialNumber() const;
	void SetSerialNumber(int);
	std::string GetDeviceName() const;
	void SetDeviceName( const std::string& sName );

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:
private:
	unsigned int m_nVendorID;
	unsigned int m_nProductID;
	unsigned int m_nSerialNumber;
	std::string m_sDeviceName;

	static int m_nAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADEVICEIDENTIFICATIONASPECT_H
