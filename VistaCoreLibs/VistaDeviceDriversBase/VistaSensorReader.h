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


#ifndef _VISTASENSORREADER_H_
#define _VISTASENSORREADER_H_


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaDeviceDriversConfig.h"
#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaNameable.h>
#include <map>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaSensorCoherenceGroup;
class IVistaDeviceDriver;
class VistaDeviceSensor;
class VistaSensorReadState;
class VistaObjectRegistry;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * \brief Interface to acquire a read state for a device sensor
 *
 * Defines the api (GetReadState()) to retrieve a VistaSensorReadState for a given
 * VistaDeviceSensor. This interface is nameable.
 * This interface does not define how the retrieval is actually done. Can be used
 * to define light-weight access interfaces to read states (for example a common read
 * state for all driver sensors).
 */
class VISTADEVICEDRIVERSAPI IVistaReadStateSource : public IVistaNameable
{
public:
	virtual VistaSensorReadState *      GetReadState( VistaDeviceSensor & ) = 0;

	virtual std::string GetNameForNameable() const;
	virtual void SetNameForNameable(const std::string &sNewName);
protected:
	IVistaReadStateSource()
		: IVistaNameable()
		, m_name() {}

	IVistaReadStateSource( const std::string &name )
		: IVistaNameable()
		, m_name(name) {}
private:
	std::string                m_name;
};


/**
 * \brief intermediate shallow interface that allows for registration or read states with sensors.
 *
 * Introduces the update method to be used to update all sensors attached  to this SensorReader.
 */
class VISTADEVICEDRIVERSAPI IVistaSensorReader : public IVistaReadStateSource
{
public:
	virtual ~IVistaSensorReader();
	virtual bool Update() = 0;
protected:
	IVistaSensorReader( const std::string &strName );

private:

};

/**
 * \brief binds the read state source to a given driver.
 *
 * Abstract interface to retrieve read states for all sensors of a given driver.
 */
class VISTADEVICEDRIVERSAPI IVddReadstateSource : public IVistaReadStateSource
{
public:
	virtual ~IVddReadstateSource() {}
	virtual std::map<VistaDeviceSensor*, VistaSensorReadState*> GetReadStatesForDriver( IVistaDeviceDriver * ) = 0;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTASENSORREADER_H_


