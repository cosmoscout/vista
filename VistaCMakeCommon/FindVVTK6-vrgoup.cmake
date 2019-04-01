

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VVTK6_FOUND )

	# VTK hides it config file quite well, so we have to search it explicitely
	set( _SEARCH_PREFIXES )
	if( EXISTS "$ENV{${_PACKAGE_NAME_UPPER}_ROOT}" )
		list( APPEND _SEARCH_PREFIXES
				"$ENV{${_PACKAGE_NAME_UPPER}_ROOT}/${VISTA_HWARCH}/lib/cmake/*"
				"$ENV{${_PACKAGE_NAME_UPPER}_ROOT}/lib/cmake/*"
		)
	endif( EXISTS "$ENV{${_PACKAGE_NAME_UPPER}_ROOT}" )

	foreach( _PATH ${VISTA_PACKAGE_SEARCH_PATHS} )
		file( TO_CMAKE_PATH ${_PATH} _PATH )
		list( APPEND _SEARCH_PREFIXES
				"${_PATH}/VTK*/${VISTA_HWARCH}/lib/*"
				"${_PATH}/VTK*/lib/*"
				"${_PATH}/VTK*/${VISTA_HWARCH}/lib/cmake/*"
				"${_PATH}/VTK*/lib/cmake/*"				
				"${_PATH}/VTK/*/${VISTA_HWARCH}/lib/*"
				"${_PATH}/VTK/*/lib/*"
				"${_PATH}/vtk*/${VISTA_HWARCH}/lib/*"
				"${_PATH}/vtk*/lib/*"
				"${_PATH}/vtk/*/${VISTA_HWARCH}/lib/*"
				"${_PATH}/vtk/*/lib/*"
				"${_PATH}/vtk/*/${VISTA_HWARCH}/lib/cmake/*"
				"${_PATH}/vtk/*/lib/cmake/*"
	)
	endforeach( _PATH ${VISTA_PACKAGE_SEARCH_PATHS} )
	
	foreach( _PATH ${_SEARCH_PREFIXES} )
		file( GLOB _TMP_FILES "${_PATH}/VTKConfig.cmake" )
		foreach( _FILE ${_TMP_FILES} )
			file( TO_CMAKE_PATH ${_FILE} _FOUND_FILE )
			string( REPLACE "/VTKConfig.cmake" "" _FOUND_PATH ${_FOUND_FILE} )
			file( TO_CMAKE_PATH ${_FOUND_PATH} _FOUND_PATH )
			list( APPEND VTK_CONFIG_DIRS ${_FOUND_PATH} )
		endforeach( _FILE ${_TMP_FILES} )
	endforeach( _PATH ${_PREFIX_PATHES} )
	if( VTK_CONFIG_DIRS )
		list( REMOVE_DUPLICATES VTK_CONFIG_DIRS )
	endif( VTK_CONFIG_DIRS )
	
	find_package( VTK ${VVTK6_FIND_VERSION} PATHS ${VTK_DIR} ${VTK_CONFIG_DIRS} )
	
	if( VTK_FOUND )
		# a VTKConfig.cmake has been found and loaded

		# check if debug libraries are available
		set( _TMP_VTK_DEBUG_LIB "_TMP_VTK_DEBUG_LIB-NOTFOUND" CACHE INTERNAL "" FORCE )
		find_library( _TMP_VTK_DEBUG_LIB "vtkCommonCoreD" PATH ${VTK_LIBRARY_DIRS} NO_DEFAULT_PATH )
		if( _TMP_VTK_DEBUG_LIB )
			set( _DEBUG_AVAILABLE TRUE )
		else( _TMP_VTK_DEBUG_LIB )
			set( _DEBUG_AVAILABLE FALSE )
		endif( _TMP_VTK_DEBUG_LIB )
		set( _TMP_VTK_DEBUG_LIB "_TMP_VTK_DEBUG_LIB-NOTFOUND" CACHE INTERNAL "" FORCE )

		if( _DEBUG_AVAILABLE )
			set( VTK6_LIBRARIES	optimized vtkalglib
								optimized vtkChartsCore
								optimized vtkCommonColor
								optimized vtkCommonComputationalGeometry
								optimized vtkCommonCore
								optimized vtkCommonDataModel
								optimized vtkCommonExecutionModel
								optimized vtkCommonMath
								optimized vtkCommonMisc
								optimized vtkCommonSystem
								optimized vtkCommonTransforms
								optimized vtkDICOMParser
								optimized vtkDomainsChemistry
								optimized vtkexoIIc
								optimized vtkexpat
								optimized vtkFiltersAMR
								optimized vtkFiltersCore
								optimized vtkFiltersExtraction
								optimized vtkFiltersFlowPaths
								optimized vtkFiltersGeneral
								optimized vtkFiltersGeneric
								optimized vtkFiltersGeometry
								optimized vtkFiltersHybrid
								optimized vtkFiltersHyperTree
								optimized vtkFiltersImaging
								optimized vtkFiltersModeling
								optimized vtkFiltersParallel
								optimized vtkFiltersParallelImaging
								optimized vtkFiltersProgrammable
								optimized vtkFiltersSelection
								optimized vtkFiltersSMP
								optimized vtkFiltersSources
								optimized vtkFiltersStatistics
								optimized vtkFiltersTexture
								optimized vtkFiltersVerdict
								optimized vtkfreetype
								optimized vtkftgl
								optimized vtkgl2ps
								optimized vtkhdf5
								optimized vtkhdf5_hl
								optimized vtkImagingColor
								optimized vtkImagingCore
								optimized vtkImagingFourier
								optimized vtkImagingGeneral
								optimized vtkImagingHybrid
								optimized vtkImagingMath
								optimized vtkImagingMorphological
								optimized vtkImagingSources
								optimized vtkImagingStatistics
								optimized vtkImagingStencil
								optimized vtkInfovisCore
								optimized vtkInfovisLayout
								optimized vtkIOAMR
								optimized vtkIOCore
								optimized vtkIOEnSight
								optimized vtkIOExodus
								optimized vtkIOExport
								optimized vtkIOGeometry
								optimized vtkIOImage
								optimized vtkIOImport
								optimized vtkIOInfovis
								optimized vtkIOLegacy
								optimized vtkIOLSDyna
								optimized vtkIOMINC
								optimized vtkIOMovie
								optimized vtkIONetCDF
								optimized vtkIOParallel
								optimized vtkIOPLY
								optimized vtkIOSQL
								optimized vtkIOVideo
								optimized vtkIOXML
								optimized vtkIOXMLParser
								optimized vtkjpeg
								optimized vtkjsoncpp
								optimized vtklibxml2
								optimized vtkmetaio
								optimized vtkNetCDF
								optimized vtkNetCDF_cxx
								optimized vtkoggtheora
								optimized vtkParallelCore
								optimized vtkpng
								optimized vtkRenderingAnnotation
								optimized vtkRenderingContext2D
								optimized vtkRenderingCore
								optimized vtkRenderingFreeType
								optimized vtkRenderingGL2PS
								optimized vtkRenderingLabel
								optimized vtkRenderingOpenGL
								optimized vtksqlite
								optimized vtksys
								optimized vtktiff
								#optimized vtkverdict
								optimized vtkzlib
								#rendering from here
								optimized vtkGeovisCore
								optimized vtkInteractionImage
								optimized vtkInteractionStyle
								optimized vtkInteractionWidgets
								optimized vtkproj4
								optimized vtkRenderingFreeTypeOpenGL
								optimized vtkRenderingImage
								optimized vtkRenderingLIC
								optimized vtkRenderingLOD
								optimized vtkRenderingVolume
								optimized vtkRenderingVolumeAMR
								optimized vtkRenderingVolumeOpenGL
								optimized vtkViewsContext2D
								optimized vtkViewsCore
								optimized vtkViewsGeovis
								optimized vtkViewsInfovis
								debug vtkalglibD
								debug vtkChartsCoreD
								debug vtkCommonColorD
								debug vtkCommonComputationalGeometryD
								debug vtkCommonCoreD
								debug vtkCommonDataModelD
								debug vtkCommonExecutionModelD
								debug vtkCommonMathD
								debug vtkCommonMiscD
								debug vtkCommonSystemD
								debug vtkCommonTransformsD
								debug vtkDICOMParserD
								debug vtkDomainsChemistryD
								debug vtkexoIIcD
								debug vtkexpatD
								debug vtkFiltersAMRD
								debug vtkFiltersCoreD
								debug vtkFiltersExtractionD
								debug vtkFiltersFlowPathsD
								debug vtkFiltersGeneralD
								debug vtkFiltersGenericD
								debug vtkFiltersGeometryD
								debug vtkFiltersHybridD
								debug vtkFiltersHyperTreeD
								debug vtkFiltersImagingD
								debug vtkFiltersModelingD
								debug vtkFiltersParallelD
								debug vtkFiltersParallelImagingD
								debug vtkFiltersProgrammableD
								debug vtkFiltersSelectionD
								debug vtkFiltersSMPD
								debug vtkFiltersSourcesD
								debug vtkFiltersStatisticsD
								debug vtkFiltersTextureD
								debug vtkFiltersVerdictD
								debug vtkfreetypeD
								debug vtkftglD
								debug vtkgl2psD
								debug vtkhdf5D
								debug vtkhdf5_hlD
								debug vtkImagingColorD
								debug vtkImagingCoreD
								debug vtkImagingFourierD
								debug vtkImagingGeneralD
								debug vtkImagingHybridD
								debug vtkImagingMathD
								debug vtkImagingMorphologicalD
								debug vtkImagingSourcesD
								debug vtkImagingStatisticsD
								debug vtkImagingStencilD
								debug vtkInfovisCoreD
								debug vtkInfovisLayoutD
								debug vtkIOAMRD
								debug vtkIOCoreD
								debug vtkIOEnSightD
								debug vtkIOExodusD
								debug vtkIOExportD
								debug vtkIOGeometryD
								debug vtkIOImageD
								debug vtkIOImportD
								debug vtkIOInfovisD
								debug vtkIOLegacyD
								debug vtkIOLSDynaD
								debug vtkIOMINCD
								debug vtkIOMovieD
								debug vtkIONetCDFD
								debug vtkIOParallelD
								debug vtkIOPLYD
								debug vtkIOSQLD
								debug vtkIOVideoD
								debug vtkIOXMLD
								debug vtkIOXMLParserD
								debug vtkjpegD
								debug vtkjsoncppD
								debug vtklibxml2D
								debug vtkmetaioD
								debug vtkNetCDFD
								debug vtkNetCDF_cxxD
								debug vtkoggtheoraD
								debug vtkParallelCoreD
								debug vtkpngD
								debug vtkRenderingAnnotationD
								debug vtkRenderingContext2DD
								debug vtkRenderingCoreD
								debug vtkRenderingFreeTypeD
								debug vtkRenderingGL2PSD
								debug vtkRenderingLabelD
								debug vtkRenderingOpenGLD
								debug vtksqliteD
								debug vtksysD
								debug vtktiffD
								#debug vtkverdictD
								debug vtkzlibD
								#rendering from here
								debug vtkGeovisCoreD
								debug vtkInteractionImageD
								debug vtkInteractionStyleD
								debug vtkInteractionWidgetsD
								debug vtkproj4D
								debug vtkRenderingFreeTypeOpenGLD
								debug vtkRenderingImageD
								debug vtkRenderingLICD
								debug vtkRenderingLODD
								debug vtkRenderingVolumeD
								debug vtkRenderingVolumeAMRD
								debug vtkRenderingVolumeOpenGLD
								debug vtkViewsContext2DD
								debug vtkViewsCoreD
								debug vtkViewsGeovisD
								debug vtkViewsInfovisD
			)

		else( _DEBUG_AVAILABLE ) # no debug libraries available
			set( VTK6_LIBRARIES	vtkalglib
								vtkChartsCore
								vtkCommonColor
								vtkCommonComputationalGeometry
								vtkCommonCore
								vtkCommonDataModel
								vtkCommonExecutionModel
								vtkCommonMath
								vtkCommonMisc
								vtkCommonSystem
								vtkCommonTransforms
								vtkDICOMParser
								vtkDomainsChemistry
								vtkexoIIc
								vtkexpat
								vtkFiltersAMR
								vtkFiltersCore
								vtkFiltersExtraction
								vtkFiltersFlowPaths
								vtkFiltersGeneral
								vtkFiltersGeneric
								vtkFiltersGeometry
								vtkFiltersHybrid
								vtkFiltersHyperTree
								vtkFiltersImaging
								vtkFiltersModeling
								vtkFiltersParallel
								vtkFiltersParallelImaging
								vtkFiltersProgrammable
								vtkFiltersSelection
								vtkFiltersSMP
								vtkFiltersSources
								vtkFiltersStatistics
								vtkFiltersTexture
								vtkFiltersVerdict
								vtkfreetype
								vtkftgl
								vtkgl2ps
								vtkhdf5
								vtkhdf5_hl
								vtkImagingColor
								vtkImagingCore
								vtkImagingFourier
								vtkImagingGeneral
								vtkImagingHybrid
								vtkImagingMath
								vtkImagingMorphological
								vtkImagingSources
								vtkImagingStatistics
								vtkImagingStencil
								vtkInfovisCore
								vtkInfovisLayout
								vtkIOAMR
								vtkIOCore
								vtkIOEnSight
								vtkIOExodus
								vtkIOExport
								vtkIOGeometry
								vtkIOImage
								vtkIOImport
								vtkIOInfovis
								vtkIOLegacy
								vtkIOLSDyna
								vtkIOMINC
								vtkIOMovie
								vtkIONetCDF
								vtkIOParallel
								vtkIOPLY
								vtkIOSQL
								vtkIOVideo
								vtkIOXML
								vtkIOXMLParser
								vtkjpeg
								vtkjsoncpp
								vtklibxml2
								vtkmetaio
								vtkNetCDF
								vtkNetCDF_cxx
								vtkoggtheora
								vtkParallelCore
								vtkpng
								vtkRenderingAnnotation
								vtkRenderingContext2D
								vtkRenderingCore
								vtkRenderingFreeType
								vtkRenderingGL2PS
								vtkRenderingLabel
								vtkRenderingOpenGL
								vtksqlite
								vtksys
								vtktiff
								#vtkverdict
								vtkzlib
								#rendering from here
								vtkGeovisCore
								vtkInteractionImage
								vtkInteractionStyle
								vtkInteractionWidgets
								vtkproj4
								vtkRenderingFreeTypeOpenGL
								vtkRenderingImage
								vtkRenderingLIC
								vtkRenderingLOD
								vtkRenderingVolume
								vtkRenderingVolumeAMR
								vtkRenderingVolumeOpenGL
								vtkViewsContext2D
								vtkViewsCore
								vtkViewsGeovis
								vtkViewsInfovis
			)

		endif( _DEBUG_AVAILABLE )

		# VTK's dlls are in the VTK_RUNTIME_LIBRARY_DIRS, so we have to add this to the library dirs
		# to find them
		set( VTK6_INCLUDE_DIRS ${VTK_INCLUDE_DIRS} )
		set( VTK6_LIBRARY_DIRS ${VTK_LIBRARY_DIRS} ${VTK_RUNTIME_LIBRARY_DIRS} )
		set( VTK6_ROOT_DIR ${VTK_INSTALL_PREFIX} )

		# note that there is also a VTK_USE_FILE, which will automatically be called
		# by vista_use_package
		set( VTK6_USE_FILE "${VTK_USE_FILE}" )
	endif( VTK_FOUND )
endif( NOT VVTK6_FOUND )

find_package_handle_standard_args( VVTK6 "VTK could not be found" VTK6_ROOT_DIR )
set( VTK_FOUND ${VVTK6_FOUND} )
