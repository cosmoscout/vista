

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )
set( SubDirs Half )

set( DirFiles
	VistaAtomicCounter.cpp
	VistaAtomicCounter.h
	VistaAutoBuffer.cpp
	VistaAutoBuffer.h
	VistaAutoBufferPool.cpp
	VistaAutoBufferPool.h
	VistaBaseConfig.h
	VistaBaseMain.cpp
	VistaBaseTypes.h
	VistaColor.cpp
	VistaColor.h
	VistaDefaultTimerImp.cpp
	VistaDefaultTimerImp.h
	VistaExceptionBase.cpp
	VistaExceptionBase.h
	VistaMathBasics.h
	VistaUtilityMacros.h
	VistaQuaternion.cpp
	VistaQuaternion.h
	VistaReferenceFrame.cpp
	VistaReferenceFrame.h
	VistaStreamManager.cpp
	VistaStreamManager.h
	VistaStreamUtils.cpp
	VistaStreamUtils.h
	VistaTimeUtils.cpp
	VistaTimeUtils.h
	VistaTimer.cpp
	VistaTimer.h
	VistaTimerImp.cpp
	VistaTimerImp.h
	VistaTransformMatrix.cpp
	VistaTransformMatrix.h
	VistaVector3D.h
	VistaVectorMath.h
	VistaVersion.cpp
	VistaVersion.h
	
	VistaSerializingToolset.cpp
	VistaSerializingToolset.h
		
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

