include(FetchContent)

# Configure OpenSG build options before declaring the content
set(OPENSG_INFINITE_REVERSE_PROJECTION ON)
set(OPENSG_BUILD_WINDOW OFF)
set(OPENSG_BUILD_TESTS OFF)

# Set linker flags for Windows
if(WIN32)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /FORCE:MULTIPLE")
endif()

# OpenVR configuration
set(BUILD_SHARED ON)
set(BUILD_UNIVERSAL OFF)
FetchContent_Declare(
    OpenVR
    GIT_REPOSITORY https://github.com/ValveSoftware/openvr.git
    GIT_TAG v1.26.7
    GIT_SHALLOW 1
)

# SDL2 configuration
set(SDL_SHARED ON)
set(SDL_STATIC OFF)
set(SDL_TEST OFF)
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.5
    GIT_SHALLOW 1
)

# SDL2_ttf configuration
set(SDL2TTF_SAMPLES OFF)
set(SDL2TTF_INSTALL OFF)
FetchContent_Declare(
    SDL2_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-2.20.2
    GIT_SHALLOW 1
)

# Make all dependencies available
FetchContent_MakeAvailable(OpenSG OpenVR SDL2 SDL2_ttf)
