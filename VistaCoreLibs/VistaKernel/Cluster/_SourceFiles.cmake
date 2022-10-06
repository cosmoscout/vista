

set( RelativeDir "./Cluster" )
set( RelativeSourceGroup "Source Files\\Cluster" )
set( SubDirs Utils )

if( VISTACORELIBS_USE_ZEROMQ )
	list( APPEND SubDirs ZeroMQExt )
endif()

set( DirFiles
	VistaClusterMaster.cpp
	VistaClusterMaster.h
	VistaClusterMode.cpp
	VistaClusterMode.h
	VistaClusterSlave.cpp
	VistaClusterSlave.h
	VistaClusterStandalone.cpp
	VistaClusterStandalone.h
	VistaDataTunnel.cpp
	VistaDataTunnel.h
	VistaNewClusterMaster.cpp
	VistaNewClusterMaster.h
	VistaNewClusterSlave.cpp
	VistaNewClusterSlave.h
	VistaReplaySlave.cpp
	VistaReplaySlave.h
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

