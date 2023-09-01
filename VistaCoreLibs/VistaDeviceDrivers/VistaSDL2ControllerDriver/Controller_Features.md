# SDL2 Controller Support
This document describes how different controllers work in different configurations.
Even though SDL2 tries to support each controllers features there are some differences.

## SDL2 Feature Support Table
The controllers sadly have a different feature set depending on OS and connection type.

| Controller       | Windows 10 | Ubuntu 20.04 |
|------------------|:----------:|:------------:|
| **_PS5_**        |   🔌 📡   |    🔌 📡     |
| Standard Buttons |   ✅ ✅   |    ✅ ✅    |
| Touchpad         |   ✅ ✅   |    🟡 🟡    |
| IMU              |   ✅ ✅   |    ❌ ❌    |
| Advanced Buttons |   ✅ ✅   |    ❌ ❌    |
| **_XBox_**       |   🔌 📡   |    🔌 📡     |
| Standard Buttons |   ✅ ✅   |    ✅ ✅    |
| Advanced Buttons |   🟡 🟡   |    🟡 🟡    |

Notes on 🟡:
- PS5 on Linux
  - The touchpad is detected as a mouse, not as the controller touchpad.
- XBox on Windows:
  - The advanced buttons map to A, B, X and Y.
  - They should be re-mappable using the XBox-Devices app, to which we don't have access.
- XBox on Linux
  - The advanced buttons map to A, B, X and Y.