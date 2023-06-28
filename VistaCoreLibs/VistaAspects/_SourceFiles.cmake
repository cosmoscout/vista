

set( RelativeDir "." )
set( RelativeSourceGroup "Source Files" )

set( DirFiles
	VistaAspectsConfig.h
	VistaAspectsUtils.cpp
	VistaAspectsUtils.h
	VistaConversion.cpp
	VistaConversion.h
	VistaDeSerializer.cpp
	VistaDeSerializer.h
	VistaExplicitCallbackInterface.cpp
	VistaExplicitCallbackInterface.h
	VistaGenericFactory.h
	VistaLocatable.cpp
	VistaLocatable.h
	VistaMarshalledObjectFactory.cpp
	VistaMarshalledObjectFactory.h
	VistaNameable.cpp
	VistaNameable.h
	VistaObjectRegistry.cpp
	VistaObjectRegistry.h
	VistaObserveable.cpp
	VistaObserveable.h
	VistaObserver.cpp
	VistaObserver.h
	VistaProperty.cpp
	VistaProperty.h
	VistaPropertyAwareable.cpp
	VistaPropertyAwareable.h
	VistaPropertyFunctor.cpp
	VistaPropertyFunctor.h
	VistaPropertyFunctorRegistry.cpp
	VistaPropertyFunctorRegistry.h
	VistaPropertyList.cpp
	VistaPropertyList.h
	VistaReferenceCountable.cpp
	VistaReferenceCountable.h
	VistaReflectionable.cpp
	VistaReflectionable.h
	VistaSerializable.cpp
	VistaSerializable.h
	VistaSerializer.cpp
	VistaSerializer.h
	VistaSimpleCallback.h
	VistaSimpleCallback.cpp
	VistaSharedCore.cpp
	VistaSharedCore.h
	VistaSharedCoreOwner.cpp
	VistaSharedCoreOwner.h
	VistaTransformable.cpp
	VistaTransformable.h
	VistaAspectsMain.cpp
	VistaUncopyable.h
	_SourceFiles.cmake
)
set( DirFiles_SourceGroup "${RelativeSourceGroup}" )

set( LocalSourceGroupFiles  )
foreach( File ${DirFiles} )
	list( APPEND LocalSourceGroupFiles "${RelativeDir}/${File}" )
	list( APPEND ProjectSources "${RelativeDir}/${File}" )
endforeach()
source_group( ${DirFiles_SourceGroup} FILES ${LocalSourceGroupFiles} )

