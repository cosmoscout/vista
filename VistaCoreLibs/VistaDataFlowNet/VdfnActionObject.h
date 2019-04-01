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


#ifndef _VDFNACTIONOBJECT_H
#define _VDFNACTIONOBJECT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <iostream>

#include "VdfnConfig.h"

#include <VistaAspects/VistaReflectionable.h>
#include "VdfnPortFactory.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This interface defines a base class for 'application space' objects that
 * can be used inside a DFN graph. The usage is meant twofold.
 * -# model you business logic with action objects. That means, use them just
      as you would use an IVistaReflectionable: observe it, use its properties,
      call its API, the whole thing.
 * -# use the symbolic GET and SET functionality of the IVistaReflectionable
      to automatically create nodes that have IVistaReflectionable getters
      and setters as outports and inports respectively.
 * So the idea is simple:
 * - subclass the IVdfnActionObject
 * - add getter and setter interfaces as you like, just as with doing it with
     an IVistaReflectionable. Note though, that you can <b>not</b> use the
     traditional getter/setter template macros from the IVistaReflectionable
     API, as they are missing type reflection. Use the IActionGet
     and IActionSet interfaces instead. They follow the same syntax,
     so it should be easy for you to define your own creation methods if you
     need advanced support. Examples are TActionGetFunctor or TActionSetFunctor.
 *
 * The interface offers a Pull() method. You can use that in case the object
 * state is determined by some internal forces, for example a timer, and not
 * only determined by calls to SET.
 *
 * The rational behind the interface is simple: you need only one node type to
 * operate on a wide variety of objects, and it is encouraged to model explicit
 * domain logic, and not operate on node and graph level.
 *
 * In order to search the functors that can be used with an VdfnActionNode
 * you can either iterate over the VistaFunctorRegistry masking out the base
 * class tag of the functors or use the IVdfnActionObject::GetActionSet() or
 * IVdfnActionObject::GetActionGet() methods to retrieve the functors with an
 * appropriate type.
 *
 * @see VdfnObjectRegistry
 * @see VdfnActionNode
 */
class VISTADFNAPI IVdfnActionObject : public IVistaReflectionable
{
public:
	virtual ~IVdfnActionObject();


	/**
	 * basic interface to define a special property for that can
	 * be used by the VdfnActionNodes. It behaves like a IVistaReflectionable
	 * property GET, but if the GET is not an IActionGet, it can not be
	 * used by the VdfnActionNode, as the normal interface lacks
	 * the type information of the property (it only conserves the name
	 * and the method to get access to it). Note that we use RTTI here
	 * to determine the type of the GET.
	 * @see GetGetterType()
	 */
	class VISTADFNAPI IActionGet : public IVistaPropertyGetFunctor
	{
	public:
		/**
		 * @param sPropname the (case sensitive) name for the property
		 * @param the IVistaReflectionable class name
		 * @param sDescription the human readable description
		 * @param nMsgTag uh... dunno... probably deprecated?
		 * @todo clearify on the nMsgTag argument
		 */
		IActionGet(const std::string &sPropname,
								const std::string &sClassName,
								const std::string &sDescription = "<none>",
								unsigned int nMsgTag = ~0)
								: IVistaPropertyGetFunctor(sPropname, sClassName, sDescription),
								  m_nMsgTag(nMsgTag)
		{
		}

		/**
		 * returns the RTTI type_info of the value type that can be
		 * accessed by this IActionGet.
		 * @see TActionGet
		 */
		virtual const std::type_info &GetGetterType() const = 0;
		unsigned int GetMessageTag() const { return m_nMsgTag; }
	private:
		unsigned int m_nMsgTag;
	};

	/**
	 * a template to simplify the creation of new action get objects.
	 * the only thing that has to be redefined is the GetGetterType() in
	 * principle. Here, GetValue() methods are given as well, to simplify
	 * a directly and typed access to the value, avoiding the IVistaPropertyAwareable
	 * interface a bit.
	 */
	template<class T>
	class TActionGet : public IActionGet
	{
	public:
		TActionGet(const std::string &sPropname,
								const std::string &sClassName,
								const std::string &sDescription = "<none>",
								unsigned int nMsgTag = ~0)
								: IActionGet(sPropname, sClassName, sDescription, nMsgTag)
		{
		}

		/**
		 * return the value on stack. Note that this might (depending on the type)
		 * involve a copy operation.
		 */
		virtual T    GetValue( const IVdfnActionObject &oObj ) const = 0;

		/**
		 * return the value as a set to the parameter oValue. May be cheaper
		 * a bit.
		 * @return true when the value of oValue was changed during the operation
		 */
		virtual bool GetValue( const IVdfnActionObject &oObj, T &oValue ) const = 0;

