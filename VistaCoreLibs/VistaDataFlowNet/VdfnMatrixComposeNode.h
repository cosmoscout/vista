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

#ifndef _VDFNMATRIXCOMPOSENODE_H
#define _VDFNMATRIXCOMPOSENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"
#include "VdfnNodeFactory.h"

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
 * VdfnMatrixComposeNode
 * Has 4 inports for translation, rotation, scale and scale rotation,
 * and invokes the VistaMatrixDeComposer to compute a transformation
 * matrix inside a DFN graph.
 * @todo use doxygen macros to document ports
 *
 * @ingroup VdfnNodes
 * @inport{translation,VistaVector3D,optional, translation amount }
 * @inport{orientation,VistaQuaternion,optional, rotation arount 0\,0\,1 }
 * @inport{scale,VistaVector3D,optional, scale amount }
 * @inport{scale orientation,VistaQuaternion,optional, scale orientation }
 *
 * @outport{out, VistaTransformMatrix, The composed matrix}
 */
class VISTADFNAPI VdfnMatrixComposeNode : public IVdfnNode
{
public:
	
	VdfnMatrixComposeNode();
	virtual ~VdfnMatrixComposeNode();

	virtual bool GetIsValid() const;
	virtual bool PrepareEvaluationRun();

	static const std::string SOutputMatrixOutPortName;

	static const std::string STranslationInPortName;
	static const std::string SOrientationInPortName;
	static const std::string SScaleInPortName;
	static const std::string SScaleOrientationInPortName;

protected:
	virtual bool DoEvalNode();

private:
	void RegisterInPrototypes();

	TVdfnPort<VistaVector3D>        *m_pInTranslation;
	TVdfnPort<VistaQuaternion>      *m_pInOrientation;
	TVdfnPort<VistaVector3D>        *m_pInScale;
	TVdfnPort<VistaQuaternion>      *m_pInScaleOrientation;
	
	TVdfnPort<VistaTransformMatrix> *m_pOutputMatrix;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //_VDFNMATRIXCOMPOSENODE_H
