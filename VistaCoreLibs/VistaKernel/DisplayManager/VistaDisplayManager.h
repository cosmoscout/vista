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


#if !defined(_VISTADISPLAYMANAGER_H)
#define _VISTADISPLAYMANAGER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>

#include <VistaBase/VistaVectorMath.h>

#include <vector>
#include <map>
#include <list>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaDisplay;
class VistaDisplaySystem;
class VistaVirtualPlatform;
class VistaViewport;
class VistaWindow;
class VistaProjection;
class VistaDisplayEntity;
class IVistaDisplayBridge;
class VistaPropertyList;
class IVistaWindowingToolkit;

class Vista2DBitmap;
class Vista2DText;
class Vista2DLine;
class Vista2DRectangle;
class IVistaSceneOverlay;
class IVistaExplicitCallbackInterface;
class IVistaTextEntity;

// prototypes
class   VistaDisplayManager;
VISTAKERNELAPI std::ostream& operator<< ( std::ostream&, const VistaDisplayManager& );


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
* VistaDisplayManager manages definitions of display systems like monitors,
* workbenches and CAVEs. Right now only one display system can be set.
*/
class VISTAKERNELAPI VistaDisplayManager : public VistaEventHandler
{
public:
	VistaDisplayManager( IVistaDisplayBridge* pDisplayBridge );

	virtual ~VistaDisplayManager();

	IVistaDisplayBridge* GetDisplayBridge() const;
	IVistaWindowingToolkit* GetWindowingToolkit() const;

	/**
	* Create a display system according to the data within the
	* given ini section. As a side effect, a suitable reference
	* frame is created, which can be retrieved directly from
	* the newly create display system.
	*/
	bool CreateDisplaySystems( const std::string& sIniSection,
								const VistaPropertyList& oConfig );

	/**
	* Destroy all display systems.
	*/
	bool ShutdownDisplaySystems();

	/**
	* Update the display manager and/or the underlying display systems.
	*/
	virtual bool Update();

	/**
	* Retrieve display entities by name.
	*/
	VistaDisplaySystem* GetDisplaySystemByName( const std::string& sName ) const;
	VistaDisplay* GetDisplayByName( const std::string& sName ) const;
	VistaWindow* GetWindowByName( const std::string& sName ) const;
	VistaViewport* GetViewportByName( const std::string& sName ) const;
	VistaProjection* GetProjectionByName( const std::string& sName ) const;

	VistaDisplayEntity* GetDisplayEntityByName( const std::string& sName ) const;

	/**
	* Retrieve display systems.
	*/
	int GetNumberOfDisplaySystems() const;
	VistaDisplaySystem* GetDisplaySystem( int iIndex = 0 ) const;

	/**
	* Retrieve displays.
	*/
	int GetNumberOfDisplays() const;
	VistaDisplay* GetDisplay( int iIndex = 0 ) const;

	/**
	* Retrieve display systems.
	*/
	std::map<std::string, VistaDisplaySystem*> GetDisplaySystems() const;
	const std::map<std::string, VistaDisplaySystem*>& GetDisplaySystemsConstRef() const;

	/**
	* Retrieve windows.
	*/
	std::map<std::string, VistaWindow*> GetWindows() const;
	const std::map<std::string, VistaWindow*>& GetWindowsConstRef() const;

	/**
	* Retrieve viewports.
	*/
	std::map<std::string, VistaViewport*> GetViewports() const;
	const std::map<std::string, VistaViewport*>& GetViewportsConstRef() const;

	/**
	* Retrieve projections.
	*/
	std::map<std::string, VistaProjection*> GetProjections() const;
	const std::map<std::string, VistaProjection*>& GetProjectionsConstRef() const;

	/**
	* Create/destroy display systems.
	*/
	virtual VistaDisplaySystem* CreateDisplaySystem( const std::string& sSectionName,
													const VistaPropertyList& oConfig );
	virtual bool DestroyDisplaySystem( VistaDisplaySystem* pDisplaySystem );

	/**
	* Create/destroy displays.
	*/
	virtual VistaDisplay* CreateDisplay( const VistaPropertyList& refProps );
	virtual bool DestroyDisplay( VistaDisplay* pDisplay );

	/**
	* Create/destroy windows.
	*/
	virtual VistaWindow* CreateVistaWindow( const std::string& sSectionName,
											const VistaPropertyList& oConfig );
	virtual bool DestroyVistaWindow( VistaWindow* pWindow );

