

set( RelativeDir "./Connections" )
set( RelativeSourceGroup "Source Files\\Connections" )

set( DirFiles
	VistaByteBufferDeSerializer.cpp
	VistaByteBufferDeSerializer.h
	VistaByteBufferSerializer.cpp
	VistaByteBufferSerializer.h
	VistaCSVDeSerializer.cpp
	VistaCSVDeSerializer.h
	VistaCSVSerializer.cpp
	VistaCSVSerializer.h
	VistaConnection.cpp
	VistaConnection.h
	VistaConnectionFile.cpp
	VistaConnectionFile.h
	VistaConnectionFileTimed.cpp
	VistaConnectionFileTimed.h
	VistaConnectionIP.cpp
	VistaConnectionIP.h
	VistaConnectionNamedPipe.cpp
	VistaConnectionNamedPipe.h
	VistaConnectionPipe.cpp
	VistaConnectionPipe.h
	VistaConnectionPulsed.h
	VistaConnectionPulsed.cpp
	VistaConnectionSerial.cpp
	VistaConnectionSerial.h
	VistaMsg.cpp
	VistaMsg.h
	VistaMsgChannel.cpp
	VistaMsgChannel.h
	VistaProgressMessage.cpp
	VistaProgressMessage.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

