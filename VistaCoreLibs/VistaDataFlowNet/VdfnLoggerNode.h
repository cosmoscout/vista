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


#ifndef _VDFNLOGGERNODE_H
#define _VDFNLOGGERNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VdfnShallowNode.h"
#include "VdfnPort.h"
#include "VdfnConfig.h"
#include <fstream>
#include <vector>
#include <list>
#include <map>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is an in-evaluation logging node that accepts only TVdfnPort<std::string>
 * type ports as an input source. It will write to a standard ofstream on any
 * evaluation that happens when the input has changed. It can be used to
 * create a table like output that can be read with a CSV reader or tools
 * like gnuplot. Note that it might be possible that the conversion from the inport
 * types has to take place outside but within the network itself, so this is a
 * time-consuming process as an addition to the writing of the data to the file.
 * On buffered file systems, the file writing might not be that expensive, while
 * the format conversion can be.
 *
 * @ingroup VdfnNodes
 * @inport{some\,depending on the inports set,string,optional,attach an inport of
           type string to write to the output log file when the port changes}
 */
class VISTADFNAPI VdfnLoggerNode : public VdfnShallowNode
{
public:

	/**
	 * Constructor, initializes the file handle that is used
	 * throughout the processing by opening a file with the
	 * name strLogFile.
	 * @param strLogFileName the log file to open, give an absolute
	          or relative path
     * @param bWriteHeader determines, whether a header line in written
	          during PrepareEvaluationRun(). The header line consists
			  of the in-port names as given using SetInPort()
     * @param bWriteTimeStamp determines whether a timestamp is prepended
	          to each record written to the log file. The timestamp is
			  the value returned be GetUpdateTimeStamp() which is set by
			  IVdfnNode::DoEvaluate()
     * @param bWriteTimeDiff determines whether a time difference between the
	          current time stamp and the last record write time stamp is prepended
			  to each record as a second attribute. Can be sometimes handy, when relative
			  times are needed (e.g., for gnuplot or timing measures).
			  Note that you can not write time diffs without writing the absolute timestamps,
			  which means that if bWriteTimeStamp is set to false no time diff will
			  be written.
     * @param liPorts the port-sort order to write the attributes in each record.
	          When empty, the attributes are sorted as determined by GetInPortNames() and
			  written to the log file, otherwise they are written in the order as given
			  by this list. If a port-name is in this list, but not connected, it will
			  appear in the log-file and its value will be constant "<not-set>" (or similar),
			  in order to keep the index and the order of each attribute as determined by the user
     * @param liTrigger determines which ports are relevant in order to trigger a record-write to
	          the log-file. If empty, a record will be written on any change of any input port,
			  otherwise, any input port given by name in the trigger list will be inspected.
			  The strategy is to keep a list of revision stamps that will be used for comparison,
			  and a record will be written on the first non-match of the stored revision and the
			  input port revision. This can be useful, if you want to record only on changes of one
			  or more input ports, e.g., equi-distant logging when using a time ticker as input, or
			  event based logging when listening on the state change of a button from a device.
	 */
	VdfnLoggerNode(const std::string &strPrefix,
		             bool bWriteHeader,
					 bool bWriteTimeStamp,
					 bool bWriteTimeDiff,
					 bool bLogToConsole,
					 const std::list<std::string> &liPorts,
					 const std::list<std::string> &liTrigger,
					 const std::string& cSep = " " );

	virtual ~VdfnLoggerNode();

	/**
	 * Overridden from CShallowNode::SetInPort(). It will check, whether the type of pPort
	 * is TVdfnPort<std::string> and only then add it to the inport map. It uses a
	 * TVdfnPortTypeCompare<TVdfnPort<std::string>*> to determine that, so derivatives should
	 * work, too.
	 * @param sName the name of the inport
	 * @param pPort the port to add, has to be of type TVdfnPort<std::string> (or derivative)
	 * @return true iff the port had the correct type, based on the evaluation of a port type compare,
	           false else
	 */
	virtual bool SetInPort(const std::string &sName,
						   IVdfnPort *pPort );

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	/**
	 * @return true if the ofstream is ok (could be opened) or at least a file name
	           was set during construction.
	 */
	bool GetIsValid() const;
	bool PrepareEvaluationRun();
	unsigned int CalcUpdateNeededScore() const;

protected:
	bool DoEvalNode();
	void WriteHeader();
	void PrepareFile();
private:
	TVdfnPortTypeCompare<TVdfnPort<std::string> >*m_pPrototype;
	bool m_bWriteHeader,
		m_bWriteTimeStamp,
		m_bWriteTimeDiff,
		m_bLogToConsole;
	double m_nLastUpdate;
	std::ofstream *m_ofstream;
	std::string m_strFileName,
		        m_strPrefix;
	std::vector<TVdfnPort<std::string>*> m_vecPorts;
	std::list<std::string> m_liInPorts,
		                   m_liTrigger;
	TVdfnPort<std::string> *m_pNotSet,
		                   *m_pFileName;

	typedef std::map<IVdfnPort*, unsigned int> REVMAP;

	mutable REVMAP m_mpRevisions;
	mutable unsigned int m_nUpdateScore;
	unsigned int m_nFileNameCnt;
	std::string m_sSep;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNLOGGINGNODE_H

