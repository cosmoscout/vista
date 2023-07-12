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

/*============================================================================*/
/*                                                                            */
/* This Demo will create a box and a sphere (textured), both located in       */
/* another sphere.                                                            */
/* Two ways of creating a sphere are demonstrated:                            */
/* oGeometryFactory.CreateSphere() and oGeometryFactory.CreateFromPropertyList*/
/*                                                                            */
/*============================================================================*/

#ifndef _GEOMETRYDEMOAPPL_H
#define _GEOMETRYDEMOAPPL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaGeometry;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class GeometryDemoAppl {
 public:
  GeometryDemoAppl();
  virtual ~GeometryDemoAppl();

  bool Init(int argc, char* argv[]);
  void Run();

 private:
  VistaSystem* m_pVistaSystem;

  VistaGeometry* m_pPlane;
  VistaGeometry* m_pBox;
  VistaGeometry* m_pDisk;
  VistaGeometry* m_pCone;
  VistaGeometry* m_pTorus;
  VistaGeometry* m_pEllipsoid;
  VistaGeometry* m_pSphere;
  VistaGeometry* m_pTriangle;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _GEOMETRYDEMOAPPL_H
