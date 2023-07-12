		 
# Transcoder

set( dirFiles_transcoder
    VistaVRPNTranscoder.cpp
)
source_group( "Source Files" FILES ${dirFiles_transcoder} )
add_library( VistaVRPNTranscoder
    ${dirFiles_transcoder}
)
target_link_libraries( VistaVRPNTranscoder
	VistaBase
	VistaAspects
    VistaInterProcComm
    VistaDeviceDriversBase
)
vista_configure_lib( VistaVRPNTranscoder )
vista_install( VistaVRPNTranscoder "VistaDeviceDrivers/VistaVRPNDriver" "DriverPlugins" )	 
set_target_properties( VistaVRPNTranscoder PROPERTIES FOLDER "Drivers" )	 
vista_set_outdir( VistaVRPNTranscoder "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
		 
# since it has third-party dependencies, we only build the driver and plugin if the package is available
vista_find_package( vrpn QUIET )

if( NOT VRPN_FOUND )
	message( STATUS "VistaVRPNDriver requires vrpn, which is not available - only transcoder will be built" )
else( NOT VRPN_FOUND )

	# Driver Plugin

	set( dirFiles_plugin
		VistaVRPNPlugin.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_plugin} )
	add_library( VistaVRPNPlugin
		${dirFiles_plugin}
	) 
	target_link_libraries( VistaVRPNPlugin
		VistaVRPNDriver
		VistaDeviceDriversBase
	)
	vista_configure_lib( VistaVRPNPlugin )
	vista_install( VistaVRPNPlugin "VistaDeviceDrivers/VistaVRPNDriver" "DriverPlugins" )
	set_target_properties( VistaVRPNPlugin PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaVRPNPlugin "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )

	# Actual Driver

	vista_use_package( vrpn REQUIRED )

	set( dirFiles_driver
		VistaVRPNDriver.h
		VistaVRPNDriver.cpp
	)
	source_group( "Source Files" FILES ${dirFiles_driver} )
	add_library( VistaVRPNDriver
		${dirFiles_driver}
	)

	target_link_libraries( VistaVRPNDriver
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaDeviceDriversBase
		VistaMath
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	vista_configure_lib( VistaVRPNDriver )
	vista_install( VistaVRPNDriver "VistaDeviceDrivers/VistaVRPNDriver" "DriverPlugins" )
	set_target_properties( VistaVRPNDriver PROPERTIES FOLDER "Drivers" )
	vista_set_outdir( VistaVRPNDriver "${VISTADEVICEDRIVERSBASE_TARGET_OUTDIR}/DriverPlugins" )
	set_property( TARGET VistaVRPNDriver PROPERTY LINK_INTERFACE_LIBRARIES "" ) # prevents auto-includion of linked libraries to other targets linking the driver
	
	
	set( VRPN_BUILD_TESTAPPS ON CACHE BOOL "Enable build of test apps for VRPN driver"  )
	
	if( VRPN_BUILD_TESTAPPS )
		add_executable( VRPN_testclient testclient/spacemouseclient.cpp )
		target_link_libraries( VRPN_testclient 
				VistaBase
				VistaAspects
				VistaInterProcComm
				VistaDeviceDriversBase
				VistaDataFlowNet
				VistaMath
				${VISTA_USE_PACKAGE_LIBRARIES}
		)

		vista_use_package( vrpn REQUIRED Server )
		add_executable( VRPN_testserver testserver/spacemouseserver.cpp )
		target_link_libraries( VRPN_testserver ${VRPN_LIBRARIES} )
		if( UNIX )
			target_link_libraries( VRPN_testserver pthread )
		endif()
	endif()
endif( NOT VRPN_FOUND )
