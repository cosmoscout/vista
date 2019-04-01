@ECHO OFF
REM In this file, we run the demo RecordReplayDemo with -replay, which replays a prevously recorded session and renders to an offscreen buffer window.
REM The offscreen buffer allows rendering with a higher resolution than the screen. The rendered images are captured at a fixed framerate of 30Hz.
REM This variant uses a window with two viewports, allowing to render a side-by-side vide
REM NOTE: please read the comments in the README.txt concerning synchronicity

SET CURRENT_PATH=%PATH%

CALL set_path_for_22RecordReplayDemo.bat

CALL 22RecordReplayDemo.exe -displayini display_desktop_offscreen_sidebyside.ini -replay testrecord -capture_frames_with_framerate 30 -capture_frames_filename testrecord_sbs/screenshot_$S$.jpg

set PATH=%CURRENT_PATH%