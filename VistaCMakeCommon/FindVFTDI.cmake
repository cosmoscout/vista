#Find script in order to link FTDI drivers for USB devices

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VFTDI_FOUND )
	vista_find_package_root( FTDI ftd2xx.h )

	if( FTDI_ROOT_DIR )
		set( FTDI_LIBRARIES ftd2xx )
		mark_as_advanced( FTDI_LIBRARIES )

		set( FTDI_INCLUDE_DIRS ${FTDI_ROOT_DIR} )
		set( FTDI_LIBRARY_DIRS ${FTDI_ROOT_DIR}/amd64 )
	endif( FTDI_ROOT_DIR )
endif( NOT VFTDI_FOUND )

find_package_handle_standard_args( VFTDI "FTDI could not be found" FTDI_ROOT_DIR )

