

set( RelativeDir "./InteractionManager/DfnNodes" )
set( RelativeSourceGroup "Source Files\\InteractionManager\\DfnNodes" )

set( DirFiles
	VistaDfn3DMouseTransformNode.cpp
	VistaDfn3DMouseTransformNode.h
	VistaDfnClusterNodeInfoNode.cpp
	VistaDfnClusterNodeInfoNode.h
	VistaDfnProximityWarningNode.cpp
	VistaDfnProximityWarningNode.h
	VistaDfnDeviceDebugNode.cpp
	VistaDfnDeviceDebugNode.h
	VistaDfnEventSourceNode.cpp
	VistaDfnEventSourceNode.h
	VistaDfnFadeoutNode.cpp
	VistaDfnFadeoutNode.h
	VistaDfnGeometryNode.cpp
	VistaDfnGeometryNode.h
	VistaDfnMouseWheelChangeDetectNode.cpp
	VistaDfnMouseWheelChangeDetectNode.h
	VistaDfnNavigationNode.cpp
	VistaDfnNavigationNode.h
	VistaDfnPortChangeHandlerBase.cpp
	VistaDfnPortChangeHandlerBase.h
	VistaDfnProjectionSourceNode.cpp
	VistaDfnProjectionSourceNode.h
	VistaDfnReferenceFrameTransformNode.cpp
	VistaDfnReferenceFrameTransformNode.h
	VistaDfnSensorFrameNode.cpp
	VistaDfnSensorFrameNode.h
	VistaDfnSystemTriggerControlNode.cpp
	VistaDfnSystemTriggerControlNode.h
	VistaDfnTextOverlayNode.h
	VistaDfnTrackballNode.cpp
	VistaDfnTrackballNode.h
	VistaDfnSimpleTextNode.cpp
	VistaDfnSimpleTextNode.h
	VistaDfnViewerSinkNode.cpp
	VistaDfnViewerSinkNode.h
	VistaDfnViewerSourceNode.cpp
	VistaDfnViewerSourceNode.h
	VistaDfnViewportSourceNode.cpp
	VistaDfnViewportSourceNode.h
	VistaDfnWindowSourceNode.cpp
	VistaDfnWindowSourceNode.h
	VistaKernelDfnNodeCreators.cpp
	VistaKernelDfnNodeCreators.h
	VistaDfnCropViewportNode.cpp
	VistaDfnCropViewportNode.h
	VistaDfnKeyCallbackNode.cpp
	VistaDfnKeyCallbackNode.h
	VistaDfnKeyStateNode.cpp
	VistaDfnKeyStateNode.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

