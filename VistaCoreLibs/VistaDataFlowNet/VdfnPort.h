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


#ifndef _VDFNPORT_H
#define _VDFNPORT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnSerializer.h"

#include <VistaAspects/VistaSerializable.h>
#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>

#include <typeinfo>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VdfnPortSerializeAdapter;
class IVdfnPortTypeCompare;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Ports define the data storage and access area for IVdfnNode instances.
 * The interface is twofold
 * -# IVdfnPort for basic storage and management within nodes
 * -# TVdfnPort for storage of the data of the node and type checking
 *
 * A port can be (more precise: must be) serializable, in order to transport
 * a DataFlowNet graph over a network. Here, we chose to use an associative
 * serializer which can decide on the way to store the data for each port
 * type individually.
 *
 * IVdfnPorts can not be constructed directly, and IVdfnPort does not define
 * an API to actually set the value (besides the AssignFrom() API)
 */
class VISTADFNAPI IVdfnPort
{
	friend class VdfnPortSerializeAdapter;
public:
	virtual ~IVdfnPort();

	/**
	 * this is an alias to GetUpdateTimeStamp()
	 * @todo check whether this API is still needed and where...
	 * @return the timestamp of the last update
	 */
	double GetLastUpdate() const;

	/**
	 * sets the update timestmap for this port
	 * @param nTs the update timestamp to be set
	 */
	void   SetUpdateTimeStamp(double nTs);

	/**
	 * @return the update timestamp for this port
	 * @see SetUpdateTimeStamp()
	 */
	double GetUpdateTimeStamp() const;

	/**
	 * each port counts its revision, it is incremented on a call
	 * to IncUpdateCounter(), manually by user code. This enables,
	 * for example, to write to the memory of the port first and then
	 * trigger the update revision.
	 * @return the revision or update counter of this port
	 * @see IncUpdateCounter()
	 */
	unsigned int GetUpdateCounter() const;

	/**
	 * increases the update counter by one, doing this marks the
	 * port as dirty.
	 * @see GetUpdateCounter()
	 */
	void         IncUpdateCounter();

	/**
	 * return a pointer to a VdfnPortSerializeAdapter which is used
	 * when trying to send the port data over the network. Subclasses have
	 * to decide on this, the default implementation returns here what subclasses
	 * have set.
	 * @return a serialize adapter if one was defined, or NULL
	 * @todo check virtual in this context
	 */
	virtual VdfnPortSerializeAdapter *GetSerializeAdapter();

	/**
	 * ports have types. types can be checked using the IVdfnPortTypeCompare
	 * interface. The memory is assumed to be floating... the caller is
	 * responsible for cleaning up and managing the pointer
	 * @return the port type compare instance for this port.
	 */
	virtual IVdfnPortTypeCompare *GetPortTypeCompare() const = 0;

	/**
	 * clone the port, results in a copy of this port with the same type.
	 * the value of the port is undefined; user code can decide to copy it.
	 * The most important aspect is that the cloned port has the same type.
	 * @return a clone of this port
	 * @todo decide about value set during clone
	 */
	virtual IVdfnPort *Clone() const = 0;


	/**
	 * defines a function that can be used to assign this port's value from
	 * a source port.
	 * @param oOther the port from which the value is to be assigned to this port
	 * @return false if assignment could not be done, true else
	 */
	virtual bool AssignFrom( const IVdfnPort *oOther ) = 0;

	/**
	 * @return a string'ified type descriptor for this port's </i>value type</i>,
			   not the type of this port (which is IVdfnPort)
	 */
	virtual std::string GetTypeDescriptor() const = 0;

protected:
	IVdfnPort();

	VdfnPortSerializeAdapter *m_pAdapter;
private:
	double       m_nLastUpdate;
	unsigned int m_nUpdateCounter;
};

/**
 * base class for port serializers. It uses
 * streaming operators for serialization. Special type ports have to provide a
 * subclass of this class. They override the Serialize() and DeSerialize() methods
 * according to their needs.
 */
class VISTADFNAPI VdfnPortSerializeAdapter : public IVistaSerializable
{
public:
	/**
	 * @param pPort the parent port for this serializer
	 */
	VdfnPortSerializeAdapter(IVdfnPort *pPort);
	virtual ~VdfnPortSerializeAdapter();

	/**
	 * writes a header into the stream for the port, containing last update and
	 * the update counter.
	 * @return the number of bytes written
	 */
	virtual int Serialize(IVistaSerializer &) const;

	/**
	 * reads a header from the stream, containing the last update and the update
	 * counter.
	 * @return the number of bytes read
	 */
	virtual int DeSerialize(IVistaDeSerializer &);

	/**
	 * needed for the IVistaSerializable API.
	 * @return "IVdfnPort"
	 */
	virtual std::string GetSignature() const;
protected:
	IVdfnPort *m_pPort;
};


/**
 * forward declaration for the definition of TVdfnPort
 */
template<class T>
class TVdfnPortSerializerAdapter;


