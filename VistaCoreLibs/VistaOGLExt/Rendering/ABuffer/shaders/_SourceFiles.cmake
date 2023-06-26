

set( RelativeDir "./Rendering/ABuffer/shaders" )
set( RelativeSourceGroup "Shader Files\\Rendering\\ABuffer" )

set( DirFiles
	VistaABuffer_Array_aux.glsl
	VistaABuffer_Array_frag.glsl
	VistaABuffer_LinktList_aux.glsl
	VistaABuffer_LinktList_frag.glsl
	VistaABuffer_Pages_aux.glsl
	VistaABuffer_Pages_frag.glsl
	VistaABufferOIT_aux.glsl
	VistaABufferOIT_Compressed_aux.glsl
	VistaABufferOIT_Compressed_frag.glsl
	VistaABufferOIT_Default_frag.glsl
	VistaABufferOIT_Default_vert.glsl
	VistaABufferOIT_Display_frag.glsl
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

