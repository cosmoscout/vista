

set( RelativeDir "./DataLaVista/Base" )
set( RelativeSourceGroup "Source Files\\DataLaVista\\Base" )

set( DirFiles
	VistaActiveComponent.cpp
	VistaActiveComponent.h
	VistaActiveDataConsumer.cpp
	VistaActiveDataConsumer.h
	VistaActiveDataProducer.cpp
	VistaActiveDataProducer.h
	VistaActiveFilter.cpp
	VistaActiveFilter.h
	VistaDLVTypes.h
	VistaDataConsumer.cpp
	VistaDataConsumer.h
	VistaDataPacket.cpp
	VistaDataPacket.h
	VistaDataProducer.cpp
	VistaDataProducer.h
	VistaDataSink.cpp
	VistaDataSink.h
	VistaDataSource.cpp
	VistaDataSource.h
	VistaDemux.cpp
	VistaDemux.h
	VistaFilter.cpp
	VistaFilter.h
	VistaInPlaceFilter.cpp
	VistaInPlaceFilter.h
	VistaPacketQueue.cpp
	VistaPacketQueue.h
	VistaPipe.cpp
	VistaPipe.h
	VistaPipeComponent.cpp
	VistaPipeComponent.h
	VistaRTC.cpp
	VistaRTC.h
	VistaRegistration.cpp
	VistaRegistration.h
	VistaRoutingPipe.cpp
	VistaRoutingPipe.h
	VistaTeeFilter.cpp
	VistaTeeFilter.h
	VistaTimeStamp.cpp
	VistaTimeStamp.h
	VistaTransformFilter.cpp
	VistaTransformFilter.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

