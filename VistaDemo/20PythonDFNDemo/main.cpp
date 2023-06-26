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

#include "ApplyColorActionObject.h"
#include "ColorChangerDfnNode.h"
#include "RandomNumberDfnNode.h"
#include "SetPositionActionObject.h"

#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnObjectRegistry.h>

#include "PyDfnNodeCreate.h"
#include <VistaBase/VistaExceptionBase.h>
#include <boost/python.hpp>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/**
 * This demo should give an overview over the possibilities of the
 * DataFlowNet (DFN). This provides an abstraction level, where
 * the user can define graphs via xml-files. These graphs represent
 * data flow networks consisting of nodes and edges. Data is generated at
 * sources, is passed along the edges to intermediate nodes where it is
 * processed, and is then fed to data sink nodes.
 * Each InteractionContext has an associated DFN-Graph, which is defined
 * in an xml file and is generated when starting ViSTA (and can be
 * reloaded and re-generated during runtime), so it is possible to alter
 * the behavior without changing the source code of an application.
 * This is especially useful as an abstraction layer, where for example
 * on a desktop, the mouse can be used to control a worldpointer, while a
 * flystick is used with the CAVE setup.
 * Instead of creating specialized code for each possible scenario, the
 * DFN allows to simply generate a common interface (a VdfnNode or
 * an ActionObject) and then create a setup-specific graph description
 * file that specifies how data is processed to match the requirements of
 * the interface. Thus, one can take the position data from any tracking
 * system, use the DFN graph to convert them to a unified reference frame,
 * and then feed them to the system. Alternatively, on could use the 2D mouse
 * coordinates, somehow transfer them to a 3D point, and set this value;
 * or one could use any other method (random numbers, pre-recorded values, ...).
 * This allows an abstraction where the application only provides the interfaces
 * and the data processing is abstracted in the graph.
 */
int main(int argc, char* argv[]) {
  Py_Initialize();

  /**
   * This demo will show how to use DfnActionObjects to create a unified interface, as
   * well as how to create new generic DfnNodes that can be used for data processing.
   * In the demo scenario, we want to create a sphere whose color is changing randomly,
   * and which can be moved with a interaction device (which is a mouse in this case).
   */
  VistaSystem* pVistaSystem = new VistaSystem;
  try {
    pVistaSystem->IntroMsg();

    /**
     * First, we initialize the ViSTA system. Note that, while the graph files are
     * already read here, the actual graphs are constructed on first evaluation.
     * Therefore, it is NOT necessary to register own DfnNodes or ActionObjects
     * before the Init() call
     */
    if (pVistaSystem->Init(argc, argv) == false)
      return -1;

    /**
     * First, we create our scene - consisting of a simple sphere
     */
    VistaSceneGraph* pSceneGraph = pVistaSystem->GetGraphicsManager()->GetSceneGraph();

    VistaTransformNode* pGeomTrans = pSceneGraph->NewTransformNode(pSceneGraph->GetRoot());

    VistaGeometryFactory oGeometryFactory(pSceneGraph);
    VistaGeometry*       pGeometry = oGeometryFactory.CreateSphere(0.05f, 64, VistaColor::WHITE);
    pSceneGraph->NewGeomNode(pGeomTrans, pGeometry);

    /**
     * Now, we want to control the position of the sphere with an external input device.
     * For this, we first need to define an Interface from the DFN to our own application:
     * the ActionObject. Please see SetPositionAxtionObject.[h/cpp] for details on how
     * to create a simple ActionObject.
     * @see SetPositionActionObject
     * When we have defined our interface, we have to register it, and thereby also give
     * it a name. We create a new Instance of the ActionObject and pass it the TransformNode
     * it should control, and then Register it with the DfnObjectRegistry with the
     * name "SpherePosition". This name will be used in the DFN graph to reference this
     * Object.
     * Now, let's take a look at how to use the ActionObject in a DFN file.
     * See configfiles/xml/sphereposition.xml for the corresponding graph that reads
     * sensor data from a mouse, transforms it to a 3D position, and feed it to
     * our ActionObject.
     */
    pVistaSystem->GetDfnObjectRegistry()->SetObject(
        "SpherePosition", new SetPositionActionObject(pGeomTrans), NULL);

    /**
     * In addition to the position of the sphere, we also want to randomly modify its color.
     * For this, we first create another ActionObject, the RandomNumberDfnNodeCreate.
     * Please see ApplyColorActionObject.[h/cpp] for details, it slightly differs
     * from the SetPositionActionObject in that it also creates a Getter/outport
     * @see ApplyColorActionObject
     */
    pVistaSystem->GetDfnObjectRegistry()->SetObject(
        "SphereColor", new ApplyColorActionObject(pGeometry), NULL);

    /**
     * In addition to the ActionObject, we also need specialized nodes that
     * provide additional functionality in the DFN graphs.
     * For this scenario, we create two own nodes by inheriting from the
     * IVdfnNode Interface.
     * ColorChangerDfnNode: Takes three float values as inports, which define the
     *                      change of an RGB color, applies the change, and outputs
     *                      The resulting color as VistaVector3D
     *                      @see ColorChangerDfnNode[.h/cpp] for details
     * RandomNumberDfnNode: Node that generates a random float value
     *                      @see RandomNumberDfnNode[.h/cpp] for details
     * After writing the DfnNode classes, we have to register them with the DfnNodeFactory
     * this is also when we define the name for the node
     */
    VdfnNodeFactory* pNodeFactory = VdfnNodeFactory::GetSingleton();
    /**
     * ColorChangerDfnNode does not require any parameters to be passed or to be
     * specified in the xml file, so we can use a default node creator
     */
    pNodeFactory->SetNodeCreator("ColorChanger", new TVdfnDefaultNodeCreate<ColorChangerDfnNode>);
    /**
     * The RandomNumberDfnNode extracts additional parameters from the xml file, so
     * we need a specialized node creator.
     */
    pNodeFactory->SetNodeCreator("RandomNumber", new RandomNumberDfnNodeCreate);

    /**
     * PyDfnNode allows you to implement DFN nodes using python. Requires one parameter, the path to
     * the script.
     *
     */
    pNodeFactory->SetNodeCreator("Python", new PyDfnNodeCreate);

    /**
     * With these new nodes and the ApplyColorActionObject, we can create a DFN graph to
     * modify the color
     * @see configfiles/xml/spherecolor.xml
     */

    /**
     * As a last note: After running the application, you will notice some .dot-files
     * in your application folder. Those are graph visualizations of the respective
     * interaction framework, and help in developing/debugging new graphs.
     * You can look at them if you have dotty/graphviz available.
     */
  } catch (VistaExceptionBase& eException) {
    eException.PrintException();
  } catch (std::exception& eException) {
    std::cerr << "Exception:" << eException.what() << std::endl;
  }

  pVistaSystem->Run();

  delete pVistaSystem;

  return 0;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "main.cpp"                                                    */
/*============================================================================*/
