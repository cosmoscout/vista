

set( RelativeDir "./OpenSG" )
set( RelativeSourceGroup "Source Files\\OpenSG" )

set( DirFiles
	OSGVistaOpenGLDrawCore.cpp
	OSGVistaOpenGLDrawCore.h
	OSGVistaOpenGLDrawCore.inl
	OSGVistaOpenGLDrawCoreBase.cpp
	OSGVistaOpenGLDrawCoreBase.h
	OSGVistaOpenGLDrawCoreBase.inl
	OSGVistaOpenGLDrawCoreFields.h
	VistaOpenSGDisplayBridge.cpp
	VistaOpenSGDisplayBridge.h
	VistaOpenSGGLOverlayForeground.cpp
	VistaOpenSGGLOverlayForeground.h
	VistaOpenSGGLOverlayForeground.inl
	VistaOpenSGGLOverlayForegroundBase.cpp
	VistaOpenSGGLOverlayForegroundBase.h
	VistaOpenSGGLOverlayForegroundBase.inl
	VistaOpenSGGLOverlayForegroundFields.h
	VistaOpenSGGraphicsBridge.cpp
	VistaOpenSGGraphicsBridge.h
	VistaOpenSGNodeBridge.cpp
	VistaOpenSGNodeBridge.h
	VistaOpenSGSystemClassFactory.cpp
	VistaOpenSGSystemClassFactory.h
	VistaOpenSGTextForeground.cpp
	VistaOpenSGTextForeground.h
	VistaOpenSGTextForeground.inl
	VistaOpenSGTextForegroundBase.cpp
	VistaOpenSGTextForegroundBase.h
	VistaOpenSGTextForegroundBase.inl
	VistaOpenSGTextForegroundFields.h
	VistaOpenSGThreadImp.cpp
	VistaOpenSGThreadImp.h
	VistaOpenSGViewportShaders.cpp
	VistaOpenSGViewportShaders.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

