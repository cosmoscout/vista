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


#ifndef _VISTADFNSENSORFRAMENODE_H
#define _VISTADFNSENSORFRAMENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <map>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
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
 * Transforms an object's position and rotation by other the input values
 * (translate, rotate, scale).
 * 
 * @ingroup VdfnNodes
 * @inport{pos_in, VistaVector3D, mandatory, position of an object}
 * @inport{ori_in, VistaQuaternion, mandatory, orientation of an object}
 * @inport{translation, VistaVector3D, optional, translation to apply}
 * @inport{rotation, VistaQuaternion, optional, rotation to apply}
 * @inport{scale, float, mandatory, scale to apply}
 * @inport{lc_ori, VistaQuaternion, optional, local orientation to apply}
 *
 * @outport{position, VistaVector3D, the far extent of the projection}  
 * @outport{orientation, VistaQuaternion, the far extent of the projection}  
 */
class VISTAKERNELAPI VistaDfnSensorFrameNode : public IVdfnNode
{
public:
	/** @todo no kernel dependencies -> move to DFN (or separate node lib) */
	VistaDfnSensorFrameNode();
	~VistaDfnSensorFrameNode();

	virtual bool GetIsValid() const;
	virtual bool PrepareEvaluationRun();

protected:
	virtual bool   DoEvalNode();
	virtual unsigned int    CalcUpdateNeededScore() const;

private:
	TVdfnPort<VistaVector3D>   *m_pPosIn,
								*m_pTranslation,
								*m_pTransPosOut;
	TVdfnPort<float>            *m_pScale;
	TVdfnPort<VistaQuaternion> *m_pOriIn,
								*m_pOrientation,
								*m_pLocalOri,
								*m_pTransOriOut;

	typedef std::map<IVdfnPort*,unsigned int> REFMAP;
	struct _sUpdate
	{
		_sUpdate()
			: m_nScore(0) {}

	   unsigned int                       m_nScore;
	   VistaTransformMatrix              m_mtPosTrans;
	   REFMAP m_mpRevision;
	} *m_pUpd;

};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNSENSORFRAMENODE_H

