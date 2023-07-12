

set( RelativeDir "./Half" )
set( RelativeSourceGroup "Source Files\\Half" )

set( DirFiles
	VistaHalf.cpp
	VistaHalf.h
	VistaHalfFunction.h
	VistaHalfLimits.h
	eLut.h
	toFloat.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

