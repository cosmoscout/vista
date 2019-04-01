

set( RelativeDir "./AsyncIO" )
set( RelativeSourceGroup "Source Files\\AsyncIO" )

set( DirFiles
	VistaAsyncEventObserver.cpp
	VistaAsyncEventObserver.h
	VistaIOHandleBasedMultiplexer.cpp
	VistaIOHandleBasedMultiplexer.h
	VistaIOMultiplexer.cpp
	VistaIOMultiplexer.h
	VistaIOMultiplexerIP.cpp
	VistaIOMultiplexerIP.h
	VistaIORequest.cpp
	VistaIORequest.h
	VistaIOScheduler.cpp
	VistaIOScheduler.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

