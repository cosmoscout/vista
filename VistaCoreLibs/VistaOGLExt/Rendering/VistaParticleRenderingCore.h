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


#ifndef __VISTAPARTICLERENDERERINGCORE_H
#define __VISTAPARTICLERENDERERINGCORE_H

#include "../VistaOGLExtConfig.h"

#include <VistaAspects/VistaObserver.h>

#include <VistaBase/VistaVector3D.h>
#include <VistaBase/VistaColor.h>

class VistaTexture;
class VistaGLSLShader;
class VflLookupTexture;
class VistaBufferObject;
class VistaVertexArrayObject;
class VistaParticleRenderingProperties;

class VISTAOGLEXTAPI VistaParticleRenderingCore : public IVistaObserver
{
public:
	VistaParticleRenderingCore();
	virtual ~VistaParticleRenderingCore();

	bool Init();

	void Draw();

	VistaParticleRenderingProperties* GetProperties();

	void SetPointSize( float fSize );
	float GetPointSize() const;

	/**
	 * Sets the scale factor for the Particle Radii.
	 * If no Particle radii texture is set, this will be 
	 * the absolute radius of parcels:
	 */
	void SetParticleRadiusScale( float fRadius );
	float GetParticleRadiusScale() const;

	/**
	 * Sets the particle Color.
	 * This color will be used if no LUT is used.
	 */
	void SetParticleColor( const VistaColor& rColor );
	const VistaColor& GetParticleColor() const;

	/**
	 * Sets the lookup texture which is used to map scalar values to colors.
	 */
	void SetLookupTexture( VistaTexture* pLUT );
	VistaTexture* GetLookupTexture() const;

	void SetLookupRange( float fMin, float fMax );
	void GetLookupRange( float& fMin, float& fMax ) const;

	void SetParticleCount( unsigned int iNumParticles );
	unsigned int GetParticleCount() const;

	void SetViewerPosition( const VistaVector3D& v3ViewerPos );
	const VistaVector3D& GetViewerPosition() const;

	void SetLightDirection( const VistaVector3D& v3LightDir );
	const VistaVector3D& GetLightDirection() const;
	
	/**
	 * Sets the texture which contains the particle data. 
	 * (rgb-Values = Particle Positions; alpha-Values = Scalar-Values)
	 * Must be a GL_TEXTURE_2D;
	 */
	void SetDataTexture( VistaTexture* pDataTexture );
	VistaTexture* GetDataTexture() const;

	/**
	 * Sets the texture which contains the particle radii in its red channel. 
	 * These particle radii will be multiplied wit the RadiusScale-Factor.
	 * If no RadiiTexture is set, all particles will have the same Radius.
	 * MUST be a GL_TEXTURE_2D;
	 */
	void SetRadiiTexture( VistaTexture* pRadiiTexture );
	VistaTexture* GetRadiiTexture() const;
	
	/**
	 * The mapping texture can be used to render Particle in a specific order.
	 * Only used if DRAW_MODE == DM_SMOKE && BLEND_MODE == ALPHA_BLENDING;
	 */
	void SetMappingTexture( VistaTexture* pMappingTexture );
	VistaTexture* GetMappingTexture() const;

	/**
	 * Sets the size of the texture which contains the particle data.
	 */
	void SetDataTextureSize( int iWidth, int iHeight );
	void GetDataTextureSize( int& iWidth, int& iHeight) const;

	// observer api
	virtual bool Observes( IVistaObserveable* pObserveable );
	virtual void Observe( IVistaObserveable* pObserveable, int nTicket);
	virtual bool ObserveableDeleteRequest( IVistaObserveable* pObserveable, int nTicket );
	virtual void ObserveableDelete( IVistaObserveable* pObserveable, int nTicket );
	virtual void ReleaseObserveable( IVistaObserveable* pObserveable, int nTicket );

	virtual void ObserverUpdate( IVistaObserveable* pObserveable, int nMsg, int nTicket );

protected:
	virtual bool InitShaders();
	virtual bool InitTextures();
	virtual void InitUniformVariables( VistaGLSLShader* pShader );

	virtual void UpdateTextures();
	virtual void UpdateUniformVariables( VistaGLSLShader* pShader );

	virtual void RenderPoints();
	virtual void RenderBillboards( VistaGLSLShader*, VistaTexture* );
	virtual void RenderTransparentBillboards();
	virtual void RenderHalos();

protected:
	VistaParticleRenderingProperties* m_pProperties;

	float	   m_fPointSize;

	float      m_fParticleRadius;
	VistaColor m_oParticleColor;

	VistaTexture* m_pLookUpTexture;
	float m_aLookupRange[2];

	VistaTexture* m_pParticleDataTexture;
	VistaTexture* m_pParticleRadiiTexture;
	VistaTexture* m_pParticleMappingTexture;
	int m_aParticleDataTextureSize[2];
	unsigned int m_iParticleCount;

	VistaVector3D m_v3ViewerPos;
	VistaVector3D m_v3LightDir;

	VistaBufferObject*		m_pVBO;
	VistaVertexArrayObject*	m_pVAO;

	VistaGLSLShader* m_pPointShader;
	VistaGLSLShader* m_pSmokeShader;
	VistaGLSLShader* m_pBillboardShader;
	VistaGLSLShader* m_pLitBBShaders[3];
	VistaGLSLShader* m_pDepthShaders[3];

	VistaGLSLShader* m_pHaloShader;

	VistaTexture* m_pSphereIlluminationTexture;
	VistaTexture* m_pGaussianBlendingTexture;
	VistaTexture* m_pSphereNormalsTexture;

	VistaTexture* m_pDefaultLookUpTexture;
	VistaTexture* m_pDefaultParticleRadiiTexture;
};


#endif // __VFLGPUPARTICLERENDERER_H

/*============================================================================*/
/*  END OF FILE                                                               */
/*============================================================================*/

