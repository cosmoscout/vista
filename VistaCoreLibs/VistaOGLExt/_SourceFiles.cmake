

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )
set( SubDirs Rendering shaders )

set( DirFiles
	VistaBufferObject.cpp
	VistaBufferObject.h
	VistaFramebufferObj.cpp
	VistaFramebufferObj.h
	VistaGLSLShader.cpp
	VistaGLSLShader.h
	VistaOGLExtConfig.h
	VistaOGLUtils.cpp
	VistaOGLUtils.h
	VistaRenderToVertexArray.cpp
	VistaRenderToVertexArray.h
	VistaRenderbuffer.cpp
	VistaRenderbuffer.h
	VistaShaderRegistry.cpp
	VistaShaderRegistry.h
	VistaTexture.cpp
	VistaTexture.h
	VistaTransformableTexture.cpp
	VistaTransformableTexture.h
	VistaGLLine.cpp
	VistaGLLine.h
	VistaGLPoints.cpp
	VistaGLPoints.h
	VistaVertexArrayObject.cpp
	VistaVertexArrayObject.h	
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
