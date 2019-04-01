

set( RelativeDir "./Cluster/Imps" )
set( RelativeSourceGroup "Source Files\\Cluster\\Imps" )

set( DirFiles
	VistaBroadcastClusterBarrier.cpp
	VistaBroadcastClusterBarrier.h
	VistaBroadcastClusterDataSync.cpp
	VistaBroadcastClusterDataSync.h
	VistaClusterBarrierIPBase.cpp
	VistaClusterBarrierIPBase.h
	VistaMulticastClusterBarrier.cpp
	VistaMulticastClusterBarrier.h
	VistaInterProcClusterBarrier.cpp
	VistaInterProcClusterBarrier.h
	VistaInterProcClusterDataSync.cpp
	VistaInterProcClusterDataSync.h
	VistaClusterBytebufferDataSyncBase.cpp
	VistaClusterBytebufferDataSyncBase.h
	VistaDummyClusterBarrier.cpp
	VistaDummyClusterBarrier.h
	VistaDummyClusterDataSync.cpp
	VistaDummyClusterDataSync.h
	VistaDummyClusterDataCollect.cpp
	VistaDummyClusterDataCollect.h
	VistaRecordReplayClusterDataSync.cpp
	VistaRecordReplayClusterDataSync.h
	VistaTCPIPClusterBarrier.cpp
	VistaTCPIPClusterBarrier.h
	VistaTCPIPClusterDataSync.cpp
	VistaTCPIPClusterDataSync.h
	VistaTCPIPClusterDataCollect.cpp
	VistaTCPIPClusterDataCollect.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

