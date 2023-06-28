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

#ifndef WORKSPACEACTIONOBJECT_H_
#define WORKSPACEACTIONOBJECT_H_

#include <VistaDataFlowNet/VdfnActionObject.h>

#include <VistaBase/VistaVectorMath.h>

class VistaGroupNode;
class VistaSceneGraph;
class IVistaTransformable;

class WorkspaceActionObject : public IVdfnActionObject {
 public:
  WorkspaceActionObject(VistaGroupNode*, IVistaTransformable*, VistaSceneGraph*);
  virtual ~WorkspaceActionObject();

  bool SetMinXPort(const float&);
  bool SetMinYPort(const float&);
  bool SetMinZPort(const float&);

  bool SetMaxXPort(const float&);
  bool SetMaxYPort(const float&);
  bool SetMaxZPort(const float&);

  bool SetScalePort(const float&);

  bool SetHandlePositionPort(const VistaVector3D&);
  bool SetHandleOrientationPort(const VistaQuaternion&);

  void updateBB();

 private:
  float minx;
  float miny;
  float minz;

  float maxx;
  float maxy;
  float maxz;

  float m_scale;

  VistaGroupNode*      m_pParent;
  VistaSceneGraph*     m_pVistaSceneGraph;
  IVistaTransformable* m_pHandle;

  // insert reflectionable code via macro here
  REFL_DECLARE
};

#endif /* WORKSPACEACTIONOBJECT_H_ */
