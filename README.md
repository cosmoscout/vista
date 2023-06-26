# ViSTA VR Toolkit

This is a fork of the ViSTA VR Toolkit specifically tailored to be used as a dependency of [CosmoScout VR](https://github.com/cosmoscout/cosmoscout-vr).
Originally, ViSTA has been developed at the RWTH Aachen. This fork is is based on the `feature/IMAGES_AND_VIEWPORTS` branch of the [original repository](https://devhub.vr.rwth-aachen.de/VR-Group/ViSTA).

ViSTA is open source under the LGPL (see LICENSE and LICENSE.LESSER). As this fork is used as dependency of [CosmoScout VR](https://github.com/cosmoscout/cosmoscout-vr), it should be built as part of CosmoScout's build process. However, you can also built ViSTA separately - see the [BUILDINFO](VistaCoreLibs/BUILDINFO) files for details.

To contact the developers, mail to cosmoscout@dlr.de or chat on [gitter](https://gitter.im/cosmoscout/community).

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
