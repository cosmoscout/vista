VRPNDriver
-------------


GENERAL INFO

This driver enables the usage of remote sensor of type ANALOG (vrpn_Analog_Remote),
BUTTON (vrpn_Button_Remote) and TRACKER (vrpn_TrackerRemote).
It was written using VRPN 7.33, but should be compatible to all 7.x where x is larger than 29. 

You can obtain vrpn here:

http://www.cs.unc.edu/Research/vrpn/index.html

In order to use it, you probably have to compile and use one 
of the vrpn servers of VRPN itself, or build one suiting your
own needs. This driver does not care about the server structure.



LICENSING

This plugin employs the vrpn library (http://www.cs.unc.edu/Research/vrpn/index.html)
to interface with the remote servers, built upon VRPN as well.
VRPN is part of the public domain, go and check their license.
However, this plugin is LGPL, just like the rest of ViSTA.


USAGE

* download VRPN, read instructions and build it suiting your needs
  and according to the instructions. This driver will try to link
  against any version available, either static or dynamic.

* assign VRPN_ROOT prior to building it to point to the directory, where
  - ${VRPN_ROOT}/include points to the VRPN headers
  - ${VRPN_ROOT}/lib points to the VRPN/quat libs
  
  In case your directory structure does not match, adapt the shipped
  makefiles of this driver according to your needs. 

* build driver and install it to your plugin directory, so ViSTA or
  your application built on top of it can find it
  
* the plugin will expose a driver of type VRPN

* add sensors with
  - type: either ANALOG | BUTTON | TRACKER (case-insensitive)
  - name: <sensor>@<ip> (this will be passed directly to VRPN for
    a connect. Note that somehow is does not seem to be possible
    to react on misspellings. So in case you read something on the
    console about a timeout... you could connect, but probably
    the sensor name is not correct).
  - setup the sensor history accordingly.
  

REMARKS

- we currently assume an update rate or any sensor below 1000Hz, 
  this might not be correct. In case you experience segfaults with 
  fast devices, please write a report or measure and fix the update
  rate accordingly.
