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

#ifndef _VISTALEAPMOTIONCOMMONSHARE_H
#define _VISTALEAPMOTIONCOMMONSHARE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaAutoBuffer.h>
#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaVector3D.h>

#define LEAPMEASURES_MAX_FINGERS 20
#define LEAPMEASURES_MAX_TOOLS 10

namespace VistaLeapMotionMeasures {
struct Base {
  Base()
      : m_driver_timestamp(0)
      , m_bVisible(false)
      , m_nId(-1)
      , m_nTimeVisible(0) {
  }

  VistaType::uint64 m_driver_timestamp; // in us since device start
  bool              m_bVisible;
  int               m_nId;
  float             m_nTimeVisible;
};

struct Pointable : public Base {
  Pointable()
      : Base()
      , m_nWidth(0)
      , m_nLength(0)
      , m_nHandId(-1)
      , m_nTouchDistance(1) {
  }

  int           m_nHandId;
  float         m_nWidth;
  float         m_nLength;
  VistaVector3D m_v3TipPosition;
  VistaVector3D m_v3StabilizedTipPosition;
  VistaVector3D m_v3TipVelocity;
  VistaVector3D m_v3Direction;
  float         m_nTouchDistance;
};

struct Finger : public Pointable {
  Finger()
      : Pointable()
      , m_bExtended(false)
      , m_nFingerType(-1) {
  }

  bool          m_bExtended;
  int           m_nFingerType; // 0: thumb, 1: index, ..., 4: pinky
  VistaVector3D m_v3MetacarpalBoneStartPoint;
  VistaVector3D m_v3ProximalBoneStartPoint;
  VistaVector3D m_v3IntermediateBoneStartPoint;
  VistaVector3D m_v3DistalBoneStartPoint;
};

struct Tool : public Pointable {};

struct HandMeasure : public Base {
  HandMeasure()
      : Base()
      , m_nPalmWidth(0)
      , m_nConfidence(0)
      , m_bIsLeft(false)
      , m_bIsRight(false) {
  }

  Finger        m_a5oFingers[5];
  Tool          m_a2oTools[2];
  VistaVector3D m_v3PalmPosition;
  VistaVector3D m_v3StabilizedPalmPosition;
  VistaVector3D m_v3PalmVelocity;
  VistaVector3D m_v3PalmNormal;
  VistaVector3D m_v3PalmDirection;
  float         m_nPalmWidth;

  VistaVector3D m_v3ElbowPosition;
  VistaVector3D m_v3WristPostion;
  float         m_nArmWidth;

  float m_nConfidence;

  bool m_bIsLeft;
  bool m_bIsRight;
};

struct FingersMeasure {
  //		static const int MAX_FINGERS = 20;
  int    m_nNumFingers;
  Finger m_aFingers[LEAPMEASURES_MAX_FINGERS];
};

struct ToolsMeasure {
  //		static const int MAX_TOOLS = 10;
  int  m_nNumTools;
  Tool m_aTools[LEAPMEASURES_MAX_TOOLS];
};

struct GestureMeasure {
  VistaType::uint64    m_driver_timestamp;
  VistaType::microtime m_nDuration;
  int                  m_nId;
  int                  m_nPerformingPointableId;

  enum Type {
    TP_SWIPE,
    TP_SCREEN_TAP,
    TP_KEY_TAP,
    TP_CIRCLE,
  };

  Type m_eType;

  VistaVector3D m_v3Position;
  VistaVector3D m_v3Direction;
  float         m_nLength;
};

struct ImageMeasure : public Base {
  VistaType::uint32 m_format, // 0 = INFRARED
      m_bytes_per_pixel;

  VistaType::uint32 m_width, m_height;

  VistaType::float32 m_ray_offset_x, m_ray_offset_y, m_ray_scale_x, m_ray_scale_y;

  // @TODO: serialization
  VistaAutoBuffer m_data;
};
} // namespace VistaLeapMotionMeasures

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTALEAPMOTIONCOMMONSHARE_H
