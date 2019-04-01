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


#include <VistaBase/VistaStreamUtils.h>

#include "VistaFramebufferObj.h"
#include "VistaTexture.h"
#include "VistaRenderbuffer.h"
#include <iostream>
#include <cassert>


/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaFramebufferObj::VistaFramebufferObj()
: m_iId(0)
, m_nActiveFBOOnBind( 0 )
, m_nFastBindnActiveFBOOnBind( 0 )
, m_bIsBound( false )
, m_bIsBoundOnFastBind( false )
{
	if (!GLEW_EXT_framebuffer_object)
	{
		vstr::errp() << " [VistaFramebufferObj] - missing EXT_framebuffer_object extension..." << endl;
		return;
	}

	// generate buffer id and create buffer
	glGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &m_nActiveFBOOnBind );
	glGenFramebuffersEXT(1, &m_iId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_nActiveFBOOnBind);
	m_nActiveFBOOnBind = 0;
}

VistaFramebufferObj::~VistaFramebufferObj()
{
	if (GLEW_EXT_framebuffer_object)
		glDeleteFramebuffersEXT(1, &m_iId);
	m_iId = 0;
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Bind                                                        */
/*                                                                            */
/*============================================================================*/
void VistaFramebufferObj::Bind()
{
	if( m_bIsBound )
	{
		vstr::warnp() << "[VistaFramebufferObj] Trying to bind an FBO that is already bound" << std::endl;
		return;
	}

	glGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &m_nActiveFBOOnBind );

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iId);
	m_bIsBound = true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Disable                                                     */
/*                                                                            */
/*============================================================================*/
void VistaFramebufferObj::Release()
{
	if( !m_bIsBound )
	{
		vstr::warnp() << "[VistaFramebufferObj] Trying to release an FBO that is not currently bound" << std::endl;
		return;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_nActiveFBOOnBind);
	m_nActiveFBOOnBind = 0;
	m_bIsBound = false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Attach                                                      */
/*                                                                            */
/*============================================================================*/
void VistaFramebufferObj::Attach(VistaTexture *pTexture,
								  GLenum eAttachment,
								  int iMipLevel /* = 0 */,
								  int iZSlice /* = 0 */)
{
	if (!pTexture)
		return;

	FastBind();
	
	GLenum eTarget = pTexture->GetTarget();
	switch (eTarget)
	{
	case GL_TEXTURE_1D:
		glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT, eAttachment, GL_TEXTURE_1D,
			pTexture->GetId(), iMipLevel);
		break;
	case GL_TEXTURE_3D:
		glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, eAttachment, GL_TEXTURE_3D,
			pTexture->GetId(), iMipLevel, iZSlice);
		break;
	default:
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, eAttachment,
			pTexture->GetTarget(), pTexture->GetId(), iMipLevel);
	}

	// TODO: correctly deal with cube maps!!!

	FastRelease();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Attach                                                      */
/*                                                                            */
/*============================================================================*/
void VistaFramebufferObj::Attach(VistaRenderbuffer *pRenderbuffer,
								  GLenum eAttachment)
{
	if (!pRenderbuffer)
		return;

	FastBind();
	
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, eAttachment,
		GL_RENDERBUFFER_EXT, pRenderbuffer->GetId());

	FastRelease();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Detach                                                      */
/*                                                                            */
/*============================================================================*/
void VistaFramebufferObj::Detach(GLenum eAttachment)
{
	// Must be called here, since nested SafeBind/Release pairs lead to side
	// effects.
	GLenum eType = GetAttachedType(eAttachment);
	
	FastBind();

	switch (eType)
	{
	case GL_NONE:
		break;
	case GL_TEXTURE:
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, eAttachment,
			GL_TEXTURE_2D, 0, 0);
		break;
	case GL_RENDERBUFFER_EXT:
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, eAttachment,
			GL_RENDERBUFFER_EXT, 0);
		break;
	}
	
	FastRelease();
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetAttachedId                                               */
/*                                                                            */
/*============================================================================*/
GLuint VistaFramebufferObj::GetAttachedId(GLenum eAttachment)
{
	FastBind();
	
	GLint iId = 0;
	glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, eAttachment,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT, &iId);
	
	FastRelease();

	return GLuint(iId);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetAttachedType                                             */
/*                                                                            */
/*============================================================================*/
GLenum VistaFramebufferObj::GetAttachedType(GLenum eAttachment)
{
	FastBind();

	GLint iType = 0;
	glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT, eAttachment,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT, &iType);

	FastRelease();

	return GLenum(iType);
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetId                                                       */
/*                                                                            */
/*============================================================================*/
GLuint VistaFramebufferObj::GetId() const
{
	return m_iId;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsValid                                                     */
/*                                                                            */
/*============================================================================*/
bool VistaFramebufferObj::IsValid() const
{
	FastBind();

	GLenum eStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	FastRelease();
	
	return eStatus == GL_FRAMEBUFFER_COMPLETE_EXT;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetStatusAsString                                           */
/*                                                                            */
/*============================================================================*/
std::string VistaFramebufferObj::GetStatusAsString() const
{
	FastBind();
	GLenum eStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	string strStatus = "UNKNOWN";

	switch(eStatus) 
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		strStatus = "FRAMEBUFFER_COMPLETE";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		strStatus = "FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		strStatus = "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		strStatus = "FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		strStatus = "FRAMEBUFFER_INCOMPLETE_FORMATS";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		strStatus = "FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		strStatus = "FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		strStatus = "FRAMEBUFFER_UNSUPPORTED";
		break;
	default:
		strStatus = "UNKNOWN_ERROR";
	}

	FastRelease();

	return strStatus;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   IsSupported                                                 */
/*                                                                            */
/*============================================================================*/
bool VistaFramebufferObj::IsSupported() const
{
	return GLEW_EXT_framebuffer_object ? true : false;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetMaxColorAttachments                                      */
/*                                                                            */
/*============================================================================*/
int VistaFramebufferObj::GetMaxColorAttachments()
{
	GLint iCount = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &iCount);

	return iCount;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   GetMaxRenderbufferSize                                      */
/*                                                                            */
/*============================================================================*/
int VistaFramebufferObj::GetMaxRenderbufferSize()
{
	GLint iSize;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &iSize);

	return iSize;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SafeBind                                                    */
/*                                                                            */
/*============================================================================*/
void VistaFramebufferObj::FastBind() const
{
	if( !m_bIsBound && ! m_bIsBoundOnFastBind )
	{
		glGetIntegerv( GL_FRAMEBUFFER_BINDING_EXT, &m_nFastBindnActiveFBOOnBind );
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_iId);
		m_bIsBoundOnFastBind = true;
	}
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   SafeRelease                                                 */
/*                                                                            */
/*============================================================================*/
void VistaFramebufferObj::FastRelease() const
{
	assert( m_bIsBound || m_bIsBoundOnFastBind );
	if( m_bIsBoundOnFastBind )
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_nFastBindnActiveFBOOnBind );
		m_bIsBoundOnFastBind = false;
	}

}

/*============================================================================*/
/*  LOKAL VARS / FUNCTIONS                                                    */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "VistaFramebufferObj.cpp"                                     */
/*============================================================================*/
