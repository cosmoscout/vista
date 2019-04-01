

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

# Note: if the optional component PGM is requested, only zeromq versions that have a dummy file callsed
#       "ZEROMQ_WITH_PGM" in their folder will be found. More precisely, it has to reside in the
#        architecture-specific folder, next to the lib- and include-folder 
#        (e.g. /home/vrsw/zeromq/zeromq-2.2.2/LINUX_X86_64/ZEROMQ_WITH_PGM). If this file is not created,
#        the cmake-find-module will still find the PGM version when using find_package( ZMQ ), but
#         not if pgm is also requested by ind_package( ZMQ REQUIRED PGM )

if( NOT VZMQ_FOUND )

	list( FIND VZMQ_FIND_COMPONENTS "PGM" _INDEX )
	
	if( _INDEX EQUAL -1 )
		vista_find_package_root( ZMQ include/zmq.h NAMES ZMQ zeromq ZeroMQ Zeromq )
	else()
		vista_find_package_root( ZMQ ZEROMQ_WITH_PGM NAMES ZMQ zeromq ZeroMQ Zeromq )
	endif()

	if( ZMQ_ROOT_DIR )
		
		set( ZMQ_INCLUDE_DIRS "${ZMQ_ROOT_DIR}/include" )
		
		if( ZMQ_VERSION_STRING VERSION_GREATER 3.0 )# AND ZMQ_VERSION_STRING VERSION_LESS 4.0 )
			if( UNIX )
				#UNIX is easy, as usual
				set( ZMQ_LIBRARY_DIRS "${ZMQ_ROOT_DIR}/lib" )
				set( ZMQ_LIBRARIES zmq )
			elseif(WIN32)
				#zmq 3.x puts the dlls in a separate bin directory AND differentiates 
				#between win32 and x86_64 builds
				if(VISTA_64BIT) 
					set( ZMQ_LIBRARY_DIRS 
						 "${ZMQ_ROOT_DIR}/lib/x64"
						 "${ZMQ_ROOT_DIR}/bin/x64" )
				else(VISTA_64BIT)
					set( ZMQ_LIBRARY_DIRS 
						 "${ZMQ_ROOT_DIR}/lib/Win32"
						 "${ZMQ_ROOT_DIR}/bin/Win32" )
				endif(VISTA_64BIT)
				#zmq insists on libraries being named libzmq.lib and libzmq.dll
				set(ZMQ_LIBRARIES libzmq)
			endif(UNIX)
		else( )
			#for older versions ==> just use the standard lib path
			#set( ZMQ_LIBRARY_DIRS "${ZMQ_ROOT_DIR}/lib" )
			#set( ZMQ_LIBRARIES libzmq )
			vista_find_library_uncached( NAMES zmq ZMQ libzmq
									PATHS "${ZMQ_ROOT_DIR}/lib"
									CACHE "ZMQ library" )
			get_filename_component( ZMQ_LIBRARY_DIRS "${VISTA_UNCACHED_LIBRARY}" PATH )
			get_filename_component( ZMQ_LIBRARIES "${VISTA_UNCACHED_LIBRARY}" NAME )
		endif( )
		
	endif( ZMQ_ROOT_DIR )

endif( NOT VZMQ_FOUND )

find_package_handle_standard_args( VZMQ "ZMQ could not be found" ZMQ_ROOT_DIR ZMQ_LIBRARIES)

