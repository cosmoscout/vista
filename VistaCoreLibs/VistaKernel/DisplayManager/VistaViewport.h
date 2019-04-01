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


#if !defined _VISTAVIEWPORT_H
#define _VISTAVIEWPORT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>


#include "VistaDisplayEntity.h"


/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplaySystem;
class VistaWindow;
class VistaProjection;
class IVistaDisplayBridge;
class VistaGLTexture;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaViewport is ...
 */
class VISTAKERNELAPI VistaViewport  : public VistaDisplayEntity
{
	friend class IVistaDisplayBridge;
	friend class VistaDisplayManager;

public:
	virtual ~VistaViewport();

	VistaDisplaySystem* GetDisplaySystem() const;
	std::string GetDisplaySystemName() const;

	VistaWindow* GetWindow() const;
	std::string GetWindowName() const;

	VistaProjection* GetProjection() const;
	std::string GetProjectionName() const;

	virtual void Debug( std::ostream & out ) const;

	/**
	 * Renders the current viewport
	 */
	virtual bool Render();

	/**
	 * Returns main texture iff available, else NULL
	 */
	VistaGLTexture* GetTextureTarget() const;
	/**
	 * Returns secondary texture (i.e. texture for left eye in stereo mode) iff available, else NULL
	 */
	VistaGLTexture* GetSecondaryTextureTarget() const;

	class CustomRenderAction
	{
	public:
		virtual ~CustomRenderAction() {}
		virtual bool DoRender( IVistaDisplayEntityData* pData, bool bRenderSecondaryViewport ) = 0;
	};
	void SetCustomRenderAction( CustomRenderAction* pAction );
	CustomRenderAction* GetCustomRenderAction() const;

	/**
	 * Get/set viewport properties. The following keys are understood:
	 *
	 * POSITION                -   [list of ints - 2 items]
	 * SIZE                    -   [list of ints - 2 items]
	 * RESIZE_WITH_WINDOW      -   [bool]
	 * REPOSITION_WITH_WINDOW  -   [bool]
	 * TEXTURE_SIZE            -   [list of ints - 2 items] size of texture, if it should be different from viewport size
	 * PASSIVE_BACKGROUND      -   [bool]
	 * TYPE                    -	[string][either NORMAL, QUADBUFFERED_STEREO, RENDER_TO_TEXTURE,
	 *											POSTPROCESS_MONO, POSTPROCESS_STEREO, OCULUS, VIVE
	 *											ANAGLYPH, or ANAGLYPH_MONOCHROME]
	 * ENABLED                 -   [bool]
	 * PRIORITY                -   [int]
	 * POSTPROCESS_SHADER      -   [string]
	 * DISPLAY_SYSTEM_NAME     -   [string][read only]
	 * WINDOW_NAME             -   [string][read only]
	 * PROJECTION_NAME         -   [string][read only]
	 */

	class VISTAKERNELAPI VistaViewportProperties : public IVistaDisplayEntityProperties
	{
	public:

		enum
		{
			MSG_POSITION_CHANGE = IVistaDisplayEntityProperties::MSG_LAST,
			MSG_PRE_RENDER,
			MSG_POST_RENDER, // @IMGTODO: right way?
			// @IMGTODO: allow multiple renders?
			MSG_SIZE_CHANGE,
			MSG_TEXTURE_SIZE_CHANGE,
			MSG_TYPE_CHANGED,
			MSG_CLEAR_COLOR_CHANGED,
			MSG_CLEAR_COLOR_BUFFER_CHANGED,
			MSG_CLEAR_DEPTH_BUFFER_CHANGED,
			MSG_CLEAR_STENCIL_BUFFER_CHANGED,
			MSG_ENABLE_STATE_CHANGED,
			MSG_PRIORITY_CHANGED,
			MSG_RESIZE_WITH_WINDOW_CHANGED,
			MSG_REPOSITION_WITH_WINDOW_CHANGED,
			MSG_LAST
		};
		
		enum ViewportType
		{
			VT_MONO,						// default viewport, single render pass
			VT_QUADBUFFERED_STEREO,			// stereo viewport, two renderpassed to left and right buffer
			VT_RENDER_TO_TEXTURE,			// renders to a texture, not to the window
			VT_RENDER_TO_TEXTURE_STEREO,	// renders to a textures, not to the window, once per eye
			VT_POSTPROCESS,					// renders once to a texture, and then executes a shader on the result to generate new image
			VT_POSTPROCESS_STEREO,			// renders twice in stereo to two textures, and then executes a shader on the result to generate new image
			VT_POSTPROCESS_QUADBUFFERED_STEREO, // renders twice in stereo to two textures, and then executes a shader on both of them to generate new image
			VT_OCULUS_RIFT,					// special viewport that renders to the oculus rift
			VT_OCULUS_RIFT_CLONE,			// special viewport that clones the first oculus rift viewport
			VT_VIVE,					    // special viewport that renders to the htc vive
			VT_ANAGLYPH,					// renders twice in stereo, and then calculates an anaglyphic image
			VT_ANAGLYPH_MONOCHROME,			// renders twice in stereo, and then calculates a monochrome anaglyphic image
		};
		static std::string GetStringForViewportType( const ViewportType eType );
		static ViewportType GetViewportTypeFromString( const std::string& sType );


		bool SetName(const std::string &sName);

		bool GetPosition(int& x, int& y) const;
		bool SetPosition(const int x, const int y);

		bool GetSize(int& w, int& h) const;
		bool SetSize(const int w, const int h);

		bool GetTextureSize(int& w, int& h) const;
		bool SetTextureSize(const int w, const int h);

		bool GetResizeWithWindow() const;
		bool SetResizeWithWindow( const bool bSet );

		bool GetRepositionWithWindow() const;
		bool SetRepositionWithWindow( const bool bSet );		ViewportType GetType() const;
		
		std::string GetTypeString() const;
		bool SetType( const ViewportType eType );
		bool SetTypeByString( const std::string& sType );

		std::string GetPostProcessShaderFile() const;
		bool SetPostProcessShaderFile( const std::string& sShader );
		
		bool GetClearColor( VistaColor& oColor ) const; // @IMGTODO
		VistaColor SetClearColor();

		bool GetClearColorBuffer() const; // @IMGTODO
		bool SetClearColorBuffer();

		bool GetClearDepthBuffer() const; // @IMGTODO
		bool SetClearDepthBuffer();

		bool GetClearStencilBuffer() const; // @IMGTODO
		bool SetClearStencilBuffer();

		bool GetClearAccumBuffer() const; // @IMGTODO
		bool SetClearAccumBuffer();

		int GetPriority() const;
		bool SetPriority( const int nPriority );

		bool GetIsEnabled() const;
		bool SetIsEnabled( const bool bSet );

		virtual std::string GetReflectionableType() const;
	protected:
		virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;

	private:
		VistaViewportProperties(
			VistaDisplayEntity *,
			IVistaDisplayBridge *);
		virtual ~VistaViewportProperties();
		friend class VistaViewport;
	};

	VistaViewportProperties *GetViewportProperties() const;

protected:
	virtual IVistaDisplayEntityProperties *CreateProperties();
	VistaViewport  (VistaDisplaySystem *pDisplaySystem,
					 VistaWindow *pWindow,
					 IVistaDisplayEntityData *pData,
					 IVistaDisplayBridge *pBridge);

	void SetProjection(VistaProjection *pProjection);

private:
	VistaDisplaySystem     *m_pDisplaySystem;
	VistaWindow            *m_pWindow;
	VistaProjection        *m_pProjection;

	CustomRenderAction     *m_pCustomRenderAction;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTAVIEWPORT_H)
