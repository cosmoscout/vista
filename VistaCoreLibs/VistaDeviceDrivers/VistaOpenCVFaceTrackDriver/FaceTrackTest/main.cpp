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

#include <VistaKernel/GraphicsManager/VistaGeomNode.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaGraphicsManager.h>
#include <VistaKernel/GraphicsManager/VistaGroupNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/VistaSystem.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

const static int   S_iArraySize    = 2; // in each direction, so actually 2*n+1
const static float S_fArraySpacing = 0.5f;
const static float S_fCubeSize     = 0.1f;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

int main(int argc, char* argv[]) {
  VistaSystem oSystem;

  if (oSystem.Init(argc, argv) == false)
    return -1;

  VistaSceneGraph*     pSceneGraph = oSystem.GetGraphicsManager()->GetSceneGraph();
  VistaGeometryFactory oFactory(pSceneGraph);
  VistaGeometry*       pGeom =
      oFactory.CreateBox(S_fCubeSize, S_fCubeSize, S_fCubeSize, 1, 1, 1, VistaColorRGB::LIGHT_GRAY);

  for (int x = -S_iArraySize; x <= S_iArraySize; ++x) {
    float fXPos = x * S_fArraySpacing;
    for (int y = -S_iArraySize; y <= S_iArraySize; ++y) {
      float fYPos = y * S_fArraySpacing;
      for (int z = -S_iArraySize; z <= S_iArraySize; ++z) {
        float               fZPos  = z * S_fArraySpacing;
        VistaTransformNode* pTrans = pSceneGraph->NewTransformNode(pSceneGraph->GetRoot());
        pTrans->SetTranslation(fXPos, fYPos, fZPos);
        pSceneGraph->NewGeomNode(pTrans, pGeom);
      }
    }
  }

  oSystem.Run();

  return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "main.cpp"                                                    */
/*============================================================================*/