		/**
		 * return the typeid structure of RTTI for T
		 */
		virtual const std::type_info &GetGetterType() const { return typeid( T ); };

	private:
	};

	/**
	 * basic interface to define a special property for that can
	 * be used by the VdfnActionNodes. It behaves like a IVistaReflectionable
	 * property SET, but if the SET is not an IActionSet, it can not be
	 * used by the VdfnActionNode, as the normal interface lacks
	 * the type information of the property (it only conserves the name
	 * and the method to get access to it). Note that we use RTTI here
	 * to determine the type of the SET.
	 * @see IActionGet
	 */
	class VISTADFNAPI IActionSet : public IVistaPropertySetFunctor
	{
	public:
		/**
		 * @param sPropname the name of the property for the SET
		 * @param sClassName the name of the class to attach the property to
		 * @param sDecription the human-readable explanation of what to set here
		 */
		IActionSet(const std::string &sPropname,
								const std::string &sClassName,
								const std::string &sDescription = "<none>")
								: IVistaPropertySetFunctor(sPropname, sClassName, sDescription)
		{
		}

		/**
		 * unimplemented. by default this means that an action set is <b>not</b>
		 * working with the regular IVistaPropertyAwareable interface, as it is
		 * not capable of delivering the values as strings. Subclasses may redefine this.
		 */
		virtual bool operator()(IVistaPropertyAwareable &, const VistaProperty &)
		{
			return false;
		}

		/**
		 * return the RTTI type of the setter functor. Needed for a matching between
		 * the port types of an VdfnActionNode and the functor needed to set the values.
		 */
		virtual const std::type_info &GetSetterType() const = 0;
	};

	/**
	 * template to simplify the usage of the IActionSet interface.
	 */
	template<class T>
	class TActionSet : public IActionSet
	{
	public:
		TActionSet(const std::string &sPropname,
				   const std::string &sClassName,
				   const std::string &sDescription = "<none>")
		: IActionSet(sPropname, sClassName, sDescription)
		{
		}

		/**
		 * set a value to the action object, using a const reference of T
		 * @return true when the value was successfully set, false else
		 */
		virtual bool SetValue( IVdfnActionObject &, const T & ) = 0;

		/**
		 * set the value using a non-reference value of T. Sadly, C++ is not
		 * able (with templates) to differentiate a reference and a value type,
		 * so the method has a different name. (either it is C++ or me is stoopit,
		 * in case you know it better: feel free to fix it)
		 * @see SetValue()
		 */
		virtual bool SetValueNoRef( IVdfnActionObject &oObj, T val)
		{
			return SetValue(oObj, val);
		}

		/**
		 * returns the RTTI type of the setter functor.
		 */
		virtual const std::type_info &GetSetterType() const { return typeid( T ); }
	};


	/**
	 * @return an IActionGet for property named strSymbol or NULL if this
	           no get functor for this symbol is either present or is simply
	           not an action get
	 */
	IActionGet *GetActionGet( const std::string &strSymbol ) const;

	/**
	 * @return an IActionSet for property named strSymbol or NULL if this
	           no set functor for this symbol is either present or is simply
	           not an action set
	 */
	IActionSet *GetActionSet( const std::string &strSymbol ) const;


	/**
	 * hack-like API which is used in conjunction with the VdfnActionNode
	 * update propagation mechanism. In case your IVdfnActionObject determines
	 * a state change and does <b>NOT</b> propagate that using the notification
	 * mechanism, overload and return true here when this is needed.
	 * @return default: false, true indicates a dirty state that can trigger
	           the VdfnActionNode update.
	 */
	virtual bool Pull();

	/**
	 * SetUpdateTimeStamp is called during the evaluation of an ActionNode instance
	 * after inports have been updated, but before the outports are updated.
	 * This Method is called whether or not an inport was changed, but passes a bool
	 * to indicate this.
	 * NOTE: This method can be called from different ActionNode instances, if the ActionObject
	 * is referenced in multiple interaction contexts

	 */
	virtual void SetUpdateTimeStamp( const VistaType::microtime dTimestamp, const bool bInportsChanged );
	VistaType::microtime GetUpdateTimeStamp() const;


	REFL_INLINEIMP(IVdfnActionObject, IVistaReflectionable);
protected:
	IVdfnActionObject();

private:
	mutable std::list<std::string> m_liBaseListCache;
	VistaType::microtime m_dUpdateTimeStamp;
};


// #####################################################################
// GETFUNCTORS
// #####################################################################

/**
 * use this to declare a get functor
 * @param C the class of your IVdfnActionObject instance
 * @param Ret the return type defined for the TActionGet template
 */
template<class C, class Ret>
class TActionGetFunctor : public IVdfnActionObject::TActionGet<Ret>
{
public:

	typedef Ret (C::*GetF)() const; /**< pointer to a function of class C, returning Ret */
	typedef std::string (*ConvFunc)( const Ret & ); /*<< conversion function from Ret to string */

