

set( RelativeDir "./IPNet" )
set( RelativeSourceGroup "Source Files\\IPNet" )

set( DirFiles
	VistaAcceptor.cpp
	VistaAcceptor.h
	VistaIPAddress.cpp
	VistaIPAddress.h
	VistaIPComm.cpp
	VistaIPComm.h
	VistaMcastIPAddress.cpp
	VistaMcastIPAddress.h
	VistaMcastSocket.cpp
	VistaMcastSocket.h
	VistaMcastSocketAddress.cpp
	VistaMcastSocketAddress.h
	VistaNetworkInfo.cpp
	VistaNetworkInfo.h
	VistaServiceIndicator.cpp
	VistaServiceIndicator.h
	VistaSocket.cpp
	VistaSocket.h
	VistaSocketAddress.cpp
	VistaSocketAddress.h
	VistaTCPClientSocket.cpp
	VistaTCPClientSocket.h
	VistaTCPServer.cpp
	VistaTCPServer.h
	VistaTCPServerSocket.cpp
	VistaTCPServerSocket.h
	VistaTCPSocket.cpp
	VistaTCPSocket.h
	VistaUDPSocket.cpp
	VistaUDPSocket.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

