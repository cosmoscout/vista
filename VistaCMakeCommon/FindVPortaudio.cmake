# $Id: FindVPortaudio.cmake 21495 2011-05-25 07:52:18Z dr165799 $

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VPORTAUDIO_FOUND )
	vista_find_package_root( Portaudio "include/portaudio.h" )	

	if( PORTAUDIO_ROOT_DIR )
		set( PORTAUDIO_INCLUDE_DIRS "${PORTAUDIO_ROOT_DIR}/include" )
        set( PORTAUDIO_LIBRARY_DIRS "${PORTAUDIO_ROOT_DIR}/lib" )
		if( VISTA_64BIT )
			set( PORTAUDIO_LIBRARIES optimized portaudio_static_x64 debug portaudiod_static_x64 )
		else( VISTA_64BIT )
			set( PORTAUDIO_LIBRARIES optimized portaudio_static debug portaudiod_static )
		endif( VISTA_64BIT )
	else( PORTAUDIO_ROOT_DIR )
		message( WARNING "vista_find_package_root - File named portaudio.h not found" )	
	endif( PORTAUDIO_ROOT_DIR )
endif( NOT VPORTAUDIO_FOUND )

find_package_handle_standard_args( VPORTAUDIO "Portaudio could not be found" PORTAUDIO_ROOT_DIR )

