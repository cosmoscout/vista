

set( RelativeDir "./Drivers" )
set( RelativeSourceGroup "Source Files\\Drivers" )

set( DirFiles
	VistaKeyboardDriver.cpp
	VistaKeyboardDriver.h
	VistaMouseDriver.cpp
	VistaMouseDriver.h
	VistaShallowDriver.cpp
	VistaShallowDriver.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

