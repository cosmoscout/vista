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


#ifndef _VISTADRIVERSENSORMAPPINGASPECT_H
#define _VISTADRIVERSENSORMAPPINGASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>

#include <vector>
#include <map>
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaDriverCreationMethod;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Some devices may allow a dynamic mapping of sensors which has to be
 * traced by the user or ''VistaKernel'' code. For that purpose, a
 * sensor mapping aspect covers all the code needed for that.
 * Examples are the ''VistaDTrackDriver'' (A.R.Tracking), where new bodies
 * can be calibrated to the system or the ID of a body may change due to
 * reconfiguration of the tracker device.
 * A side-note: the sensor mapping does some management for an active
 * device that does the IVistaDriverCreationMethod for non-existing drivers,
 * so some of the API seems duplicated.
 * @todo re-think the API / move all the type registration stuff to
         the creation method, or, to be more precise: eliminate all references
         to the type management API of the sensor mapping and replace by
         referencing the creation method API respectively.
 */
class VISTADEVICEDRIVERSAPI VistaDriverSensorMappingAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	VistaDriverSensorMappingAspect(IVistaDriverCreationMethod *);
	virtual ~VistaDriverSensorMappingAspect();


	/**
	 * a driver calls this method to register a new type and a human readable
	 * type name, the number of bytes for a single measures and an estimator
	 * for the update frequency of the sensor type (in Hz) as well as the
	 * transcoder factory to create new sensors of this type.
	 * @return the type id for this sensor type
	 * @see GetTypeId()
	 * @see GetMeasureSizeForType()
	 * @see GetUpdateRateEstimatorForType()
	 */
	unsigned int RegisterType(const std::string &strTypeName);

//	/**
//	 * retrieve a transcoder factory for a given type symbol
//	 * @param strTypeName the symbolic name to look up
//	 * @return NULL or the factory
//	 */
//	IVistaMeasureTranscoderFactory *GetTranscoderFactoryForType(const std::string &strTypeName) const;
//
//	/**
//	 * retrieve a transcoder factory for a given type id
//	 * @param nTypeId the type id to look
//	 * @return NULL or the factory
//	 */
//	IVistaMeasureTranscoderFactory *GetTranscoderFactoryForType(unsigned int nTypeId) const;


	/**
	 * upon destruction of the driver, this is a cleanup routine
	 * @return true if a type as given was unregistered
	 */
	bool         UnregisterType(const std::string &strType);

	/**
	 * retrieve the collection of registered type names.
	 */
	std::list<std::string> GetTypeNames() const;

	/**
	 * check for the existence of a given type using the symbolic name
	 */
	bool         GetIsType(const std::string &sTypeName) const;

	/**
	 * get the type id for a given symbol
	 * @see RegisterType()
	 */
	unsigned int GetTypeId(const std::string &sTypeName) const;

//	/**
//	 * retrieve the measure size for a sensor of type strTypeName
//	 * @return -1 (~0) for a non registered type
//	 */
//	unsigned int GetMeasureSizeForType(const std::string &strTypeName) const;
//
//	/**
//	 * retrieve the measure size for a sensor type id nType
//	 * @return -1 (~0) for a non registered type
//	 */
//	unsigned int GetMeasureSizeForType(unsigned int nType) const;
//
//	/**
//	 * retrieve the update estimator for a type with id nType
//	 * @return -1 (~0) for a non registered type
//	 */
//	unsigned int GetUpdateRateEstimatorForType(unsigned int nType) const;


	/**
	 * this call makes only sense when all ids are globally ordered
	 * but the driver knows different types of sensors!
	 * in case an id is a (type,sensor)-tuple, this method will find
	 * the match for the first id with the lowest type!
	 * @return ~0 iff no nRawSensorId registered in all the types.
	 */
 	unsigned int GetSensorIdByRawId( unsigned int nRawSensorId ) const;

 	/**
 	 * enables to retrieve the raw id by giving the logical id.
 	 * This call makes sense when the driver has only one available
 	 * mapping, but sensors may be optional.
 	 * This method may be slow.
 	 * @param nType the sensor type to search
 	 * @param nMappedId the logical id to search for
 	 * @return the physical id or ~0 when the id was not mapped
 	 */
 	unsigned int GetRawIdByMappedId( unsigned int nType,
 			                         unsigned int nMappedId ) const;

 	/**
 	 * retrieve the logical id for a physical sensor of a given type
 	 * @param nSensorType the type of sensor to look up
 	 * @param nRawSensorId the physical id (as is determined by the driver)
 	 * @see IVistaDeviceDriver::AddSensor()
 	 * @return the logical sensor id
 	 */
	unsigned int GetSensorId(unsigned int nSensorType,
							 unsigned int nRawSensorId) const;

	/**
	 * create a mapping from a raw id (determined by driver upon a call to
	 * IVistaDriver::AddSensor()) and a LOGICAL id, maybe determined by the device
	 * itself, all for a different type range
	 * @param nSensorType the type of the sensor id to map
	 * @param nRawSensorId the raw id , as determined by the driver
	 * @param nSensorId the logical sensor id we want to lookup
	 */
	bool SetSensorId(unsigned int nSensorType,
					 unsigned int nRawSensorId,
					 unsigned int nSensorId);

	/**
	 * retrieve the number of registered sensors for a given type.
	 * maybe useful for debugging.
	 * @return 0 for a non registered type.
	 */
	unsigned int GetNumRegisteredSensorsForType(unsigned int nType) const;


	unsigned int GetNumberOfRegisteredTypes() const;

	/**
	 * purge the complete mapping, but keep sensors
	 * @return true
	 * @todo what's this for?
	 */
	bool ClearSensorMapping();

	/**
	 * purge the type mapping, but keep sensor mapping
	 * @return true
	 * @todo what's this for?
	 */
	bool ClearTypeMapping();

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);
	static unsigned int INVALID_TYPE;
	static unsigned int INVALID_ID;
protected:
private:
	static int m_nAspectId;

	typedef std::map<unsigned int, unsigned int> IDMAP;
	class _sL
	{
	public:
		_sL(const std::string &strTypeName)
			: m_strTypeName(strTypeName)
		{}

		IDMAP        m_mapIds; /**< from logical to real */
		std::string  m_strTypeName;
//		unsigned int m_nMeasureSize,
//					 m_nUpdateEstimator;
//		IVistaMeasureTranscoderFactory *m_pFac;
	};

	typedef std::vector<_sL> MAPVEC;
	MAPVEC m_vecTypeMapping;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERSENSORMAPPINGASPECT_H

