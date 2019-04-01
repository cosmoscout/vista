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


#ifndef IDLVISTAREGISTRATION_H
#define IDLVISTAREGISTRATION_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <string>
#include <map>
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This class serves as a base-class for different packet-registration mechanisms.
 * It is supposed to be more or less an information source for clients and the
 * definition of an interface for specialized classes.
 */
class VISTAINTERPROCCOMMAPI IDLVistaRegistration
{
private:
	/**
	 * We prohibit the copying.
	 */
	IDLVistaRegistration(IDLVistaRegistration &);

//	typedef map<string, int> NAME2TYPE;
//	typedef map<int, string> TYPE2NAME;

	std::map<std::string, int> m_mpNames2Types;
	std::map<int, std::string> m_mpTypes2Names;

	static IDLVistaRegistration *m_pSingleton; /**< there can only be one... */

protected:
	IDLVistaRegistration();

	int RegisterPacketType(const std::string &SName);
	bool RegisterPacketType(const std::string &SName, int iTypeId);


	bool UnregisterPacketType(int iTypeId);
	bool UnregisterPacketType(const std::string &SPacketName);


	static bool RegisterRegistration(IDLVistaRegistration *);

	virtual int CreateTypeIdForName(const std::string &SName) = 0;

public:

	virtual std::string GetNameForTypeId(int iTypeId) const;

	virtual int       GetTypeIdForName(const std::string &SName) const;


	virtual ~IDLVistaRegistration();

	static IDLVistaRegistration *GetRegistrationSingleton();

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //IDLVISTAFILTER_H


