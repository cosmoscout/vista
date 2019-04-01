

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VNIFTI_FOUND )

	vista_find_package_root( NIFTI include/nifti/nifti1_io.h NAMES Nifti nifti NIFTI)

	if( NIFTI_ROOT_DIR )
		find_library( NIFTI_IO_LIBRARY		NAMES niftiio NIFTIIO libniftiio
					PATHS ${NIFTI_ROOT_DIR}/lib
					CACHE "NIFTI IO library" )
		find_library( NIFTI_ZNZ_LIBRARY NAMES znz ZNZ libznz
					PATHS ${NIFTI_ROOT_DIR}/lib
					CACHE "NIFTI ZNZ library" )
		find_library( NIFTI_FSLIO_LIBRARY NAMES fslio FSLIO libfslio
					PATHS ${NIFTI_ROOT_DIR}/lib
					CACHE "NIFTI FSLIO library" )
		set( NIFTI_LIBRARIES ${NIFTI_IO_LIBRARY} ${NIFTI_ZNZ_LIBRARY} ${NIFTI_FSLIO_LIBRARY})
					
		set( NIFTI_INCLUDE_DIRS ${NIFTI_ROOT_DIR}/include ${NIFTI_ROOT_DIR}/include/nifti CACHE STRING "Nifti include dir.")
		mark_as_advanced( NIFTI_INCLUDE_DIRS )
		set( NIFTI_LIBRARY_DIRS ${NIFTI_ROOT_DIR}/lib ${NIFTI_ROOT_DIR}/bin CACHE STRING "Nifti library dir.")
		mark_as_advanced( NIFTI_LIBRARY_DIRS )

	endif( NIFTI_ROOT_DIR )

endif( NOT VNIFTI_FOUND )

find_package_handle_standard_args( VNIFTI "NIFTI could not be found" NIFTI_ROOT_DIR NIFTI_LIBRARIES)

