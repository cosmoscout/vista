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


#include "VistaViewport.h"
#include "VistaDisplaySystem.h"
#include "VistaProjection.h"
#include "VistaWindow.h"
#include "VistaDisplayBridge.h"
#include "VistaDisplayManager.h"

#include <VistaAspects/VistaAspectsUtils.h>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaViewport::VistaViewport(VistaDisplaySystem *pDisplaySystem, 
							   VistaWindow *pWindow,
							   IVistaDisplayEntityData *pData,
							   IVistaDisplayBridge *pBridge)
: VistaDisplayEntity(pData, pBridge),
  m_pDisplaySystem(pDisplaySystem),
  m_pWindow(pWindow),
  m_pProjection(NULL),
  m_pCustomRenderAction(NULL)
{
}

VistaViewport::~VistaViewport()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplaySystem                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplaySystem *VistaViewport::GetDisplaySystem() const
{
	return m_pDisplaySystem;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetDisplaySystemName                                        */
/*                                                                            */
/*============================================================================*/
string VistaViewport::GetDisplaySystemName() const
{
	string sDisplaySName;

	if(m_pDisplaySystem)
	{
		sDisplaySName = m_pDisplaySystem->GetNameForNameable();
	}
	return sDisplaySName;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindow                                                   */
/*                                                                            */
/*============================================================================*/
VistaWindow *VistaViewport::GetWindow() const
{
	return m_pWindow;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetWindowName                                               */
/*                                                                            */
/*============================================================================*/
string VistaViewport::GetWindowName() const
{
	string sWindowName;

	if(m_pWindow)
	{
		sWindowName = m_pWindow->GetNameForNameable();
	}
	return sWindowName;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetProjection                                           */
/*                                                                            */
/*============================================================================*/
VistaProjection *VistaViewport::GetProjection() const
{
	return m_pProjection;
}

void VistaViewport::SetProjection(VistaProjection *pProjection)
{
	m_pProjection = pProjection;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetProjectionName                                           */
/*                                                                            */
/*============================================================================*/
string VistaViewport::GetProjectionName() const
{
	string sProjectionName;

	if(m_pProjection)
	{
		sProjectionName = m_pProjection->GetNameForNameable();
	}
	return sProjectionName;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   Debug                                                       */
/*                                                                            */
/*============================================================================*/
void VistaViewport::Debug(std::ostream &out) const
{
	//VistaDisplayEntity::Debug(out);

	out << vstr::indent << "[VistaViewport]      - name                   : " 
		<< GetNameForNameable() << std::endl;

	int x, y;
	GetViewportProperties()->GetPosition(x, y);
	out << vstr::indent << "[VistaViewport]      - position               : " 
		<< x << " / " << y << std::endl;

	GetViewportProperties()->GetSize(x, y);
	out << vstr::indent << "[VistaViewport]      - size                   : "
		<< x << " / " << y << std::endl;

	out << vstr::indent << "[VistaViewport]      - display system name    : ";
	if (m_pDisplaySystem)
		out << m_pDisplaySystem->GetNameForNameable() << std::endl;
	else
		out << "*none* (no display system given)" << std::endl;

	out << vstr::indent << "[VistaViewport]      - window name            : ";
	if (m_pWindow)
		out << m_pWindow->GetNameForNameable() << std::endl;
	else
		out <<"*none* (no window given)" << std::endl;

	out << vstr::indent << "[VistaViewport]      - projection name        : ";
	if (m_pProjection)
		out << m_pProjection->GetNameForNameable() << std::endl;
	else
		out << "*none* (no projection given)" << std::endl;
}


/*============================================================================*/
/*                                                                            */
/*  NAME      :   CreateProperties                                            */
/*                                                                            */
/*============================================================================*/
VistaDisplayEntity::IVistaDisplayEntityProperties *VistaViewport::CreateProperties()
{
	return new VistaViewportProperties(this, GetDisplayBridge());
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetViewportProperties                                       */
/*                                                                            */
/*============================================================================*/
VistaViewport::VistaViewportProperties *VistaViewport::GetViewportProperties() const
{
	return static_cast<VistaViewportProperties*>(GetProperties());
}

bool VistaViewport::Render()
{
	//std::cout << "rendering viewport " << GetNameForNameable() << std::endl;
	GetViewportProperties()->Notify( VistaViewportProperties::MSG_PRE_RENDER );
	bool bSuccess = GetDisplayBridge()->RenderViewport( this );
	GetViewportProperties()->Notify( VistaViewportProperties::MSG_POST_RENDER );
	return bSuccess;
}

VistaGLTexture* VistaViewport::GetTextureTarget() const
{
	return GetDisplayBridge()->GetViewportTextureTarget( this );
}

VistaGLTexture* VistaViewport::GetSecondaryTextureTarget() const
{
	return GetDisplayBridge()->GetViewportSecondaryTextureTarget( this );
}


VistaViewport::CustomRenderAction* VistaViewport::GetCustomRenderAction() const
{
	return m_pCustomRenderAction;
}

void VistaViewport::SetCustomRenderAction( CustomRenderAction* pAction )
{
	m_pCustomRenderAction = pAction;
}

namespace {
	const std::string sSReflectionTypeVV("VistaViewport");


	IVistaPropertyGetFunctor *aCgFunctorsVV[] =
	{

		new TVistaProperty2RefGet<int, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_INT>(
				"POSITION", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::GetPosition ),
		new TVistaProperty2RefGet<int, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_INT>(
				"SIZE", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::GetSize ),
		new TVistaProperty2RefGet<int, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_INT>(
				"TEXTURE_SIZE", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::GetTextureSize ),
		new TVistaPropertyGet<bool, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_BOOL>(
				"ENABLED", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::GetIsEnabled ),
		new TVistaPropertyGet<int, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_INT>(
				"PRIORITY", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::GetPriority ),
		new TVistaPropertyGet<std::string, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_STRING>(
				"TYPE", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::GetTypeString ),
		new TVistaPropertyGet<std::string, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_STRING>(
				"POSTPROCESS_SHADER", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::GetPostProcessShaderFile ),
		new TVistaPropertyGet<bool, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_BOOL>
				("RESIZE_WITH_WINDOW", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::GetResizeWithWindow),
		new TVistaPropertyGet<bool, VistaViewport::VistaViewportProperties, VistaProperty::PROPT_BOOL>
				("REPOSITION_WITH_WINDOW", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::GetRepositionWithWindow),
		new TVistaDisplayEntityParentPropertyGet<std::string, VistaViewport, VistaProperty::PROPT_STRING>(
				"DISPLAY_SYSTEM_NAME", sSReflectionTypeVV, &VistaViewport::GetDisplaySystemName ),
		new TVistaDisplayEntityParentPropertyGet<std::string, VistaViewport, VistaProperty::PROPT_STRING>(
				"WINDOW_NAME", sSReflectionTypeVV, &VistaViewport::GetWindowName ),
		new TVistaDisplayEntityParentPropertyGet<std::string, VistaViewport, VistaProperty::PROPT_STRING>(
				"PROJECTION_NAME", sSReflectionTypeVV, &VistaViewport::GetProjectionName ),
		NULL
	};

	IVistaPropertySetFunctor *aCsFunctorsVV[] =
	{	
		new TVistaProperty2ValSet<int, VistaViewport::VistaViewportProperties>(
				"POSITION", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::SetPosition ),
		new TVistaProperty2ValSet<int, VistaViewport::VistaViewportProperties>(
				"SIZE", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::SetSize ),
		new TVistaProperty2ValSet<int, VistaViewport::VistaViewportProperties>(
				"TEXTURE_SIZE", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::SetTextureSize ),
		new TVistaPropertySet<const string &, string,VistaViewport::VistaViewportProperties>(
				"NAME", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::SetName ),
		new TVistaPropertySet<const string &, string,VistaViewport::VistaViewportProperties>(
				"TYPE", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::SetTypeByString ),
		new TVistaPropertySet<const string &, string,VistaViewport::VistaViewportProperties>(
				"POSTPROCESS_SHADER", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::SetPostProcessShaderFile ),
		new TVistaPropertySet<bool, bool, VistaViewport::VistaViewportProperties>
				("RESIZE_WITH_WINDOW", sSReflectionTypeVV,  &VistaViewport::VistaViewportProperties::SetResizeWithWindow),
		new TVistaPropertySet<bool, bool, VistaViewport::VistaViewportProperties>
				("REPOSITION_WITH_WINDOW", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::SetRepositionWithWindow),
		new TVistaPropertySet<const bool, bool,VistaViewport::VistaViewportProperties>(
				"ENABLED", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::SetIsEnabled ),
		new TVistaPropertySet<const int, int,VistaViewport::VistaViewportProperties>(
				"PRIORITY", sSReflectionTypeVV, &VistaViewport::VistaViewportProperties::SetPriority ),
		NULL
	};
}

bool VistaViewport::VistaViewportProperties::SetName(const std::string &sName)
{
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	string strOldName = pV->GetNameForNameable();

	if(strOldName != "")
	{
		if (!pV->GetDisplaySystem()->GetDisplayManager()->RenameViewport(strOldName, sName))
			return false;
	}
	return IVistaDisplayEntityProperties::SetName(sName);

}

bool VistaViewport::VistaViewportProperties::GetPosition(int& x, int& y) const
{
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	GetDisplayBridge()->GetViewportPosition(x, y, pV);

	return true;
}

bool VistaViewport::VistaViewportProperties::SetPosition(const int x, const int y)
{
	int iCurXPos, iCurYPos;
	 
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	GetDisplayBridge()->GetViewportPosition(iCurXPos, iCurYPos, pV);

	if((iCurXPos == x) && (iCurYPos == y))
		return false;
	else
	{
		GetDisplayBridge()->SetViewportPosition(x, y, pV);
		Notify( MSG_POSITION_CHANGE );
		return true;
	}
}

bool VistaViewport::VistaViewportProperties::GetSize(int& w, int& h) const
{
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	GetDisplayBridge()->GetViewportSize(w, h, pV);

	return true;
}

bool VistaViewport::VistaViewportProperties::SetSize(const int w, const int h)
{
	int iCurW, iCurH;
	 
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	GetDisplayBridge()->GetViewportSize(iCurW, iCurH, pV);

	if((iCurW == w) && (iCurH == h))
		return false;
	else
	{
		GetDisplayBridge()->SetViewportSize(w, h, pV);
		Notify( MSG_SIZE_CHANGE );
		return true;
	}
}

bool VistaViewport::VistaViewportProperties::GetTextureSize( int& w, int& h ) const
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	GetDisplayBridge()->GetViewportTextureSize( w, h, pViewport );
	return true;
}

bool VistaViewport::VistaViewportProperties::SetTextureSize( const int w, const int h )
{
	int iCurW, iCurH;
	 
	VistaViewport *pV = static_cast<VistaViewport*>(GetParent());
	GetDisplayBridge()->GetViewportSize(iCurW, iCurH, pV);

	if((iCurW == w) && (iCurH == h))
		return false;
	else
	{
		GetDisplayBridge()->SetViewportTextureSize(w, h, pV);
		Notify( MSG_TEXTURE_SIZE_CHANGE );
		return true;
	}
}



VistaViewport::VistaViewportProperties::ViewportType VistaViewport::VistaViewportProperties::GetType() const
{
	const VistaViewport* pViewport = static_cast<const VistaViewport*>( GetParent() );
	return GetDisplayBridge()->GetViewportType( pViewport );
}

std::string VistaViewport::VistaViewportProperties::GetTypeString() const
{
	std::string sType = GetStringForViewportType( GetType() );
	if( sType.empty() )
		VISTA_THROW( "INVALID STEREO TYPE", -1 );
	return sType;
}

bool VistaViewport::VistaViewportProperties::SetType( const ViewportType eType )
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	if( GetDisplayBridge()->SetViewportType( eType, pViewport ) )
	{
		Notify( MSG_ENABLE_STATE_CHANGED );
		return true;
	}
	else
		return false;
}

bool VistaViewport::VistaViewportProperties::SetTypeByString( const std::string& sType )
{
	ViewportType eType = GetViewportTypeFromString( sType );
	if( eType == (ViewportType)-1 )
	{
		vstr::warnp() << "[VistaViewport] Unknown ViewportType \"" << sType <<"\"" << std::endl;
		return false;
	}
	return SetType( eType );
}



bool VistaViewport::VistaViewportProperties::GetIsEnabled() const
{
	const VistaViewport* pViewport = static_cast<const VistaViewport*>( GetParent() );
	return GetDisplayBridge()->GetViewportIsEnabled( pViewport );
}

bool VistaViewport::VistaViewportProperties::SetIsEnabled( const bool bSet )
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	if( GetDisplayBridge()->SetViewportIsEnabled( bSet, pViewport ) )
	{
		Notify( MSG_ENABLE_STATE_CHANGED );
		return true;
	}
	else
		return false;
}

int VistaViewport::VistaViewportProperties::GetPriority() const
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	return GetDisplayBridge()->GetViewportPriority( pViewport );
}

bool VistaViewport::VistaViewportProperties::SetPriority( const int nPriority )
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	if( GetDisplayBridge()->SetViewportPriority( nPriority, pViewport ) )
	{
		Notify( MSG_TYPE_CHANGED );
		return true;
	}
	else
		return false;
}


bool VistaViewport::VistaViewportProperties::GetResizeWithWindow() const
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	return GetDisplayBridge()->GetViewportResizeWithWindowResize( pViewport );
}

bool VistaViewport::VistaViewportProperties::SetResizeWithWindow( const bool bSet )
{
	if( bSet == GetResizeWithWindow() )
		return false;
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	GetDisplayBridge()->SetViewportResizeWithWindowResize( bSet, pViewport );
	Notify( MSG_RESIZE_WITH_WINDOW_CHANGED );
	return true;
}

bool VistaViewport::VistaViewportProperties::GetRepositionWithWindow() const
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	return GetDisplayBridge()->GetViewportRepositionWithWindowResize( pViewport );
}

