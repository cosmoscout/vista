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

#include "VdfnPersistence.h"

#include "VdfnGraph.h"
#include "VdfnNode.h"
#include "VdfnNodeFactory.h"
#include "VdfnPort.h"
#include "VdfnPortFactory.h"

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaObjectRegistry.h>
#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaTools/VistaFileSystemDirectory.h>
#include <VistaTools/VistaFileSystemFile.h>
#include <VistaTools/tinyXML/tinyxml.h>

#if defined(__GNUC__)
#include <cxxabi.h>
#endif

#include <fstream>
#include <iostream>
#include <set>

using namespace VistaXML;

namespace {
/**
 * builds (recursively) a PropertyList with nested param sections
 * from an XML description.
 * It searches for XML nodes of type 'param' and connectes their
 * 'name' and 'value' property to the name and value of a VistaProperty
 * instance. In case no explicit 'value' attribute is found, the
 * first following child of 'param' is taken as value for the property,
 * as a text value.
 */
void DiveParams(TiXmlElement* node, VistaPropertyList& oProps) {
  TiXmlHandle nd(node);

  // get first param child
  TiXmlElement* child = nd.FirstChild("param").Element();
  for (; child != NULL; child = child->NextSiblingElement()) {
    // does it have a name?
    const char* pcName = child->Attribute("name");
    if (!pcName)
      pcName = "<none>";

    TiXmlHandle   cd(child);
    TiXmlElement* pChild = cd.FirstChild("param").Element(); // let's see...
    if (pChild) {
      // yes. create a sub PropertyList called sub
      VistaPropertyList sub;
      DiveParams(child, sub);                   // recursive call
      oProps.SetPropertyListValue(pcName, sub); // *copy* to parent PropertyList given the name
    } else {
      // child param node, attach it, claim value from attribute first
      const char* pcValue = child->Attribute("value");
      if (pcValue) {
        // assign value attribute
        oProps.SetValue(pcName, pcValue);
      } else {
        // no value attribute... see if there is a text child with a proper value
        // get next text node
        TiXmlHandle next(child->FirstChild());
        if (next.Text()) {
          // ok assign
          oProps.SetValue(pcName, next.Text()->Value());
        } else {
          vstr::warnp() << "recursive parameter build did not find a value of param [" << pcName
                        << "], neither value nor a text node found." << std::endl;
        }
      }
    }
  }
}

IVdfnNode* CreateNode(
    TiXmlElement* node, const VdfnNodeFactory& fac, const std::string& strGraphTag,
    const VistaPropertyList& nodeParams = VistaPropertyList() /**< contains all node parameters!
                                                (as we do determine the name just here)
                                              */
) {
  // get type tag
  const char* pcType = node->Attribute("type");
  const char* pcName = node->Attribute("name");

  // the pcTag is an individual node property
  // and is not to be confused with the graph tag
  // given by strGraphTag
  const char* pcTag = node->Attribute("tag");

  if (!pcType || !pcName) {
    vstr::warnp() << "No Type or Name given for node creation ["
                  // node->Print( stderr, 0 );
                  << node->Value() << "] - File [" << strGraphTag << "] - Line: " << node->Row()
                  << std::endl;
    return NULL;
  }

  VistaPropertyList oProps; // props to pass to the creator factory

  VistaPropertyList oParams; // params that were read or merged from the xml description
  DiveParams(node, oParams); // create from the xml file given

  // try to merge in external props
  if (nodeParams.HasProperty(pcName)) {
    vstr::outi() << "[VdfnNode::CreateNode]: External props found for node [" << pcName << "]"
                 << std::endl;
    if (nodeParams.GetPropertyConstRef(pcName).GetPropertyType() ==
        VistaProperty::PROPT_PROPERTYLIST) // does it have a subsection for this node? and is it a
                                           // PropertyList?
    // (the latter is a sanity-check)
    {
      // yes
      const VistaPropertyList& ndParams =
          nodeParams.GetPropertyConstRef(pcName).GetPropertyListConstRef();
      oParams = VistaPropertyList::MergePropertyLists(oParams, ndParams);
      vstr::outi() << "[VdfnNode::CreateNode]: Merging creation parameters for [" << pcName
                   << "] for the node creator." << std::endl;
      oParams.Print();
    } else {
      vstr::warnp() << "[VdfnNode::CreateNode]: Merge-in PropertyList is no PropertyList:"
                    << std::endl;
      nodeParams.Print();
    }
  }

  // set (or re-set) the following properties.
  oProps.SetValue("tag", strGraphTag);
  oProps.SetValue("type", pcType);
  oProps.SetValue("name", pcName);
  oProps.SetPropertyListValue("param", oParams);

  // pass arguments for factory method and hopefully create the node
  // of desire.
  IVdfnNode* pRet;
  try {
    pRet = fac.CreateNode(pcType, oProps);
  } catch (VistaExceptionBase& x) {
    x.PrintException();
    pRet = NULL;
  }

  if (!pRet) {
    // no... did not work
    vstr::warnp() << "[Vdfn::CreateNode]: ERROR in graph [" << strGraphTag << "], line "
                  << node->Row() << ":" << std::endl;
    vstr::warni() << vstr::singleindent << "Could not create node [" << pcName << " ; " << pcType
                  << "] from factory" << std::endl;
    if (fac.GetHasCreator(pcType) == false) {
      vstr::warni() << vstr::singleindent << "Node Type [" << pcType << "] not registered"
                    << std::endl;
    } else {
      vstr::warni() << vstr::singleindent << "Node Type [" << pcType
                    << "] registered, so probably the parameters are wrong" << std::endl;
    }

    return NULL;
  }

  // set a user tag (this is something users can search for in their code
  if (pcTag)
    pRet->SetUserTag(std::string(pcTag));

  // set a type tag, so we can do some neat stuff by the symbolic type of
  // the node (remember, that the symbolic type must not necessarily mean
  // the C++ type of the node (for example when aliasing old nodes)
  pRet->SetTypeTag(pcType);

  // set the name
  pRet->SetNameForNameable(pcName);

  // return
  return pRet;
}

VdfnGraph* CreateGraph(TiXmlHandle& nodespace, TiXmlHandle& graph, TiXmlHandle& edges,
    const VdfnNodeFactory& fac, const std::string& strTag, bool bEvaluate, bool& bContainsErrors,
    const VistaPropertyList& nodeParameters = VistaPropertyList()) {
  // ok, we use a temporary collection of objects in order to retrieve already created
  // instances in this graph (referencing).
  VistaObjectRegistry reg;

  // the return value
  VdfnGraph* DN_graph = new VdfnGraph;

  // first: create all node declared in the nodespace.
  TiXmlElement* child = nodespace.FirstChild("node").Element();
  for (; child != NULL; child = child->NextSiblingElement("node")) {

    IVdfnNode* pNode = CreateNode(child, fac, strTag, nodeParameters);
    if (pNode) {

      reg.RegisterNameable(pNode);
    }
  }

  TiXmlElement* graphel = graph.FirstChild("node").Element();
  for (; graphel != NULL; graphel = graphel->NextSiblingElement()) {
    IVdfnNode* pNode = NULL;

    // check: is this a noderef?
    if (VistaAspectsComparisonStuff::StringEquals(graphel->Value(), "noderef")) {
      // yes, try to resolve from the above given registry
      const char* pcName = graphel->Attribute("name");
      pNode              = dynamic_cast<IVdfnNode*>(reg.RetrieveNameable(pcName));
    } else if (VistaAspectsComparisonStuff::StringEquals(graphel->Value(), "node")) {
      // no, create a new node with the given name
      pNode = CreateNode(graphel, fac, strTag, nodeParameters);
      if (pNode) {
        IVistaNameable* pNd;
        if ((pNd = reg.RetrieveNameable(pNode->GetNameForNameable()))) {
          vstr::warnp() << "[Vdfn::CreateGraph]: Warning in graph [" << strTag << "], line "
                        << graphel->Row() << std::endl;
          vstr::warni() << vstr::singleindent << "A node with name [" << pNode->GetNameForNameable()
                        << "] is already registered - deleting the old one" << std::endl;

          reg.UnregisterNameable(pNd);

          // this one has probably been added to the graph before...
          // note that we know for this registry, that there are only
          // nodes stored, but use a dynamic cast. We have time here
          // and it is more safe to do so.
          IVdfnNode* pDfnNode = dynamic_cast<IVdfnNode*>(pNd);
          DN_graph->RemNode(pDfnNode); // remove from graph

          delete pDfnNode; // delete node
        }

        reg.RegisterNameable(pNode);
      }
    }

    if (pNode)
      DN_graph->AddNode(pNode);
    else {
      bContainsErrors = true;
      //				std::cerr << "COULD NOT CREATE NODE [";
      //				graphel->Print(stderr,0);
      //				std::cerr << "]" << std::endl;
    }
  }

  TiXmlElement* edge = edges.FirstChild("edge").Element();
  for (; edge != NULL; edge = edge->NextSiblingElement("edge")) {
    const char* pcFromName = edge->Attribute("fromnode");
    const char* pcToName   = edge->Attribute("tonode");
    const char* pcFromPort = edge->Attribute("fromport");
    const char* pcToPort   = edge->Attribute("toport");

    if (pcFromName == NULL)
      pcFromName = "";
    if (pcToName == NULL)
      pcToName = "";
    if (pcFromPort == NULL)
      pcFromPort = "";
    if (pcToPort == NULL)
      pcToPort = "";

    IVdfnNode* pFrom = DN_graph->GetNodeByName(pcFromName);
    IVdfnNode* pTo   = DN_graph->GetNodeByName(pcToName);

    // most code below is just for error checking.
    if (pTo && pFrom) {
      // get out port of the from node
      IVdfnPort* pFromPort = pFrom->GetOutPort(pcFromPort);
      if (!pFromPort) {
        vstr::warnp() << "[Vdfn::CreateGraph]: ERROR in graph [" << strTag << "], line "
                      << edge->Row() << std::endl;
        vstr::IndentObject oIndent;
        vstr::warni() << "Construction of edge from [" << pFrom->GetNameForNameable()
                      << "::" << pcFromPort << "] to [" << pTo->GetNameForNameable()
                      << "::" << pcToPort << "] failed\n";
        vstr::warni() << "Outport [" << pcFromPort << "] does not exist" << std::endl;
        vstr::warni() << "Available outports of node [" << pFrom->GetNameForNameable()
                      << "]:" << std::endl;
        vstr::IndentObject     oIndent2;
        std::list<std::string> liOutPorts = pFrom->GetOutPortNames();
        if (liOutPorts.empty())
          vstr::warni() << vstr::singleindent << "None" << std::endl;
        else {
          for (std::list<std::string>::const_iterator it = liOutPorts.begin();
               it != liOutPorts.end(); ++it) {
            vstr::warni() << vstr::singleindent << *it << " ["
                          << VistaConversion::CleanOSTypeName(
                                 pFrom->GetOutPort((*it))->GetTypeDescriptor())
                          << "]" << std::endl;
          }
        }
        bContainsErrors = true;
      } else {
        // set this out port to the given in port
        if (pTo->SetInPort(pcToPort, pFromPort) == false) {
          // did not work
          vstr::warnp() << "[Vdfn::CreateGraph]: ERROR in graph [" << strTag << "], line "
                        << edge->Row() << std::endl;
          vstr::IndentObject oIndent;
          vstr::warni() << "Construction of edge from [" << pFrom->GetNameForNameable()
                        << "::" << pcFromPort << "] to [" << pTo->GetNameForNameable()
                        << "::" << pcToPort << "] failed.\n";
          const IVdfnPortTypeCompare* pPortTC = pFromPort->GetPortTypeCompare();
          vstr::warni() << "Inport [" << pcToPort << "] with type ["
                        << VistaConversion::CleanOSTypeName(pPortTC->GetTypeDescriptor())
                        << "] does not exist" << std::endl;
          delete pPortTC;

          vstr::warni() << "Available inports of node [" << pTo->GetNameForNameable()
                        << "]:" << std::endl;
          vstr::IndentObject     oIndent2;
          std::list<std::string> liInPorts = pTo->GetInPortNames();
          if (liInPorts.empty())
            vstr::warni() << "None" << std::endl;
          else {
            for (std::list<std::string>::const_iterator it = liInPorts.begin();
                 it != liInPorts.end(); ++it) {
              pPortTC = &(pTo->GetPortTypeCompareFor(*it));
              vstr::warni() << *it << " ["
                            << VistaConversion::CleanOSTypeName(pPortTC->GetTypeDescriptor()) << "]"
                            << std::endl;
            }
          }
          bContainsErrors = true;
        } // did work, no need to utter that.
      }
    } else {
      // One of the nodes was not found.
      vstr::warnp() << "[Vdfn::CreateGraph]: ERROR in graph [" << strTag << "], line "
                    << edge->Row() << std::endl;
      vstr::IndentObject oIndent;
      vstr::warni() << "Construction of edge from ["
                    << ((pFrom) ? (pFrom->GetNameForNameable()) : ("<null>")) << "::" << pcFromPort
                    << "] to [" << ((pTo) ? (pTo->GetNameForNameable()) : ("<null>"))
                    << "::" << pcToPort << "] failed." << std::endl;
      if (pFrom == NULL) {
        vstr::warni() << vstr::singleindent << "From-Node [" << pcFromName << "] does not exist"
                      << std::endl;
      }
      if (pTo == NULL) {
        vstr::warni() << vstr::singleindent << "To-Node [" << pcToName << "] does not exist"
                      << std::endl;
      }

      bContainsErrors = true;
    }
  }

  // eventually initialize the graph giving Evaluate(0)
  if (bEvaluate)
    DN_graph->EvaluateGraph(0);

  return DN_graph;
}

/**
 * enforce a clean string. A clean string is either empty or
 * - not starting with a digit (0-9)
 * - not containing a "-"
 * - not containing a " "
 * invalid chars are replaced or prefixed by an underscore ("_")
 */
std::string CleanNodeName(const std::string& strNodeName) {
  if (strNodeName.empty())
    return strNodeName;

  std::string strOut = strNodeName; // copy name as template for the output
  // get prefix char
  char _f = *strOut.substr(0, 1).c_str();
  if (::isdigit(int(_f))) // starts with a (0-9)?
  {
    // yes, prefix with "_"
    strOut = std::string("_") + strOut; // prepend _ in case string starts with a digit
  }

  // replace "-" and " " with "_" INPLACE
  for (std::string::iterator cit = strOut.begin(); cit != strOut.end(); ++cit) {
    if ((*cit) == '-' || (*cit) == ' ')
      (*cit) = '_';
  }

  return strOut;
}

std::string EscapeBraces(const std::string& strIn) {
  std::string strOut;
  for (std::string::const_iterator cit = strIn.begin(); cit != strIn.end(); ++cit) {
    if ((*cit) == '<' || (*cit) == '>')
      strOut.push_back('\\');

    strOut.push_back(*cit);
  }

  return strOut;
}

/**
 * create a string that does not contain a plain spacing.
 * simple white-spaces are replaced by an escaped version.
 * the routing creates an output string by copying, so it might
 * be a bit expensive to use. It is used to create commentary
 * strings that are visible in the dot output.
 */
std::string NoSpacing(const std::string& strNodeName) {
  std::string strOut;
  // replace "-" with "_" and " " with "\ " (escaped whitespace)
  for (std::string::const_iterator cit = strNodeName.begin(); cit != strNodeName.end(); ++cit) {
    if ((*cit) == ' ') {
      strOut.push_back('\\');
      strOut.push_back(' ');
    } else
      strOut.push_back(*cit);
  }

  return strOut;
}

/**
 * dumps the XML graph as DOT file.
 * @todo finish me. the method is not fully implemented.
 */
bool DumpXml(TiXmlHandle& nodespace, TiXmlHandle& graph, TiXmlHandle& edges,
    const std::string& strDotFile, const std::string& strGraph) {
  // open dot file
  std::ofstream of(strDotFile.c_str());
  if (!of.good()) {
    vstr::warnp() << "[DumpXml]: could not open file [" << strDotFile << "]" << std::endl;
    return false;
  }

  // write header
  std::string strGraphName = CleanNodeName(strGraph); // copy const-string

  // ok, lets dance...
  of << "digraph " << strGraphName << std::endl << "{" << std::endl;

  // build a little name cache
  std::set<std::string> setNodeNames, setEdgeNodes;
  std::set<std::string> setPortNames;

  // first all elements in the graph itself
  TiXmlElement* graphel = graph.ChildElement(0).Element();
  for (; graphel != NULL; graphel = graphel->NextSiblingElement()) {
    const char* pcFrom = graphel->Attribute("name");
    if (pcFrom != NULL) {
      setNodeNames.insert(std::string(pcFrom));
    }
  }

  // now all references in the edge set
  TiXmlElement* edge = edges.ChildElement(0).Element();
  for (; edge != NULL; edge = edge->NextSiblingElement()) {
    const char* pcFromName = edge->Attribute("fromnode");
    const char* pcToName   = edge->Attribute("tonode");

    if (pcFromName)
      setEdgeNodes.insert(pcFromName);
    if (pcToName)
      setEdgeNodes.insert(pcToName);

    if (pcFromName && pcToName) {
      of << CleanNodeName(pcFromName) << " -> " << CleanNodeName(pcToName);
    }

    const char* pcFromPort = edge->Attribute("fromport");
    if (pcFromPort != NULL)
      setPortNames.insert(pcFromPort);

    const char* pcToPort = edge->Attribute("toport");
    setPortNames.insert(pcToPort);

    if (pcFromPort && pcToPort) {
      of << "[label=\"" << pcFromPort << "->" << pcToPort << "\"]\n";
    } else
      of << "[label=\"malformed edge\", color=red]\n";
  }

  // end graph brace
  of << "}\n";

  // close file
  of.flush();
  of.close();

  return false;
}

/**
 * searches exports from a graph that can be used to create composite nodes, which
 * contain graphs.
 */
bool CreateExports(
    const TiXmlHandle& oExportRoot, std::list<VdfnGraph::ExportData>& liExports, bool bStrict) {
  // simply parse all elements and push CExport instances to their proper place
  TiXmlElement* graphel = oExportRoot.FirstChild("export").Element();
  for (; graphel != NULL; graphel = graphel->NextSiblingElement("export")) {
    const char* pcMapName   = graphel->Attribute("name");
    const char* pcPortName  = graphel->Attribute("port");
    const char* pcDirection = graphel->Attribute("direction");

    if (bStrict)
      if (!pcDirection && !pcPortName) {
        vstr::errp() << "VdfnPersistence::CreateExports -- "
                     << "Exception (strict set but no direction or no portname set)" << std::endl;
        return false;
      }

    if (!pcDirection)
      pcDirection = "";
    if (!pcPortName)
      pcPortName = "";

    std::string            sPortName(pcPortName);
    std::string            sDirection(pcDirection);
    std::string::size_type pos = sPortName.find_first_of('/', 0);

    if (pos == std::string::npos && bStrict)
      return false;

    std::string sPrefix, sPostfix;

    sPrefix  = sPortName.substr(0, pos);
    sPostfix = sPortName.substr(pos + 1, sPortName.length());

    std::string sMapName = (pcMapName ? std::string(pcMapName) : sPostfix);

    if (VistaAspectsComparisonStuff::StringEquals(sDirection, "IN", false)) {
      liExports.push_back(
          VdfnGraph::ExportData(sPrefix, sPostfix, sMapName, VdfnGraph::ExportData::INPORT));
    } else {
      liExports.push_back(
          VdfnGraph::ExportData(sPrefix, sPostfix, sMapName, VdfnGraph::ExportData::OUTPORT));
    }
  }

  return true;
}

VistaPropertyList LoadParameters(const std::string& strParameterFile) {
  VistaPropertyList oRet;
  TiXmlDocument     doc(strParameterFile.c_str());
  // parse returns NULL on "all-parsed" and a pointer to
  // the location in strGraphText.c_str(), where the error
  // happened.
  bool loadOkay = doc.LoadFile();
  if (loadOkay) {
    TiXmlHandle   docHandle(&doc);
    TiXmlHandle   parameters = docHandle.FirstChild("parameters");
    TiXmlElement* nodeel     = parameters.FirstChild("node").Element();
    for (; nodeel != NULL; nodeel = nodeel->NextSiblingElement("node")) {
      const char* pcNodeName = nodeel->Attribute("name");
      if (pcNodeName) {
        VistaPropertyList oParams;
        DiveParams(nodeel, oParams);
        oRet.SetPropertyListValue(pcNodeName, oParams);
      }
    }
  }
  return oRet;
}
} // end namespace

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VdfnGraph* VdfnPersistence::ParseGraph(const std::string& strGraphText,
    const std::string& strGraphTag, bool bEvaluateGraph, bool bStrict) {
  TiXmlDocument doc(strGraphTag.c_str());
  // parse returns NULL on "all-parsed" and a pointer to
  // the location in strGraphText.c_str(), where the error
  // happened.
  bool loadOkay = (doc.Parse(strGraphText.c_str()) ? false : true);
  if (loadOkay) {
    TiXmlHandle docHandle(&doc);
    TiXmlHandle module = docHandle.FirstChild("module");
    TiXmlHandle nodes  = module.FirstChild("nodespace");
    TiXmlHandle graph  = module.FirstChild("graph");
    TiXmlHandle edges  = module.FirstChild("edges");

    if (module.Element() && nodes.Element() && graph.Element() && edges.Element()) {
      bool       bContainsErrors = false;
      VdfnGraph* pGraph = CreateGraph(nodes, graph, edges, *VdfnNodeFactory::GetSingleton(),
          strGraphTag, bEvaluateGraph, bContainsErrors);

      if (bContainsErrors && bStrict) {
        delete pGraph;
      } else
        return pGraph;
    } else {
      vstr::warnp()
          << "VdfnPersistence::ParseGraph() - parsing worked, but some sections were not found?"
          << "\n"
          << vstr::indent << vstr::singleindent
          << "<module>: " << (module.Element() ? "FOUND" : "NOT FOUND") << "\n"
          << vstr::indent << vstr::singleindent
          << "<nodespace>:" << (nodes.Element() ? "FOUND" : "NOT FOUND") << "\n"
          << vstr::indent << vstr::singleindent
          << "<graph>:" << (graph.Element() ? "FOUND" : "NOT FOUND") << "\n"
          << vstr::indent << vstr::singleindent
          << "<edges>:" << (edges.Element() ? "FOUND" : "NOT FOUND") << std::endl;
    }
  } else {
    // load error, get more information from TinyXML
    vstr::warnp() << "VdfnPersistence::ParseGraph() - Error: " << doc.ErrorDesc() << std::endl;
    vstr::warni() << vstr::singleindent << "Line: " << doc.ErrorRow()
                  << " - Col: " << doc.ErrorCol() << std::endl;
  }

  return NULL;
}

VdfnGraph* VdfnPersistence::LoadGraph(const std::string& strGraphXmlFile,
    const std::string& strGraphTag, bool bEvaluate, bool bStrict,
    const std::string& strParameterFile) {
  VdfnGraph*    pGraph = NULL;
  TiXmlDocument doc(strGraphXmlFile.c_str());
  bool          loadOkay = doc.LoadFile();
  if (loadOkay) {
    bool bValid = false;

    TiXmlHandle docHandle(&doc);
    TiXmlHandle module  = docHandle.FirstChild("module");
    TiXmlHandle nodes   = module.FirstChild("nodespace");
    TiXmlHandle graph   = module.FirstChild("graph");
    TiXmlHandle edges   = module.FirstChild("edges");
    TiXmlHandle exports = module.FirstChild("exports");

    if (nodes.Element() == NULL) {
      vstr::warnp()
          << "VdfnPersistence::LoadGraph() - Compat warning: <nodespace> section not found..."
          << std::endl;
    } else if (module.Element() && graph.Element() && edges.Element()) {
      VistaPropertyList oNodeParameters;
      if (!strParameterFile.empty()) {
        // load parameters
        oNodeParameters = LoadParameters(strParameterFile);
      }

      bool bContainsErrors = false;
      bValid               = true;
      pGraph = CreateGraph(nodes, graph, edges, *VdfnNodeFactory::GetSingleton(), strGraphTag,
          bEvaluate, bContainsErrors, oNodeParameters);
      if (bStrict && bContainsErrors) {
        delete pGraph;
        pGraph = NULL;
        bValid = false;
      }
    } else {
      vstr::warnp() << "VdfnPersistence::LoadGraph() - loading [" << strGraphXmlFile
                    << "] worked, but some sections were not found?"
                    << "\n"
                    << vstr::indent << vstr::singleindent
                    << "<module>: " << (module.Element() ? "FOUND" : "NOT FOUND") << "\n"
                    << vstr::indent << vstr::singleindent
                    << "<nodespace>:" << (nodes.Element() ? "FOUND" : "NOT FOUND") << "\n"
                    << vstr::indent << vstr::singleindent
                    << "<graph>:" << (graph.Element() ? "FOUND" : "NOT FOUND") << "\n"
                    << vstr::indent << vstr::singleindent
                    << "<edges>:" << (edges.Element() ? "FOUND" : "NOT FOUND") << std::endl;
    }
    if (bValid && exports.Element()) // exports really declared!
    {
      std::list<VdfnGraph::ExportData> liExports;

      if (CreateExports(exports, liExports, bStrict) == false) {
        vstr::errp() << "VdfnPersistence::LoadGraph() - Error creating exports" << std::endl;
        if (bStrict) {
          delete pGraph;
          pGraph = NULL;
        }
      } else
        pGraph->SetExports(liExports);
    }
  } else {
    // load error, get more information from TinyXML
    vstr::errp() << "VdfnPersistence::LoadGraph() - Error: " << doc.ErrorDesc() << std::endl;
    VistaFileSystemFile file(strGraphXmlFile);
    if (file.Exists()) {
      vstr::errp() << "File exists, but syntax is wrong." << std::endl;
    } else {
      vstr::errp() << "Could not locate file by the name given [" << strGraphXmlFile
                   << "] -- wd: " << VistaFileSystemDirectory::GetCurrentWorkingDirectory()
                   << " - so go and check your configuration." << std::endl;
    }
  }

  return pGraph;
}

/** @todo implement! **/
// bool VdfnPersistence::SaveGraph( VdfnGraph *pGraph, const std::string &strGraphXmlFile )
//{
//	return false;
//}

bool VdfnPersistence::DumpXmlAsDot(const std::string& strGraphXmlFile,
    const std::string& strGraphDotFile, const std::string& strGraphName) {
  TiXmlDocument doc(strGraphXmlFile.c_str());
  bool          loadOkay = doc.LoadFile();
  if (loadOkay) {
    TiXmlHandle docHandle(&doc);
    TiXmlHandle module = docHandle.FirstChild("module");
    TiXmlHandle nodes  = module.FirstChild("nodespace");
    TiXmlHandle graph  = module.FirstChild("graph");
    TiXmlHandle edges  = module.FirstChild("edges");

    return DumpXml(nodes, graph, edges, strGraphDotFile, strGraphName);
  }
  return false;
}

// ############################################################################

namespace {

bool SaveNode(const IVdfnNode* pNode, std::ofstream& str, bool bWritePorts, bool bWriteTypes,
    bool bWriteValues) {
  if (bWritePorts || bWriteValues) {
    str << CleanNodeName(pNode->GetNameForNameable()) << " [shape=Mrecord, ";
    if (pNode->GetIsMasterSim())
      str << "color=black,fillcolor=blue,style=filled,";

    str << "label=\"{{";

    // write in ports on top
    std::list<std::string> liInPort = pNode->GetInPortNames();
    for (std::list<std::string>::const_iterator tit = liInPort.begin(); tit != liInPort.end();
         ++tit) {
      str << "<I_" << CleanNodeName(*tit) << "> " << *tit;
      if (bWriteTypes) {
        str << "\\n";
        std::string sPortType = pNode->GetPortTypeCompareFor((*tit)).GetTypeDescriptor();
#if !defined(__GNUC__)
        str << NoSpacing(EscapeBraces(sPortType));
#else
        int   state;
        char* ret = abi::__cxa_demangle(sPortType.c_str(), NULL, NULL, &state);
        if (state == 0 && ret) {
          str << NoSpacing(EscapeBraces(ret));
          free(ret);
        } else
          str << NoSpacing(EscapeBraces(sPortType));
#endif
        if (bWriteValues) {}
      }
      if (bWriteValues) {
        str << "\\n";
        IVdfnPort* pPort = pNode->GetInPort((*tit));
        if (pPort != NULL) {
          VdfnPortFactory::CPortAccess* pAcc =
              VdfnPortFactory::GetSingleton()->GetPortAccess(pPort->GetTypeDescriptor());
          if (pAcc && pAcc->m_pStringGet) {
            str << pAcc->m_pStringGet->GetValueAsString(pPort);
          } else {
            str << "\\<no stringconv\\>";
          }
        } else {
          str << "\\<not connected\\>";
        }
      }

      if (tit != --liInPort.end())
        str << "|";
    }
    str << "}|" << NoSpacing(pNode->GetNameForNameable()) << "\\n"
        << NoSpacing(pNode->GetTypeTag()) << "|{";

    // write all the ports
    std::list<std::string> liOutPort = pNode->GetOutPortNames();
    for (std::list<std::string>::const_iterator it = liOutPort.begin(); it != liOutPort.end();
         ++it) {

      str << "<O_" << CleanNodeName(*it) << "> " << *it;
      if (bWriteTypes) {
        str << "\\n";
#if !defined(__GNUC__)
        str << NoSpacing(EscapeBraces(pNode->GetOutPort(*it)->GetTypeDescriptor()));
#else
        int   state;
        char* ret = abi::__cxa_demangle(
            pNode->GetOutPort(*it)->GetTypeDescriptor().c_str(), NULL, NULL, &state);
        if (state == 0 && ret) {
          str << NoSpacing(EscapeBraces(ret));
          free(ret);
        } else
          str << NoSpacing(pNode->GetOutPort(*it)->GetTypeDescriptor());
#endif
      }
      if (bWriteValues) {
        str << "\\n";
        IVdfnPort* pPort = pNode->GetOutPort((*it));
        if (pPort != NULL) {
          VdfnPortFactory::CPortAccess* pAcc =
              VdfnPortFactory::GetSingleton()->GetPortAccess(pPort->GetTypeDescriptor());
          if (pAcc && pAcc->m_pStringGet) {
            str << pAcc->m_pStringGet->GetValueAsString(pPort);
          } else {
            str << "\\<no stringconv\\>";
          }
        } else {
          str << "\\<not connected\\>";
        }
      }

      if (it != --liOutPort.end())
        str << "|";
    }
    str << "}}\"];" << std::endl;
  } else {
    if (pNode->GetIsValid()) {
      str << CleanNodeName(pNode->GetNameForNameable()) << " [shape=Mrecord, label=\""
          << pNode->GetNameForNameable() << "\\n"
          << pNode->GetTypeTag() << "\""
          << (pNode->GetIsMasterSim() ? ", color=black,style=filled,fillcolor=blue" : "") << "];\n";
    } else {
      str << CleanNodeName(pNode->GetNameForNameable()) << " [label=\""
          << pNode->GetNameForNameable() << "\\n"
          << pNode->GetTypeTag() << "\", color=red, style=filled, fillcolor=red];\n";
    }
  }
  //
  //
  //		if(bWritePorts)
  //		{
  //
  //			// write all the ports
  //			std::list<std::string> liOutPort = pNode->GetOutPortNames();
  //			for( std::list<std::string>::const_iterator it = liOutPort.begin();
  //				it != liOutPort.end(); ++it)
  //			{
  //                // write something like mynode_value as node name (should
  //                // be unique, as node names are unique within a single graph
  //                std::string strPortName = CleanNodeName(pNode->GetNameForNameable()
  //                                                        + std::string("_")
  //                                                        + *it );
  //
  //                // and directly write an edge between the node and the port
  //                str << strPortName.c_str() << "
  //                [style=filled,fillcolor=green,shape=box,label=\""
  //                    << *it << "\\n("
  //                    << pNode->GetOutPort(*it)->GetTypeDescriptor()
  //                    << ")\\n"
  //                    << "outport"
  //                    << "\"];\n"
  //                    << CleanNodeName(pNode->GetNameForNameable())
  //					<< " -> "
  //						<< CleanNodeName(strPortName) << ";\n";
  //			}
  //			str << std::endl;
  //
  //			std::list<std::string> liInPort = pNode->GetInPortNames();
  //			for( std::list<std::string>::const_iterator tit = liInPort.begin();
  //				tit != liInPort.end(); ++tit)
  //			{
  //				if( pNode->GetInPort( *tit ) != NULL )
  //					continue; // only write unconnected ports here
  //                // write something like mynode_value as node name (should
  //                // be unique, as node names are unique within a single graph
  //                std::string strPortName = CleanNodeName(pNode->GetNameForNameable()
  //                                                        + std::string("_")
  //                                                        + *tit );
  //
  //                // and directly write an edge between the node and the port
  //                str << strPortName.c_str() << "
  //                [style=filled,color=yellow,fillcolor=yellow,shape=box,label=\""
  //                    << *tit << "\\n("
  //                    << typeid(pNode->GetPortTypeCompareFor(*tit)).name()
  //                    << ")\\n"
  //                    << "inport"
  //                    << "\"];\n"
  //					<< CleanNodeName(strPortName)
  //					<< " -> "
  //                    << CleanNodeName(pNode->GetNameForNameable()) << ";\n";
  //
  //			}
  //			str << std::endl;
  //
  //		}

  return true;
}
} // namespace

bool VdfnPersistence::SaveAsDot(VdfnGraph* pGraph, const std::string& strGraphDotFile,
    const std::string& sGraphName, bool bWritePorts, bool bWriteTypes, bool bWriteValues) {
  if (pGraph == NULL)
    return true;
  std::ofstream str(strGraphDotFile.c_str());
  if (str.good()) {
    std::string strGraphName = CleanNodeName(sGraphName); // copy const-string

    // ok, lets dance...
    str << "digraph " << strGraphName << std::endl << "{" << std::endl;

    const VdfnGraph::Nodes& nodes = pGraph->GetNodes();
    std::set<std::string>   strGroupTags;
    for (VdfnGraph::Nodes::const_iterator cit = nodes.begin(); cit != nodes.end(); ++cit) {
      std::string strGroupTag = (*cit)->GetGroupTag();
      if (!strGroupTag.empty())
        strGroupTags.insert(strGroupTag);

      SaveNode((*cit), str, bWritePorts, bWriteTypes, bWriteValues);
      str << "##\n";

      const VdfnGraph::Edges&                edges  = pGraph->GetEdges();
      const VdfnGraph::Edges::const_iterator edgeit = edges.find(*cit);
      if (edgeit != edges.end()) {
        const std::list<VdfnGraph::Connect>& liConnect = (*edgeit).second;

        for (std::list<VdfnGraph::Connect>::const_iterator t = liConnect.begin();
             t != liConnect.end(); ++t) {

          for (VdfnGraph::ConInfo::const_iterator nit = (*t).second.begin();
               nit != (*t).second.end(); ++nit) {
            std::string strFromPortName;
            (*cit)->GetNameForOutPort((*t).first, strFromPortName);

            std::string strToPortName = (*nit).second.first;

            if (bWritePorts || bWriteValues) {
              std::string strNodeName = CleanNodeName((*cit)->GetNameForNameable());
              //																	+
              //std::string("_")
              //																	+
              //strFromPortName);

              str << "\t" << strNodeName << ":O_" << CleanNodeName(strFromPortName) << " -> "
                  << CleanNodeName((*nit).first->GetNameForNameable()) << ":I_"
                  << CleanNodeName(strToPortName) << " [label=\"" << strToPortName << "\"];\n";
            } else {
              std::string strName = CleanNodeName((*cit)->GetNameForNameable());

              str << "\t" << strName << " -> " << CleanNodeName((*nit).first->GetNameForNameable())
                  << " [label=\"" << strFromPortName << " -> " << strToPortName << "\"]"
                  << ";\n";
            }
          }
        }
      }
      str << "##\n";
    }

    // look for groups

    str << "}\n";

    return true;
  }

  return false;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
