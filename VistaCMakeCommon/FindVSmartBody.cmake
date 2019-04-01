

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VSMARTBODY_FOUND )
	vista_find_package_root( SmartBody /src/SmartBody/sb/SBTypes.h NAMES SmartBodySDK smartbody SmartBody )
    
	if( SMARTBODY_ROOT_DIR )
		set( SMARTBODY_INCLUDE_DIRS 
			${SMARTBODY_ROOT_DIR}/src/SmartBody
			${SMARTBODY_ROOT_DIR}/include/vhcl
			${SMARTBODY_ROOT_DIR}/include/steersuite
			${SMARTBODY_ROOT_DIR}/include/steersuite/external
			${SMARTBODY_ROOT_DIR}/include/bonebus
			${SMARTBODY_ROOT_DIR}/include
			${SMARTBODY_ROOT_DIR}/include/python27
        )
        
        set( SMARTBODY_LIBRARY_DIRS
                ${SMARTBODY_ROOT_DIR}/bin
                ${SMARTBODY_ROOT_DIR}/lib	
               )       
        
        if( UNIX )
			set( SMARTBODY_LIBRARIES            
				SmartBody
				pprAI
				#Needs to be added, so steerlib in vrsw is linked
				steerlib
			)
			
			vista_add_package_dependency( SmartBody ALUT REQUIRED )
			vista_add_package_dependency( SmartBody PythonLibs REQUIRED )
			vista_add_package_dependency( SmartBody Boost REQUIRED system filesystem regex python)
		
			set(PYTHON_INCLUDE_DIR "/usr/include/python2.7" CACHE STRING "Path to library overwritten from FindVSmartbody" FORCE)
			set(PYTHON_LIBRARY "/usr/lib64/libpython2.7.so" CACHE STRING "Path to library overwritten from FindVSmartbody" FORCE)
		
		else()
			set( SMARTBODY_LIBRARIES            
				optimized SmartBody
				debug SmartBody_d
				optimized vhcl
                debug vhcl_d 
				optimized pprAI
				debug pprAId
			)
		endif()
     		
    endif()
endif( NOT VSMARTBODY_FOUND )

find_package_handle_standard_args( VSmartBody "SmartBody could not be found" SMARTBODY_ROOT_DIR )
