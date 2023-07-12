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

#include <cstring>
#include <string>

#include "DecreaseCallback.h"
#include "EnDisableCallback.h"
#include "IncreaseCallback.h"
#include "ResetCallback.h"
#include "ToggleAnimationCallback.h"
#include "VrmlVisualizer.h"

#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>

#include <VistaKernel/GraphicsManager/VistaNode.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>

#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaExceptionBase.h>

#include <VistaTools/VistaFileSystemDirectory.h>
#include <VistaTools/VistaProfiler.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VrmlVisualizer::VrmlVisualizer(int argc, char* argv[])
    : m_vistaSystem(VistaSystem())
    , m_vrmlProps(VistaPropertyList())
    , m_loadedNodes(std::vector<VistaTransformNode*>())
    , m_iCurrentStep(0)
    , m_graphicsEventHandler(this)
    , m_dLastTime(0)
    , m_dStepTime(0)
    , m_vecArgs(std::vector<std::string>()) {

  std::list<std::string> liSearchPaths;
  // use global configfile
  liSearchPaths.push_back("../configfiles/");
  m_vistaSystem.SetIniSearchPaths(liSearchPaths);

  // ViSTA's standard intro message
  m_vistaSystem.IntroMsg();

  // parse parameters from commandline
  for (int i = 0; i < argc; ++i) {
    m_vecArgs.push_back(std::string(argv[i]));
  }
  ParseParameters();

  // init the system
  if (!m_vistaSystem.Init(argc, argv)) {
    VISTA_THROW("[VRMLVisualizer] Failed to init VistaSystem.", 1);
  }

  m_dLastTime = m_vistaSystem.GetFrameClock();

  // is the filename or dirname not empty
  if (!m_vrmlProps.HasProperty("filename") && !m_vrmlProps.HasProperty("dirname")) {
    PrintUsage();
    VISTA_THROW("[VRMLVisualizer] No file or directory given.", 1);
  }

  m_vistaSystem.GetEventManager()->AddEventHandler(
      &m_graphicsEventHandler, VistaSystemEvent::GetTypeId(), VistaSystemEvent::VSE_PREGRAPHICS);

  // load all the data given with the parameters
  if (!LoadData()) {
    VISTA_THROW("[VRMLVisualizer] Failed to load data.", 1);
  }

  // Keyboard interaction
  VistaKeyboardSystemControl* pKeyCtrl = m_vistaSystem.GetKeyboardSystemControl();

  pKeyCtrl->BindAction(
      'd', new EnDisableCallback(m_loadedNodes), "enable / disable all loaded nodes.");
  pKeyCtrl->BindAction(
      ' ', new ToggleAnimationCallback(m_vrmlProps), "enable / disable animation.");
  pKeyCtrl->BindAction('+', new IncreaseCallback(this), "next step / increase animation speed.");
  pKeyCtrl->BindAction('-', new DecreaseCallback(this), "prev step / decrease animation speed.");
  pKeyCtrl->BindAction('r', new ResetCallback(this), "resets the animation to default.");
}

VrmlVisualizer::~VrmlVisualizer() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/**
 * Run VistaSystem.
 */
void VrmlVisualizer::Run() {
  // Start Universe
  m_vistaSystem.Run();
}

/**
 * Print usage instructions to std::cout
 */
void VrmlVisualizer::PrintUsage() {
  std::cout << "VistaVrmlVisualizer can show single *,wrl files or an set of files" << std::endl;
  std::cout << "which is animated like a flip book" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "--filename -f\t\tFilename of a single VRML file." << std::endl;
  std::cout << "--directory -d\t\tDirectory which contains VRML files to animate" << std::endl;
  std::cout << "--animation -a\t\tAnimation infofile to specify animation" << std::endl;
  std::cout << "--steptime -s\t\tGives the time for one step in animation in seconds" << std::endl;
  std::cout << "--time -t\t\tGives the time for the complete animation in seconds" << std::endl;
  std::cout << "--help -h\t\tShow this page" << std::endl;
}

/**
 * Loads all the stuff.
 */
bool VrmlVisualizer::LoadData() {
  bool flag = true;
  if (m_vrmlProps.HasProperty("dirname")) {
    VistaFileSystemDirectory dir(m_vrmlProps.GetValueOrDefault<std::string>("dirname"));
    if (!dir.Exists())
      return false;

    for (VistaFileSystemDirectory::const_iterator it = dir.begin(); it != dir.end(); ++it) {
      if ((*it)->IsFile())
        flag = AddByFilename((*it)->GetName()) & flag;
    }
  } else {
    flag = AddByFilename(m_vrmlProps.GetValueOrDefault<std::string>("filename"));
  }

  if (!m_loadedNodes.empty()) {
    m_loadedNodes[0]->SetIsEnabled(true);
  }

  return flag;
}

void VrmlVisualizer::Update() {

  VistaVector3D GroupNode;

  if (!m_vrmlProps.GetValueOrDefault<bool>("animation")) {
    m_dLastTime = m_vistaSystem.GetFrameClock();
  } else if (!m_loadedNodes.empty()) {
    double dCurrentTime = m_vistaSystem.GetFrameClock();
    while (dCurrentTime > m_dLastTime + m_dStepTime) {
      NextStep();
      m_dLastTime += m_dStepTime;
    }
  }
}

void VrmlVisualizer::NextStep() {
  m_loadedNodes[m_iCurrentStep]->SetIsEnabled(false);
  m_iCurrentStep = (m_iCurrentStep + 1) % m_loadedNodes.size();
  m_loadedNodes[m_iCurrentStep]->SetIsEnabled(true);
}

void VrmlVisualizer::PrevStep() {
  m_loadedNodes[m_iCurrentStep]->SetIsEnabled(false);
  m_iCurrentStep = (m_iCurrentStep - 1) % m_loadedNodes.size();
  m_loadedNodes[m_iCurrentStep]->SetIsEnabled(true);
}

void VrmlVisualizer::IncreaseStepSpeed() {
  m_dStepTime -= 0.1;
  if (m_dStepTime < 0)
    m_dStepTime = 0;
}

void VrmlVisualizer::DecreaseStepSpeed() {
  m_dStepTime += 0.1;
}

bool VrmlVisualizer::AddByFilename(const std::string& strFilename) {
  IVistaNode* pNode = m_vistaSystem.GetGraphicsManager()->GetSceneGraph()->LoadNode(strFilename);

  // assert first node as TransformNode
  pNode = FirstNodeAsTransformNode(pNode);

  if (!m_vistaSystem.GetGraphicsManager()->GetSceneGraph()->GetRoot()->AddChild(pNode))
    return false;

  m_loadedNodes.push_back(static_cast<VistaTransformNode*>(pNode));
  static_cast<VistaTransformNode*>(pNode)->SetIsEnabled(false);

  return true;
}

/**
 * Makes sure that the first node of the given tree is a VistaTransformNode
 */
VistaTransformNode* VrmlVisualizer::FirstNodeAsTransformNode(IVistaNode* pNode) {
  // local root of the subtree
  VistaTransformNode* pLocalRoot = NULL;
  if (pNode->GetType() == VISTA_TRANSFORMNODE) {
    // it is a TransformNode
    pLocalRoot = static_cast<VistaTransformNode*>(pNode);
  } else {
    // get the old parent
    IVistaNode* pParent = pNode->GetParent();
    // pParent may be NULL which is okay....
    pLocalRoot = m_vistaSystem.GetGraphicsManager()->GetSceneGraph()->NewTransformNode(
        dynamic_cast<VistaGroupNode*>(pParent));
    // if node had a parent, the new transformnode alreay is hanging under it
    // the AddChild call detaches from old parent and attaches to the new transform node
    pLocalRoot->AddChild(pNode);
  }
  return pLocalRoot;
}

void VrmlVisualizer::ParseParameters() {
  m_vrmlProps.RemoveProperty("dirname");
  m_vrmlProps.RemoveProperty("filename");
  m_vrmlProps.RemoveProperty("steptime");
  m_vrmlProps.RemoveProperty("time");

  // get file from parameter list
  for (int i = 1; i < m_vecArgs.size(); ++i) {
    if (std::string("--filename").compare(m_vecArgs[i]) == 0 ||
        std::string("-f").compare(m_vecArgs[i]) == 0) {
      std::cout << "f" << std::endl;
      if ((i + 1) > m_vecArgs.size()) {
        PrintUsage();
        VISTA_THROW("[VRMLVisualizer] No filename given.", 1);
      }
      m_vrmlProps.SetValue<std::string>("filename", m_vecArgs[i + 1]);
      continue;
    } else if (std::string("--directory").compare(m_vecArgs[i]) == 0 ||
               std::string("-d").compare(m_vecArgs[i]) == 0) {
      // if no dirname is given
      if ((i + 1) > m_vecArgs.size()) {
        PrintUsage();
        VISTA_THROW("[VRMLVisualizer] No directory given.", 1);
      }
      m_vrmlProps.SetValue<std::string>("dirname", m_vecArgs[i + 1]);
      continue;
    } else if (std::string("--animation").compare(m_vecArgs[i]) == 0 ||
               std::string("-a").compare(m_vecArgs[i]) == 0) {
      // if no animationfile is given
      if ((i + 1) > m_vecArgs.size()) {
        PrintUsage();
        VISTA_THROW("[VRMLVisualizer] No animationfile given.", 1);
      }
      m_vrmlProps.SetValue<std::string>("animationFilename", m_vecArgs[i + 1]);
      continue;
    } else if (std::string("--steptime").compare(m_vecArgs[i]) == 0 ||
               std::string("-s").compare(m_vecArgs[i]) == 0) {
      // if no animationfile is given
      if ((i + 1) > m_vecArgs.size()) {
        PrintUsage();
        VISTA_THROW("[VRMLVisualizer] No time for one step given.", 1);
      }
      m_vrmlProps.SetValue<double>(
          "steptime", VistaAspectsConversionStuff::ConvertToDouble(m_vecArgs[i + 1]));
      continue;
    } else if (std::string("--time").compare(m_vecArgs[i]) == 0 ||
               std::string("-t").compare(m_vecArgs[i]) == 0) {
      // if no animationfile is given
      if ((i + 1) > m_vecArgs.size()) {
        PrintUsage();
        VISTA_THROW("[VRMLVisualizer] No time for the animation given.", 1);
      }
      m_vrmlProps.SetValue<double>(
          "time", VistaAspectsConversionStuff::ConvertToDouble(m_vecArgs[i + 1]));
      continue;
    } else if (std::string("--help").compare(m_vecArgs[i]) == 0 ||
               std::string("-h").compare(m_vecArgs[i]) == 0) {
      // get some help
      PrintUsage();
      return;
    }
  }

  if (!m_vrmlProps.GetValueOrDefault<std::string>("animationFilename").empty()) {
    VistaProfiler prof;
    std::string   section = "VRML";
    if (!m_vrmlProps.HasProperty("dirname")) {
      m_vrmlProps.SetValue<std::string>(
          "dirname", prof.GetTheProfileString(section, "DIRECTORY", "",
                         m_vrmlProps.GetValueOrDefault<std::string>("animationFilename")));
    }
    if (!m_vrmlProps.HasProperty("filename")) {
      m_vrmlProps.SetValue<std::string>(
          "filename", prof.GetTheProfileString(section, "FILENAME", "",
                          m_vrmlProps.GetValueOrDefault<std::string>("animationFilename")));
    }
    if (!m_vrmlProps.HasProperty("steptime")) {
      m_vrmlProps.SetValue<double>(
          "steptime", prof.GetTheProfileFloat(section, "STEPTIME", 1,
                          m_vrmlProps.GetValueOrDefault<std::string>("animationFilename")));
    }
    if (!m_vrmlProps.HasProperty("time")) {
      m_vrmlProps.SetValue<double>(
          "time", prof.GetTheProfileFloat(section, "TIME", 0,
                      m_vrmlProps.GetValueOrDefault<std::string>("animationFilename")));
    }
  }

  m_vrmlProps.SetValue<bool>("animation",
      m_vrmlProps.HasProperty("dirname") || m_vrmlProps.HasProperty("animationFilename"));

  VistaFileSystemDirectory dir(m_vrmlProps.GetValueOrDefault<std::string>("dirname"));
  m_dStepTime = m_vrmlProps.GetValueOrDefault<double>("time") / dir.GetNumberOfEntries();
  if (m_vrmlProps.HasProperty("steptime")) {
    m_dStepTime = m_vrmlProps.GetValueOrDefault<double>("steptime");
  }

  if (m_dStepTime == 0) {
    std::cout << "[VRMLVisualizer] Warning: Steptime is 0. Setting steptime to 1." << std::endl;
    m_dStepTime = 1;
  }
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/*  END OF FILE "MyDemoAppl.cpp"                                              */
/*============================================================================*/
