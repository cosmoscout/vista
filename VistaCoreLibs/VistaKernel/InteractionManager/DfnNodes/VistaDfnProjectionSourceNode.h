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

#ifndef _VISTADFNPROJECTIONSOURCENODE_H
#define _VISTADFNPROJECTIONSOURCENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnSerializer.h>
#include <map>

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaProjection;
class VistaDisplayManager;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Outputs the projection values of the registered projection.
 *
 * @ingroup VdfnNodes
 *
 * @outport{midpoint, VistaVector3D, the midpoint of the projection}
 * @outport{normal, VistaVector3D, the normal of the projection}
 * @outport{up, VistaVector3D, the up-vector of the projection}
 * @outport{left, double, the left extent of the projection}
 * @outport{right, double, the right extent of the projection}
 * @outport{top, double, the top extent of the projection}
 * @outport{bottom, double, the bottom extent of the projection}
 * @outport{near, double, the near extent of the projection}
 * @outport{far, double, the far extent of the projection}
 */
class VISTAKERNELAPI VistaDfnProjectionSourceNode : public IVdfnNode {
 public:
  VistaDfnProjectionSourceNode(VistaProjection* pWindow);
  ~VistaDfnProjectionSourceNode();

  virtual bool GetIsValid() const;

 protected:
  virtual bool DoEvalNode();

  virtual unsigned int CalcUpdateNeededScore() const;

 private:
  TVdfnPort<VistaVector3D>*m_pMidpoint, *m_pNormal, *m_pUp;
  TVdfnPort<double>*       m_pLeft, *m_pRight, *m_pTop, *m_pBottom, *m_pNear, *m_pFar;

  class ProjectionObserver;
  VistaProjection*    m_pProjection;
  ProjectionObserver* m_pObs;

  struct _sUpdate {
    _sUpdate()
        : m_nUpdateCount(0)
        , m_bNeedsUpdate(true) {
    }

    unsigned int m_nUpdateCount;
    bool         m_bNeedsUpdate;
  } * m_pUpd;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNPROJECTIONSOURCENODE_H
