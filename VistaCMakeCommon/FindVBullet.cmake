

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VBULLET_FOUND )
	vista_find_package_root( Bullet include/btBulletCollisionCommon.h )

	if( BULLET_ROOT_DIR )
		set( BULLET_INCLUDE_DIRS "${BULLET_ROOT_DIR}/include" )
		set( BULLET_LIBRARY_DIRS "${BULLET_ROOT_DIR}/lib" )
		vista_find_library_uncached_var( _RELEASE_LIB BulletCollision PATHS ${BULLET_LIBRARY_DIRS} NO_DEFAULT_PATH )
		vista_find_library_uncached_var( _DEBUG_LIB BulletCollisionD PATHS ${BULLET_LIBRARY_DIRS} NO_DEFAULT_PATH )
		if( _RELEASE_LIB AND _DEBUG_LIB )
			set( BULLET_LIBRARIES
				optimized BulletCollision
				optimized BulletDynamics
				optimized LinearMath
				optimized BulletSoftBody
				optimized GIMPACTUtils
				optimized ConvexDecomposition
				debug BulletCollisionD
				debug BulletDynamicsD
				debug LinearMathD
				debug BulletSoftBodyD
				debug GIMPACTUtilsD
				debug ConvexDecompositionD
			)
		elseif( UNIX AND _RELEASE_LIB )
			set( BULLET_LIBRARIES
				BulletCollision
				BulletDynamics
				LinearMath
				BulletSoftBody
				GIMPACTUtils
				ConvexDecomposition
			)
		elseif( NOT VBullet_FIND_QUIET )
			if( NOT _RELEASE_LIB AND NOT _DEBUG_LIB )
				message( WARNING "Bullet libraries could not be found in \"${BULLET_LIBRARY_DIRS}\"" )
			elseif( NOT _DEBUG_LIB )
				message( WARNING "Bullet debug libraries could not be found in \"${BULLET_LIBRARY_DIRS}\"" )
			else()
				message( WARNING "Bullet release libraries could not be found in \"${BULLET_LIBRARY_DIRS}\"" )
			endif()
		endif()

	endif( BULLET_ROOT_DIR )

endif( NOT VBULLET_FOUND )

find_package_handle_standard_args( VBullet "Bullet could not be found" BULLET_ROOT_DIR BULLET_LIBRARIES )