bool VistaViewport::VistaViewportProperties::SetRepositionWithWindow( const bool bSet )
{
	if( bSet == GetResizeWithWindow() )
		return false;
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	GetDisplayBridge()->SetViewportRepositionWithWindowResize( bSet, pViewport );
	Notify( MSG_REPOSITION_WITH_WINDOW_CHANGED );
	return true;
}

string VistaViewport::VistaViewportProperties::GetReflectionableType() const
{
	return sSReflectionTypeVV;
}

int VistaViewport::VistaViewportProperties::AddToBaseTypeList(list<string> &rBtList) const
{
	int nSize = IVistaDisplayEntityProperties::AddToBaseTypeList(rBtList);
	rBtList.push_back(sSReflectionTypeVV);
	return nSize + 1;
}


VistaViewport::VistaViewportProperties::VistaViewportProperties(
	VistaDisplayEntity *pParent,
	IVistaDisplayBridge *pDisplayBridge)
  : IVistaDisplayEntityProperties(pParent, pDisplayBridge)
{
}

VistaViewport::VistaViewportProperties::~VistaViewportProperties()
{
}

std::string VistaViewport::VistaViewportProperties::GetStringForViewportType( const ViewportType eType )
{
	switch( eType )
	{
		case VT_MONO:
			return "MONO";
		case VT_QUADBUFFERED_STEREO:
			return "QUADBUFFERED_STEREO";
		case VT_RENDER_TO_TEXTURE:
			return "RENDER_TO_TEXTURE";
		case VT_RENDER_TO_TEXTURE_STEREO:
			return "RENDER_TO_TEXTURE_STEREO";
		case VT_POSTPROCESS:
			return "POSTPROCESS";
		case VT_POSTPROCESS_STEREO:
			return "POSTPROCESS_STEREO";
		case VT_POSTPROCESS_QUADBUFFERED_STEREO:
			return "POSTPROCESS_QUADBUFFERED_STEREO";
		case VT_OCULUS_RIFT:
			return "OCULUS_RIFT";
		case VT_OCULUS_RIFT_CLONE:
			return "OCULUS_RIFT_CLONE";
		case VT_VIVE:
			return "VIVE";
		case VT_ANAGLYPH:
			return "VT_ANAGLYPH";
		case VT_ANAGLYPH_MONOCHROME:
			return "ANAGLYPH_MONOCHROME";
		default:
			return "";
	};
}

