

set( RelativeDir "./DisplayManager" )
set( RelativeSourceGroup "Source Files\\DisplayManager" )
set( SubDirs )

if( VISTACORELIBS_BUILD_WINDOWIMP_GLUT )
	list( APPEND SubDirs GlutWindowImp )
	if( VISTACORELIBS_USE_OCULUSSDK )
		list( APPEND SubDirs OculusGlutWindowImp )
	endif()
	if( VISTACORELIBS_USE_VIVE )
		list( APPEND SubDirs ViveGlutWindowImp )
	endif()
endif()
if( VISTACORELIBS_BUILD_WINDOWIMP_OSG )
	list( APPEND SubDirs OpenSceneGraphWindowImp )
endif()

set( DirFiles	
	Vista2DDrawingObjects.cpp
	Vista2DDrawingObjects.h
	VistaDisplay.cpp
	VistaDisplay.h
	VistaDisplayBridge.cpp
	VistaDisplayBridge.h
	VistaDisplayEntity.cpp
	VistaDisplayEntity.h
	VistaDisplayManager.cpp
	VistaDisplayManager.h
	VistaDisplaySystem.cpp
	VistaDisplaySystem.h
	VistaColorOverlay.cpp
	VistaColorOverlay.h
	VistaProjection.cpp
	VistaProjection.h
	VistaSceneOverlay.cpp
	VistaSceneOverlay.h
	VistaSimpleTextOverlay.cpp
	VistaSimpleTextOverlay.h
	VistaTextEntity.h
	VistaViewport.cpp
	VistaViewport.h
	VistaViewportResizeToProjectionAdapter.cpp
	VistaViewportResizeToProjectionAdapter.h
	VistaVirtualPlatform.cpp
	VistaVirtualPlatform.h
	VistaWindow.cpp
	VistaWindow.h
	VistaWindowingToolkit.h
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

