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

#ifndef _VISTANODEINTERFACE_H
#define _VISTANODEINTERFACE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaAspects/VistaLocatable.h>
#include <VistaBase/VistaBaseTypes.h> // VistaType::uint64
#include <VistaBase/VistaVectorMath.h>
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaMath/VistaBoundingBox.h>

#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

enum VISTA_NODETYPE {
  VISTA_NODE,
  VISTA_GROUPNODE,
  VISTA_SWITCHNODE,
  VISTA_LEAFNODE,
  VISTA_LIGHTNODE,
  VISTA_AMBIENTLIGHTNODE,
  VISTA_DIRECTIONALLIGHTNODE,
  VISTA_POINTLIGHTNODE,
  VISTA_SPOTLIGHTNODE,
  VISTA_GEOMNODE,
  VISTA_EXTENSIONNODE,
  VISTA_OPENGLNODE,
  VISTA_LODNODE,
  VISTA_TRANSFORMNODE,
  VISTA_TEXTNODE
};

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNode;
class VistaGroupNode;

typedef std::vector<IVistaNode*>           VistaNodeVector;
typedef std::vector<IVistaNode*>::iterator VistaNodeVectorIter;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI IVistaNode : virtual public IVistaLocatable {
 public:
  /**
   * virtual destructor, note that you should use the VistaSceneGraph interface
   * to delete nodes and not delete them by calling delete on this node
   * unless you know what you are doing.
   */
  virtual ~IVistaNode() {
  }

  /**
   * Retrieve the name of the node
   */
  virtual std::string GetName() const = 0;

  /**
   * Set the name of the node. This name is used for searching operations
   * in the scene. There is no automagic way to make this name unique, this
   * is part of the application.
   */
  virtual bool SetName(const std::string& sName) = 0;

  /** Retreive the internal node type.
   * Useful to save casting operations.
   * Note that you only get an exact match, this is not a replacement
   * for a GetIsA() operator which is missing in this signature
   *	@return node type
   *	@see VISTA_NODETYPE
   */
  virtual VISTA_NODETYPE GetType() const = 0;

  /** Checks whether the node can have children i.e. whether it is a
   *	group or switch node
   *	@return bool true/false
   */
  virtual bool CanHaveChildren() const = 0;

  /** Retrieve the node's parent. NULL if node is root or orphan
   *	@return VistaGroupNode* NULL/Pointer to parent
   */
  virtual VistaGroupNode* GetParent() const = 0;
  /** Enables the node for rendering
   * @return bool true/false
   */

  /** Enables/Disables the node from rendering
   * @return bool true/false
   */
  virtual void SetIsEnabled(bool bEnabled) = 0;

  /** Retrieve current rendermode (on/off)
   * @return bool true/false
   */
  virtual bool GetIsEnabled() const = 0;

  /** Print debug information to out
   * @param ostream &out : Stream to which debug information is printed
   * @param int level : internal parameter, leave at default value
   */
  virtual void Debug(std::ostream& out, int nLevel = 0) const = 0;

  /** Inquire nodes bounding box information
   * bounding box is return in local coordinates
   * @param pMin the 'lower left' edge
   * @param pMax the 'upper right' edge
   * @return bool true/false
   */
  virtual bool             GetBoundingBox(VistaVector3D& pMin, VistaVector3D& pMax) const = 0;
  virtual bool             GetBoundingBox(VistaBoundingBox& oBox) const                   = 0;
  virtual VistaBoundingBox GetBoundingBox() const                                         = 0;

  /** Inquire nodes bounding box information
   * bounding box is return in world coordinates
   * @param pMin the 'lower left' edge
   * @param pMax the 'upper right' edge
   * @return bool true/false
   */
  virtual bool GetWorldBoundingBox(VistaVector3D& v3Min, VistaVector3D& v3Max) const = 0;
  virtual bool GetWorldBoundingBox(VistaBoundingBox& oBox) const                     = 0;
  virtual VistaBoundingBox GetWorldBoundingBox() const                               = 0;

  /**
   * Query method to check for transformation changes of the parent path of this
   * node. Structural changes, such as re-parenting and calls to the transform API
   * of the transform node cause a re-scoring. This method will iterate over all
   * parents until reaching the local root and add its local change score to
   * the result. Can be used for polling transform changes of a node.
   * It is traversing the scene graph upwards, though, so do not excess its
   * usage. As well it is unclear what happens if we start to overflow the local counters,
   * although this could take some while.
   */
  virtual VistaType::uint64 GetTransformScore() const = 0;

 protected:
  /**
   * There is no sense in creating an IVistaNode directly
   */
  IVistaNode(){};
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTANODEINTERFACE_H
