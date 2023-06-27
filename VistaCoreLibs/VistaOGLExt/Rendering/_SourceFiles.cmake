

set( RelativeDir "./Rendering" )
set( RelativeSourceGroup "Source Files\\Rendering" )
set( SubDirs ABuffer )

set( DirFiles
	VistaGeometryData.cpp
	VistaGeometryData.h
	VistaGeometryRenderingCore.cpp
	VistaGeometryRenderingCore.h
	VistaParticleRenderingProperties.cpp
	VistaParticleRenderingProperties.h
	VistaParticleRenderingCore.cpp
	VistaParticleRenderingCore.h
	VistaParticleTraceRenderingCore.cpp
	VistaParticleTraceRenderingCore.h
	VistaReferencePlane.cpp
	VistaReferencePlane.h
	VistaVolumeRaycasterCore.cpp
	VistaVolumeRaycasterCore.h
	VistaVolumeRenderingCore.cpp
	VistaVolumeRenderingCore.h
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

