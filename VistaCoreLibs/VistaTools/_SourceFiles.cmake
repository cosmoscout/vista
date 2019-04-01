

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )
set( SubDirs tinyXML )

set( DirFiles
	EncodeBinary.h
	VistaBase64.cpp
	VistaBase64.h
	VistaBasicProfiler.cpp
	VistaBasicProfiler.h
	VistaCPUInfo.cpp
	VistaCPUInfo.h
	VistaCRC32.cpp
	VistaCRC32.h
	VistaDLL.cpp
	VistaDLL.h
	VistaEnvironment.cpp
	VistaEnvironment.h
	VistaFastMeshAdaptor.cpp
	VistaFastMeshAdaptor.h
	VistaFileDataSet.cpp
	VistaFileDataSet.h
	VistaFileSystemDirectory.cpp
	VistaFileSystemDirectory.h
	VistaFileSystemFile.cpp
	VistaFileSystemFile.h
	VistaFileSystemNode.cpp
	VistaFileSystemNode.h
	VistaIniFileParser.cpp
	VistaIniFileParser.h
	VistaHalfedgeAdaptor.cpp
	VistaHalfedgeAdaptor.h
	VistaMemoryInfo.cpp
	VistaMemoryInfo.h
	VistaOctree.cpp
	VistaOctree.h
	VistaProfiler.cpp
	VistaProfiler.h
	VistaProgressBar.cpp
	VistaProgressBar.h
	VistaProtocol.cpp
	VistaProtocol.h
	VistaRandomNumberGenerator.cpp
	VistaRandomNumberGenerator.h
	VistaRingBuffer.h
	VistaStackWalker.cpp
	VistaStackWalker.h
	VistaStreams.cpp
	VistaStreams.h
	VistaToolsConfig.h
	VistaToolsStd.h
	VistaTopologyGraph.h
	VistaVoxelOctree.h
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

