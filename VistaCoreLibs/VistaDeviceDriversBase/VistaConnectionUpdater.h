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


#ifndef _VISTACONECTIONUPDATER_H
#define _VISTACONECTIONUPDATER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDeviceDriversConfig.h"
#include <VistaAspects/VistaUncopyable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaConnection;
class IVistaExplicitCallbackInterface;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * The connection updater implements a simple reactor pattern.
 * You AddConnectionUpdate connections, and once there is a read-activation
 * on it, the callback that was supplied during adding is called.
 * The updater runs as its own thread which manages a thread pool to have
 * hungry threads waiting for the processing on the callbacks.
 * after the callbacks have been processed, they are passed to the updater
 * again, waiting for new data to arrive and process again.
 * For the update to work correctly, call StartUpdaterLoop() once at the beginning
 * of your processing. It is safe to add and remove update connections that are
 * not currently processed, even when the updater is running.
 * Once you are finished with the application, call ShutdownUpdaterLoop() once to
 * let the service finish.
 */
class VISTADEVICEDRIVERSAPI VistaConnectionUpdater
{
	VISTA_UNCOPYABLE( VistaConnectionUpdater );
public:
	/**
	 * Creates an updater that allocates a VistaThreadPool with nNumJobs
	 * of threads to create. 3 is the default value. If you have a huge amount
	 * of highly active devices, you may think of enlarging the number, otherwise
	 * the default value should do, as it allows processing 3 concurrent connections.
	 */
	VistaConnectionUpdater(int nNumJobs = 3);

	/**
	 * the destructor calls the ShutdownUpdaterLoop() API. But is is considered
	 * good style to have that already called before calling the destructor.
	 */
	virtual ~VistaConnectionUpdater();

	/**
	 * Starts the updater thread which will in turn start the device dispatching
	 * thread work.
	 * @return false iff the thread could not be started (can happen if you run
	                 out of thread handles, but should not...), returns false iff
					 the thread was already started.
	 */

	bool StartUpdaterLoop();

	/**
	 * sends a shutdown signal to the updater loop running to dispatch incoming
	 * data from the connections that are registered.
	 * @param bWaitFinished the caller of this method returns only after the
	          thread work has finished and no more threads are running (can
			  take a while, for example when a device is still sending data)
			  If set to false, this call returns immediately, but there is no
			  way of detecting when it is safe to destroy this instance.
	 */
	bool ShutdownUpdaterLoop(bool bWaitFinished = true);


	/**
	 * adds a connection to be handled upon reading readiness. The ratio is as follows:
	 * when there is data ready at the connection, a background thread fetches the connection
	 * and the pInt callback and then calls the pInt callback's Do() method. When the
	 * callback returns, the connection is watched again for read readiness (or error).
	 * When dispatching, the connection is not watched. A connection can be added more then once,
	 * although that does not make sense. Note that RemConnectionUpdate() only returns the instance
	 * first found / first added.
	 * @param pCon a valid connection (!= NULL) which returns a value != ~0 on the
	          VistaConnection::GetConnectionWaitForDescriptor() method.
	 * @param pInt the callback to call when there is data ready to be read on the pCon.
	 *        Note that pInt is not freed upon destruction of this updater, it is the user's
	 *        duty to destroy pInt
	 * @see RemConnectionUpdate
	 * @return false iff the connection was != NULL but not valid, true else
	 */
	bool AddConnectionUpdate(VistaConnection *pCon,
							 IVistaExplicitCallbackInterface *pInt);


	/**
	 * Removes a callback interface from the updater thread. Callbacks that are currently processed
	 * (i.e. because there is incoming data detected) can not be removed. If a connection is registered
	 * more than once with different callbacks (that does not make sense, but hey...) the first instance
	 * registered is returned. If a callback is currently processed, or the pCon was not registered
	 * before, this method returns NULL
	 * @param pCon the key connection to look for and to remove from the update process
	 * @return NULL in case the pCon was not registered or the pCon is currently processed.
	 * @see AddConnectionUpdate()
	 *
	 */
	IVistaExplicitCallbackInterface *RemConnectionUpdate(VistaConnection *pCon);

	/**
	 * finds the callback that was registered with the updater and the respective callback instance.
	 * @param pCon the connection pointer to look for that was registered with this updater
	 * @return a pointer to the callback that was registered to be called when there is read action
	           on the pCon, or NULL if pCon was not registered beforehand
	 * @see AddConnectionUpdate()
	 */
	IVistaExplicitCallbackInterface *GetConnectionUpdate(VistaConnection *pCon) const;


	/**
	 * test, whether the update thread is running or not.
	 * @return true iff the thread updating the connections is running, false else
	 * @see StartUpdaterLoop()
	 * @see StopUpdateLoop()
	 */
	bool GetIsDispatching() const;

protected:
private:
	class UpdateThread;
	UpdateThread *m_pUpdateThread;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACONECTIONUPDATER_H

