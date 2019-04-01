

set( RelativeDir "./Stuff/ProximityWarning" )
set( RelativeSourceGroup "Source Files\\Stuff\\ProximityWarning" )

set( DirFiles
	VistaProximityFadeout.cpp
	VistaProximityFadeout.h
	VistaProximityWarningBase.cpp
	VistaProximityWarningBase.h
	VistaProximitySign.cpp
	VistaProximitySign.h
	VistaProximityBarrierTape.cpp
	VistaProximityBarrierTape.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

