

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT FFMPEG_FOUND )

	vista_find_package_root( FFMPEG include/libavcodec/avcodec.h )
	if( FFMPEG_ROOT_DIR )
		find_library( FFMPEG_LIBRARIES	NAMES avcodec avdevice avfilter avformat avutil
					PATHS ${FFMPEG_ROOT_DIR}/lib
					CACHE "FFMPEG library" )
					
		set( FFMPEG_INCLUDE_DIRS ${FFMPEG_ROOT_DIR}/include CACHE STRING "FFMPEG include dir.")
		mark_as_advanced( FFMPEG_INCLUDE_DIRS )
		set( FFMPEG_LIBRARY_DIRS ${FFMPEG_ROOT_DIR}/lib ${FFMPEG_ROOT_DIR}/bin CACHE STRING "FFMPEG library dir.")
		mark_as_advanced( FFMPEG_LIBRARY_DIRS )
	endif( FFMPEG_ROOT_DIR )
	
	set( FFMPEG_LIBRARIES
			avcodec
			avdevice
			avfilter
			avformat
			avutil 
			postproc
			swresample
			swscale )
	
	# MSVC compilers older then version 19 (vs2013) are not c99 compliant, and do not ship with inttypes.h
	if( MSVC_VERSION LESS 1900 )
#		vista_find_package( Msinttypes REQUIRED )
		set ( FFMPEG_DEPENDENCIES package Msinttypes REQUIRED )
		#vista_add_package_dependency( FFMPEG Msinttypes REQUIRED )
	endif( MSVC_VERSION LESS 1900 )

endif( NOT FFMPEG_FOUND )

find_package_handle_standard_args( VFFMPEG "FFMPEG could not be found" FFMPEG_ROOT_DIR FFMPEG_LIBRARIES)

