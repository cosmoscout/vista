# $Id: FindVTBB.cmake 21620 2011-05-30 10:28:48Z dr165799 $

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )
include( VistaHWArchSettings )

if( NOT VTBB_FOUND )

	# tbb.h marks the tbb distribution
	vista_find_package_root( TBB include/tbb/tbb.h DEBUG_OUTPUT )

	if( TBB_ROOT_DIR )
		#Provide specific libraries for release and debug build here
		set( TBB_LIBRARIES
				optimized tbb
				optimized tbbmalloc
				optimized tbbmalloc_proxy
				debug tbb
				debug tbbmalloc
				debug tbbmalloc_proxy
				
			)

		set( TBB_INCLUDE_DIRS ${TBB_ROOT_DIR}/include )
		
		# Set platform specific TBB path here
		if( UNIX )
			#UNIX is easy, as usual
			set( TBB_LIBRARY_DIRS ${TBB_ROOT_DIR}/lib )
		elseif(WIN32)
			#For WIN32, tbb contains specific libraries for different compilers
			if( MSVC )
				if( MSVC80 )
					if(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel64/vc8 
							 ${TBB_ROOT_DIR}/bin/intel64/vc8 )
					else(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel32/vc8 
							 ${TBB_ROOT_DIR}/bin/intel32/vc8 )
					endif(VISTA_64BIT)
				elseif( MSVC90 )
					if(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel64/vc9 
						     ${TBB_ROOT_DIR}/bin/intel64/vc9 )
					else(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel32/vc9 
						     ${TBB_ROOT_DIR}/bin/intel32/vc9 )
					endif(VISTA_64BIT)
				elseif( MSVC10 )
					if(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel64/vc10 
						     ${TBB_ROOT_DIR}/bin/intel64/vc10 )
					else(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel32/vc10
						     ${TBB_ROOT_DIR}/bin/intel32/vc10 )
					endif(VISTA_64BIT)
				elseif( MSVC12 )
					if(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel64/vc12 
						     ${TBB_ROOT_DIR}/bin/intel64/vc12 )
					else(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel32/vc12
						     ${TBB_ROOT_DIR}/bin/intel32/vc12 )
					endif(VISTA_64BIT)
				elseif( MSVC13 )
					if(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel64/vc13 
						     ${TBB_ROOT_DIR}/bin/intel64/vc13 )
					else(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel32/vc13
						     ${TBB_ROOT_DIR}/bin/intel32/vc13 )
					endif(VISTA_64BIT)	
				elseif( MSVC14 )
					if(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel64/vc14 
						     ${TBB_ROOT_DIR}/bin/intel64/vc14 )
					else(VISTA_64BIT) 
						set( TBB_LIBRARY_DIRS 
							 ${TBB_ROOT_DIR}/lib/intel32/vc14
						     ${TBB_ROOT_DIR}/bin/intel32/vc14 )
					endif(VISTA_64BIT)						
				else( MSVC80 )
					message( WARNING "FindPackageTBB - Unknown MSVC version" )
				endif( MSVC80 )
			else( MSVC )
				message( WARNING "FindPackageTBB - using WIN32 without Visual Studio - this will probably fail - use at your own risk!" )
			endif( MSVC )	
		endif(UNIX)
	endif( TBB_ROOT_DIR )

endif( NOT VTBB_FOUND )

find_package_handle_standard_args( VTBB "TBB could not be found" TBB_ROOT_DIR )
