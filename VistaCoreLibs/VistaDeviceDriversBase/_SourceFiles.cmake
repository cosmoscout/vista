

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )
set( SubDirs DriverAspects Drivers )

set( DirFiles
	VistaConnectionUpdater.cpp
	VistaConnectionUpdater.h
	VistaDeviceDriver.cpp
	VistaDeviceDriver.h
	VistaDeviceDriversConfig.h
	VistaDeviceSensor.cpp
	VistaDeviceSensor.h
	VistaDriverManager.cpp
	VistaDriverManager.h
	VistaDriverMap.cpp
	VistaDriverMap.h
	VistaDriverPlugDev.h
	VistaDriverUtils.cpp
	VistaDriverUtils.h
	VistaSensorReadState.cpp
	VistaSensorReadState.h
	VistaSensorReader.h
	VistaSensorReader.cpp
	VistaDriverMain.cpp
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

set( SubDirFiles "" )
foreach( Dir ${SubDirs} )
	list( APPEND SubDirFiles "${RelativeDir}/${Dir}/_SourceFiles.cmake" )
endforeach()

foreach( SubDirFile ${SubDirFiles} )
	include( ${SubDirFile} )
endforeach()

