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


#ifndef _VISTADRIVERWORKSPACEASPECT_H
#define _VISTADRIVERWORKSPACEASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

#include <string>
#include <list>
#include <map>
#include <VistaMath/VistaBoundingBox.h>


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
 * encapsulate information about the workspace of a device. drivers can add
 * workspaces and associate them with a token. Note that the aspect uses AABB
 * information to describe the bounding boxes. There might be differently shaped
 * workspaces, but then try to change this API.
 */
class VISTADEVICEDRIVERSAPI VistaDriverWorkspaceAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	VistaDriverWorkspaceAspect();
	virtual ~VistaDriverWorkspaceAspect();

	/**
	 * retrieve a workspace by its key, as AABB
	 * @see SetWorkspace()
	 * @param strKey the key
	 * @param bbOut the AABB to write to
	 * @return false if the key did not fit to a registered workspace,
	           in that case the bbOut is untouched
	 */
	bool GetWorkspace(const std::string &strKey, VistaBoundingBox &bbOut) const;

	/**
	 * sets a workspace by a key. This is usually defined by the driver code.
	 * @param strKey the key
	 * @param bb the AABB to describe the workspace given by strKey
	 * @see GetWorkspace()
	 */
	void SetWorkspace( const std::string &strKey, const VistaBoundingBox &bb );

	/**
	 * retrieve a complete list of workspaces registered with this aspect
	 * as a list of keys. select a key from the list and ask for its AABB.
	 */
	std::list<std::string> GetWorkspaceKeys() const;

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

protected:
private:
	std::map<std::string, VistaBoundingBox> m_mpWorkspaces;
	static int m_nAspectId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERLOGGINGASPECT_H