/**
 * TVdfnPort<> describes the current second level of the Port architecture.
 * The TVdfnPort<> defines the storage and access to the value of the port
 * of type T. Different access methods exist.
 *
 * - GetValue(): get by copy, or copy-in as reference
 * - GetValueRef(): obtain a reference to the memory of the stored value
 * - GetValueConstRef(): obtain read-only access to the memory of the stored value
 * - SetValue(): write to the memory by assignment, and trigger a node update
 *
 * The reference access is handy in order to avoid stack copies. However, when directly
 * writing to the memory using GetValueRef(), do not forget to manually trigger the node
 * update using IncUpdateCounter().
 *
 * Values stored in TVdfnPort must support default construction. If this is not an option
 * for you, subclass or specialize this template.
 */
template<class T>
class TVdfnPort : public IVdfnPort
{
public:
	/**
	 * default constructor, calls default constructor on stored value
	 */
	TVdfnPort()
		: m_oValue()
	{}

	/**
	 * assignment constructor, initializes the value with the oValue argument.
	 */
	TVdfnPort( const T &oValue )
		: m_oValue(oValue) {}

	virtual ~TVdfnPort() {}

	/**
	 * @return the port's value on stack (may be expensive)
	 */
	T GetValue() const                { return m_oValue; }

	/**
	 * @param oVal the place to store the value to
	 * @return true
	 */
	bool GetValue( T &oVal ) const    { oVal = m_oValue; return true; }

	/**
	 * Note that the API is non-const, in order to avoid trouble with some
	 * compilers, which automaticall make T& a const ref.
	 * @see IncUpdateCounter()
	 * @return a writeable reference to the storage of this port's value
	 */
	T &GetValueRef()                  { return m_oValue; }

	/**
	 * get a read-only reference to the value, useful to avoid stack copies
	 * @return a read-only reference to the port's value
	 */
	const T &GetValueConstRef() const { return m_oValue; }

	/**
	 * sets the value using the type's assignment operator
	 * @param oValue the value to set
	 * @param nUpdateTimeStamp the timestamp to set.
			  note that the timestamp is only copied, not interpreted.
	   @return true
	 */
	bool SetValue( const T &oValue, double nUpdateTimeStamp )
	{
		m_oValue = oValue;
		SetUpdateTimeStamp(nUpdateTimeStamp);
		IncUpdateCounter();
		return true;
	}

	/**
	 * @return the serializer for TVdfnPort<> instances.
	 */
	virtual VdfnPortSerializeAdapter *GetSerializeAdapter();

	/**
	 * Note that a call to this method will allocate a new IVdfnPortTypeCompare
	 * for type T. Be sure to collect the memory after each call or experience
	 * memory leaks.
	 * @return a port type compare for TVdfnPort<> instances
	 */
	virtual IVdfnPortTypeCompare *GetPortTypeCompare() const;

	/**
	 * clones this port, does not assign this node's value to the new
	 * port
	 * @todo decide about value cloning
	 * @return a new port with the same type as this one
	 */
	IVdfnPort *Clone() const { return new TVdfnPort<T>( *this ); }

	/**
	 * assignment function, calls SetValue() once the types of pOther and this
	 * port are identical.
	 * @param pOther the source port
	 * @return false if pOther is not of type TVdfnPort<T>, else returns SetValue()
	 */
	virtual bool AssignFrom( const IVdfnPort *pOther )
	{
		const TVdfnPort<T> *Other = dynamic_cast<const TVdfnPort<T>*>(pOther);
		if(!Other)
			return false;

		return SetValue( Other->GetValue(), Other->GetUpdateTimeStamp() );
	}

	/**
	 * @return the RTTI type name for T
	 */
	virtual std::string GetTypeDescriptor() const
	{
		return std::string((typeid(T).name() ? typeid(T).name() : "<none>"));
	}

private:
	T m_oValue;
};


/**
 * the serialize adapter for ports of type TVdfnPort<T>.
 * It uses streaming operators for serialization, so when you introduce a new
 * type T for a port, be sure to provide a valid streaming operator for it, too.
 */
template<class T>
class TVdfnPortSerializerAdapter : public VdfnPortSerializeAdapter
{
public:
	TVdfnPortSerializerAdapter(IVdfnPort *pPort)
		: VdfnPortSerializeAdapter(pPort) {}
	virtual ~TVdfnPortSerializerAdapter() {};

	/**
	 * streams first the base-class stream, and then
	 * the TVdfnPort<>::GetValueConstRef() to oSer. It uses operator<< for this
	 * on the type T
	 * @return the number of bytes streamed
	 */
	virtual int Serialize(IVistaSerializer &oSer) const
	{
		int iRet = VdfnPortSerializeAdapter::Serialize(oSer);
		oSer << (dynamic_cast<TVdfnPort<T>*>(m_pPort))->GetValueConstRef();
		return iRet + sizeof(T);
	}


	/**
	 * streams first the base-class stream, and then the TVdfnPort<T>::GetValueRef()
	 * to the port. As the update count is streamed, too, is does <b>not</b> trigger
	 * the update count for this port. It uses operator>> for the streamin on type T.
	 * @return the number of bytes streamed.
	 */
	virtual int DeSerialize(IVistaDeSerializer &oDeSer)
	{
		int iRet = VdfnPortSerializeAdapter::DeSerialize( oDeSer );

		oDeSer >> (dynamic_cast<TVdfnPort<T>*>(m_pPort))->GetValueRef();

		return iRet + sizeof(T);
	}

