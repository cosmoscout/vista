

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VVTK_FOUND )

	vista_find_package_dirs( VTK "VTKConfig.cmake" SUBDIRS "lib/cmake" "lib/cmake/vtk" "lib/cmake/vtk-*" )
		
	# VTK hides it config file quite well, so we have to search it explicitely
    vista_find_original_package( VVTK VTK NO_MODULE PATHS "${VTK_CANDIDATE_DIRS}" "${VTK_CANDIDATE_UNVERSIONED}" NO_DEFAULT_PATH )
	if( NOT VTK_FOUND )
		vista_find_original_package( VVTK VTK NO_MODULE PATHS )
	endif()
	
	if( VTK_FOUND )
		# we don't use the VTK deployed Use file but depend on vista_use_package
		# for setting include_directories and library_directories
		# set( VTK_USE_FILE "${VTK_USE_FILE}" )
		set( VTK_ROOT_DIR "${VTK_INSTALL_PREFIX}" )
		# set( VTK_INCLUDE_DIRS "${VTK_INCLUDE_DIRS}" )
		set( VTK_LIBRARY_DIRS ${VTK_LIBRARY_DIRS} ${VTK_RUNTIME_LIBRARY_DIRS} )
		# set( VTK_LIBRARIES "${VTK_LIBRARIES}" )

		set( VVTK_FOUND ${VTK_FOUND} )
	endif( VTK_FOUND )
endif( NOT VVTK_FOUND )

find_package_handle_standard_args( VVTK "VTK could not be found" VTK_ROOT_DIR )
