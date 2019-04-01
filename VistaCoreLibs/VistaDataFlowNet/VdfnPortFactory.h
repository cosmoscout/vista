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


#ifndef _VDFNPORTFACTORY_H
#define _VDFNPORTFACTORY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include <map>
#include <string>
#include <cassert>

#include "VdfnPort.h"
#include <VistaAspects/VistaConversion.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

// from device drivers
class VistaSensorMeasure;
// from aspects
class IVistaPropertyGetFunctor;
class IVistaPropertySetFunctor;
class IVistaReflectionable;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * the port factory is an important part of the reflection interface of the DFN
 * and its ability to create new ports for dynamic nodes based on a higher
 * level description. Usually, there is no need to deal with the port factory
 * for normal nodes. Some use cases are
 * - create a typed port for an IVistaPropertyGetFunctor or IVistaPropertySetFunctor.
     For basic types, the port creators already exists. Users need to register
     a new port creator when new port -> types <- have to be used.
 * - need symbolic accessor to port values without trying to deduce their type
     by downcasting.
 *
 */
class VISTADFNAPI VdfnPortFactory
{
public:
	/**
	 * basic interface for port creation, this is meant to be subclassed
	 */
	class VISTADFNAPI CPortCreationMethod
	{
	public:
		virtual ~CPortCreationMethod() {}

		/**
		 * unconditional and no parameters. subclass and register.
		 * @return a port of a proper type
		 */
		virtual IVdfnPort *CreatePort() = 0;
	};

	/**
	 * when there is port types, there are port-type compares.
	 */
	class VISTADFNAPI CPortTypeCompareCreationMethod
	{
	public:
		virtual ~CPortTypeCompareCreationMethod() {}
		/**
		 * unconditional and no parameters. subclass and register.
		 * @return a port type compare
		 */
		virtual IVdfnPortTypeCompare *CreatePortTypeCompare() = 0;
	};

	/**
	 * base class for set functors.
	 * basically, this is a transparent way of setting a value to a port
	 * which type is unknown.
	 * As one can see from the signature of Set(), its main purpose is
	 * connected to device drivers and sensor measures. Usually, users do
	 * not have to deal with that.
	 * The CPortSetFunctor is basically a function that uses the history's
	 * dictionary to look up a symbol in the data and use a get functor to
	 * retrieve the value, which is then set to the port.
	 */
	class VISTADFNAPI CPortSetFunctor
	{
	public:
		virtual ~CPortSetFunctor() {}
		/**
		 * @param pMeasure the measure to use for the set.
		 * @param pGet the property GET to retrieve a value from
		 * @param pPort the port to set the value to
		 * @param nTs the timestamp to set for the port
		 * @param nIndex set unequal to ~0 to indicate that the
		          property to set is an indexed set. Else it is
		          a "value set".
		 * @return true when a new value was set to pPort
		 */
		virtual bool Set( const VistaSensorMeasure *pMeasure,
					  IVistaPropertyGetFunctor *pGet,
					  IVdfnPort *pPort,
					  double nTs,
					  unsigned int nIndex) = 0;
	};

	class VISTADFNAPI CPortGetFunctor
	{
	public:
		virtual ~CPortGetFunctor() {}

		virtual bool Set(
			          IVistaReflectionable *pO,
			          IVistaPropertySetFunctor *pSet,
					  IVdfnPort *pPort,
					  double nTs,
					  unsigned int nIndex) = 0;
		virtual bool Get(
			          IVistaReflectionable *pO,
			          IVistaPropertyGetFunctor *pGet,
					  IVdfnPort *pPort,
					  double nTs,
					  unsigned int nIndex) = 0;
	};

	class VISTADFNAPI IPortStringGet
	{
	public:
		virtual ~IPortStringGet() {}
		virtual std::string GetValueAsString( IVdfnPort* pPort ) = 0;
	};



	class VISTADFNAPI CPortAccess
	{
	public:
		CPortAccess( CPortCreationMethod *pMethod,
					 CPortSetFunctor     *pSetFunctor,
					 IPortStringGet		 *pStringGet = NULL )
					 : m_pCreationMethod(pMethod),
					   m_pSetFunctor(pSetFunctor),
					   m_pStringGet( pStringGet )
		{
		}


		CPortCreationMethod *m_pCreationMethod;
		CPortSetFunctor     *m_pSetFunctor;
		IPortStringGet		*m_pStringGet;
	};

	class VISTADFNAPI StringGet
	{
	public:
		virtual ~StringGet() {}
		virtual bool GetStringValue(IVistaPropertyGetFunctor *,
								const VistaSensorMeasure *pMeasure,
								std::string &strOut) = 0;
	};

	class VISTADFNAPI CFunctorAccess
	{
	public:
			CFunctorAccess(	CPortTypeCompareCreationMethod *pMethod,
					 CPortGetFunctor     *pGetFunctor,
					 CPortCreationMethod *pPortCreate,
					 StringGet *pGet = NULL)
					 : m_pCreationMethod(pMethod),
					   m_pGetFunctor(pGetFunctor),
					   m_pPortCreationMethod(pPortCreate),
					   m_pGet(pGet)
		{
		}

		CPortTypeCompareCreationMethod *m_pCreationMethod;
		CPortGetFunctor     *m_pGetFunctor;
		CPortCreationMethod *m_pPortCreationMethod;
		StringGet          *m_pGet;
	};

	static VdfnPortFactory *GetSingleton();


	bool         AddPortAccess( IVistaPropertyGetFunctor *, CPortAccess *pPortAccess );
	CPortAccess *GetPortAccess( IVistaPropertyGetFunctor * ) const;
	bool         AddPortAccess( const std::string &, CPortAccess *pPortAccess );
	CPortAccess *GetPortAccess( const std::string & ) const;

	CFunctorAccess *GetFunctorAccess( IVistaPropertySetFunctor * ) const;
	bool AddFunctorAccess( IVistaPropertySetFunctor *, CFunctorAccess *pFuncAccess );
	bool AddFunctorAccess( const std::string &, CFunctorAccess *pFuncAccess );
	CFunctorAccess *GetFunctorAccess( const std::string & ) const;


protected:
	VdfnPortFactory();
	~VdfnPortFactory();
private:

	std::map<std::string, CPortAccess*> m_mpPortCreation;
	std::map<std::string , CFunctorAccess*> m_mpFuncCreation;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/**
 * Utility template, simply creates a typed templated port of type T
 */
template<class T>
class VdfnTypedPortCreate : public VdfnPortFactory::CPortCreationMethod
{
public:
	virtual IVdfnPort *CreatePort() { return new TVdfnPort<T>; }
};


/**
 * Utility class template, simply creates a typed templated port-type compare
 * of type T
 */

template<class T>
class VdfnTypedPortTypeCompareCreate : public VdfnPortFactory::CPortTypeCompareCreationMethod
{
public:
	virtual IVdfnPortTypeCompare *CreatePortTypeCompare()
	{
		return new TVdfnPortTypeCompare< TVdfnPort<T> >;
	}
};

/**
 * Utility class template to retrieve a port's value as sting
 */

template<class T>
class VdfnTypedPortStringGet : public VdfnPortFactory::IPortStringGet
{
public:

	virtual std::string GetValueAsString( IVdfnPort* pPort ) 
	{
		TVdfnPort<T>* pTypedPort = dynamic_cast<TVdfnPort<T>* >( pPort );
		assert( pTypedPort != NULL );
		return VistaConversion::ToString( pTypedPort->GetValueConstRef() );
	}

};

#endif //_VDFNPORTFACTORY_H
