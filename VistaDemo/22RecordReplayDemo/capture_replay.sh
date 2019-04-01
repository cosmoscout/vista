#!/bin/bash

# In this file, we run the demo RecordReplayDemo with -replay, which replays a prevously recorded session and renders to an offscreen buffer window.
# The offscreen buffer allows rendering with a higher resolution than the screen. The rendered images are captured at a fixed framerate of 30Hz.

source ./set_path_for_22RecordReplayDemo.sh

./22RecordReplayDemo -displayini display_desktop_offscreen.ini -replay testrecord -capture_frames_with_framerate 30 -capture_frames_filename testrecord_capture/screenshot_%S%.jpg

# If you have ffmpeg, you can use the following command to convert the images to a corresponding video
# ffmpeg  -r 30 -i testrecord_capture/screenshot_%06d.jpg -r 30 -b:v 6M -an capture.mp4