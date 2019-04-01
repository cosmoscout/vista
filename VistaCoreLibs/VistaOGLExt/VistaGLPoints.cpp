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


#include <GL/glew.h>

#include "VistaGLPoints.h"

#include "VistaTexture.h"
#include "VistaOGLUtils.h"
#include "VistaGLSLShader.h"
#include "VistaShaderRegistry.h"

#include <VistaBase/VistaStreamUtils.h>

#include <vector>
#include <stack>

using namespace std;
/*============================================================================*/
/*  STATIC Variables                                                          */
/*============================================================================*/
namespace VistaGLPoints
{
	const string g_strVert    = "VistaGLPoints_vert.glsl";
	const string g_strGeom    = "VistaGLPoints_geom.glsl";
	const string g_strHalo    = "VistaGLPoints_Halo_frag.glsl";
	const string g_strGlyph   = "VistaGLPoints_Glyph_frag.glsl";
	const string g_strPhong   = "Vista_PhongLighting_aux.glsl";
	const string g_strGooch   = "Vista_GoochShading_aux.glsl";

	struct PointType
	{
		PointType( VistaGLSLShader* pShader )
			:	pShader( pShader )
		{ }

		PointType()
			:	pShader( NULL )
		{ }

		std::vector<VistaTexture*> vecTextures;
		VistaGLSLShader*           pShader;
	};

	std::vector<PointType> g_vecTypes;
	int					   g_iEnabledType = -1;


	struct PointAttributes
	{
		PointAttributes()
			:	fPointSize( 0.01f )
			,	fHaloSize( 0.25f )
			,	v3Dir( 0.0f, 1.0f, 0.0f )
			,	bDirInViewCoord( true )
		{ }

		float	fPointSize;
		float	fHaloSize;

		VistaVector3D v3Dir;
		bool bDirInViewCoord;
	};

	PointAttributes g_oCurrentAttributes;
	std::stack<PointAttributes> g_oAttributeStack;
};
/*============================================================================*/
/*  STATIC FUNKTIONS                                                          */
/*============================================================================*/
namespace VistaGLPoints
{
	void UpdateHaloTexture()
	{
		if( g_vecTypes.size() == 0 )
			return;

		VistaTexture* pTexture = NULL ;
		if( g_vecTypes[POINTS_WITH_HALOS].vecTextures.empty() ) 
		{
			pTexture = new VistaTexture(GL_TEXTURE_1D);
			g_vecTypes[POINTS_WITH_HALOS].vecTextures.push_back(pTexture);
		}
		else
		{
			pTexture = g_vecTypes[POINTS_WITH_HALOS].vecTextures[0];
		}

		const int iWidth = 64;
		unsigned char* pData = new unsigned char[ iWidth ];

		float fBorder = ( 1 - g_oCurrentAttributes.fHaloSize );
		for(int i=0; i<iWidth; ++i)
		{
			float f = float(i)/float(iWidth-1);
			pData[i] = (f < fBorder ) ? 0 : 255;
		}

		pTexture->Bind();
		pTexture->UploadTexture( iWidth, 0, pData, true, GL_ALPHA, GL_UNSIGNED_BYTE );
		pTexture->SetWrapS( GL_CLAMP_TO_EDGE );
		pTexture->SetMinFilter( GL_LINEAR_MIPMAP_LINEAR );
		pTexture->SetMagFilter( GL_LINEAR );
		pTexture->Unbind();

		delete[] pData;
	}

