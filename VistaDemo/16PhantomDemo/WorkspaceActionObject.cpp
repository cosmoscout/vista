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

#include "WorkspaceActionObject.h"
#include "BoundingboxGeometry.h"

#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaAspects/VistaReflectionable.h>

#include <cstring>
#include <memory>

// insert reflectionable code via macro here again
REFL_IMPLEMENT_FULL(WorkspaceActionObject, IVdfnActionObject);

WorkspaceActionObject::WorkspaceActionObject(
    VistaGroupNode* pParent, IVistaTransformable* pHandle, VistaSceneGraph* pSG)
    : m_pParent(pParent)
    , m_pVistaSceneGraph(pSG)
    , m_pHandle(pHandle)
    , minx(0)
    , maxx(0)
    , miny(0)
    , maxy(0)
    , minz(0)
    , maxz(0)
    , m_scale(1) {
}

WorkspaceActionObject::~WorkspaceActionObject() {
}

/**
 * Here the boundingBox is created and updated of their bounds change somehow
 */
void WorkspaceActionObject::updateBB() {
  // Create a new bounding box
  VistaBoundingBox bb;
  float            a[3] = {minx * m_scale, miny * m_scale, minz * m_scale};
  float            b[3] = {maxx * m_scale, maxy * m_scale, maxz * m_scale};
  bb.SetBounds(a, b);
  BoundingboxGeometry geom(m_pVistaSceneGraph, bb);
  VistaGroupNode*     pBox = geom.getVistaNode();
  pBox->SetName("WAO_BB");

  // remove the old one if existing
  if (m_pParent->GetChild("WAO_BB"))
    delete m_pParent->GetChild("WAO_BB");

  // add the new one
  m_pParent->AddChild(pBox);
}

/**
 * Setters for the DFN node
 */
bool WorkspaceActionObject::SetScalePort(const float& f) {
  m_scale = f;
  updateBB();
  return true;
}

bool WorkspaceActionObject::SetMinXPort(const float& f) {
  minx = f;
  updateBB();
  return true;
}

bool WorkspaceActionObject::SetMinYPort(const float& f) {
  miny = f;
  updateBB();
  return true;
}

bool WorkspaceActionObject::SetMinZPort(const float& f) {
  minz = f;
  updateBB();
  return true;
}

bool WorkspaceActionObject::SetMaxXPort(const float& f) {
  maxx = f;
  updateBB();
  return true;
}

bool WorkspaceActionObject::SetMaxYPort(const float& f) {
  maxy = f;
  updateBB();
  return true;
}

bool WorkspaceActionObject::SetMaxZPort(const float& f) {
  maxz = f;
  updateBB();
  return true;
}

bool WorkspaceActionObject::SetHandlePositionPort(const VistaVector3D& v) {
  m_pHandle->SetTranslation(v[0] * m_scale, v[1] * m_scale, v[2] * m_scale);
  return true;
}

bool WorkspaceActionObject::SetHandleOrientationPort(const VistaQuaternion& q) {
  float v[4];
  q.GetValues(v);
  m_pHandle->SetRotation(v);
  return true;
}

// static functor registration
// This functors are self registering to this object
// The use of std::auto_ptr is here very usefull cause the
// WorkspaceActionObject or the DFN can be the last ones
// who see the pointers alive... So who deletes them? => auto_ptr
namespace {
using namespace std;
auto_ptr<IVistaPropertySetFunctor> aSetFunctors[] = {

    auto_ptr<IVistaPropertySetFunctor>(new TActionSetFunctor<WorkspaceActionObject, // class type
        float,                                                                      // in type
        const float&>                                                               // out type
        (&WorkspaceActionObject::SetMinXPort,     // function pointer
            NULL,                                 // conversion method
            "min_x",                              // reflected method name
            SsReflectionName,                     // reflected class name
            "x value of the boundings' minimum")) // description string
    ,
    auto_ptr<IVistaPropertySetFunctor>(new TActionSetFunctor<WorkspaceActionObject, // class type
        float,                                                                      // in type
        const float&>                                                               // out type
        (&WorkspaceActionObject::SetMinYPort,     // function pointer
            NULL,                                 // conversion method
            "min_y",                              // reflected method name
            SsReflectionName,                     // reflected class name
            "y value of the boundings' minimum")) // description string
    ,
    auto_ptr<IVistaPropertySetFunctor>(new TActionSetFunctor<WorkspaceActionObject, // class type
        float,                                                                      // in type
        const float&>                                                               // out type
        (&WorkspaceActionObject::SetMinZPort,     // function pointer
            NULL,                                 // conversion method
            "min_z",                              // reflected method name
            SsReflectionName,                     // reflected class name
            "z value of the boundings' minimum")) // description string
    ,
    auto_ptr<IVistaPropertySetFunctor>(new TActionSetFunctor<WorkspaceActionObject, // class type
        float,                                                                      // in type
        const float&>                                                               // out type
        (&WorkspaceActionObject::SetMaxXPort,     // function pointer
            NULL,                                 // conversion method
            "max_x",                              // reflected method name
            SsReflectionName,                     // reflected class name
            "x value of the boundings' maximum")) // description string
    ,
    auto_ptr<IVistaPropertySetFunctor>(new TActionSetFunctor<WorkspaceActionObject, // class type
        float,                                                                      // in type
        const float&>                                                               // out type
        (&WorkspaceActionObject::SetMaxYPort,     // function pointer
            NULL,                                 // conversion method
            "max_y",                              // reflected method name
            SsReflectionName,                     // reflected class name
            "y value of the boundings' maximum")) // description string
    ,
    auto_ptr<IVistaPropertySetFunctor>(new TActionSetFunctor<WorkspaceActionObject, // class type
        float,                                                                      // in type
        const float&>                                                               // out type
        (&WorkspaceActionObject::SetMaxZPort,     // function pointer
            NULL,                                 // conversion method
            "max_z",                              // reflected method name
            SsReflectionName,                     // reflected class name
            "z value of the boundings' maximum")) // description string
    ,
    auto_ptr<IVistaPropertySetFunctor>(new TActionSetFunctor<WorkspaceActionObject, // class type
        VistaVector3D,                                                              // in type
        const VistaVector3D&>                                                       // out type
        (&WorkspaceActionObject::SetHandlePositionPort, // function pointer
            NULL,                                       // conversion method
            "handle_position",                          // reflected method name
            SsReflectionName,                           // reflected class name
            "the handle's position"))                   // description string
    ,
    auto_ptr<IVistaPropertySetFunctor>(new TActionSetFunctor<WorkspaceActionObject, // class type
        VistaQuaternion,                                                            // in type
        const VistaQuaternion&>                                                     // out type
        (&WorkspaceActionObject::SetHandleOrientationPort, // function pointer
            NULL,                                          // conversion method
            "handle_orientation",                          // reflected method name
            SsReflectionName,                              // reflected class name
            "the handle's orientation"))                   // description string
    ,
    auto_ptr<IVistaPropertySetFunctor>(new TActionSetFunctor<WorkspaceActionObject, // class type
        float,                                                                      // in type
        const float&>                                                               // out type
        (&WorkspaceActionObject::SetScalePort, // function pointer
            NULL,                              // conversion method
            "scale",                           // reflected method name
            SsReflectionName,                  // reflected class name
            "the bb scale"))                   // description string
    //		, NULL // terminate array
};
}; // namespace
