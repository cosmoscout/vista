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

#ifndef _VISTADFN3DMOUSETRANSFORMNODE_H
#define _VISTADFN3DMOUSETRANSFORMNODE_H

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

class VistaViewport;
class VistaProjection;
class VistaDisplayManager;
class VistaDisplaySystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 *  Transforms x,y coordinates from window / pixel space into 3D coordinates
 *  that can be used for pointing on 3D objects.
 *
 *
 *
 * @ingroup VdfnNodes
 * @inport{x_pos, int, yes, Mouse X screen coordinate}
 * @inport{y_pos, int, yes, Mouse Y screen coordinate}
 * @outport{position, VistaVector3D, the (viewer) position, or the origin of the pointing ray.}
 * @outport{orientation, VistaQuaternion, the 3D ray's orientation}
 */
class VISTAKERNELAPI VistaVdfn3DMouseTransformNode : public IVdfnNode {
 public:
  VistaVdfn3DMouseTransformNode(
      VistaDisplaySystem* pSystem, VistaViewport* pViewport, float fOriginOffsetAlongRay);

  ~VistaVdfn3DMouseTransformNode();

  bool GetTransformPositionFromFrame() const;
  void SetTransformPositionFromFrame(bool bDo);

  float GetOriginOffsetAlongRayFrame() const;
  void  SetOriginOffsetAlongRayFrame(const float fRayOffset);

  virtual bool GetIsMasterSim() const {
    return true;
  }

  bool PrepareEvaluationRun();

 protected:
  virtual bool DoEvalNode();

 private:
  unsigned int CalcUpdateNeededScore() const;

  void UpdateOnProjectionChange();
  void UpdateOnDisplaySystemChange();
  void UpdateOnViewportChange();
  void UpdateOnPlatformChange();

  TVdfnPort<VistaVector3D>*   m_pPosition;
  TVdfnPort<VistaQuaternion>* m_pOrientation;
  TVdfnPort<int>*             m_pX, *m_pY;
  friend class TransObserver;
  class TransObserver;

  TransObserver* m_pObserver;

  VistaViewport*      m_pViewport;
  VistaProjection*    m_pProjection;
  VistaDisplaySystem* m_pSystem;

  mutable unsigned int m_nUpdateCount;
  mutable bool         m_bNeedsUpdate;

  // cached some values
  VistaVector3D m_v3Normal, m_v3Up, m_v3Midpoint, m_v3RightVector, m_v3ViewerPos;
  double        m_nLeft, m_nRight, m_nBottom, m_nTop;
  int           m_nVpW, m_nVpH;
  bool          m_bTransFromFrame;
  float         m_fOriginOffsetAlongRay;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFN3DMOUSETRANSFORMNODE_H
