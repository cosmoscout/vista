/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/


#ifndef _VISTADRIVERGENERICPARAMETERASPECT_H
#define _VISTADRIVERGENERICPARAMETERASPECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriversConfig.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>


#include <VistaAspects/VistaReflectionable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTADEVICEDRIVERSAPI VistaDriverGenericParameterAspect
                          : public IVistaDeviceDriver::IVistaDeviceDriverAspect
{
public:
	class VISTADEVICEDRIVERSAPI IParameterContainer : public IVistaReflectionable
	{
		REFL_DECLARE
	public:
		virtual ~IParameterContainer();

		virtual bool TurnDef( bool def ) { return false; }
		virtual bool Apply() { return true; }
	protected:
		IParameterContainer();
	};

	class IContainerCreate
	{
	public:
		virtual ~IContainerCreate() {}
		virtual IParameterContainer *CreateContainer() = 0;
		virtual bool DeleteContainer( IParameterContainer *pContainer ) = 0;
	};


	VistaDriverGenericParameterAspect(IContainerCreate *pCreationFct);
	virtual ~VistaDriverGenericParameterAspect();


	IParameterContainer * GetParameterContainer() const;

	template<class T>
	T *GetParameter() const
	{
		return dynamic_cast<T*>( GetParameterContainer() );
	}

	// #########################################
	// OVERWRITE IN SUBCLASSES
	// #########################################
	static int  GetAspectId();
	static void SetAspectId(int);

	virtual void Print( std::ostream& ) const;

protected:
private:
	IContainerCreate    *m_pCreate;
	mutable IParameterContainer *m_pParams;
	static int m_nAspectId;
};

template<class parent>
class TParameterContainer : public VistaDriverGenericParameterAspect::IParameterContainer
{
	REFL_INLINEIMP( TParameterContainer, VistaDriverGenericParameterAspect::IParameterContainer );
public:
	TParameterContainer( parent *pparent )
	: VistaDriverGenericParameterAspect::IParameterContainer()
	, m_parent(pparent)
	{

	}
	parent *GetParent() const { return m_parent; }

private:
	parent *m_parent;
};

template<class T, class Kreator>
class TParameterCreate : public VistaDriverGenericParameterAspect::IContainerCreate
{
public:
	TParameterCreate( T *pDriver )
	: m_pDriver( pDriver )
	{

	}

	VistaDriverGenericParameterAspect::IParameterContainer *CreateContainer()
	{
		return new Kreator( m_pDriver );
	}

	virtual bool DeleteContainer( VistaDriverGenericParameterAspect::IParameterContainer *pCont )
	{
		delete pCont;
		return true;
	}

	T *m_pDriver;
};


#define PARAMETER_CLEANUP_FUNCTION( gettervarname, settervarname ) \
	static void releaseParameterProps() \
	{ \
	IVistaPropertyGetFunctor **git = gettervarname; \
	IVistaPropertySetFunctor **sit = settervarname; \
	\
		while( *git ) \
			delete *git++; \
	\
		while( *sit ) \
			delete *sit++; \
	}

#define CALL_PARAMETER_CLEANUP_FUNCTION releaseParameterProps();

#if !defined(WIN32)
 #define PARAMETER_CLEANUP( gettervarname, settervarname ) \
	static __attribute__ ((destructor)) void releaseParameterProps() ; \
	\
	static void releaseParameterProps() \
	{ \
	IVistaPropertyGetFunctor **git = gettervarname; \
	IVistaPropertySetFunctor **sit = settervarname; \
	\
		while( *git ) \
			delete *git++; \
	\
		while( *sit ) \
			delete *sit++; \
	}

#else // !WIN32
 #include <windows.h>
 #define PARAMETER_CLEANUP( gettervarname, settervarname ) \
	\
	static void releaseParameterProps(); \
	\
	BOOL APIENTRY DllMain( HANDLE hModule, \
						   DWORD  ul_reason_for_call, \
						   LPVOID lpReserved \
						 ) \
	{ \
		switch (ul_reason_for_call) \
		{ \
		case DLL_PROCESS_ATTACH: \
		case DLL_THREAD_ATTACH: \
		case DLL_THREAD_DETACH: \
			break; \
		case DLL_PROCESS_DETACH: \
			if( lpReserved == 0 ) \
				releaseParameterProps(); \
			break; \
		} \
		return TRUE; \
	} \
	static void releaseParameterProps() \
	{ \
	IVistaPropertyGetFunctor **git = gettervarname; \
	IVistaPropertySetFunctor **sit = settervarname; \
	\
		while( *git ) \
			delete *git++; \
	\
		while( *sit ) \
			delete *sit++; \
	}
#endif // !WIN32

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADRIVERGENERICPARAMETERASPECT_H


