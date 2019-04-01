WiimoteDriver
-------------


GENERAL INFO

This is a device driver for use of Nintendo's family of Wii input
devices with the ViSTA software library for virtual reality
applications. 


DEPENDENCIES

You need version 0.12 of libwiiuse to build this driver module. You
can grab it on the author's site (http://www.wiiuse.net) or from the
resources subdirectory of this driver module. Other versions of the
library are not tested yet and may or may not work as well.


LICENSING

This plugin employs the wiiuse library (http://www.wiiuse.net) to
interface with the hardware via bluetooth, against which it links
dynamically. The library is released under both the GPL and the LGPL
license (for non-commercial use only).  For now, we use the
non-commercial LGPL form of the license which does not impose any
license restrictions on the derivative work.


USAGE

To use input devices under Linux, the BlueZ bluetooth protocol stack
is used. You need a kernel with Bluetooth support enabled, and the
BlueZ daemons running, in particular the hcid (host controller
interface daemon). For instructions how to install, configure and run
the BlueZ software, consult your distributions documentation.

todo: describe installation and setup process under MS Windows.

For the driver to detect Wii input devices, the devices have to be put
in a special "discover mode". Therefore you need to press the buttons
1 and 2 simultaneously on the device. If the LED's stop blinking
during the discovery process, press buttons 1 and 2 again, or keep
them hold down during the whole process.


REMARKS

Nintendo and Wii are trademarks of Nintendo Company Ltd.
