

set( RelativeDir "./Cluster/ZeroMQExt" )
set( RelativeSourceGroup "Source Files\\Cluster\\ZeroMQExt" )

set( DirFiles
	VistaZeroMQClusterBarrier.cpp
	VistaZeroMQClusterBarrier.h
	VistaZeroMQClusterDataSync.cpp
	VistaZeroMQClusterDataSync.h
	VistaZeroMQCommon.cpp
	VistaZeroMQCommon.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

