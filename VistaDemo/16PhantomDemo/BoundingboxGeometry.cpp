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

#include "BoundingboxGeometry.h"

#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaGeomNode.h>

BoundingboxGeometry::BoundingboxGeometry( VistaSceneGraph* pVistaSG, VistaBoundingBox &bb )
{
	VistaVector3D v3Min = bb.m_v3Min;
	VistaVector3D v3Max = bb.m_v3Max;

	pNode = pVistaSG->NewGroupNode(NULL);

	// creating lines
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Min[0],v3Min[1],v3Min[2]), VistaVector3D(v3Min[0],v3Min[1],v3Max[2])));
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Min[0],v3Min[1],v3Max[2]), VistaVector3D(v3Max[0],v3Min[1],v3Max[2])));
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Max[0],v3Min[1],v3Max[2]), VistaVector3D(v3Max[0],v3Min[1],v3Min[2])));
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Max[0],v3Min[1],v3Min[2]), VistaVector3D(v3Min[0],v3Min[1],v3Min[2])));

	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Min[0],v3Min[1],v3Min[2]), VistaVector3D(v3Min[0],v3Max[1],v3Min[2])));
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Min[0],v3Min[1],v3Max[2]), VistaVector3D(v3Min[0],v3Max[1],v3Max[2])));
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Max[0],v3Min[1],v3Max[2]), VistaVector3D(v3Max[0],v3Max[1],v3Max[2])));
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Max[0],v3Min[1],v3Min[2]), VistaVector3D(v3Max[0],v3Max[1],v3Min[2])));

	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Max[0],v3Max[1],v3Min[2]), VistaVector3D(v3Min[0],v3Max[1],v3Min[2])));
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Min[0],v3Max[1],v3Min[2]), VistaVector3D(v3Min[0],v3Max[1],v3Max[2])));
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Min[0],v3Max[1],v3Max[2]), VistaVector3D(v3Max[0],v3Max[1],v3Max[2])));
	pNode->AddChild(createLine(pVistaSG, VistaVector3D(v3Max[0],v3Max[1],v3Max[2]), VistaVector3D(v3Max[0],v3Max[1],v3Min[2])));

	// balls...
	// i := min
	// x := max
	VistaGeometryFactory gf(pVistaSG);
	VistaGeometry *ball = gf.CreateSphere(0.02f);

	VistaTransformNode *iii = pVistaSG->NewTransformNode(pNode);
	iii->SetTranslation(v3Min);
	pVistaSG->NewGeomNode(iii,ball);

	VistaTransformNode *iix = pVistaSG->NewTransformNode(pNode);
	iix->SetTranslation(v3Min[0],v3Min[1],v3Max[2]);
	pVistaSG->NewGeomNode(iix,ball);

	VistaTransformNode *xix = pVistaSG->NewTransformNode(pNode);
	xix->SetTranslation(v3Max[0],v3Min[1],v3Max[2]);
	pVistaSG->NewGeomNode(xix,ball);

	VistaTransformNode *xii = pVistaSG->NewTransformNode(pNode);
	xii->SetTranslation(v3Max[0],v3Min[1],v3Max[2]);
	pVistaSG->NewGeomNode(xii,ball);

	VistaTransformNode *ixi = pVistaSG->NewTransformNode(pNode);
	ixi->SetTranslation(v3Min[0],v3Max[1],v3Min[2]);
	pVistaSG->NewGeomNode(ixi,ball);

	VistaTransformNode *ixx = pVistaSG->NewTransformNode(pNode);
	ixx->SetTranslation(v3Min[0],v3Max[1],v3Max[2]);
	pVistaSG->NewGeomNode(ixx,ball);

	VistaTransformNode *xxx = pVistaSG->NewTransformNode(pNode);
	xxx->SetTranslation(v3Max);
	pVistaSG->NewGeomNode(xxx,ball);

	VistaTransformNode *xxi = pVistaSG->NewTransformNode(pNode);
	xxi->SetTranslation(v3Max[0],v3Max[1],v3Min[2]);
	pVistaSG->NewGeomNode(xxi,ball);

}

BoundingboxGeometry::~BoundingboxGeometry()
{
}

VistaGroupNode* BoundingboxGeometry::getVistaNode()
{
	return pNode;
}

/**
 * Little helper to connect two points with an cone
 */
VistaTransformNode* BoundingboxGeometry::createLine( VistaSceneGraph * pVistaSG, const VistaVector3D &a, const VistaVector3D &b )
{

	VistaTransformNode *trans = pVistaSG->NewTransformNode(NULL);
	VistaVector3D direction = b - a;
	float length = direction.GetLength();
	VistaQuaternion q(VistaVector3D(0,1,0),direction);
	VistaGeometryFactory oGeomFactory(pVistaSG);
	VistaGeometry *line = oGeomFactory.CreateCone(0.01f,0.01f,length);
	pVistaSG->NewGeomNode(trans,line);

	// move up to avoid rotate around the center
	trans->Translate( VistaVector3D(0,length/2.0f,0) );
	// rotate to new direction
	trans->Rotate(q);
	// move to position
	trans->Translate(a);

	return trans;
}
