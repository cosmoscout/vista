

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VPYTHONLIBS_FOUND )
	vista_find_original_package( PythonLibs )
	
	set( PYTHONLIBS_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}" )
	set( PYTHONLIBS_LINK_DIRS "${PYTHON_INCLUDE_DIRS}" )
	set( PYTHONLIBS_LIBRARIES "${PYTHON_LIBRARIES}" )
	
endif( NOT VPYTHONLIBS_FOUND )

find_package_handle_standard_args( VPythonLibs "PythonLibs could not be found" PYTHONLIBS_INCLUDE_DIRS )

