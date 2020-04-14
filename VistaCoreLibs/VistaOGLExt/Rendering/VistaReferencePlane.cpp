/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2016 RWTH Aachen University               */
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


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <GL/glew.h>

#include "VistaReferencePlane.h"

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaMathBasics.h>

#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaOGLExt/VistaShaderRegistry.h>
#include <VistaOGLExt/VistaFramebufferObj.h>
#include <VistaOGLExt/VistaGLSLShader.h>
#include <VistaOGLExt/VistaTexture.h>
#include <VistaOGLExt/VistaOGLUtils.h>

#include <iostream>
#include <cassert>
#include <cstring>

using namespace std;

/******************************************************************************/
/*  Constructor/Destructor			                                          */
/******************************************************************************/
VistaReferencePlane::VistaReferencePlane(VistaSceneGraph* const pSG)
	:	IVistaOpenGLDraw()
	,   m_pShader(NULL)
	,	m_pTexture(NULL)
	,	m_pFBO(NULL)
	,	m_pProperties(NULL)
	,	m_bTileTextureReadyForUse( false )
{
	//if(GLEW_OK != glewInit())
	//	vstr::errp() << "[VfaReferencePlane] Init'ing glew failed!" << endl;
	//	

	pSG->NewOpenGLNode(pSG->GetRoot(), this);
}

VistaReferencePlane::~VistaReferencePlane()
{
	delete m_pShader;
	delete m_pTexture;
	delete m_pFBO;
}

/******************************************************************************/
/*  IVistaOpenGLDraw Interface                                                */
/******************************************************************************/
bool VistaReferencePlane::Init()
{
	VistaShaderRegistry* pShaderReg = &VistaShaderRegistry::GetInstance();

	// *** Init Shader ***
	std::string strVert = pShaderReg->RetrieveShader("VistaReferencePlane_Tiling_vert.glsl");
	std::string strFrag = pShaderReg->RetrieveShader("VistaReferencePlane_Tiling_frag.glsl");

	if( strVert.empty() || strFrag.empty() )
	{
		vstr::errp() << "[VistaReferencePlane] - can't find required Shader." << endl;
		return false;
	}

	m_pShader = new VistaGLSLShader();
	m_pShader->InitFromStrings( strVert, strFrag );
	m_pShader->Link();

	// *** Init Texture ***
	m_pTexture = new VistaTexture( GL_TEXTURE_2D );
	m_pTexture->Bind();
	m_pTexture->UploadTexture( 1024, 1024, NULL, false );
	m_pTexture->SetWrapR( GL_REPEAT );
	m_pTexture->SetWrapS( GL_REPEAT );
	m_pTexture->SetWrapT( GL_REPEAT );
	m_pTexture->SetMagFilter( GL_LINEAR );
	m_pTexture->SetMinFilter( GL_LINEAR_MIPMAP_LINEAR );
	m_pTexture->Unbind();

	// *** Init FramebufferObject ***
	m_pFBO = new VistaFramebufferObj();
	m_pFBO->Attach( m_pTexture, GL_COLOR_ATTACHMENT0_EXT );

	// *** Create Properties ***
	m_pProperties = new VfaReferencePlaneProperties();
	m_pProperties->AttachObserver( this );

	return true;
}

bool VistaReferencePlane::Do()
{
	if( !m_pProperties || !m_pProperties->GetVisible() )
		return false;

	if( !m_bTileTextureReadyForUse ) UpdateTileTexture();

	glPushAttrib( GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthFunc(GL_ALWAYS);

	m_pTexture->Enable();
	m_pTexture->Bind();

	glColor3f(1.0f, 1.0f, 1.0f);

	VistaVector3D v3X( 1.0f, 0.0f, 0.0f);
	VistaVector3D v3Z( 0.0f, 0.0f, 1.0f);

	v3X	= m_pProperties->m_qOrientation.Rotate(v3X);
	v3Z	= m_pProperties->m_qOrientation.Rotate(v3Z);

	v3X.Normalize();
	v3Z.Normalize();

	v3X *= 0.5f * m_pProperties->m_fPlaneWidth;
	v3Z *= 0.5f * m_pProperties->m_fPlaneLength;

	VistaVector3D v3Corner1 = m_pProperties->m_v3Center - v3X - v3Z;
	VistaVector3D v3Corner2 = m_pProperties->m_v3Center + v3X - v3Z;
	VistaVector3D v3Corner3 = m_pProperties->m_v3Center + v3X + v3Z;
	VistaVector3D v3Corner4 = m_pProperties->m_v3Center - v3X + v3Z;

	float fTexCoordX = 0.5f * m_pProperties->m_fPlaneWidth / m_pProperties->m_fTileWidth;
	float fTexCoordY = 0.5f * m_pProperties->m_fPlaneLength / m_pProperties->m_fTileLength;

	glBegin(GL_QUADS);
	glTexCoord2f( -fTexCoordX, -fTexCoordY );
	glVertex3fv(&v3Corner1[0]);
	glTexCoord2f( +fTexCoordX, -fTexCoordY );
	glVertex3fv(&v3Corner2[0]);
	glTexCoord2f( +fTexCoordX, +fTexCoordY );
	glVertex3fv(&v3Corner3[0]);
	glTexCoord2f( -fTexCoordX, +fTexCoordY );
	glVertex3fv(&v3Corner4[0]);

	glEnd();

	m_pTexture->Unbind();
	m_pTexture->Disable();

	glPopMatrix();

	glPopAttrib();
	return true;
}


bool VistaReferencePlane::GetBoundingBox(VistaBoundingBox &bb)
{
	return false;
}

VistaReferencePlane::VfaReferencePlaneProperties* VistaReferencePlane::GetProperties() const
{
	return m_pProperties;
}

/******************************************************************************/
/*  IVistaObserver Interface                                                  */
/******************************************************************************/
bool VistaReferencePlane::ObserveableDeleteRequest(	
		IVistaObserveable *pObserveable, int nTicket )
{
	return true;
}

void VistaReferencePlane::ObserveableDelete(
	IVistaObserveable *pObserveable, int nTicket )
{ }

void VistaReferencePlane::ReleaseObserveable(
	IVistaObserveable *pObserveable, int nTicket )
{ }

bool VistaReferencePlane::Observes( IVistaObserveable *pObserveable )
{
	return pObserveable == m_pProperties;
}

void VistaReferencePlane::Observe( 
	IVistaObserveable *pObserveable, int eTicket )
{ }

void VistaReferencePlane::ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket)
{
	if(!m_pProperties)
		return;

	// intercept visibility change to turn text on or off
	if(	msg == VfaReferencePlaneProperties::MSG_CHANGE_LINE_WIDTH	||
		msg == VfaReferencePlaneProperties::MSG_CHANGE_TILE_DIMS	||
		msg == VfaReferencePlaneProperties::MSG_CHANGE_TILE_COLOR	||
		msg == VfaReferencePlaneProperties::MSG_CHANGE_LINE_COLOR	)
	{
		m_bTileTextureReadyForUse = false;
	}
}

/******************************************************************************/
/*  protected Interface                                                       */
/******************************************************************************/
void VistaReferencePlane::UpdateTileTexture()
{
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_VIEWPORT_BIT );

	m_pFBO->Bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_COLOR_BUFFER_BIT);

	m_pShader->Bind();

	int iLoc = -1;

	iLoc = m_pShader->GetUniformLocation( "u_v4LineColor" );
	if(iLoc != -1) m_pShader->SetUniform( iLoc, &(m_pProperties->m_oLineColor[0]) );
	iLoc = m_pShader->GetUniformLocation( "u_v4TileColor" );
	if(iLoc != -1) m_pShader->SetUniform( iLoc, &(m_pProperties->m_oTileColor[0]) );

	iLoc = m_pShader->GetUniformLocation( "u_fLineWidth" );
	if(iLoc != -1) m_pShader->SetUniform( iLoc, m_pProperties->m_fLineWidth );
	iLoc = m_pShader->GetUniformLocation( "u_fTileWidth" );
	if(iLoc != -1) m_pShader->SetUniform( iLoc, m_pProperties->m_fTileWidth );
	iLoc = m_pShader->GetUniformLocation("u_fTileLength" );
	if(iLoc != -1) m_pShader->SetUniform( iLoc, m_pProperties->m_fTileLength );

	VistaOGLUtils::BeginOrtho2D( 0.0f, 1.0f, 0.0f, 1.0f );
	glBegin(GL_QUADS);
	glVertex2f( 1.0f, 0.0f);
	glVertex2f( 1.0f, 1.0f);
	glVertex2f( 0.0f, 1.0f);
	glVertex2f( 0.0f, 0.0f);
	glEnd();
	VistaOGLUtils::EndOrtho();

	m_pShader->Release();
	m_pFBO->Release();

	m_pTexture->Bind();
	m_pTexture->GenerateMipmaps();
	m_pTexture->Unbind();

	glPopAttrib();

	m_bTileTextureReadyForUse = true;
}


/*============================================================================*/
/*  Properties                                                                */
/*============================================================================*/
static const string SsReflectionType("VflReferencePlane");



VistaReferencePlane::VfaReferencePlaneProperties::VfaReferencePlaneProperties()
	:	m_oLineColor( VistaColor::BLACK )
	,	m_oTileColor( VistaColor::WHITE )
	,	m_v3Center( 0.0f, 0.0f, 0.0f )
	,	m_qOrientation()
	,	m_fPlaneWidth( 10.0f )
	,	m_fPlaneLength( 10.0f )
	,	m_fTileWidth( 1.0f )
	,	m_fTileLength( 0.1f )
	,	m_bVisible( true )
	,	m_fLineWidth( 0.01f )
{ }

VistaReferencePlane::VfaReferencePlaneProperties::~VfaReferencePlaneProperties()
{ }

string VistaReferencePlane::VfaReferencePlaneProperties::GetReflectionableType() const
{
	return SsReflectionType;
}

int VistaReferencePlane::VfaReferencePlaneProperties::AddToBaseTypeList(list<string> &rBtList) const
{
	int nSize = IVistaReflectionable::AddToBaseTypeList(rBtList);
	rBtList.push_back(SsReflectionType);
	return nSize + 1;
}

/******************************************************************************/
/*  Get/SetVisible()                                                          */
/******************************************************************************/
bool VistaReferencePlane::VfaReferencePlaneProperties::GetVisible() const
{
	return m_bVisible;
}
bool VistaReferencePlane::VfaReferencePlaneProperties::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
	return true;
}

/******************************************************************************/
/*  Get/SetPlaneDims()                                                        */
/******************************************************************************/
bool VistaReferencePlane::VfaReferencePlaneProperties::GetPlaneDims(float &fWidth, float &fLength) const
{
	fWidth	= m_fPlaneWidth;
	fLength = m_fPlaneLength;
	return true;
}

bool VistaReferencePlane::VfaReferencePlaneProperties::SetPlaneDims(float fWidth, float fLength)
{
	m_fPlaneWidth	= fWidth < 0.0f ? 0.1f : fWidth;
	m_fPlaneLength	= fLength < 0.0f ? 0.1f : fLength;
	Notify(MSG_CHANGE_PLANE_DIMS);
	return true;
}

/******************************************************************************/
/*  Get/SetTileDims()                                                         */
/******************************************************************************/
bool VistaReferencePlane::VfaReferencePlaneProperties::GetTileDims(float &fWidth, float &fLength) const
{
	fWidth	= m_fTileWidth;
	fLength	= m_fTileLength;
	return true;
}

bool VistaReferencePlane::VfaReferencePlaneProperties::SetTileDims(float fWidth, float fLength)
{
	m_fTileWidth	= fWidth < 0.0f ? 0.1f : fWidth;
	m_fTileLength	= fLength < 0.0f ? 0.1f : fLength;

	float fSmaller = m_fTileWidth < m_fTileLength ? m_fTileWidth : m_fTileLength;
	if(fSmaller < m_fLineWidth)
	{
		vstr::warnp() << "[VistaReferencePlane] - LineWidth should be smaller than TileWidth and TileLength." << endl;
	}

	Notify(MSG_CHANGE_TILE_DIMS);

	return true;
}
/******************************************************************************/
/*  Get/SetLineWidth()                                                        */
/******************************************************************************/
float VistaReferencePlane::VfaReferencePlaneProperties::GetLineWidth() const
{
	return m_fLineWidth;
}

bool VistaReferencePlane::VfaReferencePlaneProperties::SetLineWidth(float fLineWidth)
{
	float fSmaller = m_fTileWidth < m_fTileLength ? m_fTileWidth : m_fTileLength;

	m_fLineWidth = fLineWidth <= 0.0f ? 0.01f : fLineWidth;

	if(fSmaller < m_fLineWidth)
	{
		vstr::warnp() << "[VistaReferencePlane] - LineWidth should be smaller than TileWidth and TileLength." << endl;
	}

	Notify(MSG_CHANGE_LINE_WIDTH);

	return true;
}
/******************************************************************************/
/*  Get/SetTileColor()                                                        */
/******************************************************************************/
VistaColor VistaReferencePlane::VfaReferencePlaneProperties::GetTileColor() const
{
	return m_oTileColor;
}
void VistaReferencePlane::VfaReferencePlaneProperties::GetTileColor(float pColor[4]) const
{
	m_oTileColor.GetValues( pColor, VistaColor::RGBA );
}

bool VistaReferencePlane::VfaReferencePlaneProperties::SetTileColor(float pColor[4])
{
	return SetTileColor( VistaColor(pColor, VistaColor::RGBA) );
}
bool VistaReferencePlane::VfaReferencePlaneProperties::SetTileColor(const VistaColor &val)
{	
	if (m_oTileColor == val)
		return false;

	m_oTileColor = val;
	Notify(MSG_CHANGE_TILE_COLOR);

	return true;
}

/******************************************************************************/
/*  Get/SetLineColor()                                                        */
/******************************************************************************/
VistaColor VistaReferencePlane::VfaReferencePlaneProperties::GetLineColor() const
{
	return m_oLineColor;
}
void VistaReferencePlane::VfaReferencePlaneProperties::GetLineColor(float pColor[4]) const
{
	m_oLineColor.GetValues( pColor, VistaColor::RGBA );
}

