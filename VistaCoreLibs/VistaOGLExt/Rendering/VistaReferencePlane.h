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


#ifndef _VFAREFERENCEPLANE_H
#define _VFAREFERENCEPLANE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaOGLExtConfig.h"

#include<VistaBase/VistaVector3D.h>
#include <VistaBase/VistaQuaternion.h>
#include <VistaBase/VistaColor.h>

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

#include <VistaAspects/VistaReflectionable.h>
#include <VistaAspects/VistaObserver.h>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSceneGraph;
class VistaGLSLShader;
class VistaFramebufferObj;
class VistaTexture;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * OpenGL draw node which draws a "floor plane" with tiles. This tool is no
 * "real" VflRenderable, as it should not be "below" the vis controller in the
 * scene graph.
 * If you set SetIsKeepScale to false, the floor changes the resolution of the
 * tiles with the vis controller's scale.
 */
class VISTAOGLEXTAPI VistaReferencePlane 
	:	public IVistaOpenGLDraw
	,	public IVistaObserver
{
public:
	VistaReferencePlane( VistaSceneGraph* const pSG );
	virtual ~VistaReferencePlane();

	// *** IVistaOpenGLDraw Interface ***
	virtual bool Init();
	virtual bool Do();

	virtual bool GetBoundingBox(VistaBoundingBox &bb);

	// *** Properties ***
	class VISTAOGLEXTAPI VfaReferencePlaneProperties
	:	public IVistaReflectionable
    {
    public:
        VfaReferencePlaneProperties();
        virtual ~VfaReferencePlaneProperties();

		/**
		 * Get/SetVisible
		 * Specifies if the reference plane is visible
		 */
		bool GetVisible() const;
		bool SetVisible(bool bVisible);

		/**
		 * Get/SetPlaneDims
		 * Specifies the size of the reference plane
		 */
		bool GetPlaneDims( float& fWidth, float& fLength ) const;
		bool SetPlaneDims( float  fWidth,  float fLength );

		/**
		 * Get/SetTileDims
		 * Specifies the size of each tile
		 */
		bool GetTileDims( float& fWidth, float& fLength ) const;
		bool SetTileDims( float  fWidth, float  fLength );

		/**
		 * Get/SetTileDims
		 * Specifies the width of lines between tiles
		 */
		float GetLineWidth() const;
		bool SetLineWidth( float fLineWidth );

		/**
		 * Get/SetTileColor
		 * Specifies the color of the tiles
		 */
		VistaColor GetTileColor() const;
		void GetTileColor( float pColor[4] ) const;
		bool SetTileColor( float pColor[4] );
		bool SetTileColor( const VistaColor& val );

		/**
		 * Get/SetLineColor
		 * Specifies the color of lines between tiles
		 */
		VistaColor GetLineColor() const;
		void GetLineColor( float pColor[4] ) const;
		bool SetLineColor( float pColor[4] );
		bool SetLineColor( const VistaColor& val );

		/**
		 * Get/SetCenter
		 * Specifies the center of the reference plane
		 */
		VistaVector3D GetCenter() const;
		void GetCenter( float pCenter[3] ) const;
		bool SetCenter( float pCenter[3] );
		bool SetCenter( const VistaVector3D &v3C );

		/**
		 * Get/SetOrientation
		 * Specifies the orientation of the reference plane
		 */
		VistaQuaternion GetOrientation() const;
		void SetOrientation( const VistaQuaternion& qOrientation );

		enum
		{
			MSG_CHANGE_PLANE_DIMS = 0,
			MSG_CHANGE_TILE_DIMS,
			MSG_CHANGE_PLANE_CENTER,
			MSG_CHANGE_PLANE_ORIENTATION,
			MSG_CHANGE_LINE_WIDTH,
			MSG_CHANGE_TILE_COLOR,
			MSG_CHANGE_LINE_COLOR,
			MSG_LAST
		};

		virtual std::string GetReflectionableType() const;

	protected:
		virtual int AddToBaseTypeList(std::list<std::string> &rBtList) const;

	private:
		VfaReferencePlaneProperties(const VfaReferencePlaneProperties &);
		VfaReferencePlaneProperties &operator=(VfaReferencePlaneProperties &);

		VistaColor		m_oLineColor;
		VistaColor		m_oTileColor;
		VistaVector3D	m_v3Center;
		VistaQuaternion m_qOrientation;

		float			m_fPlaneWidth;
		float			m_fPlaneLength;
		float			m_fTileWidth;
		float			m_fTileLength;
		float			m_fLineWidth;

		bool			m_bVisible;

        friend class VistaReferencePlane;
    };	

	// *** IVistaObserver Interface ***

	virtual bool ObserveableDeleteRequest(	IVistaObserveable *pObserveable, 
		int nTicket=IVistaObserveable::TICKET_NONE);
	virtual void ObserveableDelete(	IVistaObserveable *pObserveable, 
		int nTicket=IVistaObserveable::TICKET_NONE);
	virtual void ReleaseObserveable(IVistaObserveable *pObserveable, 
		int nTicket=IVistaObserveable::TICKET_NONE);
	virtual bool Observes(IVistaObserveable *pObserveable);
	virtual void Observe(IVistaObserveable *pObservable, 
		int eTicket=IVistaObserveable::TICKET_NONE);

	virtual void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket);

	VfaReferencePlaneProperties* GetProperties() const;

protected:
	void UpdateTileTexture();

private:
	VistaGLSLShader*		m_pShader;
	VistaTexture*			m_pTexture;
	VistaFramebufferObj*	m_pFBO;

	VfaReferencePlaneProperties* m_pProperties;
	bool m_bTileTextureReadyForUse;
};


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif // _VFLREFERENCEPLANE_H
