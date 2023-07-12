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

#include "VistaIntentionSelect.h"

#include <VistaKernel/GraphicsManager/VistaNodeInterface.h>
#include <algorithm>
#include <functional>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

IVistaIntentionSelectAdapter::IVistaIntentionSelectAdapter()
    : m_bSelectionEnabled(true)
    , m_fScore(0.f) {
}

IVistaIntentionSelectAdapter::~IVistaIntentionSelectAdapter() {
}

float IVistaIntentionSelectAdapter::GetContributionMultiplier() const {
  return 1.f;
}

void IVistaIntentionSelectAdapter::SetIsSelectionEnabled(bool bIsEnabled) {
  m_bSelectionEnabled = bIsEnabled;
}

bool IVistaIntentionSelectAdapter::GetIsSelectionEnabled() const {
  return m_bSelectionEnabled;
}

float IVistaIntentionSelectAdapter::GetScore() const {
  return m_fScore;
}

void IVistaIntentionSelectAdapter::SetScore(float fScore) {
  m_fScore = fScore;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool IVistaIntentionSelectLineAdapter::GetPosition(
    VistaVector3D& pTrans, const VistaReferenceFrame& oReferenceFrame) const {
  // find the closest point between the pick ray and the line defined by point 1 and 2
  // and return it as result

  VistaVector3D v3Point1, v3Point2;

  GetStartPosition(v3Point1);
  GetEndPosition(v3Point2);

  // work in the reference frame of the pick ray
  VistaVector3D v3Point1Transformed(oReferenceFrame.TransformPositionToFrame(v3Point1));
  VistaVector3D v3Point2Transformed(oReferenceFrame.TransformPositionToFrame(v3Point2));

  VistaVector3D v3U1(0.f, 0.f, -1.f);
  VistaVector3D v3U2(v3Point2Transformed - v3Point1Transformed);
  VistaVector3D v3M = v3U2.Cross(v3U1);
  float         fM2 = v3M.Dot(v3M);
  VistaVector3D v3R = v3Point1Transformed.Cross(v3M) / fM2;
  float         fT  = v3R.Dot(v3U1);

  // we want to stay within the line defined by point 1 and 2
  fT = std::min(std::max(fT, .0f), 1.f);

  VistaVector3D v3ClosestPoint(v3Point1Transformed + fT * v3U2);

  pTrans = oReferenceFrame.TransformPositionFromFrame(v3ClosestPoint);

  return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool VistaNodeAdapter::GetPosition(
    VistaVector3D& pTrans, const VistaReferenceFrame& oReferenceFrame) const {
  return m_pNode->GetWorldPosition(pTrans);
}

IVistaNode* VistaNodeAdapter::GetNode() const {
  return m_pNode;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool VistaBoundingBoxAdapter::GetPosition(
    VistaVector3D& pTrans, const VistaReferenceFrame& oReferenceFrame) const {
  // return the intersection point of ray and the local bounding box
  // if no intersection is found, return the object's center

  // the calculation is done in object space

  VistaTransformMatrix matObjectToWorldTransform;
  if (!GetNode()->GetParentWorldTransform(matObjectToWorldTransform)) {
    return false;
  }

  VistaTransformMatrix matWorldToObjectTransform = matObjectToWorldTransform.GetInverted();

  VistaVector3D start(0, 0, 0);
  VistaVector3D end(0, 0, -1);

  // transform start and end point from ray space to object space
  start = matWorldToObjectTransform * oReferenceFrame.TransformPositionFromFrame(start);
  end   = matWorldToObjectTransform * oReferenceFrame.TransformPositionFromFrame(end);

  // calculate actual intersection
  VistaVector3D direction(end - start);

  VistaBoundingBox bbox(GetNode()->GetBoundingBox());
  VistaVector3D    intersection, tmp;

  if (bbox.Intersection(&start[0], &direction[0], true, &intersection[0], tmp[0], &tmp[0], tmp[0]) >
      0) {
    pTrans = matObjectToWorldTransform * intersection;
    return true;
  }

  // if no intersection was found, return the object's center
  return VistaNodeAdapter::GetPosition(pTrans, oReferenceFrame);
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaIntentionSelect::VistaIntentionSelect()
    : m_oCone()
    , m_nStickyness(0.5f)
    , m_nSnappiness(0.5f)
    , m_nScalingDistance(0.0f) {
}

VistaIntentionSelect::~VistaIntentionSelect() {
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaIntentionSelect::SetSelectionVolume(const VistaEvenCone& cone) {
  m_oCone = cone;
}

VistaEvenCone VistaIntentionSelect::GetSelectionVolume() const {
  return m_oCone;
}

void VistaIntentionSelect::RegisterNode(IVistaIntentionSelectAdapter* pNode) {
  m_vecScores.push_back(pNode);
}

bool VistaIntentionSelect::UnregisterNode(IVistaIntentionSelectAdapter* pNode) {
  AdapterVector::iterator it = std::remove(m_vecScores.begin(), m_vecScores.end(), pNode);

  m_vecScores.erase(it, m_vecScores.end());

  return true;
}

void VistaIntentionSelect::ClearAllNodes() {
  m_vecScores.clear();
}

void VistaIntentionSelect::Update(std::vector<IVistaIntentionSelectAdapter*>& vecObj) {
  // do the scoring on all registered nodes
  for (AdapterVector::iterator it = m_vecScores.begin(); it != m_vecScores.end(); ++it) {
    IVistaIntentionSelectAdapter* pPointNode = (*it);
    if (!pPointNode->GetIsSelectionEnabled()) {
      // set to zero score, so object will not be in vecObj
      pPointNode->SetScore(0.f);
    } else {
      // regardless of the handle type we are using, we need the distance to the first point
      VistaVector3D v3Point;

      pPointNode->GetPosition(v3Point, m_ConeRef);
      float fContrib =
          CalculatePointContribution(v3Point) * pPointNode->GetContributionMultiplier();

      float fScore = std::max<float>(
          0.0f, (pPointNode->GetScore() * m_nStickyness) + (fContrib * m_nSnappiness));
      pPointNode->SetScore(fScore);
    }
  }

  // elements are sorted descending on their score
  std::sort(m_vecScores.begin(), m_vecScores.end(),
      [this](IVistaIntentionSelectAdapter* pLeft, IVistaIntentionSelectAdapter* pRight) {
        if (std::abs(pLeft->GetScore() - pRight->GetScore()) < 0.0001) {
          VistaVector3D leftPos, rightPos;
          pLeft->GetPosition(leftPos, m_ConeRef);
          pRight->GetPosition(rightPos, m_ConeRef);
          return leftPos.GetLength() < rightPos.GetLength();
        }
        return pLeft->GetScore() > pRight->GetScore();
      });

  // Might be little bit faster to allocate memory before loop
  if (vecObj.size() != m_vecScores.size())
    vecObj.reserve(m_vecScores.size());
  // push the nodes to the vector that was passed by the user
  // (assumed to be empty)
  for (AdapterVector::iterator cit = m_vecScores.begin(); cit != m_vecScores.end(); ++cit) {
    if ((*cit)->GetScore() > 0)
      vecObj.push_back(*cit);
  }
}

void VistaIntentionSelect::SetConeTransform(
    const VistaVector3D& v3Pos, const VistaQuaternion& qOri) {
  m_ConeRef.SetTranslation(v3Pos);
  m_ConeRef.SetRotation(qOri);
}

int VistaIntentionSelect::GetNodeCount() const {
  return (int)m_vecScores.size();
}

IVistaIntentionSelectAdapter* VistaIntentionSelect::GetNodeByIndex(int nIndex) const {
  if (nIndex > static_cast<int>(m_vecScores.size()))
    return NULL;
  return m_vecScores[nIndex];
}

float VistaIntentionSelect::GetNodeScore(IVistaIntentionSelectAdapter* pNode) const {
  if (std::find(m_vecScores.begin(), m_vecScores.end(), pNode) != m_vecScores.end())
    return pNode->GetScore();
  else
    return 0.0f;
}

void VistaIntentionSelect::SetStickyness(float nStickyness) {
  if (nStickyness <= 1.0f && nStickyness >= 0.0f) {
    m_nStickyness = nStickyness;
    CleanScores();
  }
}

float VistaIntentionSelect::GetStickyness() const {
  return m_nStickyness;
}

void VistaIntentionSelect::SetSnappiness(float nSnappiness) {
  if (nSnappiness <= 1.0f && nSnappiness >= 0.0f) {
    m_nSnappiness = nSnappiness;
    CleanScores();
  }
}

float VistaIntentionSelect::GetSnappiness() const {
  return m_nSnappiness;
}

void VistaIntentionSelect::SetScalingDistance(float nScalingDistance) {
  if (nScalingDistance >= 0.0f) {
    m_nScalingDistance = nScalingDistance;
    CleanScores();
  }
}

float VistaIntentionSelect::GetScalingDistance() const {
  return m_nScalingDistance;
}

void VistaIntentionSelect::CleanScores() {
  for (AdapterVector::iterator it = m_vecScores.begin(); it != m_vecScores.end(); ++it) {
    (*it)->SetScore(0.f);
  }
}

float VistaIntentionSelect::CalculatePointContribution(const VistaVector3D& v3Point) {
  VistaVector3D v3TransformedPoint = m_ConeRef.TransformPositionToFrame(v3Point);

  float fPerp = ::sqrtf(v3TransformedPoint[Vista::X] * v3TransformedPoint[Vista::X] +
                        v3TransformedPoint[Vista::Y] * v3TransformedPoint[Vista::Y]);

  float fProj = -v3TransformedPoint[Vista::Z];

  return CalculateContribution(fPerp, fProj);
}

float VistaIntentionSelect::CalculateContribution(float fPerp, float fProj) {
  float fContrib;

  if (fProj > 0) {
    fContrib = 1 - ((fPerp / fProj) / (m_oCone.GetRadius() / m_oCone.GetHeight()));

    if (m_nScalingDistance > 0.f) {
      fContrib *= std::exp(-fProj / m_nScalingDistance);
    }
  } else {
    fContrib = 0;
  }

  return fContrib;
}
