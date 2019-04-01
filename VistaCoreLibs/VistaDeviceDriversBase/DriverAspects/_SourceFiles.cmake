

set( RelativeDir "./DriverAspects" )
set( RelativeSourceGroup "Source Files\\DriverAspects" )

set( DirFiles
	VistaDeviceDriverAspectRegistry.cpp
	VistaDeviceDriverAspectRegistry.h
	VistaDeviceIdentificationAspect.cpp
	VistaDeviceIdentificationAspect.h
	VistaDriverAbstractWindowAspect.cpp
	VistaDriverAbstractWindowAspect.h
	VistaDriverConnectionAspect.cpp
	VistaDriverConnectionAspect.h
	VistaDriverForceFeedbackAspect.cpp
	VistaDriverForceFeedbackAspect.h
	VistaDriverGenericParameterAspect.cpp
	VistaDriverGenericParameterAspect.h
	VistaDriverInfoAspect.cpp
	VistaDriverInfoAspect.h
	VistaDriverLoggingAspect.cpp
	VistaDriverLoggingAspect.h
	VistaDriverMeasureHistoryAspect.cpp
	VistaDriverMeasureHistoryAspect.h
	VistaDriverProtocolAspect.cpp
	VistaDriverProtocolAspect.h
	VistaDriverReferenceFrameAspect.cpp
	VistaDriverReferenceFrameAspect.h
	VistaDriverSensorMappingAspect.cpp
	VistaDriverSensorMappingAspect.h
	VistaDriverThreadAspect.cpp
	VistaDriverThreadAspect.h
	VistaDriverWorkspaceAspect.cpp
	VistaDriverWorkspaceAspect.h
	VistaDriverEnableAspect.h
	VistaDriverEnableAspect.cpp
	
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

