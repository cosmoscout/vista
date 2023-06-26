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

#ifndef _VISTALODNODE_H
#define _VISTALODNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaKernel/GraphicsManager/VistaGroupNode.h"
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNodeBridge;
class IVistaNodeData;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaLODNode : public VistaGroupNode {
  friend class VistaSceneGraph;
  friend class IVistaNodeBridge;

 public:
  /** Set the the LOD spring values. If there is 3 level of details,
   *   it should be given 2 values. The list should be sorted from lowest
   *   to largest range. If you have 2 models and you want to show
   *   the second one at a distance larger than 4 units, the range
   *   list should comprise of the single value "4", so between 0 and
   *   4 will be matched with model / child 0
   */
  virtual bool SetRange(const std::vector<float>& rangeList);
  /** Get the the LOD spring values.
   */
  virtual bool GetRange(std::vector<float>& rangeList) const;

  /**
   * Set the center of the object (for the distance calculation).
   * This is relative to the childrens outline, most probably.
   * SetCenter(VistaVector3D(0,0,0)) means to determine distance
   * according to the center of the children's bounding box
   */
  virtual bool SetCenter(const VistaVector3D& center);
  /**Get the center of the object
   */
  virtual bool GetCenter(VistaVector3D& center) const;

  virtual ~VistaLODNode();

 protected:
  VistaLODNode();
  VistaLODNode(VistaGroupNode* pParent, IVistaNodeBridge* pBridge, IVistaNodeData* pData,
      std::string strName = "");
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTALODNODE_H
