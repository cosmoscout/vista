

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )
set( SubDirs Cluster DisplayManager EventManager GraphicsManager InteractionManager OpenSG Stuff )

set( DirFiles
	VistaDriverPropertyConfigurator.cpp
	VistaDriverPropertyConfigurator.h
	VistaFrameLoop.cpp
	VistaFrameLoop.h
	VistaKernelConfig.h
	VistaKernelMsgPort.cpp
	VistaKernelMsgPort.h
	VistaKernelMsgTab.h
	VistaSystem.cpp
	VistaSystem.h
	VistaSystemClassFactory.h
	VistaSystemCommands.cpp
	VistaSystemCommands.h
	VistaSystemConfigurators.cpp
	VistaSystemConfigurators.h
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

