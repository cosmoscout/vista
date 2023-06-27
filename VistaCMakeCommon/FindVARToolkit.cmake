

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VARTOOLKIT_FOUND )

	vista_find_package_root( ARTOOLKIT include/profile.h NAMES ARToolKit ARToolkit DEBUG_OUTPUT ) # Arbitrary header chosen

	if( ARTOOLKIT_ROOT_DIR )
		set( ARTOOLKIT_LIBRARIES 
			optimized AR
			optimized AR2
			optimized ARgsub
			optimized ARgsub_lite
			optimized ARICP
			optimized ARMulti
			optimized ARosg
			optimized ARUtil
			optimized ARvideo
			optimized ARWrapper
			optimized Eden
			optimized glut64
			optimized KPM
			optimized libjpeg
			optimized opencv_calib3d2410
			optimized opencv_core2410
			optimized opencv_features2d2410
			optimized opencv_flann2410
			optimized opencv_imgproc2410
			optimized OpenThreads
			optimized osg
			optimized osgAnimation
			optimized osgDB
			optimized osgFX
			optimized osgGA
			optimized osgManipulator
			optimized osgParticle
			optimized osgPresentation
			optimized osgShadow
			optimized osgSim
			optimized osgTerrain
			optimized osgText
			optimized osgUtil
			optimized osgViewer
			optimized osgVolume
			optimized osgWidget
			optimized pthreadVC2
			debug ARd
			debug AR2d
			debug ARgsubd
			debug ARgsub_lited
			debug ARICPd
			debug ARMultid
			debug ARosgd
			debug ARUtild
			debug ARvideod
			debug ARWrapperd
			debug Eden
			debug glut64
			debug KPMd
			debug libjpeg
			debug opencv_calib3d2410
			debug opencv_core2410
			debug opencv_features2d2410
			debug opencv_flann2410
			debug opencv_imgproc2410
			debug OpenThreads
			debug osg
			debug osgAnimation
			debug osgDB
			debug osgFX
			debug osgGA
			debug osgManipulator
			debug osgParticle
			debug osgPresentation
			debug osgShadow
			debug osgSim
			debug osgTerrain
			debug osgText
			debug osgUtil
			debug osgViewer
			debug osgVolume
			debug osgWidget
			debug pthreadVC2
		)
		mark_as_advanced( ARTOOLKIT_LIBRARIES )

		set( ARTOOLKIT_INCLUDE_DIRS ${ARTOOLKIT_ROOT_DIR}/include )
		set( ARTOOLKIT_LIBRARY_DIRS ${ARTOOLKIT_ROOT_DIR}/lib ${ARTOOLKIT_ROOT_DIR}/bin )
	endif( ARTOOLKIT_ROOT_DIR )

endif( NOT VARTOOLKIT_FOUND )

find_package_handle_standard_args( VARTOOLKIT "ARToolkit could not be found" ARTOOLKIT_ROOT_DIR )

