include(FetchContent)

FetchContent_Declare(
    OpenSG
    GIT_REPOSITORY https://github.com/cosmoscout/opensg-1.8.git
    GIT_TAG main
    GIT_SHALLOW 1
)

set(CMAKE_UNITY_BUILD On CACHE BOOL "")
set(OPENSG_USE_PRECOMPILED_HEADERS On CACHE BOOL "")
set(OPENSG_INFINITE_REVERSE_PROJECTION On CACHE BOOL "")
set(OPENSG_BUILD_WINDOW Off CACHE BOOL "")
set(OPENSG_BUILD_TESTS Off CACHE BOOL "")

# Set linker flags for Windows
if(WIN32)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /FORCE:MULTIPLE")
endif()

# OpenVR configuration
FetchContent_Declare(
    OpenVR
    GIT_REPOSITORY https://github.com/ValveSoftware/openvr.git
    GIT_TAG v2.15.6
    GIT_SHALLOW 1
)

set(BUILD_SHARED ON CACHE BOOL "")
set(BUILD_UNIVERSAL OFF CACHE BOOL "")

# FetchContent_Declare(
#     Freetype
#     GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
#     GIT_TAG VER-2-14-3
#     GIT_SHALLOW 1
# )
#
# set(FT_DISABLE_ZLIB ON CACHE BOOL "")
# set(FT_DISABLE_BZIP2 ON CACHE BOOL "")
# set(FT_DISABLE_PNG ON CACHE BOOL "")
# set(FT_DISABLE_BROTLI ON CACHE BOOL "")
#
# set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${freetype_SOURCE_DIR})

# SDL2 configuration
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.32.8
    GIT_SHALLOW 1
)

set(SDL_SHARED ON CACHE BOOL "")
set(SDL_STATIC OFF CACHE BOOL "")
set(SDL_TEST OFF CACHE BOOL "")

# SDL2_ttf configuration
# FetchContent_Declare(
#     SDL2_ttf
#     GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
#     GIT_TAG release-2.24.0
#     GIT_SHALLOW 1
# )
#
# set(SDL2TTF_SAMPLES OFF CACHE BOOL "")
# set(SDL2TTF_INSTALL OFF CACHE BOOL "")
# set(SDL2TTF_VENDORED OFF CACHE BOOL "")

# Make all dependencies available
FetchContent_MakeAvailable(OpenSG)
