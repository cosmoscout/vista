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


#ifndef _VDFNCOMPOSETRANSFORMMATRIXNODE
#define _VDFNCOMPOSETRANSFORMMATRIXNODE

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
 * VdfnDecomposeTransformMatrixNode
 * Assembles a TransformMatrix from three or four floats
 *
 * @ingroup VdfnNodes
 * @inport{i_j,float,mandatory, components of the  }
 * @inport{3_j,float,optional, components of the last row, defaults to 0, 0, 0, 1}
 *
 * @outport{out,VistaTransformMatrix, assembled matrix}
 */
class VISTADFNAPI VdfnComposeTransformMatrixNode : public IVdfnNode
{
public:
	VdfnComposeTransformMatrixNode();
	virtual ~VdfnComposeTransformMatrixNode();

	bool GetIsValid() const;

	bool PrepareEvaluationRun();

protected:
	virtual bool DoEvalNode();

private:
	TVdfnPort< float >*						m_a4x4pInPorts[4][4];
	TVdfnPort< VistaTransformMatrix >*		m_pOut;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNCOMPOSETRANSFORMMATRIXNODE
