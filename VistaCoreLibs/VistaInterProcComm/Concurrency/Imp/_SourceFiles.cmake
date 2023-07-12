

set( RelativeDir "./Concurrency/Imp" )
set( RelativeSourceGroup "Source Files\\Concurrency\\Imp" )

set( DirFiles
	VistaMutexImp.cpp
	VistaMutexImp.h
	VistaPosixProcessImp.cpp
	VistaPosixProcessImp.h
	VistaPosixSemaphoreImp.cpp
	VistaPosixSemaphoreImp.h
	VistaPosixThreadEventImp.cpp
	VistaPosixThreadEventImp.h
	VistaPosixProcessEventImp.cpp
	VistaPosixProcessEventImp.h
	VistaPriorityImp.cpp
	VistaPriorityImp.h
	VistaProcessImp.cpp
	VistaProcessImp.h
	VistaProcessEventImp.cpp
	VistaProcessEventImp.h
	VistaPthreadReaderWriterLockImp.cpp
	VistaPthreadReaderWriterLockImp.h
	VistaPthreadThreadEventImp.cpp
	VistaPthreadThreadEventImp.h
	VistaPthreadsMutexImp.cpp
	VistaPthreadsMutexImp.h
	VistaPthreadsPriorityImp.cpp
	VistaPthreadsPriorityImp.h
	VistaPthreadsThreadConditionImp.cpp
	VistaPthreadsThreadConditionImp.h
	VistaPthreadsThreadImp.cpp
	VistaPthreadsThreadImp.h
	VistaReaderWriterLockImp.cpp
	VistaReaderWriterLockImp.h
	VistaSemaphoreImp.cpp
	VistaSemaphoreImp.h
	VistaThreadConditionImp.cpp
	VistaThreadConditionImp.h
	VistaThreadEventImp.cpp
	VistaThreadEventImp.h
	VistaThreadImp.cpp
	VistaThreadImp.h
	VistaWin32MutexImp.cpp
	VistaWin32MutexImp.h
	VistaWin32PriorityImp.cpp
	VistaWin32PriorityImp.h
	VistaWin32ProcessImp.cpp
	VistaWin32ProcessImp.h
	VistaWin32ProcessEventImp.cpp
	VistaWin32ProcessEventImp.h
	VistaWin32SemaphoreImp.cpp
	VistaWin32SemaphoreImp.h
	VistaWin32ThreadConditionImp.cpp
	VistaWin32ThreadConditionImp.h
	VistaWin32ThreadEventImp.cpp
	VistaWin32ThreadEventImp.h
	VistaWin32ThreadImp.cpp
	VistaWin32ThreadImp.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

