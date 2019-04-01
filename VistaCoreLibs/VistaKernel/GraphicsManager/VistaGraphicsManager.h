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


#ifndef _VISTANNEWGRAPHICSMANAGER_H
#define _VISTANNEWGRAPHICSMANAGER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/GraphicsManager/VistaGraphicsBridge.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSceneGraph;
class IVistaNodeBridge;
class VistaGroupNode;
class VistaLightNode;
class VistaVector3D;
class VistaQuaternion;
class VistaTransformMatrix;
class VistaEventObserver;
class VistaPropertyList;

class VistaEventManager;
class VistaFrameLoop;


// prototypes
class   VistaGraphicsManager;
VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaGraphicsManager & );


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaGraphicsManager : public VistaEventHandler
{
public:
	VistaGraphicsManager( VistaEventManager *pEvMgr, VistaFrameLoop* pLoop );
	virtual ~VistaGraphicsManager();

	bool Init( IVistaNodeBridge* pNodeBridge,
	               IVistaGraphicsBridge* pGraphicsBridge );

	bool SetupScene( const std::string& sIniSection,
					const VistaPropertyList& oConfig );

	//** access functions
	VistaSceneGraph* GetSceneGraph() const;

	float GetFrameRate() const;
	int GetFrameCount() const;
	/**
	 * Get/set background color
	 */
	VistaColor  GetBackgroundColor() const;
	void SetBackgroundColor(const VistaColor & color);

	/**
	 * @return returns true if frustum culling is enabled globally, false otherwise
	 */
	bool GetFrustumCullingEnabled() const;
	/**
	 * @param enables global frustum culling if true, disables otherwise
	 */
	void SetFrustumCullingEnabled(bool bCullingEnabled);

	/**
	 * @return returns true if occlusion culling is enabled globally,
	           false otherwise
	 */
	bool GetOcclusionCullingEnabled() const;
	/**
	 * @param enables global occlusion culling if true, disables otherwise
	 */
	void SetOcclusionCullingEnabled( bool bOclCullingEnabled );

	/**
	 * @return returns true if bounding box drawing around scenegraph
	           nodes is enabled, false otherwise
	 */
	bool GetBBoxDrawingEnabled() const;
	/**
	 * @param enables bounding box drawing around scenegraph nodes if true,
	          disables otherwise
	 */
	void SetBBoxDrawingEnabled(bool bState);


	IVistaGraphicsBridge*   GetGraphicsBridge() const;
	IVistaNodeBridge*       GetNodeBridge() const;

	/**
	 * Handle graphics events, which haven't been handled by the application.
	 *
	 * @param    VistaEvent *pEvent
	 *
	 */
	virtual void HandleEvent(VistaEvent *pEvent);

	static bool RegisterEventTypes(VistaEventManager*);

	void CreateDefaultLights();

	void Debug( std::ostream& oOut ) const;
protected:

	VistaSceneGraph*				m_pSceneGraph;
	VistaGroupNode*			m_pModelRoot;
	IVistaGraphicsBridge*   m_pGraphicsBridge;
	IVistaNodeBridge*       m_pNodeBridge;
	VistaFrameLoop*			m_pLoop;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANNEWGRAPHICSMANAGER_H

