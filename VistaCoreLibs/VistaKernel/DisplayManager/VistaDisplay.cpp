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


#include "VistaDisplay.h"
#include "VistaDisplayManager.h"
#include "VistaWindow.h"
#include "VistaDisplayBridge.h"
#include <VistaAspects/VistaAspectsUtils.h>


/*============================================================================*/

// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaDisplay::VistaDisplay(VistaDisplayManager *pDMgr,
							 IVistaDisplayEntityData *pData,
							 IVistaDisplayBridge *pBridge)
	: VistaDisplayEntity(pData, pBridge), m_pDisplayManager( pDMgr )
{
}

VistaDisplay::~VistaDisplay()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayManager                                           */
/*                                                                            */
/*============================================================================*/
VistaDisplayManager *VistaDisplay::GetDisplayManager() const
{
	return m_pDisplayManager;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindowNames                                              */
/*                                                                            */
/*============================================================================*/
std::list<std::string> VistaDisplay::GetWindowNames() const
{    
	std::list<std::string> liNames;

	if (m_vecWindows.size())
	{
		for( std::vector<VistaWindow *>::const_iterator it = m_vecWindows.begin() ; 
			 it != m_vecWindows.end() ; ++it )
		{
			liNames.push_back( (*it)->GetNameForNameable() );
		}
	}
	return liNames;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetNumberOfWindows                                          */
/*                                                                            */
/*============================================================================*/
unsigned int VistaDisplay::GetNumberOfWindows() const
{
	return (unsigned int)m_vecWindows.size();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindow                                                   */
/*                                                                            */
/*============================================================================*/
VistaWindow *VistaDisplay::GetWindow(unsigned int iIndex) const
{
	if( iIndex < m_vecWindows.size() )
		return m_vecWindows[iIndex];
	return NULL;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindows                                                  */
/*                                                                            */
/*============================================================================*/
std::vector<VistaWindow *> &VistaDisplay::GetWindows() 
{
	return m_vecWindows;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaDisplay::Debug(std::ostream &out) const
{ 
	VistaDisplayEntity::Debug(out);

	out << " [VistaDisplay] - display string:  " << GetDisplayProperties()->GetDisplayString() 
		<< std::endl;
	out << " [VistaDisplay] - windows:         " << GetNumberOfWindows() 
		<< std::endl;

	if (m_vecWindows.size())
	{
		out << " [VistaDisplay] - window names:    ";
		unsigned int i;
		for (i=0; i<m_vecWindows.size(); ++i)
		{
			if (i>0)
				out << ", ";
			out << m_vecWindows[i]->GetNameForNameable();
		}
		out << std::endl;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateProperties                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplay::IVistaDisplayEntityProperties *VistaDisplay::CreateProperties()
{
	return new VistaDisplayProperties(this, GetDisplayBridge());
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplayProperties                                        */
/*                                                                            */
/*============================================================================*/
VistaDisplay::VistaDisplayProperties *VistaDisplay::GetDisplayProperties() const
{
	return static_cast<VistaDisplayProperties*>(GetProperties());
}


namespace {
	const std::string sSReflectionTypeVD("VistaDisplay");

	IVistaPropertyGetFunctor *aCgFunctorsVD[] =
	{
		new TVistaPropertyGet<std::string, VistaDisplay::VistaDisplayProperties, VistaProperty::PROPT_STRING>
		("DISPLAY_STRING", sSReflectionTypeVD,
		 &VistaDisplay::VistaDisplayProperties::GetDisplayString),
		new TVistaDisplayEntityParentPropertyGet<unsigned int, VistaDisplay, VistaProperty::PROPT_INT>
		("NUMBER_OF_WINDOWS", sSReflectionTypeVD,
		 &VistaDisplay::GetNumberOfWindows),
		new TVistaDisplayEntityParentPropertyGet<std::list<std::string>, VistaDisplay, VistaProperty::PROPT_LIST>
		("WINDOW_NAMES", sSReflectionTypeVD,
		 &VistaDisplay::GetWindowNames),
		NULL
	};

	IVistaPropertySetFunctor *aCsFunctorsVD[] =
	{
		new TVistaPropertySet<const string &, string,VistaDisplay::VistaDisplayProperties>
		("NAME", sSReflectionTypeVD,
		 &VistaDisplay::VistaDisplayProperties::SetName ),
		new TVistaPropertySet<const std::string &, std::string, VistaDisplay::VistaDisplayProperties>
		("DISPLAY_STRING", sSReflectionTypeVD,
		 &VistaDisplay::VistaDisplayProperties::SetDisplayString ),
		NULL
	};
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SetName                                                     */
/*                                                                            */
/*============================================================================*/
bool VistaDisplay::VistaDisplayProperties::SetName(const std::string &sName)
{
	VistaDisplay *pD = static_cast<VistaDisplay*>(GetParent());

	string strOldName = pD->GetNameForNameable();
	if(strOldName != "")
	{
		if (!pD->GetDisplayManager()->RenameDisplay(strOldName, sName))
			return false;
	}
	return IVistaDisplayEntityProperties::SetName(sName);
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetDisplayString                                        */
/*                                                                            */
/*============================================================================*/
std::string VistaDisplay::VistaDisplayProperties::GetDisplayString() const
{
	return m_strDisplayString;
}

bool VistaDisplay::VistaDisplayProperties::SetDisplayString(const std::string &sDispString)
{
	if(m_strDisplayString != sDispString)
	{
		m_strDisplayString = sDispString;
		Notify(MSG_DISPLAYSTRING_CHANGE);
		return true;
	}
	return false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetReflectionableType                                       */
/*                                                                            */
/*============================================================================*/
string VistaDisplay::VistaDisplayProperties::GetReflectionableType() const
{
	return sSReflectionTypeVD;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   AddToBaseTypeList                                           */
/*                                                                            */
/*============================================================================*/
int VistaDisplay::VistaDisplayProperties::AddToBaseTypeList(list<string> &rBtList) const
{
	int nSize = IVistaDisplayEntityProperties::AddToBaseTypeList(rBtList);
	rBtList.push_back(sSReflectionTypeVD);

	return nSize + 1;
}

VistaDisplay::VistaDisplayProperties::VistaDisplayProperties(
	VistaDisplayEntity *pParent,
	IVistaDisplayBridge *pBridge)
  : IVistaDisplayEntityProperties(pParent,pBridge)
{

}

VistaDisplay::VistaDisplayProperties::~VistaDisplayProperties()
{
}


