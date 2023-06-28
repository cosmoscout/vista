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

#ifndef CVDFNPLANECONSTRAINTNODE_H_
#define CVDFNPLANECONSTRAINTNODE_H_

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaBase/VistaVectorMath.h>

#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>

class IVistaDriverForceFeedbackAspect;
class VistaPlaneConstraint;

class VdfnPlaneConstraintNode : public IVdfnNode {
 public:
  VdfnPlaneConstraintNode(IVistaDriverForceFeedbackAspect* pAsp);
  virtual ~VdfnPlaneConstraintNode();

  bool GetIsValid() const;

  bool PrepareEvaluationRun();

 protected:
  bool DoEvalNode();

 private:
  IVistaDriverForceFeedbackAspect* m_pForceFeedback;
  TVdfnPort<VistaVector3D>*m_pOrigin, *m_pNormal, *m_pInternalForce, *m_pOriginOut, *m_pNormalOut;
  TVdfnPort<VistaTransformMatrix>* m_pToDriver;
  TVdfnPort<float>*                m_pStiffness;
  TVdfnPort<float>*                m_pDamping;
  VistaPlaneConstraint*            m_pPlaneConstraint;
};

/*============================================================================*/
/*								C R E A T O R
 */
/*============================================================================*/

class VdfnPlaneConstraintNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VdfnPlaneConstraintNodeCreate(VistaDriverMap* pMap);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaDriverMap* m_pMap;
};

#endif /* CVDFNPLANECONSTRAINTNODE_H_ */
