Welcome to the Vista RecordReplayDemo
Vista's recording utilizes the Cluster mode to save the whole interaction to file, and allows a session recorded that way to be replayed later.
Recording works by intercepting all data that is distributed from master to slaves by ClusterDataSyncs and writing it to a file.
This is possible for either Standalone mode (in which case, the Standalone is changed to a Master without slaves) or as a NewClusterMaster.
During replay, a ReplaySlave is created, which simply reads the data from the replay files, which produces the same slave behavior as before.

What this can achieve:
Replay the exact recorded interaction of a cluster-synchronous application
Replay with different binaries (32bit and 64bit may be compatible, but don't have to) on a different computer (even a different platform, unless it has a different endianess) 

What this can not achieve:
Replay non-cluster-synchronous applications
Replay applications with manually synchronized data, e.g. over DataTunnels or direct IP connections (butSynchronization over ExternalMsgEvents or VistaClusterDataSync are pssible)
Change the interaction of the recorded application

Things to note:
When replaying, you can change some elements of the application. In general, you can change anything that (a) does not change the interaction behavior and (b) does not change the transferreddata
 - You can modify the display configuration (unless you determine interaction-dependent params from the display, e.g. to convert mouse movement from pixels to navigation).
   This can be used, for example, to render hi-res screenshots or videosfrom offscreen buffers,
   or to render a "Point-Of-View"-video of what a person has seen in a CAVE by rendering to a display with HMD_MODE = TRUE,
   or to change the Field of View or Eye Offset, or to render Side-by-Side stereo videos, etc...
 - It is no problem to change 3D models, rendering algoriths, shaders etc. -- even if the high-quality rendering takes longer, the replay will still proceed per frame as before, so this is no problem
 - The replay is always a ClusterFollower
 - You can use the frame clock or the event times of SystemEvent, InteractionEvents and ExternalMessage events for synchronous times -- as usual -- but keep in mind that these represent the times of recording, not replay.
   Try not to overwrite your original log files with the replay ;) (Although, of course, by default output files should only be written on the ClusterLeader)
 - You have to provide the exact same InteractionContexts with the same DFN graphs
    - use the same interactionini as for the original record (note that, since replay works as a slave, you don't require the actual devices to be connected, but the drivers/transcoders must be available)
	- Do not disable any interaction contexts - if you do, the replay will throw an exception with the name of the missing context (enabling/adding new ones should not crash the app, but replaying ignores the new contexts)
	- Ensure that all dataflow net graphs are compatible to the recordings
	  - Easiest to achieve that: Use the same graphs
	  - More advanced: you can modify the graphs, but have to ensure that all MasterSim nodes stay untouched (and no new ones are added).
	  - Thus, you can, for example, add/remove text overlays, change the parameters, or even remove edges, add new non-mastersim nodes and thus, for example, replace an actual DriverSensor measure with a constant value (as long as the DriverSensorNode is still in the graph).
	  - If you do change the dfn nets in an invalid way, prepare for segfaults
	  
So how do you actually record and replay?

To start recording, either add an entry RECORD = foldername to the master section in the cluster ini, or use the command line argument "-record foldername".
This will create a folder of the chosen name (you can use the special tag %DATE% in the foldername, which will be replaced by a string representing YYMMDD_hhmmss), which will contain the record files.
Keep in mind that these may become rather large - typically a few GB per hour, but can be bigger if many or large sensor histories are used, or if big data is explicitely synced.

To replay a recorded session, pass the command line argument -record foldername.

These two options are show in the run scripts start_[record|replay].[sh|bat]

Additionally, the scripts capture_replay.[sh|bat] and capture_replay_sidebyside.[sh|bat] further show how to create video captures of recorded sessions.
For this, the displayini is changed to use an offscreen buffer and -- in the case of sidebyside rendering -- two viewports.
To periodically capture screenshots for videos, one can either modify the application to make the screenshots (either manually or using VistaFrameSeriesCapture), or use command line parameters.
The options are:
-capture_frames <n>                 Captures every n'th frame
-capture_frames_periodically <t>    Captures frame every t seconds
-capture_frames_with_framerate <f>  Captures frame at f Hz frequency framerate - this may capture multiple screenshots per frame to match the framerate, or skip frames. Best option for videos.
-capture_frames_filename <pattern>  Captured files are written with the according filename pattern - can contain subdirectories, file ending is optional (defaults to jpg).
									You can use the special markup tags (e.g. %S% or $S$ for screenshot count) -- see VistaFrameSeriesCapture for a documentation of these
									

you can also use the scripts provided with this demo to capture a recorded session
capture_replay.[sh|bat] captures screenshots from a large, multisampled offscreen buffer. This represents a high-quality video rendering.
capture_replay_sidebyside.[sh|bat] also c-capture_frames_with_framerate, which allows converting the images to a video, e.g. using ADobe Premeiere, ImagesToVideo or ffmpeg. (the linux-script capture_replay_sidebyside.sh will perform a conversion if ffmpeg is present)

IMPORTANT NODE
When using only the keyboard or spacenavigator navigation, the replayed sessions should look exactly like the original.
However, if you use the mouse-based trackball, you may notice that that the resulting replay differs from the original if the window sizes are changed.
This is because the trackball uses the window size to normalize its movement, and thus the interaction is no longer the same.
This shows one of the pitfalls or record-replay, and that sources of dyssonchronicity may be easy to miss. Keep an eye on this
	  

