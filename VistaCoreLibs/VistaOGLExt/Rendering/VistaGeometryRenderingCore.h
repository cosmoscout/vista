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


#ifndef __VistaGeometryRenderingCore_h
#define __VistaGeometryRenderingCore_h

#include "../VistaOGLExtConfig.h"

#include <VistaBase/VistaColor.h>

#include <vector>

class VistaGeometryData;
class VistaGLSLShader;
class VistaTexture;

class VISTAOGLEXTAPI VistaGeometryRenderingCore
{
public:
	VistaGeometryRenderingCore();
	virtual ~VistaGeometryRenderingCore();

	void SetData( const VistaGeometryData* pData );

	// *** Shader specification ***
	void SetSurfaceShader( VistaGLSLShader* pShader );
	void SetLineShader( VistaGLSLShader* pShader );
	VistaGLSLShader* GetSurfaceShader() const;
	VistaGLSLShader* GetLineShader() const;

	void SetSurfaceColor( const VistaColor& rColor );
	void SetLineColor( const VistaColor& rColor );
	const VistaColor& GetSurfaceColor() const;
	const VistaColor& GetLineColor() const;

	void SetUsedTextures( const std::vector<VistaTexture*>& vecTuextres,
						  bool bTransferOwnership = false );
	std::vector<VistaTexture*>& GetUsedTextures();

	void SetUniform( const std::string& strName, float f1);
	void SetUniform( const std::string& strName, float f1, float f2);
	void SetUniform( const std::string& strName, float f1, float f2, float f3);
	void SetUniform( const std::string& strName, float f1, float f2, float f3, float f4);
	void SetUniform( const std::string& strName, int i );

	virtual void Draw();

	static VistaGLSLShader* GetDefaultShader();

protected:
	virtual void PrepareRendering();
	virtual void CleanUpRendering();

	virtual void RenderSurface();
	virtual void RenderLines();


	class UniformVariable
	{
	public:
		UniformVariable( const std::string& strName, unsigned int Components, float f1, float f2=0.0f, float f3=0.0f, float f4=0.0f);
		UniformVariable( const std::string& strName, int iValue );
		~UniformVariable();

		void RefreshUniformForShader( VistaGLSLShader* pShader );
		const std::string& GetName() const;

	private:
		std::string	 m_strName;
		unsigned int m_uiType;
		float		 m_fValues[4];
		int			 m_iValue;
	};
	void UpdateUniform( const UniformVariable& rUniform );
	void RefreshUniformsForShader( VistaGLSLShader* pShader );

private:
	const VistaGeometryData* m_pData;

	VistaGLSLShader*	m_pSurfaceShader;
	VistaGLSLShader*	m_pLineSchader;

	VistaColor	m_oSurfaceColor;
	VistaColor  m_oLineColor;

	std::vector<VistaTexture*>		m_vecUsedTextures;
	bool							m_bOwnTextures;
	std::vector<UniformVariable>	m_pUniformVariables;

	static VistaGLSLShader* m_pDefaultShader;
};

#endif // Include guard.
