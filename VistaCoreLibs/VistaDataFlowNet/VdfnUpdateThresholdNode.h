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


#ifndef _VDFNUPDATETHRESHOLDNODE_H
#define _VDFNUPDATETHRESHOLDNODE_H



/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnNode.h"
#include "VdfnPort.h"

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
 *
 * @ingroup VdfnNodes
 */
class VISTADFNAPI VdfnUpdateThresholdNode : public IVdfnNode
{
public:
	VdfnUpdateThresholdNode( double dThreshold );
	virtual ~VdfnUpdateThresholdNode();

	bool PrepareEvaluationRun();

protected:
	virtual bool DoEvalNode();

private:

	double m_dSum;
	double m_dThreshold;

	TVdfnPort<double> *m_pIn;
	TVdfnPort<double> *m_pOut;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNTIMERNODE_H