	void InitSphareTextures()
	{
		VistaTexture* pNormalTexture = new VistaTexture(GL_TEXTURE_2D);
		VistaTexture* pDeapthTexture  = new VistaTexture(GL_TEXTURE_2D);


		int iRes = 128;
		float* pNormals = VistaOGLUtils::CreateSphereNormalsTextureData( iRes, iRes, true );

		float* pDeapth = new float[iRes*iRes];
		for( int i = 0; i < iRes*iRes; ++i )
			pDeapth[i] = 2*pNormals[ 4*i + 2 ] - 1;

		pNormalTexture->Bind();
		pNormalTexture->UploadTexture( iRes, iRes, pNormals, true, GL_RGBA, GL_FLOAT );
		pNormalTexture->SetWrapS( GL_CLAMP_TO_EDGE );
		pNormalTexture->SetWrapT( GL_CLAMP_TO_EDGE );
		pNormalTexture->SetMinFilter( GL_LINEAR_MIPMAP_LINEAR );
		pNormalTexture->SetMagFilter( GL_LINEAR );
		pNormalTexture->Unbind();

		pDeapthTexture->Bind();
		pDeapthTexture->UploadTexture( iRes, iRes, pDeapth, true, GL_ALPHA, GL_FLOAT );
		pDeapthTexture->SetWrapS( GL_CLAMP_TO_EDGE );
		pDeapthTexture->SetWrapT( GL_CLAMP_TO_EDGE );
		pDeapthTexture->SetMinFilter( GL_LINEAR_MIPMAP_LINEAR );
		pDeapthTexture->SetMagFilter( GL_LINEAR );
		pDeapthTexture->Unbind();

		g_vecTypes[SPHERE_WITH_PHONG_LIGTHING].vecTextures.push_back( pNormalTexture );
		g_vecTypes[SPHERE_WITH_PHONG_LIGTHING].vecTextures.push_back( pDeapthTexture );

		g_vecTypes[SPHERE_WITH_GOOCH_SHADING].vecTextures.push_back( pNormalTexture );
		g_vecTypes[SPHERE_WITH_GOOCH_SHADING].vecTextures.push_back( pDeapthTexture );

		delete[] pNormals;
		delete[] pDeapth;
	}

	void InitConeTextures()
	{
		VistaTexture* pNormalTexture = new VistaTexture(GL_TEXTURE_2D);
		VistaTexture* pDeapthTexture = new VistaTexture(GL_TEXTURE_2D);

		int iRes = 128;
		float* pNormals = VistaOGLUtils::CreateConeNormalsTextureData( iRes, iRes, true );
		float* pDeapth  = VistaOGLUtils::CreateConeDeapthTextureData( iRes, iRes );

		pNormalTexture->Bind();
		pNormalTexture->UploadTexture( iRes, iRes, pNormals, true, GL_RGBA, GL_FLOAT );
		pNormalTexture->SetWrapS( GL_CLAMP_TO_EDGE );
		pNormalTexture->SetWrapT( GL_CLAMP_TO_EDGE );
		pNormalTexture->SetMinFilter( GL_LINEAR_MIPMAP_LINEAR );
		pNormalTexture->SetMagFilter( GL_LINEAR );
		pNormalTexture->Unbind();

		pDeapthTexture->Bind();
		pDeapthTexture->UploadTexture( iRes, iRes, pDeapth, true, GL_ALPHA, GL_FLOAT );
		pDeapthTexture->SetWrapS( GL_CLAMP_TO_EDGE );
		pDeapthTexture->SetWrapT( GL_CLAMP_TO_EDGE );
		pDeapthTexture->SetMinFilter( GL_LINEAR_MIPMAP_LINEAR );
		pDeapthTexture->SetMagFilter( GL_LINEAR );
		pDeapthTexture->Unbind();

		g_vecTypes[CONE_WITH_PHONG_LIGTHING].vecTextures.push_back( pNormalTexture );
		g_vecTypes[CONE_WITH_PHONG_LIGTHING].vecTextures.push_back( pDeapthTexture );

		g_vecTypes[CONE_WITH_GOOCH_SHADING].vecTextures.push_back( pNormalTexture );
		g_vecTypes[CONE_WITH_GOOCH_SHADING].vecTextures.push_back( pDeapthTexture );

		delete[] pNormals;
		delete[] pDeapth;
	}

	VistaGLSLShader* CreateShader(
		const string& strVS_Name,
		const string& strGS_Name,
		const string& strFS_Name )
	{
		VistaShaderRegistry* pShaderReg = &VistaShaderRegistry::GetInstance();

		string strVS = pShaderReg->RetrieveShader( strVS_Name );
		string strGS = pShaderReg->RetrieveShader( strGS_Name );
		string strFS = pShaderReg->RetrieveShader( strFS_Name );

		if(strVS.empty() || strGS.empty() || strFS.empty())
		{
			vstr::errp() << "[VistaGLLine] - required shader not found." << endl;
			vstr::IndentObject oIndent;
			if( strVS.empty() ) vstr::erri() << "Can't find " << strVS_Name << endl;
			if( strGS.empty() ) vstr::erri() << "Can't find " << strGS_Name << endl;
			if( strFS.empty() ) vstr::erri() << "Can't find " << strFS_Name << endl;
			return NULL;
		}

		VistaGLSLShader* pShader = new VistaGLSLShader();

		pShader->InitVertexShaderFromString(   strVS );
		pShader->InitGeometryShaderFromString( strGS );
		pShader->InitFragmentShaderFromString( strFS );

		if( !pShader->Link() ) { delete pShader; return NULL; }

		return pShader;
	}

