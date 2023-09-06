# SDL2 Controller Support

This document describes how different controllers work in different configurations.
Even though SDL2 tries to support each controllers features there are some differences.

## SDL2 Controller Usage

The following config enables the controller support in the interaction ini file:

```ini
DEVICEDRIVERS = ..., SDL2CONTROLLER
INTERACTIONCONTEXTS = ..., CONTROLLEREXAMPLE

[CONTROLLEREXAMPLE]
ROLE = CONTROLLEREXAMPLE
GRAPH = xml/sdl2_controller_example.xml

[SDL2CONTROLLER]
TYPE = SDL2CONTROLLER
HISTORY = 10
SENSORS = SDL2CONTROLLER_MAIN
CONTROLLER_DB = gamecontrollerdb.txt

[SDL2CONTROLLER_MAIN]
RAWID = 0
```

How the `gamecontrollerdb.txt` is specified, can be read here: https://github.com/gabomdq/SDL_GameControllerDB.
A sample db file is in the VistaDemo folder.

The corresponding `sdl2_controller_example.xml` file could look like this:

```xml

<module>
    <nodespace>
    </nodespace>
    <graph>
        <node name="controller_source" type="DriverSensor">
            <param name="sensor_index" value="0"/>
            <param name="driver" value="SDL2CONTROLLER"/>
        </node>

        <node name="controller" type="HistoryProject">
        </node>

        <!-- ... -->
    </graph>
    <edges>
        <!-- ... -->
    </edges>
</module>
```

The `controller` node would then offer the following outports:

| Port                   | Type  | Range / Unit |
|------------------------|-------|:------------:|
| A_PRESSED              | bool  |              |
| B_PRESSED              | bool  |              |
| X_PRESSED              | bool  |              |
| Y_PRESSED              | bool  |              |
|                        |       |              |
| DPAD_UP_PRESSED        | bool  |              |
| DPAD_DOWN_PRESSED      | bool  |              |
| DPAD_LEFT_PRESSED      | bool  |              |
| DPAD_RIGHT_PRESSED     | bool  |              |
|                        |       |              | 
| STICK_LEFT_PRESSED     | bool  |              |
| STICK_RIGHT_PRESSED    | bool  |              |
|                        |       |              | 
| SHOULDER_LEFT_PRESSED  | bool  |              |
| SHOULDER_RIGHT_PRESSED | bool  |              |
|                        |       |              | 
| BACK_PRESSED           | bool  |              |
| GUIDE_PRESSED          | bool  |              |
| START_PRESSED          | bool  |              |
|                        |       |              | 
| STICK_LEFT_X           | float |   [-1, 1]    |
| STICK_LEFT_Y           | float |   [-1, 1]    |
|                        |       |              | 
| STICK_RIGHT_X          | float |   [-1, 1]    |
| STICK_RIGHT_Y          | float |   [-1, 1]    |
|                        |       |              | 
| TRIGGER_LEFT           | float |    [0, 1]    |
| TRIGGER_RIGHT          | float |    [0, 1]    |
|                        |       |              | 
| **Advanced Features:** |       |              | 
|                        |       |              | 
| MISC1_PRESSED          | bool  |              |
|                        |       |              | 
| PADDLE1_PRESSED        | bool  |              |
| PADDLE2_PRESSED        | bool  |              |
| PADDLE3_PRESSED        | bool  |              |
| PADDLE4_PRESSED        | bool  |              |
|                        |       |              |
| HAS_ACCEL_SENSOR       | bool  |              |
| IMU_ACCELERATION       | vec3  |    m/s^2     |
|                        |       |              |
| HAS_GYRO_SENSOR        | bool  |              |
| IMU_GYRO               | vec3  |     °/s      |
|                        |       |              | 
| TOUCHPAD_PRESSED       | bool  |              |
|                        |       |              |
| TOUCHPAD_PRESSED       | bool  |              |
| TOUCHPAD_FINGER_1_X    | float |    [0, 1]    |
| TOUCHPAD_FINGER_1_Y    | float |    [0, 1]    |
|                        |       |              |
| TOUCHPAD_FINGER_2_DOWN | bool  |              |
| TOUCHPAD_FINGER_2_X    | float |    [0, 1]    |
| TOUCHPAD_FINGER_2_Y    | float |    [0, 1]    |

## SDL2 Feature Support Table

The controllers sadly have a different feature set depending on OS and connection type.

| Controller       | Windows 10 | Ubuntu 20.04 |
|------------------|:----------:|:------------:|
| **_PS5_**        |   🔌 📡    |    🔌 📡     |
| Standard Buttons |    ✅ ✅     |     ✅ ✅      |
| Touchpad         |    ✅ ✅     |    🟡 🟡     |
| IMU              |    ✅ ✅     |     ❌ ❌      |
| Advanced Buttons |    ✅ ✅     |     ❌ ❌      |
| **_XBox_**       |   🔌 📡    |    🔌 📡     |
| Standard Buttons |    ✅ ✅     |     ✅ ✅      |
| Advanced Buttons |   🟡 🟡    |    🟡 🟡     |

Notes on 🟡:

- PS5 on Linux
    - The touchpad is detected as a mouse, not as the controller touchpad.
- XBox on Windows:
    - The advanced buttons map to A, B, X and Y.
    - They should be re-mappable using the XBox-Devices app, to which we don't have access.
- XBox on Linux
    - The advanced buttons map to A, B, X and Y.