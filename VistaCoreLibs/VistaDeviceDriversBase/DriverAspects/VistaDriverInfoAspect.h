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


#ifndef _VISTADRIVERINFOASPECT_H
#define _VISTADRIVERINFOASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>


#include <VistaAspects/VistaPropertyAwareable.h>

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
 * A driver may be able to provide the user with additional information,
 * which it already may deliver in human readable form. This aspect can be
 * used in term to retrieve them in the form of a PROPLIST, which can be
 * printed or traversed in a GUI.
 */
class VISTADEVICEDRIVERSAPI VistaDriverInfoAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	VistaDriverInfoAspect();
	virtual ~VistaDriverInfoAspect();


	/**
	 * API to use by a driver, as the PROPLIST is returned as
	 * a reference that can be written to. Users should use the
	 * read-only API
	 * @see GetInfoProps()
	 * @return a reference to the props to be written to.
	 */
	VistaPropertyList &GetInfoPropsWrite();

	/**
	 * returns a copy of the driver props for further inspection.
	 * @return the driver info props.
	 */
	VistaPropertyList  GetInfoProps() const;

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:
private:
	VistaPropertyList m_oProps;
	static int m_nAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERINFOASPECT_H


