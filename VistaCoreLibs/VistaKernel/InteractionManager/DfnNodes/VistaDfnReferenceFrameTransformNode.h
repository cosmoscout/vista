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

#ifndef _VISTADFNREFERENCEFRAMETRANSFORMNODE_H
#define _VISTADFNREFERENCEFRAMETRANSFORMNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaReferenceFrame;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Node that takes a configurable amount of in-positions, in-orientations and in-transformations
 * and transforms them into or out of a selected reference frame.

 * @ingroup VdfnNodes
 * @inport{in_position + idx, VistaVector3D, optional, the in-position with index idx to transform
 to out_position + idx}
 * @inport{in_orientation + idx, VistaQuaternion, optional, the in-orientation with index idx to
 transform to out_orientation + idx}
 * @inport{in_matrix + idx, VistaTransformMatrix, optional, the in-transformation with index idx to
 transform to out_transformation + idx}
 * @outport{out_position + idx, VistaVector3D, the in-position with index idx transformed from
 in_position + idx}
 * @outport{out_oriantation + idx, VistaVector3D, the in-orientation with index idx transformed from
 in_orientation + idx}
 * @outport{out_matrix + idx, VistaVector3D, the in-transformation with index idx transformed from
 in_matrix + idx}

 */
class VISTAKERNELAPI VistaDfnReferenceFrameTransformNode : public IVdfnNode {
 public:
  VistaDfnReferenceFrameTransformNode(VistaReferenceFrame* pFrame, bool bTransformToFrame,
      int iNrPositionPorts, int iNrOrientationPorts, int iNrMatrixPorts);
  ~VistaDfnReferenceFrameTransformNode();

  virtual bool GetIsValid() const;

  bool PrepareEvaluationRun();

 protected:
  virtual bool DoEvalNode();

 private:
  VistaReferenceFrame* m_pFrame;
  bool                 m_bTransformFromFrame;

  std::vector<TVdfnPort<VistaVector3D>*>        m_vecInPortsPos;
  std::vector<TVdfnPort<VistaVector3D>*>        m_vecOutPortsPos;
  std::vector<TVdfnPort<VistaQuaternion>*>      m_vecInPortsOri;
  std::vector<TVdfnPort<VistaQuaternion>*>      m_vecOutPortsOri;
  std::vector<TVdfnPort<VistaTransformMatrix>*> m_vecInPortsMat;
  std::vector<TVdfnPort<VistaTransformMatrix>*> m_vecOutPortsMat;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNREFERENCEFRAMETRANSFORMNODE_H
