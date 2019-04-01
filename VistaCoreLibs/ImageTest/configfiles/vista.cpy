
[DEBUG]
DisplayManager       = stdout
KernelDisplay        = stdout
SpaceMouse           = stdout

[GRAPHICS]
Lights               = LIGHT_D0, LIGHT_D1, LIGHT_D2, LIGHT_D3

[KEYB_MAIN]
RAWID                = 0

[KEYBOARD]
DEFAULTWINDOW        = TRUE
HISTORY              = 1
SENSORS              = KEYB_MAIN
TYPE                 = KEYBOARD

[KEYBOARDCONTROL]
DRIVER               = KEYBOARD
SENSORIDX            = 0

[KEYCONTROL]
GRAPH                = keyboard.xml
ROLE                 = SYSTEMCONTROL

[LIGHT_D0]
AmbientColor         = 0.1, 0.1, 0.1
DiffuseColor         = 0.6, 0.6, 0.6
Direction            = 1.0, -1.0, 0.0
SpecularColor        = 0.0,0.0,0.0
Type                 = DIRECTIONAL

[LIGHT_D1]
AmbientColor         = 0.1, 0.1, 0.1
DiffuseColor         = 0.6, 0.6, 0.6
Direction            = 0.0, 0.0, 1.0
SpecularColor        = 0.0, 0.0, 0.0
Type                 = DIRECTIONAL

[LIGHT_D2]
AmbientColor         = 0.1, 0.1, 0.1
DiffuseColor         = 0.6, 0.6, 0.6
Direction            = -1.0, 1.0, 1.0
SpecularColor        = 0.5, 0.5, 0.5
Type                 = DIRECTIONAL

[LIGHT_D3]
AmbientColor         = 0.1, 0.1, 0.1
DiffuseColor         = 0.3, 0.3, 0.36
Direction            = 0.0, 0.0, -1.0
SpecularColor        = 0.0, 0.0, 0.0
Type                 = DIRECTIONAL

[LIGHT_D4]
AmbientColor         = 0.1, 0.1, 0.1
Attenuation          = 0.1, 0.05, 0.0
DiffuseColor         = 0.3, 0.3, 0.3
Position             = 0.0, 2.0, 0.0
SpecularColor        = 0.2, 0.2, 0.2
Type                 = POINT

[MOUSE]
DEFAULTWINDOW        = TRUE
HISTORY              = 10
SENSORS              = MOUSE_MAIN
TYPE                 = MOUSE

[MOUSE_MAIN]
RAWID                = 0

[MOUSECTX]
GRAPH                = mouse.xml
ROLE                 = WORLDPOINTER

[SYSTEM]
CLUSTERINI           = display_desktop.ini
DEVICEDRIVERS        = MOUSE, KEYBOARD
DISPLAYINI           = display_desktop.ini
DRIVERPLUGINS        = 
DUMPGRAPHS           = FALSE
GRAPHICSSECTION      = GRAPHICS
INTERACTIONCONTEXTS  = KEYCONTROL, MOUSECTX
