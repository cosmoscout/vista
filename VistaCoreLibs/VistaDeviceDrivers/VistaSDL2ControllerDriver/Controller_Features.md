# SDL2 Controller Support
This document describes how different controllers work in different configurations.
Even though SDL2 tries to support each controllers features there are some differences.

## SDL2 Names
The controllers have different names in SDL2, depending on OS and the type of the connection.

**PlayStation DualSense Edge**
- Windows
  - 🔌: DualSense Edge Wireless Controller
  - 📡: DualSense Edge Wireless Controller
- Linux
  - 🔌: Sony Interactive Entertainment DualSense Edge Wireless Controller
  - 📡: PS5 Controller

**XBox One Elite 2**
- Windows
  - 🔌: Controller (Xbox One For Windows)
  - 📡: Xbox One Elite 2 Controller
- Linux
  - 🔌: Xbox One Elite 2 Controller
  - 📡: Xbox One Elite 2 Controller

## SDL2 Feature Support
The controllers sadly have a different feature set depending on OS and connection type.

| Controller       | Windows | Linux |
|------------------|:-------:|:-----:|
| **_PS5_**        | 🔌 📡  | 🔌 📡 |
| Standard Buttons | ✅ ✅  | ✅ 🟡|
| Touchpad         | ✅ ✅  | 🟡 🟡|
| IMU              | 🟡 🟡  | ❌ ❌|
| Advanced Buttons | ✅ ✅  | ❌ ❌|
| **_XBox_**       | 🔌 📡  | 🔌 📡 |
| Standard Buttons | ✅ ✅  | ✅ ✅|
| Advanced Buttons | 🟡 🟡  | 🟡 🟡|

Notes on 🟡:
- PS5 on Windows
  - While the IMU seems to be working, it is not quite clear if the mapping is correct.
- PS5 on Linux
  - When using the Controller via Bluetooth some buttons and axis are mapped wrong.
  - The touchpad is detected as a mouse, not as the controller touchpad.
- XBox on Windows:
  - The advanced buttons map to A, B, X and Y.
  - They should be re-mappable using the XBox-Devices app, to which we don't have access.
- XBox on Linux
  - The advanced buttons map to A, B, X and Y.