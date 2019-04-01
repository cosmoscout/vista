@ECHO OFF
REM In this file, we start a the demo to record data to a folder "testrecord"

SET CURRENT_PATH=%PATH%

CALL set_path_for_22RecordReplayDemo.bat

CALL 22RecordReplayDemo.exe -record testrecord

set PATH=%CURRENT_PATH%