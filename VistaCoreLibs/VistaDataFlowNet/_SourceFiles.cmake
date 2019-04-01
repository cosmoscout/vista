

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	Vdfn3DNormalizeNode.cpp
	Vdfn3DNormalizeNode.h
	VdfnAbsoluteNode.h
	VdfnActionNode.cpp
	VdfnActionNode.h
	VdfnActionObject.cpp
	VdfnActionObject.h
	VdfnAggregateNode.h
	VdfnApplyTransformNode.cpp
	VdfnApplyTransformNode.h
	VdfnAxisRotateNode.cpp
	VdfnAxisRotateNode.h
	VdfnBinaryOpNode.h
	VdfnBinaryOps.h
	VdfnChangeDetectNode.h
	VdfnCompose3DVectorNode.cpp
	VdfnCompose3DVectorNode.h
	VdfnComposeQuaternionFromDirectionsNode.cpp
	VdfnComposeQuaternionFromDirectionsNode.h
	VdfnComposeQuaternionNode.cpp
	VdfnComposeQuaternionNode.h
	VdfnComposeTransformMatrixNode.cpp
	VdfnComposeTransformMatrixNode.h
	VdfnCompositeNode.cpp
	VdfnCompositeNode.h
	VdfnConditionalRouteNode.cpp
	VdfnConditionalRouteNode.h
	VdfnConfig.h
	VdfnConstantValueNode.h
	VdfnCounterNode.h
	VdfnDecompose3DVectorNode.cpp
	VdfnDecompose3DVectorNode.h
	VdfnDecomposeQuaternionNode.cpp
	VdfnDecomposeQuaternionNode.h
	VdfnDecomposeTransformMatrixNode.cpp
	VdfnDecomposeTransformMatrixNode.h
	VdfnDelayNode.h
	VdfnDemultiplexNode.h
	VdfnDifferenceNode.h
	VdfnDriverSensorNode.cpp
	VdfnDriverSensorNode.h
	VdfnDumpHistoryNode.cpp
	VdfnDumpHistoryNode.h
	VdfnForceFeedbackNode.cpp
	VdfnForceFeedbackNode.h
	VdfnGetElementNode.h
	VdfnGetTransformNode.cpp
	VdfnGetTransformNode.h
	VdfnGraph.cpp
	VdfnGraph.h
	VdfnHistoryPort.cpp
	VdfnHistoryPort.h
	VdfnHistoryProjectNode.cpp
	VdfnHistoryProjectNode.h
	VdfnInvertNode.cpp
	VdfnInvertNode.h
	VdfnLatestUpdateNode.h
	VdfnLoggerNode.cpp
	VdfnLoggerNode.h
	VdfnMatrixComposeNode.cpp
	VdfnMatrixComposeNode.h
	VdfnModuloCounterNode.h
	VdfnMultiplexNode.h
	VdfnNegateNode.h
	VdfnNode.cpp
	VdfnNode.h
	VdfnNodeCreators.cpp
	VdfnNodeCreators.h
	VdfnNodeFactory.cpp
	VdfnNodeFactory.h
	VdfnObjectRegistry.cpp
	VdfnObjectRegistry.h
	VdfnOutstreamNode.cpp
	VdfnOutstreamNode.h
	VdfnPersistence.cpp
	VdfnPersistence.h
	VdfnPort.cpp
	VdfnPort.h
	VdfnPortFactory.cpp
	VdfnPortFactory.h
	VdfnProjectVectorNode.cpp
	VdfnProjectVectorNode.h
	VdfnQuaternionSlerpNode.cpp
	VdfnQuaternionSlerpNode.h
	VdfnRangeCheckNode.h
	VdfnReEvalNode.cpp
	VdfnReEvalNode.h
	VdfnReadWorkspaceNode.cpp
	VdfnReadWorkspaceNode.h
	VdfnSamplerNode.cpp
	VdfnSamplerNode.h
	VdfnSerializer.cpp
	VdfnSerializer.h
	VdfnSetTransformNode.cpp
	VdfnSetTransformNode.h
	VdfnShallowNode.cpp
	VdfnShallowNode.h
	VdfnThresholdNode.h
	VdfnTimerNode.cpp
	VdfnTimerNode.h
	VdfnToggleNode.cpp
	VdfnToggleNode.h
	VdfnTrackingRedundancyNode.cpp
	VdfnTrackingRedundancyNode.h
	VdfnTypeConvertNode.h
	VdfnTypeConvertNodeCreator.cpp
	VdfnTypeConvertNodeCreator.h
	VdfnUpdateThresholdNode.cpp
	VdfnUpdateThresholdNode.h
	VdfnUtil.cpp
	VdfnUtil.h
	VdfnValueToTriggerNode.h
	VdfnVariableNode.h
	VdfnVectorDecomposeNode.h

#	VdfnSensorReader.cpp
#	VdfnSensorReader.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

