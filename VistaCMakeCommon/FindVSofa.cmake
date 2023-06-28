

# Tested with: Sofa 1.0 rev10511@svn://scm.gforge.inria.fr/svn/sofa/trunk/Sofa

include( FindPackageHandleStandardArgs )
include( VistaFindUtils )

if( NOT VSOFA_FOUND )
	vista_find_package_root( SOFA include/sofa/core/SofaLibrary.h )

	if( SOFA_ROOT_DIR )

		set( SOFA_INCLUDE_DIRS 
			${SOFA_ROOT_DIR}/include
			${SOFA_ROOT_DIR}/modules
			${SOFA_ROOT_DIR}/applications
			${SOFA_ROOT_DIR}/extlibs
        )
		
		if( WIN32 )
			if( MSVC )
				if( MSVC10 )
					set( SOFA_LIBRARY_DIRS ${SOFA_ROOT_DIR}/lib/ ${SOFA_ROOT_DIR}/bin/ )
				else( MSVC10 )
					set( SOFA_LIBRARY_DIRS ${SOFA_ROOT_DIR}/lib/ ${SOFA_ROOT_DIR}/bin/ )
					message( WARNING "FindPackageSOFA - Unknown MSVC version" )
				endif( MSVC10 )				
			else( MSVC )
                set( SOFA_LIBRARY_DIRS ${SOFA_ROOT_DIR}/lib/ ${SOFA_ROOT_DIR}/bin/ )
				message( WARNING "FindPackageSOFA - using WIN32 without Visual Studio - this will probably fail - use at your own risk!" )
			endif( MSVC )
		
			

			set( SOFA_LIBRARIES 
				optimized opengl32.lib
				optimized glu32.lib
				optimized gdi32.lib
				optimized user32.lib
				optimized FlowVR_1_0.lib
				optimized newmat_1_0.lib
				optimized ObjectCreator_1_0.lib
				optimized QGLViewer_1_0.lib
				optimized Qwt_1_0.lib
				optimized Sensable_1_0.lib
				optimized SofaBaseAnimationLoop_1_0.lib
				optimized SofaBaseCollision_1_0.lib
				optimized SofaBaseLinearSolver_1_0.lib
				optimized SofaBaseMechanics_1_0.lib
				optimized SofaBaseTopology_1_0.lib
				optimized SofaBaseVisual_1_0.lib
				optimized SofaBoundaryCondition_1_0.lib
				optimized SofaComponentAdvanced_1_0.lib
				optimized SofaComponentBase_1_0.lib
				optimized SofaComponentCommon_1_0.lib
				optimized SofaComponentGeneral_1_0.lib
				optimized SofaComponentMain_1_0.lib
				optimized SofaComponentMisc_1_0.lib
				optimized SofaConstraint_1_0.lib
				optimized SofaCore_1_0.lib
				optimized SofaDefaultType_1_0.lib
				optimized SofaDeformable_1_0.lib
				optimized SofaDenseSolver_1_0.lib
				optimized SofaEigen2Solver_1_0.lib
				optimized SofaEngine_1_0.lib
				optimized SofaEulerianFluid_1_0.lib
				optimized SofaExplicitOdeSolver_1_0.lib
				optimized SofaExporter_1_0.lib
				optimized SofaGraphComponent_1_0.lib
				optimized SofaGuiCommon_1_0.lib
				optimized SofaGuiGlut_1_0.lib
				optimized SofaGuiMain_1_0.lib
				optimized SofaGuiQt_1_0.lib
				optimized SofaHaptics_1_0.lib
				optimized SofaHelper_1_0.lib
				optimized SofaImplicitOdeSolver_1_0.lib
				optimized SofaLoader_1_0.lib
				optimized SofaMeshCollision_1_0.lib
				optimized SofaMisc_1_0.lib
				optimized SofaMiscCollision_1_0.lib
				optimized SofaMiscEngine_1_0.lib
				optimized SofaMiscFem_1_0.lib
				optimized SofaMiscForceField_1_0.lib
				optimized SofaMiscMapping_1_0.lib
				optimized SofaMiscSolver_1_0.lib
				optimized SofaMiscTopology_1_0.lib
				optimized SofaModeler_1_0.lib
				optimized SofaNonUniformFem_1_0.lib
				optimized SofaObjectInteraction_1_0.lib
				optimized SofaOpenglVisual_1_0.lib
				optimized SofaPreconditioner_1_0.lib
				optimized SofaRigid_1_0.lib
				optimized SofaSimpleFem_1_0.lib
				optimized SofaSimulationCommon_1_0.lib
				optimized SofaSimulationGraph_1_0.lib
				optimized SofaSimulationTree_1_0.lib
				optimized SofaSphFluid_1_0.lib
				optimized SofaTopologyMapping_1_0.lib
				optimized SofaUserInteraction_1_0.lib
				optimized SofaValidation_1_0.lib
				optimized SofaVolumetricData_1_0.lib
				optimized tinyxml_1_0.lib
				optimized glut32.lib
				optimized comctl32.lib
				optimized AdvAPI32.lib
				optimized Shell32.lib
				optimized WSock32.lib
				optimized WS2_32.lib
				optimized Ole32.lib
				optimized zlib.lib
				optimized libpng.lib
				optimized glew32.lib
				optimized Qt3Support4.lib
				optimized QtXml4.lib
				optimized QtOpenGL4.lib
				optimized QtGui4.lib
				optimized QtCore4.lib

				debug opengl32.lib
				debug glu32.lib
				debug gdi32.lib
				debug user32.lib
				debug FlowVR_1_0d.lib
				debug newmat_1_0d.lib
				debug ObjectCreator_1_0d.lib
				debug QGLViewer_1_0.lib
				debug Qwt_1_0d.lib
				debug Sensable_1_0d.lib
				debug SofaBaseAnimationLoop_1_0d.lib
				debug SofaBaseCollision_1_0d.lib
				debug SofaBaseLinearSolver_1_0d.lib
				debug SofaBaseMechanics_1_0d.lib
				debug SofaBaseTopology_1_0d.lib
				debug SofaBaseVisual_1_0d.lib
				debug SofaBoundaryCondition_1_0d.lib
				debug SofaComponentAdvanced_1_0d.lib
				debug SofaComponentBase_1_0d.lib
				debug SofaComponentCommon_1_0d.lib
				debug SofaComponentGeneral_1_0d.lib
				debug SofaComponentMain_1_0d.lib
				debug SofaComponentMisc_1_0d.lib
				debug SofaConstraint_1_0d.lib
				debug SofaCore_1_0d.lib
				debug SofaDefaultType_1_0d.lib
				debug SofaDeformable_1_0d.lib
				debug SofaDenseSolver_1_0d.lib
				debug SofaEigen2Solver_1_0d.lib
				debug SofaEngine_1_0d.lib
				debug SofaEulerianFluid_1_0d.lib
				debug SofaExplicitOdeSolver_1_0d.lib
				debug SofaExporter_1_0d.lib
				debug SofaGraphComponent_1_0d.lib
				debug SofaGuiCommon_1_0d.lib
				debug SofaGuiGlut_1_0d.lib
				debug SofaGuiMain_1_0d.lib
				debug SofaGuiQt_1_0d.lib
				debug SofaHaptics_1_0d.lib
				debug SofaHelper_1_0d.lib
				debug SofaImplicitOdeSolver_1_0d.lib
				debug SofaLoader_1_0d.lib
				debug SofaMeshCollision_1_0d.lib
				debug SofaMisc_1_0d.lib
				debug SofaMiscCollision_1_0d.lib
				debug SofaMiscEngine_1_0d.lib
				debug SofaMiscFem_1_0d.lib
				debug SofaMiscForceField_1_0d.lib
				debug SofaMiscMapping_1_0d.lib
				debug SofaMiscSolver_1_0d.lib
				debug SofaMiscTopology_1_0d.lib
				debug SofaModeler_1_0d.lib
				debug SofaNonUniformFem_1_0d.lib
				debug SofaObjectInteraction_1_0d.lib
				debug SofaOpenglVisual_1_0d.lib
				debug SofaPreconditioner_1_0d.lib
				debug SofaRigid_1_0d.lib
				debug SofaSimpleFem_1_0d.lib
				debug SofaSimulationCommon_1_0d.lib
				debug SofaSimulationGraph_1_0d.lib
				debug SofaSimulationTree_1_0d.lib
				debug SofaSphFluid_1_0d.lib
				debug SofaTopologyMapping_1_0d.lib
				debug SofaUserInteraction_1_0d.lib
				debug SofaValidation_1_0d.lib
				debug SofaVolumetricData_1_0d.lib
				debug tinyxml_1_0d.lib
				debug glut32.lib
				debug comctl32.lib
				debug AdvAPI32.lib
				debug Shell32.lib
				debug WSock32.lib
				debug WS2_32.lib
				debug Ole32.lib
				debug zlib.lib
				debug libpng.lib
				debug glew32.lib
				debug Qt3Supportd4.lib
				debug QtXmld4.lib
				debug QtOpenGLd4.lib
				debug QtGuid4.lib
				debug QtCored4.lib
			)		
		endif( WIN32 )

		set( SOFA_DEFINITIONS 	-D_MSVC
								-D_WINDOWS
								-DUNICODE
								-DWIN32
								-DQT_LARGEFILE_SUPPORT
								-DSOFA_QT4
								-DSOFA_DEV
								-DSOFA_GUI_QTVIEWER
								-DSOFA_GUI_GLUT
								-DSOFA_DUMP_VISITOR_INFO
								-DSOFA_HAVE_ZLIB
								-DSOFA_HAVE_PNG
								-DSOFA_HAVE_GLEW
								-DSOFA_XML_PARSER_TINYXML
								-DSOFA_GUI_QT
								-DMINI_FLOWVR
								-DQT_QT3SUPPORT_LIB
								-DQT3_SUPPORT
								-DQT_XML_LIB
								-DQT_OPENGL_LIB
								-DQT_GUI_LIB
								-DQT_CORE_LIB
								-DQT_THREAD_SUPPORT )

	endif( SOFA_ROOT_DIR )

endif( NOT VSOFA_FOUND )

find_package_handle_standard_args( VSOFA "SOFA could not be found" SOFA_ROOT_DIR )