VistaViewport::VistaViewportProperties::ViewportType VistaViewport::VistaViewportProperties::GetViewportTypeFromString( const std::string& sType )
{
	VistaAspectsComparisonStuff::StringCompareObject oCompare( false );
	if( oCompare( sType, "MONO" ) )
	{
		return VT_MONO;
	}
	else if( oCompare( sType, "STEREO" ) || oCompare( sType, "QUADBUFFER_STEREO" ) )
	{
		return VT_QUADBUFFERED_STEREO;
	}
	else if( oCompare( sType, "RENDER_TO_TEXTURE" ) || oCompare( sType, "RTT" ) )
	{
		return VT_RENDER_TO_TEXTURE;
	}
	else if( oCompare( sType, "RENDER_TO_TEXTURE_STEREO" ) || oCompare( sType, "RTT_STEREO" ) )
	{
		return VT_RENDER_TO_TEXTURE_STEREO;
	}
	else if( oCompare( sType, "POSTPROCESS" )  )
	{
		return VT_POSTPROCESS;
	}
	else if( oCompare( sType, "POSTPROCESS_STEREO" )  )
	{
		return VT_POSTPROCESS_STEREO;
	}
	else if( oCompare( sType, "POSTPROCESS_QUADBUFFERED_STEREO" ) || oCompare( sType, "POSTPROCESS_QUADBUFFER_STEREO" ) )
	{
		return VT_POSTPROCESS_QUADBUFFERED_STEREO;
	}
	else if( oCompare( sType, "OCULUS" ) || oCompare( sType, "OCULUS_RIFT" )  )
	{
		return VT_OCULUS_RIFT;
	}
	else if( oCompare( sType, "OCULUS_CLONE" ) || oCompare( sType, "OCULUS_RIFT_CLONE" )  )
	{
		return VT_OCULUS_RIFT_CLONE;
	}
	else if( oCompare( sType, "VIVE" ))
	{
		return VT_VIVE;
	}
	else if( oCompare( sType, "ANAGLYPH" ) || oCompare( sType, "ANAGLYPHIC"  ) )
	{
		return VT_ANAGLYPH;
	}
	else if( oCompare( sType, "ANAGLYPH_MONOCHROME" ) || oCompare( sType, "ANAGLYPHIC_MONOCHROME"  ) )
	{
		return VT_ANAGLYPH_MONOCHROME;
	}

	return (ViewportType)-1;
}

std::string VistaViewport::VistaViewportProperties::GetPostProcessShaderFile() const
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	return GetDisplayBridge()->GetViewportPostProcessShaderFile( pViewport );
}

bool VistaViewport::VistaViewportProperties::SetPostProcessShaderFile( const std::string& sShader )
{
	VistaViewport* pViewport = static_cast<VistaViewport*>( GetParent() );
	return GetDisplayBridge()->SetViewportPostProcessShaderFile( sShader, pViewport );
}



