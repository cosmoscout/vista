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



#ifndef _VISTATEXTURE_H
#define _VISTATEXTURE_H

/*============================================================================*/
/* INCLUDES			                                                          */
/*============================================================================*/
#include <GL/glew.h>
#include "VistaOGLExtConfig.h"


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaTexture provides a basic abstraction for OpenGL textures.
 */
class VISTAOGLEXTAPI VistaTexture
{
public:
	/**
	 * Creates a new OpenGL texture.
	 * Note, that the newly-generated texture will be bound to texture unit 0
	 * after the c'tor returns successfully.
	 */
	VistaTexture(GLenum eTarget);
	VistaTexture(GLenum eTarget, GLuint iId, bool bManaged = false);
	virtual ~VistaTexture();

	virtual void Bind();
	virtual void Unbind();

	/**
	 * Activate the given texture unit and bind the texure.
	 * Note, that the specified texture unit DOES NOT remain active after this
	 * method returns.
	 */
	virtual void Bind(GLenum eTextureUnit);
	virtual void Unbind(GLenum eTextureUnit);
		
	/**
	 * En-/disable the texture target that was passed to the c'tor at creation
	 * for the currently active texture unit.
	 */
	virtual void Enable();
	virtual void Disable();

	/**
	 * Same as Enable() but also activates the specified texture unit before
	 * enabling the texture target.
	 */
	virtual void Enable(GLenum eTextureUnit);
	virtual void Disable(GLenum eTextureUnit);

	GLenum GetTarget() const;
	GLuint GetId() const;

	/**
	 * Set and retrieve maximum texture coordinates. Typically, they will be
	 * set to 1.0f (as is the default), but if you are using RECT textures or
	 * textures, which are padded, you can set (and use) these purely for
	 * convenience reasons ...
	 */
	void  SetMaxS(float fMax);
	float GetMaxS() const;
	void  SetMaxT(float fMax);
	float GetMaxT() const;
	void  SetMaxR(float fMax);
	float GetMaxR() const;

	/**
	 * Set and retrieve texture min/mag filter settings.
	 * Side-effect: binds the texture to the current texture unit.
	 */
	void   SetMinFilter(GLenum eMode);
	GLenum GetMinFilter();
	void   SetMagFilter(GLenum eMode);
	GLenum GetMagFilter();
	
	/**
	 * Set and retrieve texture wrapping settings.
	 * Side-effect: binds the texture to the current texture unit.
	 */
	void   SetWrapS(GLenum eMode);
	GLenum GetWrapS();
	void   SetWrapT(GLenum eMode);
	GLenum GetWrapT();
	void   SetWrapR(GLenum eMode);
	GLenum GetWrapR();

	/**
	 * Generates all mipmap levels.
	 * @return true on success.
	 */
	bool GenerateMipmaps();

	/**
	 *  Upload texture to OpenGL server.
	 *  Hint: internal format is always set to GL_RGBA8
	 *  Sets min filters to LINEAR or LINEAR_MIPMAP_LINEAR!
	 *
	 *  @param   width              texture width
	 *  @param   height             texture height (ignored for GL_TEXTURE_1D)
	 *  @param   pData              texture data
	 *  @param   bGenMipmaps        generate mipmaps
	 *  @param   eInputPixelFormat  the format of the texture pData is pointing to - or the number of components
	 *  @param   eDataFormat        the data format of pData (GL_UNSIGNED_CHAR)
	 *  @returns bool true on success, else false
	 */
	bool UploadTexture(	const int &iWidth, const int &iHeight, void *pData,
						bool bGenMipmaps = true, GLenum ePixelFormat = GL_RGBA,
						GLenum eDataFormat = GL_UNSIGNED_BYTE);

protected:
	GLenum	m_eTarget;
	GLuint	m_uiId;
	bool	m_bManaged;

	float	m_fMaxS;
	float	m_fMaxT;
	float	m_fMaxR;
};

#endif // _VISTATEXTURE_H


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

