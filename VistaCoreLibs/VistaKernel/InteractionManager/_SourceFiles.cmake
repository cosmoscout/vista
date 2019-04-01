

set( RelativeDir "./InteractionManager" )
set( RelativeSourceGroup "Source Files\\InteractionManager" )
set( SubDirs DfnNodes )

set( DirFiles
	VistaIntentionSelect.cpp
	VistaIntentionSelect.h
	VistaInteractionContext.cpp
	VistaInteractionContext.h
	VistaInteractionEvent.cpp
	VistaInteractionEvent.h
	VistaInteractionManager.cpp
	VistaInteractionManager.h
	VistaKeyboardSystemControl.cpp
	VistaKeyboardSystemControl.h
	VistaObjectSetMove.cpp
	VistaObjectSetMove.h
	VistaUserPlatform.cpp
	VistaUserPlatform.h
	VistaVirtualPlatformAdapter.cpp
	VistaVirtualPlatformAdapter.h
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

