/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*            Copyright (c) 2016-2019 German Aerospace Center (DLR)           */
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

#ifndef _VISTAINTENTIONSELECT_H
#define _VISTAINTENTIONSELECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaUtilityMacros.h>
#include <VistaBase/VistaVectorMath.h>
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaMath/VistaGeometries.h>
#include <queue>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNode;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*
 * IVistaIntentionSelectAdapter is a base adapter class for all objects which should be
 * registered at the IntentionSelect mechanism. All these objects need is a position.
 *
 *
 * You should make sure the object's positions and the IntentionSelect cone are in the same
 * reference frame.
 */
class VISTAKERNELAPI IVistaIntentionSelectAdapter {
 public:
  virtual ~IVistaIntentionSelectAdapter();
  IVistaIntentionSelectAdapter();
  virtual bool GetPosition(
      VistaVector3D& pTrans, const VistaReferenceFrame& oReferenceFrame) const = 0;
  /**
   * The contribution multiplier can be used to adjust the priority of individual
   * adapters. By default, this method returns 1.0. Usually, only a slight increase,
   * e.g., to 1.05, already boosts the priority of a single adapters.
   */
  virtual float GetContributionMultiplier() const;
  void          SetIsSelectionEnabled(bool bIsEnabled);
  bool          GetIsSelectionEnabled() const;
  float         GetScore() const;

 private:
  friend class VistaIntentionSelect;
  void  SetScore(float fScore);
  bool  m_bSelectionEnabled;
  float m_fScore;
};

/*
 * VistaIntentionSelectLineAdapter is a base adapter class for all line-objects which should be
 * registered at the IntentionSelect mechanism. All these objects need are a starting and an end
 * position.
 *
 *
 * You should make sure the object's positions and the IntentionSelect cone are in the same
 * reference frame.
 */
class VISTAKERNELAPI IVistaIntentionSelectLineAdapter : public IVistaIntentionSelectAdapter {
 public:
  virtual ~IVistaIntentionSelectLineAdapter() {
  }
  IVistaIntentionSelectLineAdapter()
      : IVistaIntentionSelectAdapter() {
  }

  virtual bool GetPosition(VistaVector3D& pTrans, const VistaReferenceFrame& oReferenceFrame) const;
  virtual bool GetStartPosition(VistaVector3D& pTrans) const = 0;
  virtual bool GetEndPosition(VistaVector3D& pTrans) const   = 0;
};

// @todo: add support for circle, etc,. handles. some math on how a circle handle could be realized
// can be found here: http://www.geometrictools.com/Documentation/DistanceLine3Circle3.pdf

/*
 * Specific adapter implementation for IVistaNode.
 *
 */
class VISTAKERNELAPI VistaNodeAdapter : public IVistaIntentionSelectAdapter {
 public:
  VistaNodeAdapter()
      : IVistaIntentionSelectAdapter()
      , m_pNode(NULL) {
  }

  VistaNodeAdapter(IVistaNode* pNode)
      : IVistaIntentionSelectAdapter()
      , m_pNode(pNode) {
  }

  virtual bool GetPosition(VistaVector3D& pTrans, const VistaReferenceFrame& oReferenceFrame) const;

  IVistaNode* GetNode() const;

 private:
  IVistaNode* m_pNode;
};

/*
 * Specific adapter implementation for IVistaNode which performs bounding box
 * tests in addition. To make this adapter work properly, the ScalingDistance
 * of the IntentionSelect has to be set to a value > 0.
 *
 */
class VISTAKERNELAPI VistaBoundingBoxAdapter : public VistaNodeAdapter {
 public:
  VistaBoundingBoxAdapter()
      : VistaNodeAdapter() {
  }

  VistaBoundingBoxAdapter(IVistaNode* pNode)
      : VistaNodeAdapter(pNode) {
  }

  virtual bool GetPosition(VistaVector3D& pTrans, const VistaReferenceFrame& oReferenceFrame) const;
};

/*
 * IntenSelect mechanism. For more information see the original publication.
 * Gerwin de Haan and Michal Koutek and Frits H. Post
 * IntenSelect: Using Dynamic Object Rating for Assisting 3D Object Selection
 * Proceedings of the 9th Internationial Immersive Projection Technology Workshop
 * (IPT) and 11th Eurographics Workshop on Virtual Environments (EGVE)
 */
class VISTAKERNELAPI VistaIntentionSelect {
 private:
 public:
  /**
   * Sets the snappiness and stickyness to 0.5 each.
   */
  VistaIntentionSelect();
  ~VistaIntentionSelect();

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // SELECTION VOLUME CONTROL
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /**
   * The search cone to use for the scoring. Has to be defined in a proper reference
   * frame, e.g., world space for every query.
   * The selection cone can be set any time and is used for the next call to
   * Update().
   */
  void          SetSelectionVolume(const VistaEvenCone& cone);
  VistaEvenCone GetSelectionVolume() const;

  /**
   * Determines the world transformation of the selection volume.
   * @param v3Pos the origin position of the selection cone
                                  in the same coordinate frame as the registered
   IVistaIntentionSelectAdapters
   * @param qOri  the rotation around the origin position
                                  in the same coordinate frame as the registered
   IVistaIntentionSelectAdapters
   */
  void SetConeTransform(const VistaVector3D& v3Pos, const VistaQuaternion& qOri);

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // ITEM MANAGEMENT
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  void RegisterNode(IVistaIntentionSelectAdapter* pNode);
  bool UnregisterNode(IVistaIntentionSelectAdapter* pNode);

  void ClearAllNodes();

  int                           GetNodeCount() const;
  IVistaIntentionSelectAdapter* GetNodeByIndex(int nIndex) const;
  VISTA_DEPRECATED float        GetNodeScore(IVistaIntentionSelectAdapter* pNode) const;

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // QUERY METHODS
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /**
   * The most interesting method for users. Call in order to query the current
   * scoring order on all nodes that are registered with this IntenSelect instance.
   * vecObj is assumed to be empty. The first node in the NDQUEUE is the node with
   * the most score, e.g., the one pointed at by the user. This node is followed by
   * a number of other nodes in descending positive score. Nodes with score 0 are not
   * in the resulting vector anymore.
   */
  void Update(std::vector<IVistaIntentionSelectAdapter*>& vecObj);

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // SCORING PARAMETERS
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  /**
   * Determines the amount of influence of a past scoring to a current
   * scoring. A high value gives more importance to a past scoring,
   * resulting in a kind of 'the focus will stick' to the object.
   * A low value indicates that old values are not as important, resulting
   * in a more frequent change of the focus.
   * A call to this method cleans all old scores (if any)
   * @param nStickyness in the range of [0,1],
                    - 0: forget old scores during the scoring
                    - 1: use mainly old scores for the scoring
   */
  void  SetStickyness(float nStickyness);
  float GetStickyness() const;

  /**
   * Determines the amount of influence of a new scoring in comparison
   * to old scores. A high value (1.0) indicates that the focus should
   * move rather directly, resulting in a fast 'snap' from one object
   * to the other, while low values (0.0) indicate that the natural
   * decrease of the score over time should be used mainly.
   * A call to this method cleans all old scores (if any)
   * @param nSnappiness [0,1]
                    - 0: do not snap by little changes in the current score
                    - 1: do snap as fast as possible
   */
  void  SetSnappiness(float nSnappiness);
  float GetSnappiness() const;

  /**
   * Determines the amount of influence of distance to the scoring of
   * a point. The scoring will exponentially decay and will be reduced
   * to 1/e of its value when the given distance is reached.
   * A call to this method cleans all old scores (if any)
   * @param nScalingDistance should be larger or equal to zero.
                    - 0: disable distance attenuation
                    - Small values: fast distance attenuation
                    - Large values: slow distance attenuation
   */
  void  SetScalingDistance(float nScalingDistance);
  float GetScalingDistance() const;

 protected:
 private:
  void CleanScores();

  float CalculatePointContribution(const VistaVector3D& v3Point);
  float CalculateContribution(float fPerp, float fProj);

  VistaEvenCone m_oCone;

  typedef std::vector<IVistaIntentionSelectAdapter*> AdapterVector;
  AdapterVector                                      m_vecScores;

  VistaReferenceFrame m_ConeRef;
  float               m_nStickyness, m_nSnappiness, m_nScalingDistance;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAINTENTIONSELECT_H
