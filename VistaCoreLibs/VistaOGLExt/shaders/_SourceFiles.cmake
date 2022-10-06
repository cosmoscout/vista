

set( RelativeDir "./shaders" )
set( RelativeSourceGroup "Shaders" )

set( DirFiles
	Vista_GoochShading_aux.glsl
	Vista_PhongLighting_aux.glsl
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

