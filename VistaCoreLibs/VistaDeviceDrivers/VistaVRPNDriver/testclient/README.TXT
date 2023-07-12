USAGE
-----

1) export your LD_LIBRARY_PATH to point to the VISTA libs properly
2) export your VDD_DRIVERS to point to the path where the drivers and transcoders can
   be found.
3) start the VRPN testserver (needs a path as to the VISTA libs as well)
4) go to the bin directory where the binaries are located (typically VRPNDriver/bin)
4) start the client
   - for a quick button test:
     ./VRPN_SpaceNavClientD ../testclient/buttonOut.xml SpaceNav0@localhost SpaceNav0@localhost BUTTON -1
   - for a quick analog test:
     ./VRPN_SpaceNavClientD ../testclient/analogOut.xml SpaceNav0@localhost SpaceNav0@localhost ANALOG -1
     
         