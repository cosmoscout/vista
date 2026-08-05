find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

function(vista_add_gtest target)
  add_executable(${target} ${ARGN})

  target_link_libraries(${target}
    PRIVATE
      GTest::gtest
      GTest::gtest_main
  )

  # Windows: copy dependent runtime DLLs next to the test executable.
  if (WIN32)
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:${target}>
        $<TARGET_FILE_DIR:${target}>
      COMMAND_EXPAND_LISTS
    )
  endif ()

  # Linux/macOS: ensure loader can resolve shared libs from nearby folders.
  if (UNIX)
    set_target_properties(${target} PROPERTIES
        BUILD_RPATH_USE_ORIGIN ON
        BUILD_RPATH "$ORIGIN;$ORIGIN/..;$ORIGIN/../..;$ORIGIN/../../.."
        INSTALL_RPATH "$ORIGIN;$ORIGIN/..;$ORIGIN/../..;$ORIGIN/../../.."
    )
  endif ()

  # Discover tests at test time (not during build/link).
  gtest_discover_tests(${target}
      DISCOVERY_MODE PRE_TEST
  )
endfunction()

vista_add_gtest(VistaMathBasicsTest
    VistaMathBasicsTest.cpp
    ../VistaTestingUtils/VistaTestingCompare.h
)
target_link_libraries(VistaMathBasicsTest
  PRIVATE
    Vista::Base
)

vista_add_gtest(VistaVector3DTest
    VistaVector3DTest.cpp
    ../VistaTestingUtils/VistaTestingCompare.h
)
target_link_libraries(VistaVector3DTest
  PRIVATE
    Vista::Base
)

vista_add_gtest(VistaGenericFactoryTest
    VistaGenericFactoryTest.cpp
)
target_link_libraries(VistaGenericFactoryTest
  PRIVATE
    Vista::Base
    Vista::Aspects
)

vista_add_gtest(VistaSensorMeasureTest
    VistaSensorMeasureTest.cpp
    VistaTestDriver.h
    VistaTestDriver.cpp
)
target_link_libraries(VistaSensorMeasureTest
  PRIVATE
    Vista::Base
    Vista::Aspects
    Vista::InterProcComm
    Vista::Tools
    Vista::DeviceDriversBase
)

vista_add_gtest(VistaHistoryProjectNodeTest
    VistaHistoryProjectNodeTest.cpp
    VistaTestDriver.h
    VistaTestDriver.cpp
)
target_link_libraries(VistaHistoryProjectNodeTest
  PRIVATE
    Vista::Base
    Vista::Aspects
    Vista::InterProcComm
    Vista::Tools
    Vista::DeviceDriversBase
    Vista::DataFlowNet
)

vista_add_gtest(VistaBase64Test
    VistaBase64Test.cpp
    ../VistaTestingUtils/VistaTestingCompare.h
    ../VistaTestingUtils/VistaTestingRandom.h
)
target_link_libraries(VistaBase64Test
  PRIVATE
    Vista::Base
    Vista::Tools
)

vista_add_gtest(VistaReflectionableTest
    VistaReflectionableTest.cpp
    ../VistaTestingUtils/VistaTestingCompare.h
    ../VistaTestingUtils/VistaTestingRandom.h
    ../VistaTestingUtils/VistaTestingDummyStruct.h
)
target_link_libraries(VistaReflectionableTest
  PRIVATE
    Vista::Base
    Vista::Aspects
    Vista::Tools
)

vista_add_gtest(VistaConversionTest
    VistaConversionTest.cpp
    ../VistaTestingUtils/VistaTestingCompare.h
    ../VistaTestingUtils/VistaTestingRandom.h
)
target_link_libraries(VistaConversionTest
  PRIVATE
    Vista::Base
    Vista::Aspects
    Vista::Tools
)

vista_add_gtest(VistaSerializablesTest
    VistaSerializablesTest.cpp
    ../VistaTestingUtils/VistaTestingCompare.h
    ../VistaTestingUtils/VistaTestingRandom.h
)
target_link_libraries(VistaSerializablesTest
  PRIVATE
    Vista::Base
    Vista::Aspects
    Vista::InterProcComm
    Vista::Tools
)

vista_add_gtest(VistaTestingUtilsTest
    VistaTestingUtilsTest.cpp
    ../VistaTestingUtils/VistaTestingCompare.h
    ../VistaTestingUtils/VistaTestingRandom.h
    ../VistaTestingUtils/VistaTestingDummyStruct.h
)
target_link_libraries(VistaTestingUtilsTest
  PRIVATE
    Vista::Base
    Vista::Aspects
    Vista::InterProcComm
    Vista::Tools
)
