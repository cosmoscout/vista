

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VDIRECTX_FOUND )
	if( NOT DIRECTX_ROOT_DIR )
		set( _SEARCH_DIRS $ENV{VRDEV} $ENV{VISTA_EXTERNAL_LIBS}
						${CMAKE_PREFIX_PATH} $ENV{CMAKE_PREFIX_PATH}
						${CMAKE_SYSTEM_PREFIX_PATH} $ENV{CMAKE_PREFIX_PATH}
						${CMAKE_INCLUDE_PATH} $ENV{CMAKE_SYSTEM_INCLUDE_PATH}
						${CMAKE_SYSTEM_INCLUDE_PATH} $ENV{CMAKE_SYSTEM_INCLUDE_PATH}
		)
		foreach( _PATH ${_SEARCH_DIRS} )
			file( TO_CMAKE_PATH ${_PATH} _PATH )
			file( GLOB _TMP_FILES "${_PATH}/*DirectX*/include/dinput.h" )
			if( _TMP_FILES )
				list( GET _TMP_FILES 0 _FILE )
				get_filename_component( _FOLDER ${_FILE} PATH ) # one up - include dir
				get_filename_component( _FOLDER ${_FOLDER} PATH ) # one up - root dir
				set( DIRECTX_ROOT_DIR ${_FOLDER} CACHE PATH "DirectX SDK root directory" FORCE )
				break()
			endif( _TMP_FILES )
		endforeach( _PATH ${_SEARCH_DIRS} )
		vista_find_package_root( DirectX include/dinput.h )
	endif( NOT DIRECTX_ROOT_DIR )

	if( DIRECTX_ROOT_DIR )
		set( DIRECTX_INCLUDE_DIRS ${DIRECTX_ROOT_DIR}/include )
		if( CMAKE_CL_64 )
			set( DIRECTX_LIBRARY_DIRS ${DIRECTX_ROOT_DIR}/lib/x64 )
		else( CMAKE_CL_64 )
			set( DIRECTX_LIBRARY_DIRS ${DIRECTX_ROOT_DIR}/lib/x86 )
		endif( CMAKE_CL_64 )
		set( DIRECTX_LIBRARIES
			dinput8
			dsound
			dwrite
			DxErr
			dxgi
			dxguid
			XInput
		)

	endif( DIRECTX_ROOT_DIR )

endif( NOT VDIRECTX_FOUND )

find_package_handle_standard_args( VDirectX "DirectX could not be found" DIRECTX_ROOT_DIR )

