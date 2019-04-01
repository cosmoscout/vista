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


#ifndef _VISTADRIVERMAP_H
#define _VISTADRIVERMAP_H



/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"

#include <VistaAspects/VistaObserveable.h>

#include <map>
#include <string>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaDeviceDriver;
class IVistaDriverCreationMethod;
class IVddReadstateSource;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * The driver map <b>can</b> be used (usually is) for driver management.
 * It defines a collection of
   - drivers
   - driver creation methods
 * the driver creation methods are used to instantiate new drivers for
 * processing by using a TYPE symbol. Each type symbol is unique, whereas there
 * can be many drivers of the same type (imagine you have more than one tracker
 * attached). You can have more than one driver map, but that should not be
 * necessary. The driver map offers iterators to iterate over registered
 * drivers or creation methods.
 * A driver map is observable, so when new devices are registered or deleted,
 * the driver map will give notice to observers. Might by handy for GUI
 * programming.
 */
class VISTADEVICEDRIVERSAPI VistaDriverMap : public IVistaObserveable
{
public:
	VistaDriverMap();

	/**
	 * will release memory on *all* still registered driver creation methods.
	 * Before that, the IVistaDriverCreationMethod::OnUnload() method
	 * will be called.
	 * @see UnregisterDriverCreationMethod()
	 */
	virtual ~VistaDriverMap();


	enum
	{
		MSG_ADDDRIVER = IVistaObserveable::MSG_LAST,
		MSG_REMDRIVER,
		MSG_DELDRIVER,
		MSG_ADDDRIVERCREATIONMETHOD,
		MSG_UNREGISTERDRIVERCREATIONMETHOD,
		MSG_LAST
	};


	/**
	 * adds a new driver with a unique name. In case a driver with sName
	 * is already registered, the method will overwrite the old binding
	 * and utter a MSG_ADDDRIVER. In case the same driver was registered
	 * with the same name, nothing will happen.
	 * @return false if pDriver was already registered as sName
	 */
	bool                AddDeviceDriver(const std::string &sName,
										IVistaDeviceDriver *pDriver);

	/**
	 * query for a driver given a name
	 * @return true when a driver with sName was registered, false else
	 */
	bool                GetIsDeviceDriver(const std::string &sName) const;

	/**
	 * query for a driver given a name
	 * @return a pointer to the driver that was registered with name sName
	 */
	IVistaDeviceDriver *GetDeviceDriver(const std::string &sName) const;

	/**
	 * delete the driver by a name. The driver destructor is called.
	 * MSG_DELDRIVER is uttered AFTER the driver was deleted.
	 * @return false is no driver with sName was registered beforehand, true else
	 */
	bool                DelDeviceDriver(const std::string &sName);

	/**
	 * remove a driver from this map, given a name. the driver destructor is NOT called.
	 * MSG_REMDRIVER is uttered AFTER the driver was removed.
	 * @return false in case no driver with sName was registered, true else
	 */
	bool                RemDeviceDriver(const std::string &sName);

	bool                RemDeviceDriver( IVistaDeviceDriver * );

	/**
	 * same as RemDeviceDriver(), but returns the removed driver as a result.
	 * MSG_REMDRIVER is uttered AFTER the driver was removed.
	 * @return NULL if no driver named sName was found, a pointer to it else
	 */
	IVistaDeviceDriver *RemAndGetDeviceDriver(const std::string &sName);

	/**
	 * typedef for a more smooth iteration. Note that this iterator exports
	 * write access, so use with care.
	 */
	typedef std::map<std::string, IVistaDeviceDriver*>::iterator iterator;

	/**
	 * const version of the access iterator.
	 */
	typedef std::map<std::string, IVistaDeviceDriver*>::const_iterator const_iterator;

	/**
	 * start of the driver iteration
	 */
	iterator begin();

	/**
	 * end of the driver iteration
	 */
	iterator end();


	/**
	 * const API, start of the driver iteration
	 */
	const_iterator begin() const;

	/**
	 * const API, end of the driver iteration
	 */
	const_iterator end() const;

	/**
	 * the number of registered drivers
	 */
	size_t   size() const;

	// ##########################################################
	// CREATION METHOD API
	// ##########################################################

	/**
	 * register a creation method for a TYPE of drivers. TYPE names
	 * have to be unique. Use the bForceRegistration parameter to
	 * overwrite existing creation methods on purpose.
	 * In case of overwriting, the old pointer is lost, so be sure to claim
	 * the memory for it somewhere.
	 * @see UnregisterDriverCreationMethod()
	 * @see GetDriverCreationMethod()
	 * @param sTypeName the type name to register
	 * @param pMethod a pointer to a creation method, must not be NULL
	 * @param bForceRegistration overwrite existing creation methods.
	 */
	bool RegisterDriverCreationMethod(const std::string &sTypeName,
		IVistaDriverCreationMethod *pMethod,
		bool bForceRegistration = false);

	/**
	 * unregister a previously registered driver creation method by its
	 * type name symbol.
	 * @param sTypeName the type to unregister
	 * @param bDeleteDriverCreationMethod while at it... release the memory.
	 */
	bool UnregisterDriverCreationMethod(const std::string &sTypeName,
		bool bDeleteDriverCreationMethod = true);

	static bool DeleteDriverCreationMethod( IVistaDriverCreationMethod * );

	/**
	 * query for a creation method by giving a type
	 * @see RegisterDriverCreationMethod()
	 * @return NULL if this type was not registered before
	 */
	IVistaDriverCreationMethod *GetDriverCreationMethod(const std::string &sTypeName) const;

	/**
	 * typedef for creation method iteration
	 */
	typedef std::map<std::string, IVistaDriverCreationMethod*>::iterator crm_iterator;

	/**
	 * const version for creation method iteration
	 */
	typedef std::map<std::string, IVistaDriverCreationMethod*>::const_iterator crm_const_iterator;


	/**
	 * creation method iteration begin
	 */
	crm_iterator crm_begin();

	/**
	 * createion method iteration end
	 */
	crm_iterator crm_end();

	/**
	 * const version, creation method iteration begin
	 */
	crm_const_iterator crm_begin() const;

	/**
	 * const version, creation method iteration end
	 */
	crm_const_iterator crm_end() const;

	/**
	 * the number of registered creation methods
	 */
	size_t crm_size() const;

	void Purge(bool bDelete=false);


	IVddReadstateSource *GetReadstateSource() const;
	void SetReadstateSource( IVddReadstateSource * );
protected:
private:
	typedef std::map<std::string, IVistaDeviceDriver*> DEVMAP;
	DEVMAP m_mpDevices;

	typedef std::map<std::string, IVistaDriverCreationMethod*> CRMAP;
	CRMAP   m_mapCreationMethods;

	IVddReadstateSource *m_pReadstateSource;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
#endif //_VISTADRIVERMAP_H
