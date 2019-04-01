include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VHDF5_FOUND )
	vista_find_package_root( HDF5 include/hdf5.h NAMES hdf5 HDF5 )

	if( HDF5_ROOT_DIR )
		set( HDF5_INCLUDE_DIRS ${HDF5_ROOT_DIR}/include/ )
		set( HDF5_LIBRARY_DIRS ${HDF5_ROOT_DIR}/lib )
		if ( WIN32 )
		  set (HDF5_LIBRARIES_OPT libhdf5_cpp libhdf5 )
		  set (HDF5_LIBRARIES_DBG libhdf5_cpp_D libhdf5_D )
		  
		  set( HDF5_LIBRARIES optimized ${HDF5_LIBRARIES_OPT} debug ${HDF5_LIBRARIES_DBG}  )
		else()
		  # TODO: set for linux
		  set( HDF5_LIBRARIES hdf5_cpp hdf5 )
		endif ( WIN32 )
	endif( HDF5_ROOT_DIR )

endif( NOT VHDF5_FOUND )

find_package_handle_standard_args( VHDF5 "hdf5 could not be found" HDF5_ROOT_DIR )
