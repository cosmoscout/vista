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


#ifndef _VDFNCOMPOSEQUATERNIONFROMDIRECTIONSNODE
#define _VDFNCOMPOSEQUATERNIONFROMDIRECTIONSNODE

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnNode.h"
#include "VdfnPort.h"

#include "VdfnSerializer.h"

#include <VistaBase/VistaVectorMath.h>

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
class VISTADFNAPI VdfnComposeQuaternionFromDirectionsNode : public IVdfnNode
{
public:
	VdfnComposeQuaternionFromDirectionsNode();
	virtual ~VdfnComposeQuaternionFromDirectionsNode();

	bool PrepareEvaluationRun();

protected:
	virtual bool DoEvalNode();

private:
	TVdfnPort<VistaVector3D>*		m_pInFrom;
	TVdfnPort<VistaVector3D>*		m_pInTo;
	TVdfnPort<VistaQuaternion>*		m_pOut;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif 
