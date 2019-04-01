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


#ifndef _VISTASHALLOWDRIVER_H
#define _VISTASHALLOWDRIVER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

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
 * This is a very dummy driver that does nothing but manage sensors and aspects
 * that are attached to and removed from it. It is mainly used in VistaCluster
 * mode where dummy drivers are set up and simply stuffed with data coming in
 * from the network. In order to do this, you have to know what you are doing,
 * admitted. But the cluster code can give a start. The DoSensorUpdate() routine
 * will always return true, as there is nothing to do for the shallow driver.
 * It is merely a proxy for aspects and a container for sensors.
 */
class VISTADEVICEDRIVERSAPI VistaShallowDriver : public IVistaDeviceDriver
{
public:
	VistaShallowDriver(IVistaDriverCreationMethod *crm);
	~VistaShallowDriver();

	/**
	 * @return NULL this method should not be used for the shallow driver
			   as it is a special purpose implementation for a driver that
			   does only do management tasks
	 */
	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

protected:
	bool DoSensorUpdate(VistaType::microtime dTs);
	bool DoConnect() { return true; }
	bool DoDisconnect() { return true; }
private:
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASHALLOWDRIVER_H

