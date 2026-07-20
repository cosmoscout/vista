file(GLOB AdditionalSources
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

if (VISTACORELIBS_BUILD_WINDOWIMP_GLUT)
    file(GLOB GlutWindowImpSources
            "${CMAKE_CURRENT_SOURCE_DIR}/GlutWindowImp/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/GlutWindowImp/*.h"
    )
    list(APPEND AdditionalSources ${GlutWindowImpSources})

    if (VISTACORELIBS_USE_OCULUSSDK)
        file(GLOB OculusGlutWindowImpSources
                "${CMAKE_CURRENT_SOURCE_DIR}/OculusGlutWindowImp/*.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/OculusGlutWindowImp/*.h"
        )
        list(APPEND AdditionalSources ${OculusGlutWindowImpSources})
    endif ()

    if (VISTACORELIBS_USE_OPENVR)
        file(GLOB OpenVRGlutWindowImpSources
                "${CMAKE_CURRENT_SOURCE_DIR}/OpenVRGlutWindowImp/*.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/OpenVRGlutWindowImp/*.h"
        )
        list(APPEND AdditionalSources ${OpenVRGlutWindowImpSources})
    endif ()
endif ()

if (VISTACORELIBS_BUILD_WINDOWIMP_OSG)
	file(GLOB OpenSceneGraphWindowImpSources
			"${CMAKE_CURRENT_SOURCE_DIR}/OpenSceneGraphWindowImp/*.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/OpenSceneGraphWindowImp/*.h"
	)
	list(APPEND AdditionalSources ${OpenSceneGraphWindowImpSources})
endif ()

if (VISTACORELIBS_BUILD_WINDOWIMP_SDL2)
	file(GLOB SDL2WindowImpSources
			"${CMAKE_CURRENT_SOURCE_DIR}/SDL2WindowImp/*.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/SDL2WindowImp/*.h"
	)
	list(APPEND AdditionalSources ${SDL2WindowImpSources})

    if (VISTACORELIBS_USE_OPENVR)
		file(GLOB OpenVRSDL2WindowImpSources
				"${CMAKE_CURRENT_SOURCE_DIR}/OpenVRSDL2WindowImp/*.cpp"
				"${CMAKE_CURRENT_SOURCE_DIR}/OpenVRSDL2WindowImp/*.h"
		)
		list(APPEND AdditionalSources ${OpenVRSDL2WindowImpSources})
    endif ()
endif ()

list(APPEND ProjectSources ${AdditionalSources})