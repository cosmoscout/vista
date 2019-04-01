

set( RelativeDir "./Cluster" )
set( RelativeSourceGroup "Source Files\\Cluster" )
set( SubDirs Imps )

set( DirFiles
	VistaClusterBarrier.cpp
	VistaClusterBarrier.h
	VistaClusterDataSync.cpp
	VistaClusterDataSync.h
	VistaClusterDataCollect.cpp
	VistaClusterDataCollect.h
	VistaClusterSyncEntity.cpp
	VistaClusterSyncEntity.h
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

