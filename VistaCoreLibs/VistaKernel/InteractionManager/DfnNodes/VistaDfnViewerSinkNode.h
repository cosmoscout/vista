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

#ifndef _VISTADFNVIEWERSINKNODE_H
#define _VISTADFNVIEWERSINKNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaVectorMath.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <map>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaDisplaySystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Sets the virtual viewer position and orientation, can as well be used to
 * change the left and right eye offset.
 *
 * @ingroup VdfnNodes
 * @inport{position, VistaVector3D, optional if orientation is set, the world position of the
 * viewer}
 * @inport{orientation, VistaQuaternion, optional if position is set, the world orientation of the
 * viewer}
 * @inport{left_eye, VistaVector3D, optional, left eye offset value in the display system}
 * @inport{right_eye, VistaVector3D, optional, right eye offset value in the display system}
 */
class VISTAKERNELAPI VistaDfnViewerSinkNode : public IVdfnNode {
 public:
  VistaDfnViewerSinkNode(VistaDisplaySystem* pSys);
  ~VistaDfnViewerSinkNode();

  virtual bool GetIsValid() const;
  virtual bool PrepareEvaluationRun();

  VistaDisplaySystem* GetDisplaySystem() const;
  void                SetDisplaySystem(VistaDisplaySystem*);

 protected:
  virtual bool DoEvalNode();

 private:
  TVdfnPort<VistaVector3D>*   m_pPosition, *m_pLeftEye, *m_pRightEye;
  TVdfnPort<VistaQuaternion>* m_pOrientation;

  VistaDisplaySystem* m_pSystem;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNWINDOWNODE_H
