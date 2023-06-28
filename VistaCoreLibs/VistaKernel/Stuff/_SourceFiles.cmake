

set( RelativeDir "./Stuff" )
set( RelativeSourceGroup "Source Files\\Stuff" )
set( SubDirs ProximityWarning )

set( DirFiles
	VistaAC3DLoader.cpp
	VistaAC3DLoader.h
	VistaFramerateDisplay.cpp
	VistaFramerateDisplay.h
	VistaInteractionHandlerBase.cpp
	VistaInteractionHandlerBase.h
	VistaKernelProfiling.h
	VistaStreamManagerExt.cpp
	VistaStreamManagerExt.h
	VistaVirtualConsole.cpp
	VistaVirtualConsole.h
	VistaEyeTester.cpp
	VistaEyeTester.h
	VistaFrameSeriesCapture.cpp
	VistaFrameSeriesCapture.h
	VistaRuntimeLimiter.cpp
	VistaRuntimeLimiter.h
	VistaAdaptiveVSyncControl.cpp
	VistaAdaptiveVSyncControl.h
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

