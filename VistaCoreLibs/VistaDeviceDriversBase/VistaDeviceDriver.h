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


#ifndef _VISTADEVICEDRIVER_H
#define _VISTADEVICEDRIVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"

#include <vector>
#include <map>
#include <list>
#include <string>
#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaReferenceCountable.h>

#include <VistaAspects/VistaSerializable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDeviceSensor;
class VistaSensorMeasure;
class IVistaDriverCreationMethod;
class VistaDriverMeasureHistoryAspect;
class VistaDriverEnableAspect;
class IVistaMeasureTranscoderFactory;
class IVistaTranscoderFactoryFactory;
class VistaAverageTimer;
class VistaWindowAverageTimer;

class IVistaDriverMeta;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * The base class for device drivers. A device driver exports a number of
 * sensors and aspects, where the latter can be associated by external means.
 * A device driver realizes a periodic sampling of underlying (maybe OS specific
 * implementations for the sampling of device data, storing the sample in a
 * device sensor.
 *
 * In order to write a driver for your own purpose, one should take a look at
 * the methods
 * - PreUpdate()
 * - PostUpdate()
 * - PhysicalEnable()
 * - DoSensorUpdate()
 *
 * A sensor measure must consist of the following protocol (order is important).
 * -# decode the data to store in the VistaDeviceSensor and determine its
	  storage index
 * -# call MeasureStart() with the appropriate index for the sensor
 * -# set the data in the VistaDeviceSensor storage area
 * -# call MeasureStop() with the appropriate index
 * -# call VistaDeviceSensor::SetUpdateTimeStamp() on the sensor updated.
 *
 * Especially the order of 4->5 is *very* important, as the update timestamp
 * is used for the indication of a sensor change.
 *
 * A measuring takes place during the call of DoSensorUpdate().
 *
 * How to <b>use</b> a driver:<br>
 * - create driver (using the factory from the plugin)
 * - query aspects for configuration
 * - call IVistaDeviceDriver::Connect()
 * - loop over IVistaDeviceDriver::Update() somehow
 * - query sensors for new data
 */
class VISTADEVICEDRIVERSAPI IVistaDeviceDriver
{
	VISTA_UNCOPYABLE( IVistaDeviceDriver );
public:
	virtual ~IVistaDeviceDriver();

	// #####################################################################
	// UPDATE TYPE API
	// #####################################################################
	//@{
	// @name UPDATE TYPE API
	enum eUpdateType
	{
		UPDATE_EXPLICIT_POLL =0,            /**< updated once per frame
										  by the interaction manager */
		UPDATE_CONNECTION_THREADED,  /**< use the interaction manager's
										  threaded update mechanism when
										  there is one control connection */
		UPDATE_CUSTOM_THREADED       /**< the device driver has its own
										  update thread as an aspect */
	};

	/**
	 * Returns the update type for the driver instance.
	 * The update type is determined by specialized classes and can not
	 * be changed during run-time.
	 * @see eUpdateType
	 */
	eUpdateType GetUpdateType() const;
	//@}

	/**
	 * Implement this function for the initial device-setup. The device should
	 * perform all initialization and feature detection up to the point where
	 * processing/measuring could be started right away.
	 * The starting/stopping of the actual measurement is then done via the
	 * PhysicalEnable API.
	 */
	bool Connect();
	bool GetIsConnected() const;
	bool Disconnect();

	// #####################################################################
	// SENSOR QUERY API
	// #####################################################################
	//@{
	/**
	 * Returns the number of available sensors for this driver.
	 * @return the number of sensors for this driver in [0,inf)
	 */
	unsigned int        GetNumberOfSensors() const;

	/**
	 * Enumeration method to get a sensor for this driver by index.
	 * This method is mainly used for debugging purposes.
	 * @see GetNumberOfSensors()
	 * @param uiIndex the index to query
	 * @return NULL if uiIndex > GetNumberOfSensors()
	 */
	VistaDeviceSensor *GetSensorByIndex(unsigned int uiIndex) const;

	/**
	 * Searches for a sensor with the name give in strName.
	 * Takes linear time in the number of sensors registered.
	 * If two sensors have the same name, the first one found will be returned.
	 * @param strName case-sensitive name to compare
	 * @return the (first) sensor with the name strName
	 */
	VistaDeviceSensor *GetSensorByName( const std::string &strName ) const;

	static size_t ANYPOS;
	static unsigned int INVALID_INDEX;
	static size_t UNKNOWN_SIZE;
	static VistaType::microtime UNKNOWN_ACCESS_DURATION;

	class VISTADEVICEDRIVERSAPI AllocMemoryFunctor
	{
	public:
		virtual ~AllocMemoryFunctor() {}
		virtual void operator()( VistaSensorMeasure &m ) const = 0;
	};

	class VISTADEVICEDRIVERSAPI DefaultAllocFunctor : public AllocMemoryFunctor
	{
	public:
		virtual ~DefaultAllocFunctor() {}
		virtual void operator()( VistaSensorMeasure &m ) const {}
	};


	class VISTADEVICEDRIVERSAPI ClaimMemoryFunctor
	{
	public:
		virtual ~ClaimMemoryFunctor() {}
		virtual void operator()( VistaSensorMeasure & ) const = 0;
	};

	class VISTADEVICEDRIVERSAPI DefaultClaimFunctor : public ClaimMemoryFunctor
	{
	public:
		virtual ~DefaultClaimFunctor() {}
		virtual void operator()( VistaSensorMeasure & ) const {}
	};

	/**
	 * Can by used by client code to add dynamically configured sensors.
	 * Usually, this is not necessary for plain user code. Do not
	 * use this API unless you know what you are doing.
	 */
	virtual unsigned int AddDeviceSensor(VistaDeviceSensor *pSensor, size_t pos = ANYPOS, 
		                                 AllocMemoryFunctor *amf = NULL,
										 ClaimMemoryFunctor *cmf = NULL );


	/**
	 * remove a sensor by pointer. Do not use this API unless you know what
	 * you are doing.
	 * @return false if the pointer was not found in the vector of sensors.
	 */
	virtual bool         RemDeviceSensor(VistaDeviceSensor *pSensor);

	/**
	 * Sets up the history properties of the specified sensor. In case you have
	 * an asynchronously running driver, this is an absolute must to call
	 * with the proper arguments.
	 * @param pSensor the sensor to setup it's history
	 * @param nMaxHistoryAccessTimeInMsec this is the maximum time ANY reader on the
	                                history is accessing the history. For
									example if you know that you want to render
									with 30Hz on your graphical output and all
									readers live in the rendering thread, then 33.3
									is the value to set here.
	 * @param nMaxSlotsToRead define the maximum number of slots to read within the time frame
	                    specified by nMaxHistoryAccessTimeInMsec for ALL readers of the
						history. If you are doing latest-sampling, a value of 1 is ok,
						it must not be 0.
	 * @see IVistaDriverCreationMethod::GetUpdateEstimatorFor()
	 */
	bool SetupSensorHistory( VistaDeviceSensor *pSensor,
							 unsigned int nMaxSlotsToRead,
							 VistaType::microtime nMaxHistoryAccessTimeInMsec );


	bool RequestHistorySize( VistaDeviceSensor *pSensor,
							 unsigned int nMaxSlotsToRead,
			                 VistaType::microtime nMaxHistoryAccessTimeInMsec );


	class SensorInfo
	{
	public:

		SensorInfo()
			: m_sensorName()
			, m_sensorTypeHint()
			, m_sensorHistorySize( UNKNOWN_SIZE )
			, m_MaxAccessTimeInMs( UNKNOWN_ACCESS_DURATION )
		{}

		SensorInfo( const std::string &sensor_name,
			const std::string &type_hint,
			        size_t sensor_history_size,
					VistaType::microtime access_time_duration_in_ms )
					: m_sensorName( sensor_name )
					, m_sensorTypeHint( type_hint )
					, m_sensorHistorySize ( sensor_history_size )
					, m_MaxAccessTimeInMs( access_time_duration_in_ms )
		{}

		bool IsValid() const
		{
			return m_sensorHistorySize != UNKNOWN_SIZE
				&& m_MaxAccessTimeInMs != UNKNOWN_ACCESS_DURATION;
		}

		std::string m_sensorName,
			        m_sensorTypeHint;
		size_t m_sensorHistorySize;
		VistaType::microtime m_MaxAccessTimeInMs;
	};

	void RegisterSensorInfo( unsigned int nSensorIndex,
		                             const SensorInfo &sensor_info);

	SensorInfo GetSensorInfo( unsigned int nSensorIndex ) const;
	


	//@}
	// #####################################################################
	// SPECIFIC DRIVER API
	// #####################################################################
	//@{
	/**
	 * This method can be used to realize, e.g., polling requests and is called
	 * right before update. Note that the call to PreUpdate() is realized independent
	 * of the driver's enable state.
	 * This method must be called by the same thread that calls Update() later on.
	 * @return true iff anything was all right, false if problems occurred (see driver
					Specific documentation)
	 * @see Update()
	 * @see PostUpdate()
	 */
	virtual bool PreUpdate();


	/**
	 * A call to this method will cause the driver to take another snapshot of its
	 * device's values and store them in the associated sensors. If the enabled flag
	 * of this driver is set to false, this method will return false. This
	 * method will additionally determine the time stamp that is used to stamp the
	 * sensor data with.
	 * @return false iff GetIsEnabled() == false, or when an update call could not
	 *               be proceeded successfully.
	 */
	bool         Update();


	/**
	 * This method is called right after a call to Update, regardless of the Update()
	 * method's return value. Can be overridden by special drivers to realize a kind of
	 * cleanup after the call to update (e.g., clearance of flags).
	 * This method must be called by the same thread that calls Update() later on.
	 * @return true iff anything was all right, false else (see driver specific documentation
					for a meaning of a false return)
	 * @see PreUpdate()
	 * @see Update()
	 */
	virtual bool PostUpdate();

	/**
	 * Reflects whether this driver is enabled or not. Disabled drivers will
	 * not update an sensor data and usually stop communication or indicate
	 * the data source that no more information is needed. Furthermore, the
	 * Update() method will take a peek at the flag and eventually skip the call
	 * to its internal update routines. A driver can realize an internal method
	 * (PhysicalEnable()) to stop the attached device from sending data and
	 * thus avoiding overflow.
	 * @return true when Update() will be processed
	 * @see SetIsEnabled()
	 */
	bool GetIsEnabled() const;

	/**
	 * Sets the enable state to true or false.
	 * @param bEnabled true iff you do not want that the driver
			  delivers updates of sensor data anymore.
	 */
	void SetIsEnabled(bool bEnabled);
	//@}

	// #####################################################################
	// ASPECTS
	// #####################################################################
	//@{
	/**
	 * Drivers can be associated to aspects, partially they export aspects
	 * themselves, partially they can be associated with these aspects
	 * without even knowing it. The basic API for an aspect is mainly
	 * about identification by using a static and a dynamic Id.
	 * The static Id is registered at run-time and can be queried using
	 * this generic interface.
	 */
	class VISTADEVICEDRIVERSAPI IVistaDeviceDriverAspect
	{
		friend class IVistaDeviceDriver;
	public:
		/**
		 * Query the dynamic aspect id of this aspect.
		 * Can be used to compare it against the statically
		 * given GetAspectId()
		 * @see GetAspectId()
		 */
		int        GetId() const;

		// #########################################
		// OVERWRITE IN SUBCLASSES
		// #########################################

		/**
		 * Statically query the id for a specific aspect class.
		 * @return -1 iff no id was currently assigned to this
					  aspect
		 */
		static int GetAspectId();

		/**
		 * Sets the aspect id for this aspect class. This can be
		 * done only once (as long as GetAspectId() returns -1)
		 * and is then fixed during the complete run-time.
		 */
		static void SetAspectId(int);

		virtual void Print( std::ostream& ) const;
	protected:
		/**
		 * Can be used by subclasses to set an instance's class id,
		 * usually during construction.
		 * @param id the id to return by a call to GetId()
		 * @see GetId()
		 */
		void       SetId(int id);

		/**
		 * The constructor determines whether an instance can be unregistered
		 * by a call to IVistaDeviceDriver::UnregisterAspect(). This attribute
		 * is not mutable after the construction of an instance. All aspects
		 * that are exported by drivers should be marked as unregisterable.
		 * @param bCanBeUnregistered true iff a call to IVistaDeviceDriver::UnregisterAspect()
				  can be successful.
		 */
		IVistaDeviceDriverAspect(bool bCanBeUnregistered = false);

		/**
		 * Protected destructor. Subclasses can decide to export their own, otherwise
		 * an aspect remains un-deleteable.
		 */
		virtual ~IVistaDeviceDriverAspect();
	private:
		int        m_nMyAspectId;
		bool       m_bCanBeUnregistered;
		static int m_nAspectId;
	};

	/**
	 * Returns an aspect of a specified type id, iff it one is registered
	 * with this specific instance.
	 * @param nId the aspect id (usually one queried by the static GetAspectId() API
	 * @return NULL iff no aspect of that type is registered with the driver instance
	 */
	IVistaDeviceDriverAspect *GetAspectById(int nId) const;

	template<class aspType>
	aspType *GetAspectAs( int nId ) const
	{
		return dynamic_cast<aspType*>( GetAspectById(nId ) );
	}


	template<class aspType>
	aspType *GetAspect() const
	{
		return dynamic_cast<aspType*>( GetAspectById( aspType::GetAspectId() ) );
	}

	/**
	 * Registers an aspect with this instance. Registered instances belong to the
	 * driver and will be deleted once the driver is deleted.
	 * Iff you do not like that, call UnregisterAspect() before deleting the
	 * driver.
	 * @param pAspect the aspect instance to register.
	 * @return true iff the aspect could be registered.
					false, iff an aspect of that type is already registered.
	 */
	virtual bool RegisterAspect(IVistaDeviceDriverAspect *pAspect);

	enum eDeleteAspectBehavior
	{
		DELETE_ASPECT,
		DO_NOT_DELETE_ASPECT
	};

	enum eUnregistrationBehavior
	{
		FORCE_UNREGISTRATION,
		DO_NOT_FORCE_UNREGISTRATION
	};
	/**
	 * Unregisters an aspect with this driver. Can be used to delete the aspect
	 * while unregistering it.
	 * @param pAspect the aspect instance to unregister.
	 * @param bDelete true iff delete is to be called on the pointer after the
					  unregistration process.
	 * @param bForce  force unregistration regardless of the flag that was used
	 *                during construction of the aspect.
	 * @return false iff the aspect was not registered with the driver instance
	 */
	virtual bool UnregisterAspect(IVistaDeviceDriverAspect *pAspect,
			eDeleteAspectBehavior bDelete = DELETE_ASPECT,
			eUnregistrationBehavior bForce  = DO_NOT_FORCE_UNREGISTRATION);

	/**
	 * Claim list of registered aspects for this driver.
	 * @return a list of ints, which can be used to query the GetAspectById() API
	 * @see GetAspectById()
	 */
	std::list<int> EnumerateAspectIds() const;

	//@}
	// ################################################################
	// CLIENT READER API
	// ################################################################
	//@{
	/**
	 * tells the driver to furnish the current measures in oder to let
	 * clients read it. This is used by the InteractionManager once per
	 * frame and should not be used by client code.
	 */
	void SwapSensorMeasures( std::vector<int> *liIds = NULL );

	//@}

	/**
	 * Non-stable API.
	 * For testing purposes: attach to an existing tracker stream without trying to re-open
	 * the device stream. May not make sense for all kind of drivers, but for those
	 * that support non-attached streaming, e.g., by a UDP stream.
	 * @return iff the driver tries to attach to a stream only without trying to re-initialize the device
	 */
	virtual bool GetAttachOnly() const;

	/**
	 * Non-stable API.
	 * Sets a flag whether the driver should try to attach to a data stream from a tracker
	 * or should try to open and initialize the driver connections as usual. The current driver
	 * specialization might ignore this flag, though. Not all drivers actually *can* omit
	 * the initialization phase, mostly those with non-bound (e.g., UDP) connections.
	 * @param bAttach true iff no init or attaching should be performed
	 */
	virtual void SetAttachOnly(bool bAttach);


	const VistaWindowAverageTimer& GetFrequencyTimer() const;
	VistaType::microtime GetAverageUpdateFrequency() const;
	const VistaAverageTimer& GetUpdateTimer() const;
	VistaType::microtime GetAverageUpdateTime() const;

	IVistaDriverCreationMethod *GetFactory() const;
protected:
	/**
	 * Protected constructor.
	 * @param crm a pointer to the creation method which will live throughout the
	 *            lifetime of this driver.
	 */
	IVistaDeviceDriver(IVistaDriverCreationMethod *crm, unsigned int nDefHistSize = ~0);

	// ####################################################################
	// SUBCLASS PROCESS CODE
	// ####################################################################
	//@{
	/**
	 * Realizes the driver specific way of enabling or disabling the
	 * driver. A disabled driver should not send any updates in any way,
	 * and simply be a passive driver. An active driver can either be queried
	 * or sends updates all the time. A disabled driver is free to ignore
	 * any attempt to configure or use it, depending on the protocol.
	 * This method is called by SetIsEnabled().
	 * @return true iff the operation was successful
	 */
	//bool PhysicalEnable(bool bEnable);


	/**
	 * The routine to implement for specific drivers.
	 * This method is called when the driver is enabled and either an update
	 * request is pending by the interaction manager or the connection threaded
	 * method or the custom threaded method detects that an update is possible.
	 * The time stamp can be used to mark sensors with in order to indicate the time
	 * of the update.
	 */
	virtual bool DoSensorUpdate(VistaType::microtime dTs) = 0;
	/**
	 * Implement this function for the initial device-setup. The device should
	 * perform all initialization and feature detection up to the point where
	 * processing/measuring could be started right away.
	 * The starting/stopping of the actual measurement is then done via the
	 * PhysicalEnable API.
	 */
	virtual bool DoConnect()    = 0;
	virtual bool DoDisconnect() = 0;

	// ###################################################################
	// SENSOR MEASURE API
	// ###################################################################
public:
	enum eMeasureStartBehavior
	{
		RETURN_NULL,
		RETURN_CURRENT_SLOT
	};
	/**
	 * indicate a start of the measure for a sensor that is given by its
	 * index in the m_vecSensors vector. The timestamp can be used to be
	 * passed as the time stamp of the measurement.
	 * @param nSensorIdx the sensor to start a measure for
	 * @param dTs the time stamp to record for the measure
	 * @param getCurrent compatibility flag to trigger lookup of current measure space
	 *        (this is a map lookup, so it will decrease performance when not needed).
	 *        Default is set to 'false', so MeasureStart() will return NULL then.
	 * @return a pointer to the memory slot to use for reading new data (or NULL when
	 *         getCurrent is false or no memory available or a sensor with the given id
	 *         was not found.)
	 */
	VistaSensorMeasure *MeasureStart(unsigned int nSensorIdx, VistaType::microtime dTs, eMeasureStartBehavior getCurrent = RETURN_NULL);
	VistaSensorMeasure *MeasureStart( VistaDeviceSensor &, VistaType::microtime dTs, eMeasureStartBehavior getCurrent = RETURN_NULL);

	/**
	 * Stops the measure for the sensor given by the index.
	 * @param nSensorIdx the sensor to stop the measurement for.
	 */
	void MeasureStop(unsigned int nSensorIdx);
	void MeasureStop( VistaDeviceSensor& );

protected:
	/**
	 * The measure history aspect that is exported by and kind of driver.
	 * Client code can, but should not use this.
	 */
	VistaDriverMeasureHistoryAspect *m_pHistoryAspect;

	//@}
	// ####################################################################
	// SUBCLASS CONFIGURE CODE
	// ####################################################################
	//@{
	/**
	 * This method is usually called by a special device driver constructor
	 * once and then nevermore ;) Sets the update type to the given argument.
	 */
	void         SetUpdateType(eUpdateType eTp);

	/**
	 * remove a sensor by index
	 * @return false if index > m_vecSensors.size()
	 */
	bool         RemDeviceSensorByIndex(unsigned int uiIndex);
	//@}
	/**
	 * Simple cleanup method that can be called by the destructor of the
	 * specialized classes to delete all existing sensors.
	 */
	void         DeleteAllSensors();

	bool HasSensorPointer( VistaDeviceSensor * ) const;

private:
	void SetIsConnected( bool bConnected );
	eUpdateType                                      m_eUpdType;
	std::vector<VistaDeviceSensor*>                 m_vecSensors;
	typedef std::map<int, IVistaDeviceDriverAspect*> ASPECTMAP;
	ASPECTMAP m_mapAspects;

	typedef std::map<unsigned int, SensorInfo> SENSORINFOMAP;
	SENSORINFOMAP m_defaultHistorySizes; // history size record per sensor index


	VistaWindowAverageTimer*    m_pFrequencyTimer;
	VistaAverageTimer*          m_pUpdateTimer;

	IVistaDriverCreationMethod *m_pFactory;
	bool                       m_isConnected;
	mutable VistaDriverEnableAspect    *m_pEnableAspect;

	bool m_bEnabledFlag;
};




class VISTADEVICEDRIVERSAPI IMeasureSerializeAdapter : public IVistaSerializable
{
public:
	virtual ~IMeasureSerializeAdapter() {}

	void SetWriteableSensorMeasure( VistaSensorMeasure *measure )
	{
		m_measure = measure;
	}

	VistaSensorMeasure *GetWriteableSensorMeasure() const
	{
		return m_measure;
	}

	void SetReadOnlySensorMeasure( const VistaSensorMeasure *measure )
	{
		m_const_measure = measure;
	}

	const VistaSensorMeasure *GetReadOnlySensorMeasure() const
	{
		return m_const_measure;
	}

private:
	VistaSensorMeasure *m_measure;

	const VistaSensorMeasure *m_const_measure;
protected:
	IMeasureSerializeAdapter()
	: IVistaSerializable()
	, m_measure(0)
	, m_const_measure(0)
	{}
};




class VISTADEVICEDRIVERSAPI IVistaDriverCreationMethod : public IVistaReferenceCountable
{
	friend class VistaDriverMap;
public:

	/**
	 * @brief little helper class to unregister and delete transcoder factories
	 */
	class _unregisterAndDeleteTranscoderFac
	{
	public:
		_unregisterAndDeleteTranscoderFac( IVistaDriverCreationMethod *crm )
		: m_crm(crm) {}

	void operator()( const std::string &strTypeName ) const;

		IVistaDriverCreationMethod *m_crm;
	};


	virtual ~IVistaDriverCreationMethod();

    /**
     * The driver creation routine in itself. This is the one to overload
     * by subclasses mainly. Has no arguments (obviously), and not even a
     * name (for no specific reason... @todo think about a name.
     */
	virtual IVistaDeviceDriver *CreateDriver() = 0;


	/**
	 * @param strName the type to register as a symbol,
	          this name is used on the USER side, so it should be a short
	          mnemonic, something like BODY, SENSOR or the like
	 * @param nMeasureSize the number of bytes for a single measure of this type
	 * @param nUpdateEstimator the number of maximum samples per second (in Hz) that this
			  sensor type is updated (RTFM of the device to get an estimate here)
     * @param pFac a pointer to the transcoder factory to use for this sensor type
              during creation.
	 * @return ~0 in case of non-success (type already registered)
	 * @see GetTranscoderTypes()
	 */
	virtual unsigned int RegisterSensorType( const std::string &strName,
									 unsigned int nMeasureSize,
									 unsigned int nUpdateEstimator,
									 IVistaMeasureTranscoderFactory *pFac );

	virtual bool         UnregisterType(const std::string &strType,
								bool bDeleteFactories = false);
	virtual unsigned int GetTypeFor( const std::string &strType ) const;
	virtual bool GetTypeNameFor( unsigned int nTypeId, std::string &strDest ) const;
	virtual unsigned int GetMeasureSizeFor( unsigned int nType ) const;
	virtual bool SetMeasureSizeFor( const std::string &strType, unsigned int nNewSize );
	virtual unsigned int GetUpdateEstimatorFor(unsigned int nType ) const;
	virtual std::list<std::string> GetTypeNames() const;
	virtual std::list<unsigned int> GetTypes() const;

	/**
	 * Non-stable API.
	 * Returns an instance of a transcoder factory for a sensor given by index.
	 * Note that this API is important for the driver to work in the ViSTA cluster
	 * mode. As such, it is supposed to be an <b>expert</> level API.
	 * Most users can ignore this API, driver implementors must implement it.
	 * @param nType the type id that is registered with the sensor mapping aspect,
			  iff this driver has one. If you need this id, query the sensor mapping aspect,
			  otherwise, give a ~0 = 0xFFFFFF here
	 * @return an instance of a measure transcoder factory that can be used to create
			   a measure transcode for a specific sensor.
	 */
	virtual IVistaMeasureTranscoderFactory *GetTranscoderFactoryForSensor(unsigned int nType) const;
	virtual IVistaMeasureTranscoderFactory *GetTranscoderFactoryForSensor(const std::string &strTypeName) const;


	std::list<std::string> GetTranscoderTypes() const;

	virtual void OnUnload();

	/**
	 * used by the RegisterSensorType() API
	 * @todo check whether we need this API in the public interface
	 */
	void AddTranscoderType( const std::string &strType );
	IVistaTranscoderFactoryFactory *GetMetaFactory() const;

	static const unsigned int INVALID_TYPE;
	static const unsigned int INVALID_SIZE;
	static const unsigned int INVALID_ESTIMATE;

protected:
	IVistaDriverCreationMethod(IVistaTranscoderFactoryFactory *metaFac);


private:



	// typedefs to ease the pain of writing this...
	struct _sL
	{
		_sL()
			: m_pFac(NULL),
			  m_nMeasureSize(0),
			  m_nUpdateEstimator(0) {}

		_sL(IVistaMeasureTranscoderFactory *pFac,
			unsigned int nMeasureSize,
			unsigned int nUpdateEstimator)
			: m_pFac(pFac),
			  m_nMeasureSize(nMeasureSize),
			  m_nUpdateEstimator(nUpdateEstimator)
		{
		}

		IVistaMeasureTranscoderFactory *m_pFac;
		unsigned int m_nMeasureSize,
					 m_nUpdateEstimator;
	};

	typedef std::map<std::string,unsigned int> MAPNAME;
	typedef std::map<unsigned int, _sL> FACMAP;

	typedef std::list<std::string> TRANTP;

	unsigned int m_nTypeCount;
	MAPNAME m_mapTypeNames;
	FACMAP  m_mapFactories;
	TRANTP  m_liTranscoderTypes;

	IVistaTranscoderFactoryFactory *m_metaFac;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
#endif //_VISTADEVICEDRIVER_H