	VistaGLSLShader* CreateShader(
		const string& strVS_Name,
		const string& strGS_Name,
		const string& strFS_Name,
		const string& strEXT_Name )
	{
		VistaShaderRegistry* pShaderReg = &VistaShaderRegistry::GetInstance();

		string strVS  = pShaderReg->RetrieveShader( strVS_Name  );
		string strGS  = pShaderReg->RetrieveShader( strGS_Name  );
		string strFS  = pShaderReg->RetrieveShader( strFS_Name  );
		string strEXT = pShaderReg->RetrieveShader( strEXT_Name );

		if( strVS.empty() || strGS.empty() || strFS.empty() || strEXT.empty() )
		{
			vstr::errp() << "[VistaGLLine] - required shader not found."  << endl;
			vstr::IndentObject oIndent;
			if(  strVS.empty() ) vstr::erri() << "Can't find " << strVS_Name  << endl;
			if(  strGS.empty() ) vstr::erri() << "Can't find " << strGS_Name  << endl;
			if(  strFS.empty() ) vstr::erri() << "Can't find " << strFS_Name  << endl;
			if( strEXT.empty() ) vstr::erri() << "Can't find " << strEXT_Name << endl;
			return NULL;
		}

		VistaGLSLShader* pShader = new VistaGLSLShader();

		pShader->InitVertexShaderFromString(   strVS  );
		pShader->InitGeometryShaderFromString( strGS  );
		pShader->InitFragmentShaderFromString( strFS  );
		pShader->InitFragmentShaderFromString( strEXT );

		if( !pShader->Link() ) { delete pShader; return NULL; }

		return pShader;
	}

	void InitPointTypes()
	{
		VistaGLSLShader* pHalo = 
			CreateShader( g_strVert, g_strGeom, g_strHalo );
		VistaGLSLShader* pPhong = 
			CreateShader( g_strVert, g_strGeom, g_strGlyph, g_strPhong  );
		VistaGLSLShader* pGooch = 
			CreateShader( g_strVert, g_strGeom, g_strGlyph, g_strGooch );

		g_vecTypes.resize( NUM_TYPES );
		g_vecTypes[ POINTS_WITH_HALOS          ].pShader = pHalo;
		g_vecTypes[ SPHERE_WITH_PHONG_LIGTHING ].pShader = pPhong;
		g_vecTypes[ SPHERE_WITH_GOOCH_SHADING  ].pShader = pGooch;
		g_vecTypes[ CONE_WITH_PHONG_LIGTHING   ].pShader = pPhong;
		g_vecTypes[ CONE_WITH_GOOCH_SHADING    ].pShader = pGooch;

		UpdateHaloTexture();
		InitSphareTextures();
		InitConeTextures();
	}
};

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

void VistaGLPoints::SetPointSize( float fSize )
{
	if( g_oCurrentAttributes.fPointSize == fSize )
		return;

	g_oCurrentAttributes.fPointSize = fSize;

	if( g_iEnabledType >= 0 )
	{
		// a shader is currently bound so we have to update the uniform variable.
		VistaGLSLShader* pShader = g_vecTypes[ g_iEnabledType ].pShader;
		int iUniLoc = pShader->GetUniformLocation("u_fPointSize");
		if( -1 != iUniLoc ) glUniform1f( iUniLoc, fSize );
	}
}


void VistaGLPoints::SetHaloSize(float fHaloSize)
{
	if( g_oCurrentAttributes.fHaloSize != fHaloSize )
	{
		g_oCurrentAttributes.fHaloSize = fHaloSize;
		UpdateHaloTexture();
	}
}

void VistaGLPoints::SetDirection( const VistaVector3D& v3Dir, bool bInViewCoord )
{
	if( g_oCurrentAttributes.v3Dir == v3Dir && 
		g_oCurrentAttributes.bDirInViewCoord == bInViewCoord )
		return;

	g_oCurrentAttributes.v3Dir = v3Dir;
	g_oCurrentAttributes.bDirInViewCoord = bInViewCoord;

	if( g_iEnabledType >= 0 )
	{
		// a shader is currently bound so we have to update the uniform variable.
		VistaGLSLShader* pShader = g_vecTypes[ g_iEnabledType ].pShader;
		int iUniLoc = pShader->GetUniformLocation("u_v3Dir");
		if( -1 != iUniLoc ) 
			glUniform3f( iUniLoc, v3Dir[0], v3Dir[1], v3Dir[2] );

		iUniLoc = pShader->GetUniformLocation("u_bDirInViewCoord");
		if( -1 != iUniLoc ) 
			glUniform1i( iUniLoc, bInViewCoord );
	}
}
/*============================================================================*/

