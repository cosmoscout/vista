DTrackSDK: functions to receive and process DTrack UDP packets (ASCII protocol)
Copyright (C) 2000-2007, Advanced Realtime Tracking GmbH


How to receive and process DTrack tracking data
-----------------------------------------------

DTrack uses ethernet (UDP/IP datagrams) to send measurement data
to other applications. There are two different types of data formats
available: an ASCII one and a binary. We recommend to use the ASCII
version. Binary data contain just basic results.

DTrack supplies the possibility to control a reduced set of
functions through remote commands via ethernet. This is done by
sending UDP datagrams to DTrack, that contain short command strings (ASCII).

The formats and all other necessary definitions are described in
'ARTtrack & DTrack Manual: Technical Appendix'.


Sample source codes for an own interface
----------------------------------------

The 'DTrackSDK' sample source codes show how to receive UDP packets from
DTrack (only ASCII protocol) and to convert them into easier to handle data.
They are available in C as well as C++, and should work for both Unix and Windows.
The programs have successfully been tested under Linux, Irix, Windows 2000 and
Windows XP.

The library is distributed under the GNU LGPL (GNU Lesser General Public
License). You can modify and/or include the sources into own software (for
details see 'license_lgpl.txt'). 

- dtrack.h     : C library of routines for receiving and processing
  dtrack.c       DTrack tracking data (only ASCII format), and for
                 sending command strings to DTrack

- DTrack.hpp   : C++ class for receiving and processing
  DTrack.cpp     DTrack tracking data (only ASCII format), and for
                 sending command strings to DTrack

- example_without_remote_control.c     : sample without usage of remote commands (C)
- example_with_remote_control.c        : sample with usage of remote commands (C)

- example_without_remote_control.cpp   : sample without usage of remote commands (C++)
- example_with_remote_control.cpp      : sample with usage of remote commands (C++)


The library 'DTrackSDK'
-----------------------

The library provides these routines:

- initialize the library; this has to be done before using other functions; needs
  informations about hostname or IP address and port number(s) of DTrack:
  C   : dtrack_init()
  C++ : DTrack()

- check if initialization was successfull:
  C++ : DTrack::valid()

- leave the library properly:
  C   : dtrack_exit()
  C++ : ~DTrack()

- check last receive/send error:
  C   : dtrack_timeout(), dtrack_udperror(), dtrack_parseerror()
  C++ : DTrack::timeout(), DTrack::udperror(), DTrack::parseerror()

- receive and process one DTrack data packet (UDP; ASCII); the routine transfers the
  string into easier to use data structs:
  C   : dtrack_receive()
  C++ : DTrack::receive()

- getting data of the last received DTrack data packet:
  C++ : DTrack::get_framecounter(), DTrack::get_timestamp(), DTrack::get_num_body(), DTrack::get_body(),
        DTrack::get_num_flystick(), DTrack::get_flystick(), DTrack::get_num_meatool(), DTrack::get_meatool(),
        DTrack::get_num_hand(), DTrack::get_hand(), DTrack::get_num_marker(), DTrack::get_marker()

- send remote commands (UDP; ASCII) to DTrack:
  C   : dtrack_cmd_cameras(), dtrack_cmd_tracking(), dtrack_cmd_sending_data(),
        dtrack_cmd_sending_fixed_data()
  C++ : DTrack::cmd_cameras(), DTrack::cmd_tracking(), DTrack::cmd_sending_data(),
        DTrack::cmd_sending_fixed_data()


Advanced Realtime Tracking GmbH
Am Oeferl 6
D-82362 Weilheim
Germany

http://www.ar-tracking.de