bool VistaReferencePlane::VfaReferencePlaneProperties::SetLineColor(float pColor[4])
{
	return SetLineColor( VistaColor(pColor, VistaColor::RGBA) );
}
bool VistaReferencePlane::VfaReferencePlaneProperties::SetLineColor(const VistaColor &val)
{
	if (m_oLineColor == val)
		return false;

	m_oLineColor = val;
	Notify(MSG_CHANGE_LINE_COLOR);

	return true;
}
/******************************************************************************/
/*  Get/SetCenter()                                                           */
/******************************************************************************/
VistaVector3D VistaReferencePlane::VfaReferencePlaneProperties::GetCenter() const
{
	return m_v3Center;
}
void VistaReferencePlane::VfaReferencePlaneProperties::GetCenter(float pCenter[3]) const
{
	m_v3Center.GetValues(pCenter);
}

bool VistaReferencePlane::VfaReferencePlaneProperties::SetCenter(float pCenter[3])
{
	return this->SetCenter(VistaVector3D(pCenter));
}
bool VistaReferencePlane::VfaReferencePlaneProperties::SetCenter(const VistaVector3D &v3C)
{
	if (m_v3Center == v3C)
		return false;

	m_v3Center = v3C;
	Notify(MSG_CHANGE_PLANE_CENTER);

	return true;
}

/******************************************************************************/
/*  Get/SetOrientation()                                                      */
/******************************************************************************/
VistaQuaternion VistaReferencePlane::VfaReferencePlaneProperties::GetOrientation() const
{
	return m_qOrientation;
}

void VistaReferencePlane::VfaReferencePlaneProperties::SetOrientation(
	const VistaQuaternion& qOrientation )
{
	if( qOrientation != m_qOrientation )
	{
		m_qOrientation = qOrientation;
		Notify( MSG_CHANGE_PLANE_ORIENTATION );
	}
}


/******************************************************************************/
/*  Get/SetOrientation()                                                      */
/******************************************************************************/
static IVistaPropertyGetFunctor *aCgFunctorsVSD[] =
{
	new TVistaPropertyGet<bool, VistaReferencePlane::VfaReferencePlaneProperties, VistaProperty::PROPT_BOOL>
	( "VISIBLE", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::GetVisible),

	new TVistaProperty2RefGet<float, VistaReferencePlane::VfaReferencePlaneProperties>
	( "PLANE_DIMS", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::GetPlaneDims),

	new TVistaProperty2RefGet<float, VistaReferencePlane::VfaReferencePlaneProperties>
	( "TILE_DIMS", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::GetTileDims),

	new TVistaPropertyGet<float, VistaReferencePlane::VfaReferencePlaneProperties, VistaProperty::PROPT_DOUBLE>
	("LINE_WIDTH", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::GetLineWidth),

	new TVistaPropertyArrayGet<VistaReferencePlane::VfaReferencePlaneProperties, float, 4 >
	( "TILE_COLOR", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::GetTileColor),

	new TVistaPropertyArrayGet<VistaReferencePlane::VfaReferencePlaneProperties, float, 4 >
	( "LINE_COLOR", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::GetLineColor),

	new TVistaPropertyArrayGet<VistaReferencePlane::VfaReferencePlaneProperties, float, 3 >
	( "CENTER", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::GetCenter),

	NULL
};

static IVistaPropertySetFunctor *aCsFunctors[] =
{
	new TVistaPropertySet<bool, bool, VistaReferencePlane::VfaReferencePlaneProperties>
	("VISIBLE", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::SetVisible),

	new TVistaProperty2ValSet<float, VistaReferencePlane::VfaReferencePlaneProperties>
	( "PLANE_DIMS", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::SetPlaneDims),

	new TVistaProperty2ValSet<float, VistaReferencePlane::VfaReferencePlaneProperties>
	( "TILE_DIMS", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::SetTileDims),

	new TVistaPropertySet<float, float,
	VistaReferencePlane::VfaReferencePlaneProperties>
	("LINE_WIDTH", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::SetLineWidth),

	new TVistaPropertyArraySet<VistaReferencePlane::VfaReferencePlaneProperties, float, 4>
	("TILE_COLOR", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::SetTileColor),

	new TVistaPropertyArraySet<VistaReferencePlane::VfaReferencePlaneProperties, float, 4>
	("LINE_COLOR", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::SetLineColor),

	new TVistaPropertyArraySet<VistaReferencePlane::VfaReferencePlaneProperties, float, 3>
	("CENTER", SsReflectionType,
	&VistaReferencePlane::VfaReferencePlaneProperties::SetCenter),

	NULL
};