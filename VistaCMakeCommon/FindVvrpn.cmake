

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )


vista_find_package_root( vrpn include/vrpn_Connection.h )


set( _VRPN_SERVER_REQUESTED FALSE )
if( Vvrpn_FIND_COMPONENTS )
	set( _VRPN_SERVER_REQUESTED FALSE )
	foreach( _COMPONENT ${Vvrpn_FIND_COMPONENTS} )
		if( _COMPONENT STREQUAL "Server" )
			set( _VRPN_SERVER_REQUESTED TRUE )
		else()
			message( WARNING "FindVvrpn requests unknown component ${_COMPONENT}" )
		endif()		
	endforeach()
endif()

if( VRPN_ROOT_DIR )
	set( VRPN_INCLUDE_DIRS "${VRPN_ROOT_DIR}/include" )
	set( VRPN_LIBRARY_DIRS "${VRPN_ROOT_DIR}/lib" )
	
	vista_check_library_exists( _VRPN_DLL_EXISTS vrpndll "${VRPN_LIBRARY_DIRS}" )
	vista_check_library_exists( _VRPN_STATIC_EXISTS vrpn "${VRPN_LIBRARY_DIRS}" )
		
	set( _VRPN_LIB_NAME )
	if( _VRPN_DLL_EXISTS AND( BUILD_SHARED_LIBS OR NOT _VRPN_STATIC_EXISTS ) )
		set( _VRPN_LIB_NAME vrpndll )
	elseif( _VRPN_STATIC_EXISTS )
		set( _VRPN_LIB_NAME vrpn )
	else()
		# no lib exists :(
	endif()
	
	
	if( _VRPN_SERVER_REQUESTED )
		vista_check_library_exists( _VRPN_SERVER_DLL_EXISTS vrpnserverdll "${VRPN_LIBRARY_DIRS}" )
		vista_check_library_exists( _VRPN_SERVER_STATIC_EXISTS vrpnserver "${VRPN_LIBRARY_DIRS}" )
		set( _VRPN_SERVER_LIB_NAME )
		if( _VRPN_SERVER_DLL_EXISTS AND( BUILD_SHARED_LIBS OR NOT _VRPN_SERVER_STATIC_EXISTS ) )
			set( _VRPN_SERVER_LIB_NAME vrpnserverdll )
			set( Vvrpn_Server_FOUND TRUE )
		elseif( _VRPN_SERVER_STATIC_EXISTS )
			set( _VRPN_SERVER_LIB_NAME vrpnserver )
			set( Vvrpn_Server_FOUND TRUE )
		else()
			# no lib exists :(
			set( Vvrpn_Server_FOUND FALSE )
			if( Vvrpn_FIND_REQUIRED_Server )
				message( ERROR "Module FindVvrpn could not find required component \"Server\"" )
			endif()
		endif()
	endif()
	
endif()

if( _VRPN_LIB_NAME )	
	vista_check_library_exists( _VRPN_DEB1_EXISTS ${_VRPN_LIB_NAME}d "${VRPN_LIBRARY_DIRS}" )
	vista_check_library_exists( _VRPN_DEB2_EXISTS ${_VRPN_LIB_NAME}D "${VRPN_LIBRARY_DIRS}" )		
	
	if( _VRPN_DEB1_EXISTS )
		set( VRPN_LIBRARIES optimized ${_VRPN_LIB_NAME} debug ${_VRPN_LIB_NAME}d )
	elseif( _VRPN_DEB2_EXISTS )
		set( VRPN_LIBRARIES optimized ${_VRPN_LIB_NAME} debug ${_VRPN_LIB_NAME}D )
	else()
		set( VRPN_LIBRARIES ${_VRPN_LIB_NAME} )
	endif()
endif()

if( _VRPN_SERVER_REQUESTED AND _VRPN_SERVER_LIB_NAME )	
	vista_check_library_exists( _VRPN_SERVER_DEB1_EXISTS ${_VRPN_SERVER_LIB_NAME}d "${VRPN_LIBRARY_DIRS}" )
	vista_check_library_exists( _VRPN_SERVER_DEB2_EXISTS ${_VRPN_SERVER_LIB_NAME}D "${VRPN_LIBRARY_DIRS}" )		
	
	if( _VRPN_SERVER_DEB1_EXISTS )
		set( VRPN_LIBRARIES optimized ${_VRPN_SERVER_LIB_NAME} debug ${_VRPN_SERVER_LIB_NAME}d )
	elseif( _VRPN_SERVER_DEB2_EXISTS )
		set( VRPN_LIBRARIES optimized ${_VRPN_SERVER_LIB_NAME} debug ${_VRPN_SERVER_LIB_NAME}D )
	else()
		set( VRPN_LIBRARIES ${_VRPN_SERVER_LIB_NAME} )
	endif()
endif()

set( _VRPN_QUAT_LIB_NAME quat )
vista_check_library_exists( _VRPN_QUAT_EXISTS ${_VRPN_QUAT_LIB_NAME} "${VRPN_LIBRARY_DIRS}" )
vista_check_library_exists( _VRPN_QUAT1_EXISTS ${_VRPN_QUAT_LIB_NAME}d "${VRPN_LIBRARY_DIRS}" )
vista_check_library_exists( _VRPN_QUAT2_EXISTS ${_VRPN_QUAT_LIB_NAME}D "${VRPN_LIBRARY_DIRS}" )		

if( _VRPN_QUAT_EXISTS )
	if( _VRPN_QUAT1_EXISTS )
		list( APPEND VRPN_LIBRARIES optimized ${_VRPN_QUAT_LIB_NAME} debug ${_VRPN_QUAT_LIB_NAME}d )
	elseif( _VRPN_QUAT2_EXISTS )
		list( APPEND VRPN_LIBRARIES optimized ${_VRPN_QUAT_LIB_NAME} debug ${_VRPN_QUAT_LIB_NAME}D )
	else()
		list( APPEND VRPN_LIBRARIES ${_VRPN_QUAT_LIB_NAME} )
	endif()
endif()

find_package_handle_standard_args( Vvrpn "vrpn could not be found" VRPN_ROOT_DIR VRPN_LIBRARIES )
