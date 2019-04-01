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


#include "VdfnPortFactory.h" 
#include <VistaAspects/VistaPropertyFunctor.h>


/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace
{
	VdfnPortFactory *SFactory = NULL;
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnPortFactory::VdfnPortFactory()
{
}

VdfnPortFactory::~VdfnPortFactory()
{
	// should clean up memory!
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VdfnPortFactory *VdfnPortFactory::GetSingleton()
{
	if( SFactory == NULL)
		SFactory = new VdfnPortFactory;

	return SFactory;
}

bool VdfnPortFactory::AddPortAccess( IVistaPropertyGetFunctor *pGetF, CPortAccess *pPortAccess )
{
	return AddPortAccess( pGetF->GetPropertyClassName(), pPortAccess );
}

VdfnPortFactory::CPortAccess *VdfnPortFactory::GetPortAccess( IVistaPropertyGetFunctor *pGetF ) const
{
	return GetPortAccess( pGetF->GetPropertyClassName() );
}

bool VdfnPortFactory::AddPortAccess( const std::string &strDesc, CPortAccess *pPortAccess )
{
	// simply overwrite, make cleaner on some day...
	m_mpPortCreation[strDesc] = pPortAccess;
	return true;
}

VdfnPortFactory::CPortAccess *VdfnPortFactory::GetPortAccess( const std::string &strDesc ) const
{
	std::map<std::string, CPortAccess*>::const_iterator cit = m_mpPortCreation.find(strDesc);
	if( cit == m_mpPortCreation.end() )
		return NULL;
	return (*cit).second;
}

VdfnPortFactory::CFunctorAccess *VdfnPortFactory::GetFunctorAccess( IVistaPropertySetFunctor *pFunc ) const
{
	return GetFunctorAccess( pFunc->GetPropertyClassName() );
}

bool VdfnPortFactory::AddFunctorAccess( IVistaPropertySetFunctor *pFunc, CFunctorAccess *pFuncAccess )
{
	return AddFunctorAccess( pFunc->GetPropertyClassName(), pFuncAccess ) ;
}

bool VdfnPortFactory::AddFunctorAccess( const std::string &strName, CFunctorAccess *pFuncAccess )
{
	m_mpFuncCreation[strName] = pFuncAccess;
	return true;
}

VdfnPortFactory::CFunctorAccess *VdfnPortFactory::GetFunctorAccess( const std::string &strName ) const
{
	std::map<std::string, CFunctorAccess*>::const_iterator cit = m_mpFuncCreation.find(strName);
	if(cit != m_mpFuncCreation.end())
		return (*cit).second;
	return NULL;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


