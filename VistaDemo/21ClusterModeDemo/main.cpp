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


/**
 * Description:
 * This is the ClusterModeDemo, which explains how to set up the VistaClusterMode,
 * how it works, and what to take care of to keep nodes synchronized.
 * 
 * How to use this demo:
 * - read the general info in this file
 * - read the start_clustered.bat|sh file to see how to start masters and slaves
 * - read the configfile display_desktop_clustermode.ini to see how to configure
 *      the cluster mode using configfiles
 * - read the comments in ClusterModeDemo.cpp for info on how to synchonize data
 *      and events
 */

/**
 * General info on clustering in Vista
 * Vista uses multiple, synchronized applications for cluster rendering.
 * For each sub-image, the same application (named slave) is started and runs
 * on a node, and performs the same application steps. All slaves are synchronized
 * by a master node. All nodes perform the same calculations in order to maintain
 * a synchronous application state. If different states would perform different
 * computations, the nodes would diverge and generate non-matching images.
 * Therefore, the master nodes distributes Events to the slaves tio keep them in sync.
 * These are the SystemEvents (general steps per frame), InteractionEvents (distribute
 * the contents of data flow graphs that have to be sync, which includes driver data),
 * and ExternalMessageEvents (from the MessagePort or user-emitted).
 * Furthermore, the image swapping is synchronized so that all node displays
 * show the image for the same timestamp/frame.
 *
 * Vista uses a cluster mode to abstract clustermode behavior. These are:
 *    - Standalone: Only a single application is running
 *    - NewClusterMaster: Master node of a clustered vista application
 *    - NewClusterSlave: One of the slaves of a clustered vista application
 *    - ClusterMaster/ClusterSlave: Older versions of clustering, kept for compatibility
 *
 * Since only some events are synchronized in cluster mode, it is important that all
 * Vista cluster nodes are performing the exact same calculation, otherwise risking
 * diverging images. Therefore, some things should be kept in mind:
 *    - Ensure that all calculations that influence the main thread application state
 *      are deterministic, i.e. generate the same results on all nodes.
 *    - For random numbers, use the VistaRandomNumberGenerator::GetStandardRNG(), which
 *      generates the same values for all nodes
 *    - When timing is used to make application-state decisions, only use cluster-sync
 *      times, e.g. from system events or by explicitly syncing timestamps (example in 
 *      ClusterModeDemo.cpp)
 *    - Data from external sources (e.g. driver data, HPC computation results) should only
 *      be received on the master node, and then distributed to the slaves (examples in the
 *      ClusterModeDemo.cpp)
 *    - If calculations are performed asynchronously, the results might influence the
 *      main application thread differently. Thus, either distribute the results from
 *      the master to the slaves, or synchronize the read-back time.
 */


#include "ClusterModeDemo.h"

#include <VistaBase/VistaExceptionBase.h>

#include <iostream>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int	main( int argc, char *argv[] )
{
	try
	{
		ClusterModeDemo oDemoAppl( argc, argv );
		// start application as an endless loop
		oDemoAppl.Run();
	}
	catch( VistaExceptionBase &e )
	{
		e.PrintException();
	}
	catch( std::exception &e )
	{
		std::cerr << "Exception:" << e.what() << std::endl;
	}

	return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "main.cpp"                                                    */
/*============================================================================*/
