

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	ApplyColorActionObject.cpp
	ApplyColorActionObject.h
	ColorChangerDfnNode.cpp
	ColorChangerDfnNode.h
	RandomNumberDfnNode.cpp
	RandomNumberDfnNode.h
	SetPositionActionObject.cpp
	SetPositionActionObject.h
	PyDfnNodeCreate.cpp
	PyDfnNodeCreate.h
	PyDfnNode.cpp
	PyDfnNode.h
	main.cpp
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

