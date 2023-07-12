

set( RelativeDir "./DataLaVista/Construct" )
set( RelativeSourceGroup "Source Files\\DataLaVista\\Construct" )

set( DirFiles
	VistaCheckError.cpp
	VistaCheckError.h
	VistaNullError.cpp
	VistaNullError.h
	VistaPipeChecker.cpp
	VistaPipeChecker.h
	VistaPipeConstructionKit.cpp
	VistaPipeConstructionKit.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