	/**
	 * @param fct the GET function pointer
	 * @param cFct a (floating) conversion function from Ret to string
	 */
	TActionGetFunctor( GetF fct,
		ConvFunc cFct,
		const std::string &strPropName,
		const std::string &strClassName,
		const std::string &strDescriptor )
		// note on the syntax here:
		// gcc does (correctly) not recognize the base TActionSet<T>
		// without a proper qualification, as it does not know during
		// compile time where the nested TActionSet<T> is coming from,
		// so we hint it explicitly at the nested scope of IVdfnActionObject
		// msvc seems to accept the code, but this seems to be another
		// one of the m$ types of working with stoopid programmers ;)
	: IVdfnActionObject::TActionGet<Ret>(strPropName, strClassName, strDescriptor),
		  m_Fct(fct),
		  m_cFct(cFct)
	{
	}

	virtual bool operator()(const IVistaPropertyAwareable &oObj, VistaProperty &oProp) const
	{
		if(m_cFct == NULL) // without the conversion function, it does not make much sense to continue
			return false;

		// we have to claim a value
		// note that this can raise an exception, when trying to assign a NULL reference
		const IVdfnActionObject &oAcObj = dynamic_cast<const IVdfnActionObject&>(oObj);
		Ret val;
		if(GetValue(oAcObj,val)) // get as side effect, true -> change
		{
			// ok, this worked
			// but here we are: convert to a string using the m_cFct, provided during
			// construction and set the value in the prop provided
			oProp.SetValue( (*m_cFct)(val) );
			return true;
		}
		return false; // get value failed?
	}


	virtual Ret GetValue(const IVdfnActionObject &oObj) const
	{
		Ret value;
		GetValue(oObj, value);
		return value;
	}

	/**
	 * call the getter straightforward.
	 * @return true
	 */
	virtual bool GetValue( const IVdfnActionObject &oObj, Ret &oValue ) const
	{
		oValue = (static_cast<const C&>(oObj).*m_Fct)();
		return true;
	}


private:
	GetF m_Fct;
	ConvFunc m_cFct;
};

// #####################################################################
// SETFUNCTORS
// #####################################################################

/**
 * @param C the class that contains the set function in its interface
 * @param Ret the return value (type of the setter signature element)
 * @param ArgT the argument type (can be different to Ret, for example const Ret&)
 */
template<class C, class Ret, class ArgT>
class TActionSetFunctor : public IVdfnActionObject::TActionSet<Ret>
{
public:
	typedef bool (C::*SetF)( ArgT ); /**< the pointer to the set function */
	typedef Ret (*ConvFunc)( const std::string & ); /**< conversion function from string to Ret */

	TActionSetFunctor( SetF fct,
		ConvFunc cFct,
		const std::string &strPropName,
		const std::string &strClassName,
		const std::string &strDescriptor )
			// note on the syntax here:
			// gcc does (correctly) not recognize the base TActionSet<T>
			// without a proper qualification, as it does not know during
			// compile time where the nested TActionSet<T> is coming from,
			// so we hint it explicitly at the nested scope of IVdfnActionObject
			// msvc seems to accept the code, but this seems to be another
			// one of the m$ types of working with stoopid programmers ;)
	: IVdfnActionObject::TActionSet<Ret>(strPropName, strClassName, strDescriptor),
		  m_Fct(fct),
		  m_cFct(cFct)
	{
	}


	virtual bool operator()(IVistaPropertyAwareable &oObj, const VistaProperty &oProp)
	{
		if(m_cFct == NULL) // not much sense continuing without conversion function
			return false;

		IVdfnActionObject &oAct = dynamic_cast<IVdfnActionObject&>(oObj);
		Ret value = (*m_cFct)(oProp.GetValue()); // get string, convert to type
		return SetValue( oAct, value );
	}


	/**
	 * simply call the set function on the value
	 */
	bool SetValue( IVdfnActionObject &oObj, const Ret &value )
	{
		return (static_cast<C&>(oObj).*m_Fct)(value);
	}
private:
	SetF m_Fct;
	ConvFunc m_cFct;
};

/**
 * needed for generic port access with type safety.
 * it basically defines two methods
 * -# Set(): having an reflectionable and a property set functor <i>and</i>
             a port to get the value from (+ timestamp and optional index),
             how to get the value from the port into the object
 * -# Get(): having the same for a SET, how to read it off the object and
             stuff it into a port
 *
 * The code is always the same, only the type differs.
 */
