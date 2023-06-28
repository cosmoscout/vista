

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	VistaKernelOpenSGExtConfig.h
	VistaOpenSGGeometryTools.cpp
	VistaOpenSGGeometryTools.h
	VistaOpenSGMaterialTools.cpp
	VistaOpenSGMaterialTools.h
	VistaOpenSGNormalMapMaterial.cpp
	VistaOpenSGNormalMapMaterial.h
	VistaOpenSGNormalMapMaterialShaders.cpp
	VistaOpenSGParticleManager.cpp
	VistaOpenSGParticleManager.h
	VistaOpenSGParticleManagerParticleChanger.cpp
	VistaOpenSGParticleManagerParticleChanger.h
	VistaOpenSGParticles.cpp
	VistaOpenSGParticles.h
	VistaOpenSGPerMaterialShader.cpp
	VistaOpenSGPerMaterialShader.h
	VistaOpenSGPhongShader.cpp
	VistaOpenSGPhongShader.h
	VistaOpenSGMultiMaterialShaderCreator.cpp
	VistaOpenSGMultiMaterialShaderCreator.h
	VistaOpenSGMultiMaterial.cpp
	VistaOpenSGMultiMaterial.h
	VistaOpenSGShadow.cpp
	VistaOpenSGShadow.h
	VistaOpenSGSkybox.cpp
	VistaOpenSGSkybox.h
	VistaOpenSGTextureLoader.cpp
	VistaOpenSGTextureLoader.h
	VistaOpenSGToonShader.cpp
	VistaOpenSGToonShader.h
	VistaOpenSGWebInterface.cpp
	VistaOpenSGWebInterface.h
	VistaOpenSGModelOptimizer.cpp
	VistaOpenSGModelOptimizer.h
	VistaOpenSGGraphicsStatsOverlay.cpp
	VistaOpenSGGraphicsStatsOverlay.h
	VistaOpenSGHaloShader.h
	VistaOpenSGHaloShader.cpp
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

