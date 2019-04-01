
if( NOT VISTACORELIBS_BUILD_KERNELOPENSGEXT )
	message( FATAL_ERROR "13KernelOpenSGExtDemo cannot be built - VistaCorelibs component VistaKernelOpenSGExt is not activated" )
endif()
		
add_subdirectory( ParticlesDemo )
add_subdirectory( ShadowDemo )