	/**
	* Create/destroy viewports.
	*/
	virtual VistaViewport* CreateViewport( const std::string& sSectionName,
											 const VistaPropertyList& oConfig,
											 VistaDisplaySystem* pDisplaySystem );
	virtual bool DestroyViewport( VistaViewport* pViewport );

	/**
	* Create/destroy projections.
	*/
	virtual VistaProjection* CreateProjection( VistaViewport* pViewport,
										const std::string& sIniSection,
										const VistaPropertyList& oProps );
	virtual bool DestroyProjection( VistaProjection* pProjection );

	virtual void Debug( std::ostream& oOut ) const;

	/**
	* Helper methods to keep track of renamed display entities.
	*/
	bool RenameDisplaySystem( const std::string& sOldName,
						const std::string& sNewName );
	bool RenameViewport( const std::string& sOldName,
						const std::string& sNewName );
	bool RenameProjection( const std::string& sOldName,
						const std::string& sNewName );
	bool RenameWindow( const std::string& sOldName,
						const std::string& sNewName );
	bool RenameDisplay( const std::string& sOldName,
						const std::string& sNewName );

	/**
	 * Makes a screenshot from the specified window, and stores it as image
	 * with the provided filename. If the filename has no extension, a default
	 * image type (usually jpg) will be used.
	 * @param sWindowName - name of the window to be used
	 * @param sFilename - file name (with or without extension)	
	 * @return true on success, false on failure
	 */
	bool MakeScreenshot( VistaWindow* pWindow,
						const std::string& sFilename ) const;	
	bool MakeScreenshot( const std::string& sWindowName,
						const std::string& sFilename ) const;	

	Vista2DText* New2DText( const std::string& sViewportName = "" );
	Vista2DBitmap* New2DBitmap( const std::string& sViewportName = "" );
	Vista2DLine* New2DLine( const std::string& sViewportName = "" );
	Vista2DRectangle* New2DRectangle( const std::string& sViewportName = "" );

	/**
	 * The IVistaTextEntity defines how to render text based on
	 */
	IVistaTextEntity* CreateTextEntity();

	bool AddSceneOverlay( IVistaSceneOverlay* pOverlay,
						const std::string& sViewportName = "" );
	bool AddSceneOverlay( IVistaSceneOverlay* pOverlay,
						VistaViewport* pViewport );
	bool RemSceneOverlay( IVistaSceneOverlay* pOverlay,
						const std::string& sViewportName = "" );
	bool RemSceneOverlay( IVistaSceneOverlay* pOverlay,
						VistaViewport* pViewport );

	virtual void HandleEvent(VistaEvent* pEvent);

	static bool RegisterEventTypes(VistaEventManager* pEventManager );

	/**
	 * Render the current scene to all active windows
	 */
	bool DrawFrame();
	/**
	 * Swap the Buffers of all active Windows, and displays them
	 */
	bool DisplayFrame();
	/**
	 * Sets the callbacks that should be called for the main update loop.
	 * Should be set by the VistaFrameLoop, but can be re-set after initialization,
	 * but do so only! if you have a very good reason to do so (like defining your
	 * own FrameLoop)
	 */
	bool SetDisplayUpdateCallback( IVistaExplicitCallbackInterface* pCallback );

	struct VISTAKERNELAPI RenderInfo
	{
		VistaViewport* m_pViewport;
		VistaWindow* m_pWindow;
		VistaTransformMatrix m_matCameraTransform;
		VistaTransformMatrix m_matProjection;

		enum EyeRenderMode
		{
			ERM_MONO,
			ERM_LEFT,
			ERM_RIGHT,
		};
		EyeRenderMode m_eEyeRenderMode;
	};
	const RenderInfo* GetCurrentRenderInfo() const;

private:
	/**
	* backward link to vista's main object
	*/
	IVistaDisplayBridge*						m_pBridge;
	IVistaWindowingToolkit*						m_pWindowingToolkit;
	std::vector<VistaDisplaySystem*>			m_vecDisplaySystems;
	std::vector<VistaDisplay*>					m_vecDisplays;
	std::vector<VistaReferenceFrame*>			m_vecRefFrames;

	// provide data structures for mapping from name to display entity
	std::map<std::string, VistaDisplaySystem*>	m_mapDisplaySystems;
	std::map<std::string, VistaDisplay*>		m_mapDisplays;
	std::map<std::string, VistaWindow*>			m_mapWindows;
	std::map<std::string, VistaViewport*>		m_mapViewports;
	std::map<std::string, VistaProjection*>		m_mapProjections;

	// Observer
	class ViewportObserver;
	ViewportObserver* m_pViewportObserver;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTADISPLAYMANAGER_NEW_H)
