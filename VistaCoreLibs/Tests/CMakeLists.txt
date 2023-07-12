

#require_vistacommon_version( 35191 )
include( VistaTestingUtils )

vista_use_package( GTest REQUIRED SYSTEM_HEADERS )

vista_add_pathscript_dynamic_lib_path( "${CMAKE_BINARY_DIR}/lib" )
vista_add_pathscript_dynamic_lib_path( "${CMAKE_BINARY_DIR}/lib/DriverPlugins" )


if( VISTACORELIBS_BUILD_BASE )

	vista_configure_test( VistaMathBasicsTest VistaMathBasicsTest.cpp
												../VistaTestingUtils/VistaTestingCompare.h )
	target_link_libraries( VistaMathBasicsTest
		VistaBase
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	add_dependencies( VistaMathBasicsTest VistaBase )
	
	vista_configure_test( VistaVector3DTest VistaVector3DTest.cpp
												../VistaTestingUtils/VistaTestingCompare.h )
	target_link_libraries( VistaVector3DTest
		VistaBase
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	add_dependencies( VistaVector3DTest VistaBase )
	
	#vista_configure_test( VistaQuaternionTest VistaQuaternionTest.cpp
	#											../VistaTestingUtils/VistaTestingCompare.h
	#											VistaTestRotations.h )
	#target_link_libraries( VistaQuaternionTest
	#	VistaBase
	#	${VISTA_USE_PACKAGE_LIBRARIES}
	#)
	#
	#vista_configure_test( VistaTransformMatrixTest VistaTransformMatrixTest.cpp
	#											../VistaTestingUtils/VistaTestingCompare.h
	#											VistaTestRotations.h )
	#target_link_libraries( VistaTransformMatrixTest
	#	VistaBase
	#	${VISTA_USE_PACKAGE_LIBRARIES}
	#)
	
	#vista_configure_test( VistaReferenceFrameTest VistaReferenceFrameTest.cpp
	#											../VistaTestingUtils/VistaTestingCompare.h )
	#target_link_libraries( VistaReferenceFrameTest
	#	VistaBase
	#	${VISTA_USE_PACKAGE_LIBRARIES}
	#)
endif()

if( VISTACORELIBS_BUILD_ASPECTS )
	vista_configure_test( VistaGenericFactoryTest VistaGenericFactoryTest.cpp )
	target_link_libraries( VistaGenericFactoryTest
		VistaBase
		VistaAspects
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
endif()

if( VISTACORELIBS_BUILD_DEVICEDRIVERSBASE )
		vista_configure_test( VistaSensorMeasureTest
											VistaSensorMeasureTest.cpp
											VistaTestDriver.h
											VistaTestDriver.cpp )
	target_link_libraries( VistaSensorMeasureTest
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaTools
		VistaDeviceDriversBase
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
endif()

if( VISTACORELIBS_BUILD_DATAFLOWNET )
		vista_configure_test( VistaHistoryProjectNodeTest
											VistaHistoryProjectNodeTest.cpp
											VistaTestDriver.h
											VistaTestDriver.cpp )
	target_link_libraries( VistaHistoryProjectNodeTest
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaTools
		VistaDeviceDriversBase
		VistaDataFlowNet
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
endif()

if( VISTACORELIBS_BUILD_TOOLS )

	vista_configure_test( VistaBase64Test VistaBase64Test.cpp
											../VistaTestingUtils/VistaTestingCompare.h
											../VistaTestingUtils/VistaTestingRandom.h )
	target_link_libraries( VistaBase64Test
		VistaBase
		VistaTools
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	
	vista_configure_test( VistaReflectionableTest VistaReflectionableTest.cpp
											 ../VistaTestingUtils/VistaTestingCompare.h
											../VistaTestingUtils/VistaTestingRandom.h
											../VistaTestingUtils/VistaTestingDummyStruct.h )
	target_link_libraries( VistaReflectionableTest
		VistaBase
		VistaAspects
		VistaTools
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	
	vista_configure_test( VistaConversionTest VistaConversionTest.cpp 
											../VistaTestingUtils/VistaTestingCompare.h
											../VistaTestingUtils/VistaTestingRandom.h )
	target_link_libraries( VistaConversionTest
		VistaBase
		VistaAspects
		VistaTools
		${VISTA_USE_PACKAGE_LIBRARIES}
	)

	vista_configure_test( VistaSerializablesTest VistaSerializablesTest.cpp
											../VistaTestingUtils/VistaTestingCompare.h
											../VistaTestingUtils/VistaTestingRandom.h )
	target_link_libraries( VistaSerializablesTest
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaTools
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
	
	vista_configure_test( VistaTestingUtilsTest VistaTestingUtilsTest.cpp
											../VistaTestingUtils/VistaTestingCompare.h
											../VistaTestingUtils/VistaTestingRandom.h
											../VistaTestingUtils/VistaTestingDummyStruct.h )
	target_link_libraries( VistaTestingUtilsTest
		VistaBase
		VistaAspects
		VistaInterProcComm
		VistaTools
		${VISTA_USE_PACKAGE_LIBRARIES}
	)
		
endif()


