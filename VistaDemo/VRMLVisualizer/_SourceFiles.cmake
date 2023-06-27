

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	DecreaseCallback.cpp
	DecreaseCallback.h
	EnDisableCallback.cpp
	EnDisableCallback.h
	IncreaseCallback.cpp
	IncreaseCallback.h
	main.cpp
	ResetCallback.cpp
	ResetCallback.h
	ToggleAnimationCallback.cpp
	ToggleAnimationCallback.h
	VrmlEventHandler.cpp
	VrmlEventHandler.h
	VrmlVisualizer.cpp
	VrmlVisualizer.h
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

