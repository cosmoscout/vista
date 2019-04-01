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


#ifndef _VDFNHISTORYPROJECTNODE_H
#define _VDFNHISTORYPROJECTNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnReEvalNode.h"
#include "VdfnPortFactory.h"
#include "VdfnHistoryPort.h"

#include <map>
#include <list>
#include <string>
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
 * The history project node is used to collect a number of columns of properties
 * from a history, for the current (lazy sampling), most current (hot sampling),
 * or all unprocessed (history sampling) rows in the device history.
 * The columns are defined during construction time, by a list of symbols, e.g.,
 * names, for the columns to be named as. The node checks whether the history
 * contains properties with that name and adds outgoing ports with a proper type
 * for this node. The type is derived from the settings in the VdfnPortFactory.
 * If a property was not found in the history, no outport is created. This may
 * lead to incomplete graphs. The outports are created every time the history
 * inport is assigned during SetInPort(). Another inport switched between lazy,
 * hot and history sampling, but this is optional (lazy is default).
 * When a property is an indexed get property, the project node creates a number
 * of outports that do not correspond 1:1 to property names, but with a suffix,
 * showing the index. For example DSCALAR for 0-2 indices is expanded to
 * DSCALAR_0, DSCALAR_1, DSCALAR_2. The driver transcoder property is still DSCALAR.
 *
 * @ingroup VdfnNodes
 * @inport{history,VdfnHistoryPortData*,mandatory,the incoming history to process}
 * @inport{sampling_mode,int,optional,0 for lazy (default)\, 1 for hot sampling\, 2 for history sampling}
 * @outport{some\,depending on history,some\,depending on history,outputs will be
            dynamically created\,depending on the inport history and projection mode}
 *
 * @see ReEvalNode
 */
class VISTADFNAPI VdfnHistoryProjectNode : public IVdfnReEvalNode
{
public:
	enum eMode
	{
		MD_LAZY = 0,
		MD_HOT,
		MD_ITERATE,
		MD_INDEXED
	};
	/**
	 * @param liOutPorts the names of the properties to have as outports for this
	          project node. The order is not important.
	 */
	VdfnHistoryProjectNode(const std::list<std::string> &liOutPorts, 
			                   eMode eInitialMode = MD_LAZY );

	/**
	 * @todo check whether this code is legacy.
	 */
	virtual bool SetInPort(const std::string &sName, IVdfnPort*);


	/**
	 * The history project node is valid, as soon as a history is connected to it.
	 * @return true iff the port "history" is properly set.
	 */
	virtual bool GetIsValid() const;
//	virtual bool PrepareEvaluationRun();

	/**
	 * Since the history processing mode may need multiple evaluation runs per
	 * frame, it is a ReEvalNode, and thus needs to specify whether or not
	 * it requires another evaluation run.
	 * @see ReEvalNode
	 */
	virtual bool GetNeedsReEvaluation() const;

protected:
	virtual bool DoEvalNode();
	virtual unsigned int CalcUpdateNeededScore() const override;
private:
	/**
	 * pre-condition: history port set
	 */
	void UpdateOutPortMap();


	struct _sSetHlp
	{
		_sSetHlp()
			: m_pFunctor(NULL),
			m_nIndex(~0) {}

		_sSetHlp( VdfnPortFactory::CPortSetFunctor *pFunctor,
				  unsigned int nIndex )
				  : m_pFunctor(pFunctor),
					m_nIndex(nIndex)
		{
		}

		VdfnPortFactory::CPortSetFunctor *m_pFunctor;
		unsigned int                       m_nIndex;
	};

	std::map<IVdfnPort*, _sSetHlp> m_mpPortSetter;
	struct _sPortMap
	{
		_sPortMap() {}
		_sPortMap(const std::string &sFrom, const std::string &sTo)
			: m_strFrom(sFrom),
			m_strTo(sTo) {}

		std::string m_strFrom,
					m_strTo;
	};

	std::list< _sPortMap > m_liOutPorts;
	std::list<std::string> m_liOriginalPorts;
	HistoryPort    *m_pHistory;
	TVdfnPort<int> *m_pSamplingMode;
	TVdfnPort<unsigned int> *m_pIterateState,
							  *m_pBackwardIndex,
							  *m_pOutputIndex;
	unsigned int	m_nUnprocessedMeasures;
	eMode m_eInitialMode;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNHISTORYPROJECTNODE_H
