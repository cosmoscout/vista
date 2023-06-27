

set( RelativeDir "./Rendering/ABuffer" )
set( RelativeSourceGroup "Source Files\\Rendering\\ABuffer" )

set( DirFiles
	VistaABuffer.cpp
	VistaABuffer.h
	VistaABuffer_Array.cpp
	VistaABuffer_Array.h
	VistaABuffer_LinkedList.cpp
	VistaABuffer_LinkedList.h
	VistaABuffer_Pages.cpp
	VistaABuffer_Pages.h
	VistaABufferOIT.cpp
	VistaABufferOIT.h	
	VistaABufferCSG.cpp
	VistaABufferCSG.h	
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )