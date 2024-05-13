

set( RelativeDir "./DisplayManager/SDL2WindowImp" )
set( RelativeSourceGroup "Source Files\\DisplayManager\\SDL2WindowImp" )

set( DirFiles
	VistaSDL2TextEntity.cpp
	VistaSDL2TextEntity.h
	VistaSDL2WindowingToolkit.cpp
	VistaSDL2WindowingToolkit.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