template<class T>
class TActionSet : public VdfnPortFactory::CPortGetFunctor
{
public:
	virtual bool Set(
		          IVistaReflectionable *pObj,
		          IVistaPropertySetFunctor *pSet,
				  IVdfnPort *pPort,
				  double nTs,
				  unsigned int nIndex)
	{
		// downcast to a proper type
		// We could use a static_cast
		// then, I hear you say... well, basically, but we were screwed
		// in case the IVdfnActionObject specialization was constructed
		// using multiple inheritance. In that case, we might end up with
		// a bad pointer here, not NULL, but wrong. Using dc<> is safe to go,
		// even at the cost of a few cycles.
		typename IVdfnActionObject::TActionSet<T> *pActionSet
			= dynamic_cast<IVdfnActionObject::TActionSet<T>*>(pSet);

		// do the same for the port, note that we assume a TVdfnPort<> here,
	    // not a IVdfnPort
		TVdfnPort<T> *pT = dynamic_cast<TVdfnPort<T>*>(pPort);
		if(pActionSet && pT) // sanity check
		{
			// worth to continue...
			IVdfnActionObject *pAObj = dynamic_cast<IVdfnActionObject*>(pObj);
			if(pAObj) // action object?
			{
				// yes, simply call SetValue()
				return pActionSet->SetValue( *pAObj, pT->GetValue() );
			}
		}
		return false; // no new value was set
	}

	/**
	 * Take the value from the get and set it to the port.
	 * @todo check on nIndex
	 */
	virtual bool Get(
		          IVistaReflectionable *pObj,
		          IVistaPropertyGetFunctor *pGet,
				  IVdfnPort *pPort,
				  double nTs,
				  unsigned int nIndex)
	{
		typename IVdfnActionObject::TActionGet<T> *pActionGet
			= dynamic_cast<IVdfnActionObject::TActionGet<T>*>(pGet);

		TVdfnPort<T> *pT = dynamic_cast<TVdfnPort<T>*>(pPort);
		if(pActionGet && pT)
		{
			IVdfnActionObject *pAObj = dynamic_cast<IVdfnActionObject*>(pObj);
			if(pAObj)
			{
				return pT->SetValue( pActionGet->GetValue(*pAObj), nTs );
			}
		}
		return false;
	}
};

/**
 * need a special template for bool types, as somehow it is not possible
 * to reference on bools(?!)
 */
// template<>
// class TActionSet<bool> : public VdfnPortFactory::CPortGetFunctor
// {
// public:
// 	virtual bool Set(
// 		          IVistaReflectionable *pObj,
// 		          IVistaPropertySetFunctor *pSet,
// 				  IVdfnPort *pPort,
// 				  double nTs,
// 				  unsigned int nIndex)
// 	{
// 		IVdfnActionObject::TActionSet<bool> *pActionSet
// 			= dynamic_cast<IVdfnActionObject::TActionSet<bool>*>(pSet);
// 
// 		TVdfnPort<bool> *pT = dynamic_cast<TVdfnPort<bool>*>(pPort);
// #if defined(DEBUG)
// 		if(pT == NULL)
// 			vstr::errp() << "orig type = " << typeid( *pPort ).name() << std::endl;
// #endif
// 		if(pActionSet && pT)
// 		{
// 			IVdfnActionObject *pAObj = dynamic_cast<IVdfnActionObject*>(pObj);
// 
// 			if(pAObj)
// 			{
// 				return pActionSet->SetValueNoRef( *pAObj, pT->GetValue() );
// 			}
// 		}
// #if defined(DEBUG)
// 		else
// 		{
// 			vstr::errp() << "Obj: " << pObj->GetNameForNameable() << "\n";
// 			vstr::erri() << vstr::singleindent
// 					<< "pActionSet == " << pActionSet << " ; pT == " << pT << std::endl;
// 			vstr::erri() << vstr::singleindent
// 					<< (pActionSet ? pActionSet->GetNameForNameable() : "<none>") << std::endl;
// 		}
// #endif
// 
// 		return false;
// 	}
// 
// 	/**
// 	 * @todo check on this... simply not implemented?
// 	 */
// 	virtual bool Get(
// 		          IVistaReflectionable *pObj,
// 		          IVistaPropertyGetFunctor *pSet,
// 				  IVdfnPort *pPort,
// 				  double nTs,
// 				  unsigned int nIndex)
// 	{
// 		typename IVdfnActionObject::TActionGet<T> *pActionGet
// 			= dynamic_cast<IVdfnActionObject::TActionGet<T>*>(pGet);
// 
// 		TVdfnPort<T> *pT = dynamic_cast<TVdfnPort<T>*>(pPort);
// 		if(pActionGet && pT)
// 		{
// 			IVdfnActionObject *pAObj = dynamic_cast<IVdfnActionObject*>(pObj);
// 			if(pAObj)
// 			{
// 				return pT->SetValue( pActionGet->GetValue(*pAObj), nTs );
// 			}
// 		}
// 		return false;
// 	}
// };
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNACTIONOBJECT_H

