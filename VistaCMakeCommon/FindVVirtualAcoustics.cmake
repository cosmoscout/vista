

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VVIRTUALACOUSTICS_FOUND )
	set( VABASE_FOUND FALSE )
	vista_find_package_root( VirtualAcoustics "include/VACore.h" )
	vista_find_package_root( VirtualAcoustics "VABase/Include/VACore.h" )
	
	list( FIND VirtualAcoustics_FIND_COMPONENTS VANet VANET_REQUESTED )
	list( FIND VirtualAcoustics_FIND_COMPONENTS VACore VACORE_REQUESTED )
	list( FIND VirtualAcoustics_FIND_COMPONENTS 3rdParty 3RDPARTY_REQUESTED )
	
	if( VIRTUALACOUSTICS_ROOT_DIR )
	
		if( EXISTS "${VIRTUALACOUSTICS_ROOT_DIR}/VABase/Include/VACore.h" )
			set( VA_USES_OLD_DEPLOYMENT_STRUCTURE TRUE )
			if( VISTA_64BIT )
				set( POSTFIX x64 )
			else()
				if( WIN32 )
					set( POSTFIX Win32 )
				else()
					set( POSTFIX x86 )
				endif()
			endif()
		else()
			set( VA_USES_OLD_DEPLOYMENT_STRUCTURE FALSE )
		endif()
		
		if( VA_USES_OLD_DEPLOYMENT_STRUCTURE )
			set( VIRTUALACOUSTICS_INCLUDE_DIRS "${VIRTUALACOUSTICS_ROOT_DIR}/VABase/Include" )
			if( EXISTS "${VIRTUALACOUSTICS_ROOT_DIR}/lib/${POSTFIX}" )
				set( VIRTUALACOUSTICS_LIBRARY_DIRS "${VIRTUALACOUSTICS_ROOT_DIR}/lib/${POSTFIX}" )
			elseif( EXISTS "${VIRTUALACOUSTICS_ROOT_DIR}/VABuild/Build/Debug_${POSTFIX}" )
				set( VIRTUALACOUSTICS_LIBRARY_DIRS "${VIRTUALACOUSTICS_ROOT_DIR}/VABuild/Build/Debug_${POSTFIX}" "${VIRTUALACOUSTICS_ROOT_DIR}/VABuild/Build/Release_${POSTFIX}"	)	
			endif()
		else()	
			set( VIRTUALACOUSTICS_INCLUDE_DIRS "${VIRTUALACOUSTICS_ROOT_DIR}/include" )
			set( VIRTUALACOUSTICS_LIBRARY_DIRS "${VIRTUALACOUSTICS_ROOT_DIR}/lib" )
		endif()		

		if( VIRTUALACOUSTICS_LIBRARY_DIRS )
			set( VABASE_FOUND TRUE )
			if( VA_USES_OLD_DEPLOYMENT_STRUCTURE )			
				set( VIRTUALACOUSTICS_LIBRARIES optimized VABaseDll debug VABaseDllD )			
			else()
				set( VIRTUALACOUSTICS_LIBRARIES optimized VABase debug VABaseD )			
			endif()
								
			if( EXISTS "${VIRTUALACOUSTICS_ROOT_DIR}/VANet/Include/VANetServer.h" OR EXISTS "${VIRTUALACOUSTICS_ROOT_DIR}/include/VANetServer.h" )
				set( VVirtualAcoustics_VANet_FOUND TRUE )
				set( VirtualAcoustics_VANet_FOUND TRUE )
				
				if( VANET_REQUESTED OR NOT VirtualAcoustics_FIND_COMPONENTS )
					if( VA_USES_OLD_DEPLOYMENT_STRUCTURE )
						list( APPEND VIRTUALACOUSTICS_INCLUDE_DIRS "${VIRTUALACOUSTICS_ROOT_DIR}/VANet/Include" )
						list( APPEND VIRTUALACOUSTICS_LIBRARIES optimized VANetDll debug VANetDllD )
					else()
						list( APPEND VIRTUALACOUSTICS_LIBRARIES optimized VANet debug VANetD )
					endif()
				endif()
			else()
				set( VVirtualAcoustics_VANet_FOUND FALSE )
				set( VirtualAcoustics_VANet_FOUND FALSE )
			endif()
			
			if( EXISTS "${VIRTUALACOUSTICS_ROOT_DIR}/VACore/Include/VACoreFactory.h" OR EXISTS "${VIRTUALACOUSTICS_ROOT_DIR}/include/VACoreFactory.h"  )
				set( VVirtualAcoustics_VACore_FOUND TRUE )
				set( VirtualAcoustics_VACore_FOUND TRUE )			
				
				if( VACORE_REQUESTED OR NOT VirtualAcoustics_FIND_COMPONENTS )
					if( VA_USES_OLD_DEPLOYMENT_STRUCTURE )
						list( APPEND VIRTUALACOUSTICS_INCLUDE_DIRS "${VIRTUALACOUSTICS_ROOT_DIR}/VACore/Include" )
						list( APPEND VIRTUALACOUSTICS_LIBRARIES optimized VACoreDll debug VACoreDllD )
						list( APPEND VIRTUALACOUSTICS_LIBRARY_DIRS "${VIRTUALACOUSTICS_ROOT_DIR}/Raven/lib/${POSTFIX}" "${VIRTUALACOUSTICS_ROOT_DIR}/RavenNet/lib/${POSTFIX}" )
					else()
						list( APPEND VIRTUALACOUSTICS_LIBRARIES optimized VACore debug VACoreD )
					endif()
				endif()
			else()
				set( VVirtualAcoustics_VACore_FOUND FALSE )
				set( VirtualAcoustics_VACore_FOUND FALSE )
			endif()
			
			
			if( 3RDPARTY_REQUESTED AND VA_USES_OLD_DEPLOYMENT_STRUCTURE )
				list( APPEND VIRTUALACOUSTICS_LIBRARY_DIRS 	"${VIRTUALACOUSTICS_ROOT_DIR}/3rdParty/fftw3/bin/${POSTFIX}" 
															"${VIRTUALACOUSTICS_ROOT_DIR}/3rdParty/ipp/bin/${POSTFIX}"
															"${VIRTUALACOUSTICS_ROOT_DIR}/3rdParty/tbb/bin/${POSTFIX}" )
				set( VVirtualAcoustics_3rdParty_FOUND TRUE )
			endif()
		endif()
	endif()
endif()


find_package_handle_standard_args( VVirtualAcoustics REQUIRED_VARS VIRTUALACOUSTICS_LIBRARY_DIRS VIRTUALACOUSTICS_INCLUDE_DIRS HANDLE_COMPONENTS 
									)#FAIL_MESSAGE "VirtualAcoustics with requested components ${VVirtualAcoustics_FIND_COMPONENTS} could not be found (VABase found: ${VABASE_FOUND} | VANet found: ${VirtualAcoustics_VANet_FOUND} | VACore found: ${VirtualAcoustics_VACore_FOUND} )" )

