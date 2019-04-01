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


#include "VistaMarshalledObjectFactory.h"

#include "VistaSerializable.h"
#include "VistaSerializer.h"
#include "VistaDeSerializer.h"

#include <cassert>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaMarshalledObjectFactory::VistaMarshalledObjectFactory()
	:	m_iTypeCounter(0),
		m_pFactory(new VistaGenericFactory<IVistaSerializable, 
							VistaMarshalledObjectFactory::LocalTypeInfo>)
{
}

VistaMarshalledObjectFactory::~VistaMarshalledObjectFactory()
{
	delete m_pFactory;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaType::sint32 VistaMarshalledObjectFactory::RegisterType( 
								IVistaSerializable *pType, 
								IVistaCreator<IVistaSerializable> *pCreator )
{
	assert(m_pFactory != NULL);
	
	//register with the internal factory
	LocalTypeInfo oLocalType = typeid(*pType);
	if(!m_pFactory->RegisterCreator(oLocalType, pCreator))
		return -1;

	//generate new tpye id and insert into translation maps
	VistaType::sint32 iNewGlobalTypeId = this->GetNextTypeId();
	m_mapGlobalTypeToLocalType.insert(std::make_pair(iNewGlobalTypeId, oLocalType));
	m_mapLocalTypeToGlobalType.insert(std::make_pair(oLocalType, iNewGlobalTypeId));

	return iNewGlobalTypeId;
}


VistaType::sint32 VistaMarshalledObjectFactory::GetGlobalTypeId( 
								const IVistaSerializable *pType ) const
{
	LocalTypeInfo oLocalType = typeid(*pType);
	TLocalToGlobalMap::const_iterator itGlobalType = 
						m_mapLocalTypeToGlobalType.find(oLocalType);

	if(itGlobalType == m_mapLocalTypeToGlobalType.end())
	{
		//not registered
		return -1;
	}
	else
	{
		return itGlobalType->second;
	}
}


int VistaMarshalledObjectFactory::MarshalObject(const IVistaSerializable *pObject, 
												 IVistaSerializer &rSer ) const
{
	//map object type to global identifier
	VistaType::sint32 iGlobalType = this->GetGlobalTypeId(pObject);
	if(iGlobalType == -1)
	{
		//type not registered
		return -1;
	}
	//serialize the type identifier
	int iRet = 0, iSum = 0;
	iRet = rSer.WriteInt32(iGlobalType);
	
	if(iRet < 0)
	{
		//type id serialization failed
		return -1;
	}
	iSum += iRet;
	
	//serialize the object state right behind it
	iRet = rSer.WriteSerializable(*pObject);
	
	if(iRet < 0)
	{
		//serializable serialization failed
		return -1;
	}
	iSum += iRet;
	
	//done - return total number of bytes written
	return iSum;
}

IVistaSerializable * VistaMarshalledObjectFactory::UnmarshalObject(IVistaDeSerializer &rDeser) const
{
	//retrieve the type identifier 
	VistaType::sint32 iGlobalTypeId = 0;
	if(rDeser.ReadInt32(iGlobalTypeId) < 0)
	{
		//deserialization of type id failed
		return NULL;
	}
	//map global to local type identifier
	TGlobalToLocalMap::const_iterator itLocalId = m_mapGlobalTypeToLocalType.find(iGlobalTypeId);

	if(itLocalId == m_mapGlobalTypeToLocalType.end())
	{
		//type id not registered
		return NULL;
	}
	const LocalTypeInfo &rLocalType = itLocalId->second;

	//create our new object from local type identifier
	IVistaSerializable *pNewObj = m_pFactory->CreateInstance(rLocalType);
	if(pNewObj == NULL)
	{
		//object creation failed - possibly because creator was not registered properly
		return NULL;
	}

	//deserialize the rest of the object
	if(rDeser.ReadSerializable(*pNewObj) < 0)
	{
		//deserialization failed
		delete pNewObj;
		return NULL;
	}

	//...done!
	return pNewObj;
}

VistaType::sint32 VistaMarshalledObjectFactory::GetNextTypeId()
{
	return m_iTypeCounter++;
}

void VistaMarshalledObjectFactory::Clear()
{
	m_mapGlobalTypeToLocalType.clear();
	m_mapLocalTypeToGlobalType.clear();
	m_iTypeCounter = 0;
	delete m_pFactory;
	m_pFactory = new VistaGenericFactory<IVistaSerializable, 
							VistaMarshalledObjectFactory::LocalTypeInfo>;
}

size_t VistaMarshalledObjectFactory::GetNumTypes() const
{
	return m_mapGlobalTypeToLocalType.size();
}


VistaMarshalledObjectFactory::LocalTypeInfo::LocalTypeInfo( const type_info& oInfo )
	: m_rTypeInfo(oInfo)
{

}

VistaMarshalledObjectFactory::LocalTypeInfo::LocalTypeInfo( const LocalTypeInfo &rOther )
	: m_rTypeInfo(rOther.m_rTypeInfo)
{

}

bool VistaMarshalledObjectFactory::LocalTypeInfo::operator<( const LocalTypeInfo &rRHS ) const
{
	return this->m_rTypeInfo.before(rRHS.m_rTypeInfo) != 0;
}

bool VistaMarshalledObjectFactory::LocalTypeInfo::operator==( const LocalTypeInfo &rRHS ) const
{
	return this->m_rTypeInfo == rRHS.m_rTypeInfo;
}

const type_info& VistaMarshalledObjectFactory::LocalTypeInfo::GetTypeInfo() const
{
	return m_rTypeInfo;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