bool VistaGLPoints::Enable( int iType /*= POINT_WITH_HALO*/ )
{
	if( g_vecTypes.size() < NUM_TYPES )
		InitPointTypes();

	if(	iType < 0 || static_cast< std::size_t >(iType) >= g_vecTypes.size() ) 
		return false;

	if(	g_iEnabledType >= 0 )
		return false;

	VistaGLSLShader* pShader                 = g_vecTypes[iType].pShader;
	const vector<VistaTexture*>& vecTextures = g_vecTypes[iType].vecTextures;
	if(!pShader) 
		return false; 

	glPushAttrib( GL_TEXTURE_BIT | GL_ENABLE_BIT );

	glEnable( GL_ALPHA_TEST );
	glAlphaFunc( GL_GREATER, 0.5 );

	for ( std::size_t n = 0; n<vecTextures.size(); ++n )
		vecTextures[n]->Bind( GL_TEXTURE0 + static_cast< GLint >( n ) );

	pShader->Bind();
	g_iEnabledType = iType;

	int iUniLoc = pShader->GetUniformLocation( "u_fPointSize" );
	if(-1 != iUniLoc) 
		glUniform1f( iUniLoc, g_oCurrentAttributes.fPointSize);
	iUniLoc = pShader->GetUniformLocation( "u_v3Dir" );
	if(-1 != iUniLoc) 
		glUniform3f( iUniLoc, 
			g_oCurrentAttributes.v3Dir[0],
			g_oCurrentAttributes.v3Dir[1],
			g_oCurrentAttributes.v3Dir[2] );

	iUniLoc = pShader->GetUniformLocation("u_bDirInViewCoord");
	if( -1 != iUniLoc ) 
		glUniform1i( iUniLoc, g_oCurrentAttributes.bDirInViewCoord );

	return true;
}
/*============================================================================*/
bool VistaGLPoints::Disable()
{
	if( g_iEnabledType < 0 )
		return false;

	VistaGLSLShader* pShader                 = g_vecTypes[g_iEnabledType].pShader;
	const vector<VistaTexture*>& vecTextures = g_vecTypes[g_iEnabledType].vecTextures;

	pShader->Release();
	for ( std::size_t n = 0; n<vecTextures.size(); ++n )
		vecTextures[n]->Unbind( GL_TEXTURE0 + static_cast< GLint>( n ) );

	g_iEnabledType = -1;

	glPopAttrib();

	return true;
}
/*============================================================================*/
bool VistaGLPoints::PushAttrib()
{
	// @TODO test for stack overflow
	g_oAttributeStack.push( g_oCurrentAttributes );
	return true;
}
bool VistaGLPoints::PopAttrib()
{
	if( g_oAttributeStack.empty() )
	{
		vstr::errp() << "[VistaGLPoints::PopAttrib] Stack underflow!" << endl;
		return false;
	}

	SetPointSize( g_oAttributeStack.top().fPointSize );
	SetHaloSize( g_oAttributeStack.top().fHaloSize );
	SetDirection( g_oAttributeStack.top().v3Dir, 
		          g_oAttributeStack.top().bDirInViewCoord );
	g_oAttributeStack.pop();

	return true;
}
/*============================================================================*/
int VistaGLPoints::AddCustomPointType(
	VistaGLSLShader* pShader, const std::vector<VistaTexture*>& vecTextures )
{
	if( g_vecTypes.size() < NUM_TYPES )
		InitPointTypes();

	g_vecTypes.push_back( PointType( pShader ) );
	g_vecTypes.back().vecTextures = vecTextures;

	return static_cast< int >( g_vecTypes.size() ) - 1;
}

VistaGLSLShader* VistaGLPoints::GetShader( int iType )
{
	if( g_vecTypes.size() < NUM_TYPES )
		InitPointTypes();

	if( iType<0 || static_cast<unsigned int>(iType) >= g_vecTypes.size() )
		return NULL;

	return g_vecTypes[iType].pShader;

}
const std::vector<VistaTexture*>* VistaGLPoints::GetTextures( int iType )
{
	if( g_vecTypes.size() < NUM_TYPES )
		InitPointTypes();

	if( iType<0 || static_cast<unsigned int>(iType) >= g_vecTypes.size() )
		return NULL;

	return &(g_vecTypes[iType].vecTextures);
}
/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
