

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	VistaKernelOpenSGExtConfig.h
	VistaOpenSGMaterialTools.cpp
	VistaOpenSGMaterialTools.h
	VistaOpenSGGraphicsStatsOverlay.cpp
	VistaOpenSGGraphicsStatsOverlay.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

