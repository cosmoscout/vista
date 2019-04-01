IRMan Driver
------------


GENERAL INFO
------------
This is a driver for the IRMan infra-red controller, see: http://www.intolect.com/irmandetail.htm
or all its successors (untested, though, but the website states compatibility to the IRMan protocol).
It does build on Vista internal classes and has no external dependencies.


USAGE
-----

The driver needs the first connection ("MAIN") to be set-up correctly (typically this is the serial
port with 9600,8,n,1). It marks itself as CONNECTION_THREADED. There are not strong assumptions about 
the type of the connection, so an emulation of the serial port under the hood of a VistaConnection should do, 
but this is untested. 
The driver sets the connection to 'blocking' during update to avoid driver-level polling and
re-sets it to 'non-blocking' after reading. This could confuse an emulation layer.
As well, it tries to 'suck' all bytes on the line upon the first connect, this works well for a
non-blocking serial line (buffer-flush), but could be a problem for emulation layers as well.

The driver exports a single sensor that gives the 6-byte key code as received by the IRMan when connected.
There is no conversion of the key-code to any kind of action in this driver, use a seperate step in 
your application for that.

TRANSCODER
----------

The standard implementation allows to query:

* KEYCODE: the 6-byte value as native type (6-bytes copy-wise, you need
           access to the header of the driver for getting the transcoder by type, though) 
* KEY: as an int64 (where the upper-most two bytes are filled with 00 00).