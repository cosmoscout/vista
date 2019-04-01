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


#ifndef _VISTAGEOMNODE_H
#define _VISTAGEOMNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/GraphicsManager/VistaLeafNode.h>
#include <VistaAspects/VistaUncopyable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaGroupNode;
class VistaGeometry;
class IVistaNodeBridge;
class IVistaNodeData;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaGeomNode : public VistaLeafNode
{
	VISTA_UNCOPYABLE( VistaGeomNode );
	friend class VistaSceneGraph;
	friend class IVistaNodeBridge;
public:
	/** Retrieve node's geometry
	 * @return VistaGeometry* : Pointer to geometry object currently attached to node
	 */
	virtual VistaGeometry* GetGeometry() const;
	/** Set node's geometry
	 * @param VistaGeometry* pGeom : Pointer to new geometry object
	 * @return bool true/false
	 */
	virtual bool SetGeometry(VistaGeometry* pGeom);
	virtual bool CanHaveChildren() const;

	virtual ~VistaGeomNode();

	virtual bool ScaleGeometry(const float fX, const float fY, const float xZ);

	void Debug( std::ostream& oOut, int nLevel = 0 ) const;

/**
 * @todo should we become an observer for our geometry?
 */
#if 0
	/** Observer interface */
	virtual bool ObserveableDeleteRequest(IVistaObserveable *pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) = 0;
	virtual void ObserveableDelete(IVistaObserveable *pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) = 0;
	virtual void ReleaseObserveable(IVistaObserveable *pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) = 0;
	virtual void ObserverUpdate(IVistaObserveable *pObserveable, int msg, int ticket) = 0;

	virtual bool Observes(IVistaObserveable *pObserveable) = 0;
	virtual void Observe(IVistaObserveable *pObservable, int eTicket=IVistaObserveable::TICKET_NONE) = 0;
#endif

protected:
	VistaGeomNode( VistaGroupNode*                        pParent,
					VistaGeometry*                         pGgeom,
					IVistaNodeBridge*                       pBridge,
					IVistaNodeData*                         pData,
					std::string                                     strName = "");

	VistaGeometry* m_pGeometry;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAGEOMNODE_H

