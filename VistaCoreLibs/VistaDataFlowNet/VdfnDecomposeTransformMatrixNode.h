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


#ifndef _VDFNDECOMPOSETRANSFORMMATRIXNODE_H
#define _VDFNDECOMPOSETRANSFORMMATRIXNODE_H

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
 * Disassembles a TransformMatrix and outputs its components to the outports
 *
 * @ingroup VdfnNodes
 * @inport{in,VistaTransformMatrix,mandatory, TransformMatrix to be decomposed }
 *
 * @outport{i_j,float, component (i,j) of the input matrix (i and j from 0 to 3 )}
 */
class VISTADFNAPI VdfnDecomposeTransformMatrixNode : public IVdfnNode
{
public:
	VdfnDecomposeTransformMatrixNode();
	virtual ~VdfnDecomposeTransformMatrixNode();
	
	bool PrepareEvaluationRun();

protected:
	virtual bool DoEvalNode();

private:
	std::vector< TVdfnPort< float >* >		m_vecOutPorts;
	TVdfnPort< VistaTransformMatrix >*		m_pIn;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNDECOMPOSETRANSFORMMATRIXNODE_H
