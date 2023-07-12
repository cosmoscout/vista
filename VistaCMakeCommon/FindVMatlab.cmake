# $Id: FindVMatlab.cmake 21495 2016-08-17 07:52:18Z js908001 $

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VMATLAB_FOUND )

	vista_find_package_root( Matlab "extern/include/mex.h" )
	
	if( MATLAB_ROOT_DIR )
		
		set( MATLAB_INCLUDE_DIRS "${MATLAB_ROOT_DIR}/extern/include" )
		
		if( VISTA_64BIT )
			if( MSVC )
				set( MATLAB_LIBRARY_DIRS "${MATLAB_ROOT_DIR}/extern/lib/win64/microsoft" )
				set( MATLAB_MEX_EXTENSION "mexw64" )
			else()
				set( MATLAB_LIBRARY_DIRS "${MATLAB_ROOT_DIR}/extern/lib/win64/mingw" )
				set( MATLAB_MEX_EXTENSION "mexm64" )
			endif()
		else()
			if( MSVC )
				set( MATLAB_LIBRARY_DIRS "${MATLAB_ROOT_DIR}/extern/lib/win32/microsoft" )
				set( MATLAB_MEX_EXTENSION "mexw32" )
			else()
				set( MATLAB_LIBRARY_DIRS "${MATLAB_ROOT_DIR}/extern/lib/win32/mingw" )
				set( MATLAB_MEX_EXTENSION "mexm32" )
			endif()
		endif()
		
		set( MATLAB_LIBRARIES libmex libmx )
	
	endif( MATLAB_ROOT_DIR )
	
endif()

find_package_handle_standard_args( VMatlab "Mathworks Matlab could not be found" MATLAB_ROOT_DIR MATLAB_LIBRARIES )
