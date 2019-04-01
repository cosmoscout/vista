

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	DemoEvent.cpp
	DemoEvent.h
	DemoHandler.cpp
	DemoHandler.h
	EventDemoAppl.cpp
	EventDemoAppl.h
	GreedyObserver.cpp
	GreedyObserver.h
	ObserverRegisterCallback.cpp
	ObserverRegisterCallback.h
	ProcessCallback.cpp
	ProcessCallback.h
	TimeObserver.cpp
	TimeObserver.h
	main.cpp
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

