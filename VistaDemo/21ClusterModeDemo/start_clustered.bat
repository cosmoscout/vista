@ECHO OFF
REM In this file, we will first start the slaves, and then the master

SET CURRENT_PATH=%PATH%

CALL set_path_for_21ClusterModeDemo.bat

ECHO starting slave 1
START 21ClusterModeDemo.exe -newclusterslave DesktopSlave1
ECHO starting slave 2
START 21ClusterModeDemo.exe -newclusterslave DesktopSlave2

REM We want to sleep two seconds to allow the slaves to start properly, before we
REM start the master. Since batch files don't have a sleep, we use dirty hacks
ECHO sleeping for 3 seconds
ping 127.0.0.1 -n 4 -w 1000 > nul

ECHO starting the master
START 21ClusterModeDemo.exe -newclustermaster DesktopMaster

set PATH=%CURRENT_PATH%