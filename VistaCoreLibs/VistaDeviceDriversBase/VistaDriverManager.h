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


#ifndef __VISTADRIVERMANAGER_H
#define __VISTADRIVERMANAGER_H


#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDriverUtils.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>

#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaConnectionUpdater;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a utility class for
 * - initializing Vdd
 * - managing driver plugins (adding / removing)
 * - keeping a driver map to work with
 * - Update all active devices correctly
 */
class VISTADEVICEDRIVERSAPI VistaDriverManager
{
public:
	/**
	 * The constructor sets up the Vdd for you. It will
	 * - call VddUtils::InitVdd()
	 * - initialize plugins for you
	 *
	 * The plugins are loaded from:
	 * - the path pointed to using the environment variable VDD_DRIVERS
	 * - if that one is empty, the
	 */
	VistaDriverManager( VistaDriverMap & );
	~VistaDriverManager();


	///@name management API
	IVistaDeviceDriver *CreateAndRegisterDriver( const std::string &strDriverName,
			                                     const std::string &strDriverClassName );

	IVistaDriverCreationMethod *GetCreationMethodForClass( const std::string &strDriverClassName ) const;

	void RegisterDriver( const std::string &strDriverName, IVistaDeviceDriver *pDriver );
	void RegisterDriverPlugin( const VddUtil::VistaDriverPlugin & oPlugin );

	void UnregisterDriver( IVistaDeviceDriver * );

	void SetEnableStateOnAllDrivers( bool bState );

	///@name runtime-API

	bool ConnectAllDrivers();
	bool DisconnectAllDrivers();

	VistaDriverMap &GetDriverMap();
	const VistaDriverMap &GetDriverMap() const;

	enum eLauchOption
	{
		E_LAUNCH_ALL_AS_THREAD=0,
		E_LAUNCH_ONLY_THOSE_WITH_THREAD_ASPECT
	};

	bool StartAsyncDriverDispatch( eLauchOption launch_option = E_LAUNCH_ALL_AS_THREAD );
	bool StopAsyncDriverDispatch();

	int InitAllAvailablePlugins( const std::string &search_dir_root = "" );

	bool InitPlugin( const std::string &driver_name, const std::string &search_dir_root = "" );


	struct PlugDesc
	{
		PlugDesc() {}

		PlugDesc( const std::string &transcoderName,
			      const std::string &crmSourceName )
			: m_TranscoderSourceName( transcoderName )
			, m_DriverCreationSourceName( crmSourceName )
		{}

		std::string m_TranscoderSourceName,
			        m_DriverCreationSourceName;

		VddUtil::VistaDriverPlugin m_plg;
	};

	bool LoadPlugin( PlugDesc &dsc ) const;
	bool UnloadPlugin( PlugDesc &dsc ) const;

private:

	int DisposePlugins();


	void StopThreadedDevices();

	std::vector<VddUtil::VistaDriverPlugin> m_vecDriverPlugins;

	VistaDriverMap& m_Drivers;

	VistaConnectionUpdater *m_pConnUpdater;
};



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif // __VISTADRIVERMANAGER_H
