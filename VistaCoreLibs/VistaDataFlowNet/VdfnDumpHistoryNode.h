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


#ifndef _VDFNDUMPHISTORYNODE_H
#define _VDFNDUMPHISTORYNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <iostream>

#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"
#include "VdfnHistoryPort.h"

#include <fstream>

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
 * class that can be used for debugging, it dumps the history with context
 * information to the file specified during construction. In case you use this
 * in a cluster, provide a unique file name for each client.
 *
 * @ingroup VdfnNodes
 * @inport{history,History,mandatory,inport single argument\, provide the history
                                     to dump on update}
 */
class VISTADFNAPI VdfnDumpHistoryNode : public IVdfnNode
{
public:
	VdfnDumpHistoryNode(const std::string &strFileName);
	bool PrepareEvaluationRun();
protected:
	bool DoEvalNode();
private:
	HistoryPort    *m_pHistory;
	std::ofstream   m_ofstream;
};


class VISTADFNAPI VdfnDumpHistoryDefaultCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VdfnDumpHistoryDefaultCreate();
	IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNCOUNTERNODE_H

