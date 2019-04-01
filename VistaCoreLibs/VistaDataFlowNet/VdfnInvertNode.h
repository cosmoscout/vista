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


#ifndef _VDFNINVERTNODE_H
#define _VDFNINVERTNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnSerializer.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"


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
 * code template to define an invert operation. define a spacialized template
 * method TVdfnInvertNode<>::DoEvalNode() with your desired type and do not
 * forget to export it properly if you chose to write one staying in the DLL.
 * See TVdfnInvertNode.cpp for further inspiration.
 *
 * @ingroup VdfnNodes
 * @inport{in,type T,mandatory,the value to invert using this node}
 * @outport{out,type T, the in^-1 (hopefully)}
 */
template<class T>
class TVdfnInvertNode : public IVdfnNode
{
public:
	TVdfnInvertNode()
	: IVdfnNode(),
	  m_pIn(NULL),
	  m_pOut( new TVdfnPort<T> )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<T> >);
		RegisterOutPort( "out", m_pOut );
	}

	bool PrepareEvaluationRun()
	{
		m_pIn = dynamic_cast<TVdfnPort<T>*>(GetInPort("in"));
		return GetIsValid();
	}
protected:
	/**
	 * note that the method remains unimplemented for the template. For
	 * user defined types, specialize this template and define this function.
	 * @return true usually
	 */
	bool DoEvalNode();
private:
	TVdfnPort<T> *m_pIn,
	             *m_pOut;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VDFNINVERTNODE_H

