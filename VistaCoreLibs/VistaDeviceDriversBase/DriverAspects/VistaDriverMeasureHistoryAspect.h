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


#ifndef _VISTADRIVERMEASUREHISTORYASPECT_H
#define _VISTADRIVERMEASUREHISTORYASPECT_H


/**
 * @page DriverHistoryPage History
 *
 * A history is a user definable buffer that can be used for time series or multimodal interaction technology.
 *
 * @section DriverHistoryImplementedBy Implemented by
 *
 * This feature is implemented by all drivers. It is part of the baseclass.
 *
 * @section DriverHistorySectionsKeys Section keys
 * <table>
 * 	<tr><th>Name</th><th>Type</th><th>Remark</th></tr>
 * 	<tr><td>HISTORY</td><td>int</td>
 * 		<td>the number of slots to allocate for the user readable history</td>
 *  </tr>
 * </table>
 *
 * @section DriverHistoryRemerks Remarks
 *
 * The history is a collection of indexed slots.
 * The user must provide the information on how many of those blocks
 * can be read at maximum during a read phase of a client on the history.
 * In order to get a glimps on what that means,
 * try to get an information about the update rate of the device and
 * estimate a maximum time for you client code to read on the history.
 *
 * @section DriverHistoryCodeToLookAt Code to look at
 *
 * - VistaGenericHistoryConfigurator
 * - VistaSensorMappingConfigurator
 * - VistaDriverMeasureHistoryAspect
 */

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <map>

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
 * All drivers export a measure history which gives access to the history
 * trace of input for sensors that are exported by the driver and the
 * configuration of the history. Note that the MeasureHistoryAspect stores
 * //raw// data and not transformed values.
 * Note that all sensors have to be registered with the history aspect
 * or nothing will work as expected.
 * The protocol is simple.
   - call MeasureStart() on a given sensor,
     this will mark a slot on the sensor for you
   - call GetCurrentSlot() on the sensor to write there
   - when done, call MeasureStop() on the sensor
 */
class VISTADEVICEDRIVERSAPI VistaDriverMeasureHistoryAspect : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	VistaDriverMeasureHistoryAspect(unsigned int nDefHistSize = ~0);
	virtual ~VistaDriverMeasureHistoryAspect();


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// measure API
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * start a measure on a sensor. mandatory to call before getting the
	 * current slot. Do not forget to stop the current measure using the
	 * MeasureStop() API. The timestamp given will be noted in the history
	 * of the sensor, in that case it is the oldest time value accessible
	 * at the time of data acquisition.
	 * @see GetCurrentSlot()
	 * @see MeasureStop()
	 */
	void                 MeasureStart(VistaDeviceSensor*, VistaType::microtime dTs);

	/**
	 * retrieve the current slot of the history to write to for the sensor given.
	 * @return NULL when measure start was not called properly or there
	           is no memory for the history to write to
	 */
	VistaSensorMeasure *GetCurrentSlot(VistaDeviceSensor*) const;


	/**
	 * stop measuring for the sensor. Without a call to that method, no
	 * change in sensor data will be visible for outside watchers and
	 * MeasureStart() as well as GetCurrentSlot() will always retrieve the
	 * same slot (if any!)
	 */
	void                 MeasureStop(VistaDeviceSensor*);


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// management API
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	/**
	 * adjust the history size for the given sensor. Note that an old history
	 * is lost completely due to memory re-allocation. So it is wise to call
	 * this API only once or when it is clear that old data can be thrown away.
	 * The history size is given in terms of slots on a per-sensor level.
	 * @param nUserSize the number of slots the user wants to be guaranteed
	          to have access to
	 * @param nDriverSize the number of slots that the driver can write to
	          while the user is accessing the buffer. Use with care and take
	          a good look at the examples.
	 * @param nSlotLength the number of bytes per slot
	 * @return false if the sensor was not registered with this history
	 */
	bool                 SetHistorySize(VistaDeviceSensor *, unsigned int nUserSize,
						unsigned int nDriverSize,
						unsigned int nSlotLength);

	/**
	 * returns the number of slots finally allocated for this history and sensor.
	 * @return the number of slots allocated.
	 */
	unsigned int         GetHistorySize(VistaDeviceSensor *) const;

	unsigned int         GetDriverWriteHistorySize( VistaDeviceSensor * ) const;
	unsigned int         GetUserReadHistorySize( VistaDeviceSensor * ) const;

	/**
	 * register a sensor. The registration is pointer-wise, registering a sensor
	 * more than once is a waste of cycles, and may reset the history on the sensor,
	 * in case a default history size is given.
	 * @see SetDefaultHistorySize()
	 */
	bool                 RegisterSensor(VistaDeviceSensor *, IVistaDeviceDriver::AllocMemoryFunctor *amf = NULL 
		                                                   , IVistaDeviceDriver::ClaimMemoryFunctor *cmf = NULL,
														   bool bReRegister = false );

	bool                 SetAllocAndClaimFunctor( VistaDeviceSensor *,
		                                         IVistaDeviceDriver::AllocMemoryFunctor *amf,
		                                         IVistaDeviceDriver::ClaimMemoryFunctor *cmf );

	/**
	 * Unregisters a sensor from this history. A sensor that was not registered
	 * can not be unregistered.
	 * @return true
	 */
	bool                 UnregisterSensor(VistaDeviceSensor * );

	/**
	 * query, whether a sensor is registered with this history.
	 */
	bool                 GetIsRegistered(VistaDeviceSensor *) const;


	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

	static unsigned int INVALID_READING;

protected:
private:
	static int m_nAspectId;


	struct _sL
	{
		_sL()
			: m_pMeasures(NULL)
			, m_amf(NULL)
			, m_cmf(NULL)
			, m_bInitialized(false)
		{
		}

		_sL(VistaMeasureHistory *pMeasures,
			IVistaDeviceDriver::AllocMemoryFunctor *amf = NULL,
			IVistaDeviceDriver::ClaimMemoryFunctor *cmf = NULL,
			bool bInitialized = false )
			: m_pMeasures(pMeasures)
			, m_nIt(m_pMeasures->m_rbHistory.GetCurrent())
			, m_amf(amf)
			, m_cmf(cmf)
			, m_bInitialized(bInitialized)
		{
		}

		VistaMeasureHistory *m_pMeasures;
		bool m_bInitialized;
		IVistaDeviceDriver::AllocMemoryFunctor *m_amf;
		IVistaDeviceDriver::ClaimMemoryFunctor *m_cmf;

		VistaMeasureHistory::Buffer::iterator m_nIt;
	};

	typedef std::map<VistaDeviceSensor*, _sL> HISTORY;
	HISTORY m_mapHistories;
	unsigned int m_nDefHistSize;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //_VISTADRIVERMEASUREHISTORYASPECT_H
