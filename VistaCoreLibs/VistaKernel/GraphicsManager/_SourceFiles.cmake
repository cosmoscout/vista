

set( RelativeDir "./GraphicsManager" )
set( RelativeSourceGroup "Source Files\\GraphicsManager" )

set( DirFiles
	Vista3DText.cpp
	Vista3DText.h
	VistaExtensionNode.cpp
	VistaExtensionNode.h
	VistaGeomNode.cpp
	VistaGeomNode.h
	VistaGeometry.cpp
	VistaGeometry.h
	VistaGeometryFactory.cpp
	VistaGeometryFactory.h
	VistaGraphicsBridge.cpp
	VistaGraphicsBridge.h
	VistaGraphicsManager.cpp
	VistaGraphicsManager.h
	VistaGroupNode.cpp
	VistaGroupNode.h
	VistaLODNode.cpp
	VistaLODNode.h
	VistaLeafNode.cpp
	VistaLeafNode.h
	VistaLightNode.cpp
	VistaLightNode.h
	VistaNode.cpp
	VistaNode.h
	VistaImage.cpp
	VistaImage.h
	VistaGLTexture.cpp
	VistaGLTexture.h
	VistaImageAndTextureFactory.cpp
	VistaImageAndTextureFactory.h
	VistaNativeGLImageAndTextureFactory.cpp
	VistaNativeGLImageAndTextureFactory.h
	VistaNodeBridge.cpp
	VistaNodeBridge.h
	VistaNodeInterface.h
	VistaOpenGLDraw.cpp
	VistaOpenGLDraw.h
	VistaOpenGLNode.cpp
	VistaOpenGLNode.h
	VistaSceneGraph.cpp
	VistaSceneGraph.h
	VistaSwitchNode.cpp
	VistaSwitchNode.h
	VistaTextNode.cpp
	VistaTextNode.h
	VistaTransformNode.cpp
	VistaTransformNode.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