	/**
	 * returns a static string TVdfnPort<T>, as the RTTI decription for T
	 * may vary from system to system.
	 * @return "TVdfnPort<T>"
	 */
	virtual std::string GetSignature() const
	{
		return "TVdfnPort<T>";
	}
};

/**
 * creates a proper serialize adapted to TVdfnPort<T> type ports.
 */
template<class T>
VdfnPortSerializeAdapter *TVdfnPort<T>::GetSerializeAdapter()
{
	if(m_pAdapter == NULL)
		return (m_pAdapter = new TVdfnPortSerializerAdapter<T>(this));

	return IVdfnPort::GetSerializeAdapter();
}

/**
 * the port-type compare API which decides on the equality of port types.
 */
class VISTADFNAPI IVdfnPortTypeCompare
{
public:
	virtual ~IVdfnPortTypeCompare()
	{
	}

	/**
	 * @param pPort the port to check for type matching
	 * @return true if pPort has a comparable type to this port type compare
	 */
	virtual bool IsTypeOf( const IVdfnPort *pPort ) const = 0;

	/**
	 * clones this port-type compare
	 * @return a clone of this port type compare
	 */
	virtual IVdfnPortTypeCompare *Clone() const = 0;

	/**
	 * utility function to assign pPort to a memory region defined
	 * by a subclass of IVdfnPortTypeCompare.
	 * @param pPort the port to assign from
	 * @return true when the assignment was done, false else
	 */
	virtual bool Assign( IVdfnPort *pPort ) = 0;


	/**
	 * @return the mangled RTTI name of the port type to expect
	 */
	virtual std::string GetTypeDescriptor() const = 0;

	virtual IVdfnPort *CreatePort() const = 0;
protected:
	IVdfnPortTypeCompare();
	IVdfnPortTypeCompare( const IVdfnPortTypeCompare & );
	IVdfnPortTypeCompare &operator=(const IVdfnPortTypeCompare &);
};


/**
 * a port type compare for TVdfnPort<T> types.
 */
template<class T>
class TVdfnPortTypeCompare : public IVdfnPortTypeCompare
{
public:
	/**
	 * creates a normal port type compare. this port type compare
	 * can not assign ports to a memory region.
	 */
	TVdfnPortTypeCompare()
	: IVdfnPortTypeCompare(),
	  m_pPortStore(NULL)
	{}

	TVdfnPortTypeCompare( const TVdfnPortTypeCompare &other )
	: IVdfnPortTypeCompare()
	, m_pPortStore( other.m_pPortStore )
	{}

	/**
	 * @param pPortStore the pointer to the memory region to assign an
			  incoming port from, can be NULL
	 * @see Assign()
	 */
	TVdfnPortTypeCompare(T **pPortStore)
	: IVdfnPortTypeCompare(),
	  m_pPortStore(pPortStore)
	{}

	/**
	 * determines equality based on the ability to downcast pPort to type T.
	 * This, at the same time, fixes equality on the level of ports, not on the
	 * level of the types they represent.
	 * TVdfnPort<int> is thus not equal to TVdfnPort<unsigned int>, although int
	 * and unsigned int have something in common.
	 * @todo check the type matching based on ports
	 * @return true if pPort can be downcasted to T
	 */
	bool IsTypeOf( const IVdfnPort *pPort ) const
	{
		return (dynamic_cast<const T*>( pPort ) != NULL);
	}

	/**
	 * @return returns a simple clone of this port type compare.
	 */
	IVdfnPortTypeCompare *Clone() const
	{
		return new TVdfnPortTypeCompare<T>(*this);
	}

	/**
	 * the template port type compare keeps a pointer to the
	 * storage where this assignment operator points to.
	 * it will downcast and assign the pointer properly when called.
	 * @return false when no storage was assigned during construction
			   returns finally, whether the memory region to assign contains
			   the same value as the argument
	 * @param pPort the pointer to assign from
	 */
	bool Assign( IVdfnPort *pPort )
	{
		if(m_pPortStore == NULL)
			return false;
		(*m_pPortStore) = dynamic_cast<T*>(pPort);
		return ((*m_pPortStore) == pPort);
	}

	/**
	 * @return the mangled RTTI name of the port type T
	 */
	virtual std::string GetTypeDescriptor() const
	{
		return std::string((typeid(T).name() ? typeid(T).name() : "<none>"));
	}

	IVdfnPort *CreatePort() const
	{
		return new T;
	}

private:
	T **m_pPortStore; /**< pointer to storage to assign to, can be NULL */
};

/**
 * Define PortType-Compare API for TVdfnPort<T> types
 * @todo check who calls this API and what happens to the memory alloc'ed here
 */
template<class T>
IVdfnPortTypeCompare *TVdfnPort<T>::GetPortTypeCompare() const
{
	return new TVdfnPortTypeCompare<TVdfnPort<T> >;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H
