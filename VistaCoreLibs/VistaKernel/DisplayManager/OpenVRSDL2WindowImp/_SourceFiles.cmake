# $Id: _SourceFiles.cmake 23946 2011-10-29 16:52:36Z dr165799 $

set( RelativeDir "./DisplayManager/OpenVRSDL2WindowImp" )
set( RelativeSourceGroup "Source Files\\DisplayManager\\OpenVRSDL2WindowImp" )

set( DirFiles
	VistaOpenVRSDL2WindowingToolkit.cpp
	VistaOpenVRSDL2WindowingToolkit.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

