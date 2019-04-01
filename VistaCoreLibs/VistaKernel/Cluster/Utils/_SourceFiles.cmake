

set( RelativeDir "./Cluster/Utils" )
set( RelativeSourceGroup "Source Files\\Cluster\\Utils" )

set( DirFiles
	VistaGSyncSwapBarrier.cpp
	VistaGSyncSwapBarrier.h
	VistaMasterDataTunnel.cpp
	VistaMasterDataTunnel.h
	VistaMasterSlaveUtils.cpp
	VistaMasterSlaveUtils.h
	VistaSlaveDataTunnel.cpp
	VistaSlaveDataTunnel.h
	VistaStandaloneDataTunnel.cpp
	VistaStandaloneDataTunnel.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

