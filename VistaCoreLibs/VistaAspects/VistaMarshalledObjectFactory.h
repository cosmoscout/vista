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


#ifndef _VISTAMARSHALLEDOBJECTFACTORY_H
#define _VISTAMARSHALLEDOBJECTFACTORY_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
#include <map>
#include <typeinfo>
using std::type_info;

#include "VistaAspectsConfig.h"
#include "VistaGenericFactory.h"

#include <VistaBase/VistaBaseTypes.h>

#include <cstddef>
#include "VistaUncopyable.h"
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaSerializable;
class IVistaSerializer;
class IVistaDeSerializer;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaMarshalledObjectFactory is used to implement object marshalling/
 * unmarshalling (i.e. package a given object into a serialized form, including
 * not only its current state but also type information).
 *
 * To ensure correctness across system boundaries, classes have to be 
 * registered with the factory before use. This registration process has to 
 * be done in <bf>exactly the same order</bf> for all communication partners. 
 * For example, if objects are send back and forth between two different 
 * applications, make sure that both applications register their objects 
 * in exactly the same order.
 *
 * If you want to marshal <it>composite objects</it>, there are two possibilities:
 *
 * 1) Marshalling composites with known internal types
 * If your composite only contains sub-components whose exact type is known,
 * this type information can be used during (un-)marshalling.
 * 
 * 2) Marshalling composites with variable internal types
 * If you don't know the exact internal types a priori, i.e. if you use
 * polymorphism inside your composite, the exact types can be deduced
 * via the marshalling mechanism. In order to work this out, the composite
 * object needs access to the factory, which can be granted by using a 
 * specialized creator which passes a pointer to the factory to the composite
 * upon construction.
 */
class VISTAASPECTSAPI VistaMarshalledObjectFactory
{
	VISTA_UNCOPYABLE( VistaMarshalledObjectFactory )
public:
	VistaMarshalledObjectFactory();
	
	virtual ~VistaMarshalledObjectFactory();
	/**
	 * Register specific implementation sub-class of IVistaSerializable given
	 * in pType with the given object creator. Double-registration of the same
	 * type will result in failure.
	 *
	 * NOTE: The factory will take ownership of the creator object, i.e. the calling
	 *       client must not delete it! The object pointed to by pType, however, remains
	 *		 in the client's possession, i.e. the client has to get rid of it on its own!
	 *
	 * \return global type id for the newly registered type, -1 in case of failure
	 */
	VistaType::sint32 RegisterType(IVistaSerializable *pType, 
								   IVistaCreator<IVistaSerializable> *pCreator);

	/**
	 *	Clear the entire factory i.e. delete all registered types and creators
	 */
	void Clear();

	/**
	 *	Return the number of registered types
	 */
	size_t GetNumTypes() const;

	/**
	 * retrieve the global type id (if any) for the given object
	 */
	VistaType::sint32 GetGlobalTypeId(const IVistaSerializable *pType) const;

	/**
	 * Marshal an object i.e. pack its type and state information
	 * into the given serializer
	 *
	 * \return number of bytes written to rSer, -1 in case of failure.
	 */
	int MarshalObject(const IVistaSerializable *pObject, IVistaSerializer &rSer) const;

	/**
	 * Unmarshal an object i.e. unpack it from the byte stream provided
	 * by the given deserializer.
	 * NOTE: This effectively corresponds to a new call, i.e. the created
	 *       object is allocated herein. Therefore, clients assume responsibility
	 *       for the returned object
	 *
	 * \return newly created, unmarshalled object, NULL in case of failure
	 */
	IVistaSerializable *UnmarshalObject(IVistaDeSerializer &rDeser) const;
	
protected:
	/**
	 * generate unique global type identifier by counting up.
	 */
	VistaType::sint32 GetNextTypeId();

	/**
	 * Helper class for type_info storage
	 * 
	 * We locally use the output of typeid to identify objects.
	 * This ensures that we do not have to carry an object type identifier
	 * with each registered class, obviating the need for a specific 
	 * type identification interface in marshalled objects.
	 * However, we cannot rely on having identical typeid output accross
	 * system boundaries, hence we insert a translation between local 
	 * type_info and "global id". The latter is used for marshalling
	 * purposes.
	 */
	class LocalTypeInfo
	{
	public:
		LocalTypeInfo(const type_info& oInfo);
		LocalTypeInfo(const LocalTypeInfo &rOther);

		bool operator<(const LocalTypeInfo &rRHS) const;
		bool operator==(const LocalTypeInfo &rRHS) const;

		const type_info& GetTypeInfo() const;
	protected:
	private:
		const type_info& m_rTypeInfo;
	};

private:
	typedef std::map<LocalTypeInfo, VistaType::sint32> TLocalToGlobalMap;
	/**
	 * map the local type_info-based hash key to the globally unique int32 id.
	 */
	TLocalToGlobalMap m_mapLocalTypeToGlobalType;

	typedef std::map<VistaType::sint32, LocalTypeInfo> TGlobalToLocalMap;
	/**
	 * map the globally unique type identifier to the back locally used one.
	 */
	TGlobalToLocalMap m_mapGlobalTypeToLocalType;
	/**
	 * type id counter 
	 */
	VistaType::sint32 m_iTypeCounter;
	/**
	 * The factory here will produce serializable objects and 
	 * will do so based on their (local) type_info hash.
	 */
	VistaGenericFactory<IVistaSerializable, LocalTypeInfo> *m_pFactory;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMARSHALLEDOBJECTFACTORY_H
