

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VMPI_FOUND )

	set( _FLAG )
	if( MPI_FIND_REQUIRED )
		set( _FLAG REQUIRED )
	endif( MPI_FIND_REQUIRED )
	if( MPI_FIND_QUIETLY )
		set( _FLAG ${_FLAG} QUIET )
	endif( MPI_FIND_QUIETLY )
	
	if(WIN32)
			set( MPI_CXX_SKIP_MPICXX ON )
	endif(WIN32)
		
	find_package(MPI ${_FLAG} )
	
	if( MPI_FOUND )
		
		set( MPI_DEFINITIONS ${MPI_COMPILE_FLAGS} )
		set( MPI_INCLUDE_DIRS ${MPI_INCLUDE_PATH} )
		foreach( _LIB ${MPI_LIBRARIES} )
			get_filename_component( _DIR ${_LIB} PATH )
			list( APPEND MPI_LIBRARY_DIRS ${_DIR} )
		endforeach()
		list( REMOVE_DUPLICATES MPI_LIBRARY_DIRS )
	endif( MPI_FOUND )

endif( NOT VMPI_FOUND )

find_package_handle_standard_args( VMPI "MPI could not be found" MPI_INCLUDE_DIRS )