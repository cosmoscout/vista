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


#ifndef _VISTADEVICEDRIVERASPECTREGISTRY_H
#define _VISTADEVICEDRIVERASPECTREGISTRY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <map>
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
 * Singleton to manage the registration and deregistration of aspects.
 * @todo Should be used with care in the context of plugins.
 */
class VISTADEVICEDRIVERSAPI VistaDeviceDriverAspectRegistry
{
public:
	/**
	 * retrieve the one and only registry for aspects ids
	 */
	static VistaDeviceDriverAspectRegistry *GetSingleton();

	/**
	 * register a string and get back a token for that aspect.
	 * The token defines the user-level side, e.g., 'WINDOW'
	 * is a string the will be seen and used by, well..., users.
	 * Internally, only an int is used for processing.
	 * @param strAspectToken the aspects token to register
	 * @return an uint >= 0
	 */
	unsigned int RegisterAspect( const std::string &strAspectToken );

	/**
	 * the reverse mapping of the RegisterAspect() api, get back the
	 * string for the uint that was registered beforehand. Mostly
	 * used for debugging prints and the like. Can take some time
	 * to evaluate, so do not call frequently.
	 * @see RegisterAspect()
	 * @param a uint to query an id for
	 * @return the string that was registered to the id given
	 */
	std::string  GetTokenForId( unsigned int ) const;
protected:
private:
	VistaDeviceDriverAspectRegistry();

	std::map<unsigned int, std::string> m_mpTokenMap;
	unsigned int m_nNextAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADEVICEDRIVERASPECTREGISTRY_H

