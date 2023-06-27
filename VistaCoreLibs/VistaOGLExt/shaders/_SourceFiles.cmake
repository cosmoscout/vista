

set( RelativeDir "./shaders" )
set( RelativeSourceGroup "Shaders" )

set( DirFiles
	Vista_GoochShading_aux.glsl
	Vista_PhongLighting_aux.glsl
	VistaGLLine_Adjacency_geom.glsl
	VistaGLLine_Arrow_frag.glsl
	VistaGLLine_Arrow_geom.glsl
	VistaGLLine_Cylinder_frag.glsl
	VistaGLLine_Cylinder_geom.glsl
	VistaGLLine_Default_geom.glsl
	VistaGLLine_Default_vert.glsl
	VistaGLLine_HaloLine_frag.glsl
	VistaGLLine_TruncatedCone_frag.glsl
	VistaGLLine_TruncatedCone_geom.glsl
	VistaGLLine_TruncatedCone_vert.glsl
	VistaGLLine_Tubelets_frag.glsl
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

