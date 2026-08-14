# ViSTA VR Toolkit

This is a fork of the ViSTA VR Toolkit specifically tailored to be used as a dependency of [CosmoScout VR](https://github.com/cosmoscout/cosmoscout-vr).
Originally, ViSTA has been developed at the RWTH Aachen. This fork is is based on the `feature/IMAGES_AND_VIEWPORTS` branch of the [original repository](https://devhub.vr.rwth-aachen.de/VR-Group/ViSTA).

ViSTA is open source under the LGPL (see LICENSE and LICENSE.LESSER). As this fork is used as dependency of [CosmoScout VR](https://github.com/cosmoscout/cosmoscout-vr), it should be built as part of CosmoScout's build process. However, you can also built ViSTA separately - see the [BUILDINFO](VistaCoreLibs/BUILDINFO) files for details.

To contact the developers, mail to cosmoscout@dlr.de.

## Building

### Prerequisites

Initialize git submodules (includes vcpkg):

```bash
git submodule update --init
```

### Using CMake Presets

ViSTA uses CMake presets for easy configuration and building. The minimum required CMake version is 3.28.

```bash
# Structure of the available presets
cmake --workflow <platform>-<generator>-<release/debug>

# List all presets
cmake --workflow --list-presets

# Examples:
cmake --workflow windows-ninja-debug
cmake --workflow windows-vs-release
cmake --workflow linux-ninja-debug
cmake --workflow linux-make-release
```

### Manual CMake Configuration

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target install
```

## Using ViSTA in Another Project with vcpkg

ViSTA is available as a vcpkg port. To use it in your project:

1. Copy the port from the `port` folder to your vcpkg ports directory:

```bash
cp -r port <vcpkg-root>/ports/vista
```

2. Update the revision and SHA512 in `<vcpkg-root>/ports/vista/portfile.cmake`:

- Set `REF` to the desired git commit hash or tag
- Generate the SHA512 hash for that revision and update the `SHA512` value

3. Add ViSTA to your vcpkg.json:

```json
{
  "dependencies": [
    "vista"
  ]
}
```

4. In your CMake project, use find_package:

```cmake
find_package(Vista CONFIG REQUIRED)

target_link_libraries(your_target PRIVATE Vista::<Component>)
```

### Features

- `window-sdl2` (default): Integrates the SDL2 windowing toolkit and input devices.
- `window-glut`: Integrates the GLUT windowing toolkit and input devices.
- `openvr` (default): Enables OpenVR support in the windowing toolkits and input devices.
- `driver-openal-record`: Enables the OpenAL based record driver.
- `driver-opencv-capture`: Enables the OpenCV based capture driver.
- `driver-opencv-face-track`: Enables the OpenCV base face tracking driver.
- `driver-openni`: Enables the OpenNI driver.
- `driver-wiimote`: Enables the wiimote driver.
- `driver-directx-gamepad`: Enables the DirectX gamepad driver (windows only).
- `demo-vtk`: Enables building the vtk demo.
- `networking-zmq`: Enables networking with ZeroMQ.
- `tests`: Tests are being build.


## Contributing

Some notes on possible ways of contributing to ViSTA are sketched in [CONTRIBUTING.md](CONTRIBUTING.md).

## Major changes compared to base branch

This fork of ViSTA contains several changes which have been made before the initial commit to this repository. Here is a rough list of those changes:

* VistaCMakeCommon is distributed as part of the repository.
* Add postprocessing support for quadbuffer stereo mode.
* Rename VistaTexture to VistaGLTexture in order to prevent name clash.
* Add optional scaling distance to intention select.
* Add a bounding box node adapter.
* Several methods of the volume raycaster are made virtual.
* Add support for OpenVR on Windows and Linux.
* Add possibility to change cursor type with glut windowing toollkit.
* Remove all inheritances from std::unitary_function.

## Architecture
![VistaModuleTree](https://user-images.githubusercontent.com/9581540/194340299-8772fa0a-cc7f-4e03-b473-1e200cd8deb1.svg)
