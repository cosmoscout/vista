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


#include "VdfnActionObject.h" 
#include <VistaAspects/VistaPropertyFunctorRegistry.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVdfnActionObject::IVdfnActionObject() 
: IVistaReflectionable()
, m_dUpdateTimeStamp( 0 )
{
}

IVdfnActionObject::~IVdfnActionObject() 
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVdfnActionObject::IActionGet *IVdfnActionObject::GetActionGet( const std::string &strSymbol ) const
{
	VistaPropertyFunctorRegistry *pReg = VistaPropertyFunctorRegistry::GetSingleton();
	if(m_liBaseListCache.empty())
	{
		// ... ah, yes
		// cache it
		m_liBaseListCache = GetBaseTypeList();
	}
	// use the cached version for the get-functor
	return dynamic_cast<IActionGet*>(pReg->GetGetFunctor(strSymbol, 
		   GetReflectionableType(), m_liBaseListCache));
}

IVdfnActionObject::IActionSet *IVdfnActionObject::GetActionSet( const std::string &strSymbol ) const
{
	VistaPropertyFunctorRegistry *pReg = VistaPropertyFunctorRegistry::GetSingleton();
	// ok, ok... it is a const-setter, but we might need to cache the
	// base-class list, as this is unlikely to change, so we check
	// whether we have to build it
	if(m_liBaseListCache.empty())
	{
		// ... ah, yes
		// cache it
		m_liBaseListCache = GetBaseTypeList();
	}
	// use the cached version for the get-functor
	return dynamic_cast<IActionSet*>(pReg->GetSetFunctor(strSymbol, 
		   GetReflectionableType(), m_liBaseListCache));
}

bool IVdfnActionObject::Pull()
{
	return false;
}

void IVdfnActionObject::SetUpdateTimeStamp( const VistaType::microtime dTimestamp,
											const bool bInportsChanged )
{
	m_dUpdateTimeStamp = dTimestamp;
}

VistaType::microtime IVdfnActionObject::GetUpdateTimeStamp() const
{
	return m_dUpdateTimeStamp;
}





