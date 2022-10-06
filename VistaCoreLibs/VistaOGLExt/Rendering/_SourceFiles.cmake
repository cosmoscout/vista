

set( RelativeDir "./Rendering" )
set( RelativeSourceGroup "Source Files\\Rendering" )

set( DirFiles
	VistaGeometryData.cpp
	VistaGeometryData.h
	VistaGeometryRenderingCore.cpp
	VistaGeometryRenderingCore.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )
