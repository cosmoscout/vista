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


#ifndef _VDFNPERSISTENCE_H
#define _VDFNPERSISTENCE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include <string>
#include <list>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VdfnGraph;
class IVdfnNode;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Persist an un-persistent graph for the DFN API.
 * These are merely utility functions that allow to read xml graphs using
 * tinyXML (as part of the VistaTools). The graphs can be saved as dot files,
 * so you can visualize them using graphviz / dotty to inspect your creation.
 * Graphs can be read from files and from strings in memory.
 */
class VISTADFNAPI VdfnPersistence
{
public:
	/**
	 * Loads a graph from file.
	 * @param strGraphXmlFile an absolute or relative path to the graph xml file
	                    to load.
     * @param strGraphTag this tag is passed to the nodes as "tag" value, it can be used
                    to create quick hacks, for example set a node in/out-value to the
                    tag as given here. Mainly used by ViSTA in cluster mode currently.
                    The node creation method is passed a VistaPropertyList with params, one
                    of them being strGraphTag the value of "tag"
                    If you are unsure what to pass here, pass the empty string.
     * @param bEvaluateGraph a graph needs an initial VdfnGraph::Evaluate(0) call in order
                    to function properly. Pass true here, if you want to have that call
                    done at the end of loading. Sometimes, a few parts might be adjusted
                    after loading and before a first call to Evaluate(), if you have a
                    case like this: pass false here but do not forget to call the
                    VdfnGraph::Evaluate(0) after you did what needed to be done.
     * @param bStrict the strict flag can be set to true in order to cause NULL to be returned
                    on encounter of an error. false can lead to return incomplete graphs.
     * @return NULL iff the file was not found or somehow corrupt a pointer to a VdfnGraph
				    else. However, the graph may not be as expected, then it is a 'partial' graph.
				    For example, if the content of
                    the file are not set correctly (for example wrong edges). In the
                    latter case there currently is only an output to stderr with the
                    tinyXML error message.
	 */
	static VdfnGraph *LoadGraph( const std::string &strGraphXmlFile,
								  const std::string &strGraphTag,
								  bool bEvaluateGraph = true,
								  bool bStrict = false,
								  const std::string &strParameterFileName = "" );
	/**
	 * Same as LoadGraph(), but parses the graph from the string given in strGraphText.
	 * @see LoadGraph()
	 * @param strGraphText the full XML graph as a string to parse
	 * @param strGraphTag see LoadGraph()
	 * @param bEvaluateGraph see LoadGraph()
	 * @param bStrict see LoadGraph()
	 */
	static VdfnGraph *ParseGraph( const std::string &strGraphText,
			                       const std::string &strGraphTag,
			                       bool bEvaluateGraph = true,
			                       bool bStrict = false);

	/**
	 * not implemented, yet. Ignore.
	 * @return false always.
	 * @todo implement!
	 */
	//static bool SaveGraph( VdfnGraph *pGraph, const std::string &strGraphXmlFile );
	//static bool SaveAsXml( VdfnGraph *pGraph, const std::string &strGraphXmlFile );

	/**
	 * Saves a VdfnGraph as dot file to be inspected with dotty / graphviz, for example
	 * for documentation or debugging.
	 * @param pGraph a pointer to the graph to dump
	 * @param strGraphDotFile the filename to write (relative or absolute)
	 * @param strGraphName the name of the graph in the dotty file. Should be
	                       'dotty-clean', which means no "." or other special
	                       non-ansi characters and whitespace.
	 * @param bWritePorts if set to true, ports are explicitly written as nodes.
	                     Note that only connected in-ports are written, and usually
	                     all out-ports.
	 * @param bWriteTypes set to true when you want to dump the type names to the dot as well.
	          but beware: the type names are RTTI types, so they might not be readable and long,
	          cluttering up the graph drawing.
	 * @param bWriteValues set to true when you want to also write the values of each port
	          to the graph, which is helpful to debug the current state of the DFN.
			  This automatically enables Writing of Types (even if set to false)
	 * @return true iff the file could be written, false else
	 */
	static bool SaveAsDot( VdfnGraph *pGraph,
		                   const std::string &strGraphDotFile,
						   const std::string &strGraphName,
						   bool bWritePorts,
						   bool bWriteTypes = false,
						   bool bWriteValues = false );

	/**
	 * the method tries to dump an unparsed XML as DOT, in order to find logical errors
	 * in the network definition. As some parts of the graph are constructed dynamically,
	 * it not worthwile to take a look at the resulting dot, but check the logic graph.
	 * However, this API is not yet finished.
	 * @todo experimental API, to be finished some day.
	 */
	static bool DumpXmlAsDot( const std::string &strGraphXmlFile,
			                  const std::string &strGraphDotFile,
			                  const std::string &strGraphName );
protected:
private:
	VdfnPersistence() {}
	~VdfnPersistence() {}
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNPERSISTENCE_H

