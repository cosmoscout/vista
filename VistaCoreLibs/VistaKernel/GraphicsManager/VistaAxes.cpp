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

#include "VistaAxes.h"

#include <iostream>

#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaBase/VistaStreamUtils.h>

#include <cstdio>

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

// CONSTRUCTOR
VistaAxes::VistaAxes(VistaSceneGraph* pVistaSceneGraph, VistaGroupNode* pParent, float fSizeX,
    float fSizeY, float fSizeZ) {
  if (fSizeX < 0.01f || fSizeY < 0.01f || fSizeZ < 0.01f) {
    vstr::errp() << "[VistaAxes] Size to small!" << std::endl;
    return;
  }

  VistaGeometryFactory geomFactory(pVistaSceneGraph);

  // create axes
  m_vecAxes.push_back(geomFactory.CreateCone(
      0.05f, 0.05f, fSizeX - 0.1f, 32, 32, 32, VistaColor::RED, true, true, true));
  m_vecAxes.push_back(geomFactory.CreateCone(
      0.05f, 0.05f, fSizeY - 0.1f, 32, 32, 32, VistaColor::GREEN, true, true, true));
  m_vecAxes.push_back(geomFactory.CreateCone(
      0.05f, 0.05f, fSizeZ - 0.1f, 32, 32, 32, VistaColor::BLUE, true, true, true));

  // create tips
  m_vecTips.push_back(
      geomFactory.CreateCone(0.07f, 0, 0.1f, 32, 32, 32, VistaColor::GRAY, true, true, true));
  m_vecTips.push_back(
      geomFactory.CreateCone(0.07f, 0, 0.1f, 32, 32, 32, VistaColor::GRAY, true, true, true));
  m_vecTips.push_back(
      geomFactory.CreateCone(0.07f, 0, 0.1f, 32, 32, 32, VistaColor::GRAY, true, true, true));

  // center sphere
  m_pCenter = geomFactory.CreateSphere(0.1f, 21, VistaColor::GRAY);

  // put all together
  m_pGroup = pVistaSceneGraph->NewGroupNode(pParent);

  // X axis
  VistaTransformNode* transX = pVistaSceneGraph->NewTransformNode(m_pGroup);
  transX->SetRotation(
      VistaQuaternion(VistaAxisAndAngle(VistaVector3D(0, 0, 1), Vista::DegToRad(90))));
  transX->SetTranslation(VistaVector3D((fSizeX - 0.1f) * 0.5f, 0, 0));
  pVistaSceneGraph->NewGeomNode(transX, m_vecAxes[0]);
  // tip
  VistaTransformNode* transXtip = pVistaSceneGraph->NewTransformNode(transX);
  transXtip->SetRotation(
      VistaQuaternion(VistaAxisAndAngle(VistaVector3D(0, 0, 1), Vista::DegToRad(180))));
  transXtip->SetTranslation(VistaVector3D(0, -fSizeX * 0.5f, 0));
  pVistaSceneGraph->NewGeomNode(transXtip, m_vecTips[0]);

  // Y axis
  VistaTransformNode* transY = pVistaSceneGraph->NewTransformNode(m_pGroup);
  transY->SetTranslation(VistaVector3D(0, (fSizeY - 0.1f) * 0.5f, 0));
  pVistaSceneGraph->NewGeomNode(transY, m_vecAxes[1]);
  // tip
  VistaTransformNode* transYtip = pVistaSceneGraph->NewTransformNode(transY);
  transYtip->SetTranslation(VistaVector3D(0, fSizeY * 0.5f, 0));
  pVistaSceneGraph->NewGeomNode(transYtip, m_vecTips[1]);

  // Z axis
  VistaTransformNode* transZ = pVistaSceneGraph->NewTransformNode(m_pGroup);
  transZ->SetRotation(
      VistaQuaternion(VistaAxisAndAngle(VistaVector3D(1, 0, 0), Vista::DegToRad(90))));
  transZ->SetTranslation(VistaVector3D(0, 0, (fSizeZ - 0.1f) * 0.5f));
  pVistaSceneGraph->NewGeomNode(transZ, m_vecAxes[2]);
  // tip
  VistaTransformNode* transZtip = pVistaSceneGraph->NewTransformNode(transZ);
  // transZtip->SetRotation( VistaQuaternion( VistaAxisAndAngle( VistaVector3D(0,0,1),
  // Vista::DegToRad(180) ) ) );
  transZtip->SetTranslation(VistaVector3D(0, fSizeZ * 0.5f, 0));
  pVistaSceneGraph->NewGeomNode(transZtip, m_vecTips[2]);

  // center
  pVistaSceneGraph->NewGeomNode(m_pGroup, m_pCenter);

  m_pGroup->SetName("VistaAxes");
  // Scale(width, height, length);
}

// DESTRUCTOR
VistaAxes::~VistaAxes() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
IVistaNode* VistaAxes::GetVistaNode() {
  return m_pGroup;
}

#if 0
bool VistaAxes::Init()
{
	
	m_pGroup = m_pVistaSceneGraph->NewGroupNode(m_pVistaNode);
	m_pGroup->SetName("cs_Group");

						// Coordinate System (unit size)
	m_pOrigin = new VistaSphere(m_pVistaSceneGraph, m_pGroup, 0.1f, 10, 10, false);
	m_pOrigin->SetName("cs_Origin");
	//m_pOrigin->ShowBoundingBox(true);

						// X Axis
	VistaVector3D axis[3];
	axis[0][2] = 1;
	axis[1][1] = 0;
	axis[2][0] = 1;
	float rotate[3];
	rotate[0] = -90;
	rotate[1] =   0;
	rotate[2] =  90;
	VistaColor colors[3];
	colors[0] =  VistaColor(0.8f,0.2f,0.2f);
	colors[1] =  VistaColor(0.2f,0.8f,0.2f);
	colors[2] =  VistaColor(0.2f,0.2f,0.8f);

	for(int i=0; i < 3; ++i)
	{
		static char buffer[32];
		sprintf(buffer, "(%3.1f, %3.1f, %3.1f)-axis", axis[i][0], axis[i][1], axis[i][2]);

		m_pAxis[i] = new VistaCylinder(m_pVistaSceneGraph, m_pGroup, 0.81f, 0.05f, 10, false, false);
		m_pAxis[i]->SetColor(colors[i]);
		m_pAxis[i]->SetName(buffer);
		m_pAxis[i]->SetTranslation(VistaVector3D(0.0f,0.49f,0.0f));
		if(rotate[i])
		m_pAxis[i]->Rotate( VistaQuaternion( VistaAxisAndAngle( axis[i], Vista::DegToRad(rotate[i]) )));

		m_pAxisHat[i] = new VistaCone(m_pVistaSceneGraph, m_pGroup, 0.2f, 0.1f, 10, true, true);
		m_pAxisHat[i]->SetColor(colors[i]);
		sprintf(buffer, "(%3.1f, %3.1f, %3.1f)-arrow", axis[i][0], axis[i][1], axis[i][2]);

		m_pAxisHat[i]->SetName(buffer);
		m_pAxisHat[i]->SetTranslation(VistaVector3D(0.0f,0.9f,0.0f));
			if(rotate[i] != 0.0f)
			m_pAxisHat[i]->Rotate( VistaQuaternion( VistaAxisAndAngle( axis[i], Vista::DegToRad(rotate[i]) )));
	 }
	return true;
 }
#endif
