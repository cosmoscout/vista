@ECHO OFF
REM In this file, we run the demo RecordReplayDemo with -replay, which replays a prevously recorded session

SET CURRENT_PATH=%PATH%

CALL set_path_for_22RecordReplayDemo.bat

CALL 22RecordReplayDemo.exe -replay testrecord

set PATH=%CURRENT_PATH%