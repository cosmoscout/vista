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

#ifndef _VISTAOBJECTSETMOVE_H
#define _VISTAOBJECTSETMOVE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaVectorMath.h>
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaMath/VistaIndirectXform.h>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaTransformable;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Simple drag & drop auxilliary class for transformables. The precoditions for
 * the use of this class are simple: provide all coordinates in world space and
 * a transformable that implements the GetParentWorldTransform() API (retrieve
 * transformation of the transformable's global parent frame). As stated in the name:
 * this is a SET move, which means you can drag and drop any number of objects
 * currently in this selection set.
 * - register all transformables to be moved in this set using the
     RegisterObject() API <b>before</b> any call to either StartMove() or Move()
 * - call StartMove() with the coordinates of the pick ray origin in world coordinates
 * - call Move() upon every update of the pick ray's origin in world coordinates
 * Your registered objects are moves as if "picked on a stick".
 * Objects can be unregistered at any time. When you add objects to the set, call
 * StartMove() again to properly initialize the set's transforms. This should be
 * safe to call, even when in a dragging operation.
 * @todo correct this by using a matrix implementation (correct indirectxform)
 * -> use world matrices, not pos/ori!
 */
class VISTAKERNELAPI VistaObjectSetMove {
 public:
  VistaObjectSetMove();
  ~VistaObjectSetMove();

  class IMoveFilter {
   public:
    virtual ~IMoveFilter() {
    }
    virtual bool Move(const VistaVector3D& startPos, const VistaQuaternion& startOri,
        const VistaVector3D& plannedPos, const VistaQuaternion& plannedOri,
        VistaVector3D& allowedPos, VistaQuaternion& allowedOri) = 0;
  };
  /**
   * Is to be called <b>once</b> at the start of your drag operation.
   * Works only on registered instances.
   * @param v3Pos the origin position in world space of the pick ray
   * @param qOri the orientation of the pick ray in world space
   */
  bool StartMove(const VistaVector3D& v3Pos, const VistaQuaternion& qOri);

  /**
   * Is to be called on every change of the origin of the pick ray of
   * your drag operation on the set. Provide the values in world reference
   * frame.
   * @param v3Pos the origin of your pick ray in wcs
   * @param qOri the orientation of your pick ray in wcs
   */
  bool Move(const VistaVector3D& v3Pos, const VistaQuaternion& qOri);

  /**
   * Register an object with this pick set. Call StartMove() <b>after</b>
   * every registration, as this initializes the values for the transformation
   * on all regitered items in this pick set. Should be safe to call during a
   * drag operation.
   */
  bool RegisterObject(IVistaTransformable* pTrans);
  bool GetIsRegistered(IVistaTransformable* pTrans) const;

  /**
   *  Unregister an instance from this pick set. Should be safe to call during
   *  a drag operation.
   */
  bool UnregisterObject(IVistaTransformable* pTrans);

  IMoveFilter* GetMoveFilter() const;
  void         SetMoveFilter(IMoveFilter*);

 protected:
 private:
  struct _sLookup {
    _sLookup()
        : m_pObj(NULL) {
    }

    _sLookup(IVistaTransformable* pTrans)
        : m_pObj(pTrans) {
    }

    bool operator==(const _sLookup& oOther) const {
      return (m_pObj == oOther.m_pObj);
    }

    IVistaTransformable* m_pObj;
    VistaIndirectXform   m_nXForm;
    VistaVector3D        m_v3Trans; // trans @ the beginning of the drag
    VistaQuaternion      m_qOri;    // ori @ the beginning of the drag
  };

  typedef std::vector<_sLookup> SELECTVEC;
  SELECTVEC                     m_vecRegisteredObjects;

  IMoveFilter* m_pMoveFilter;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASELECTIONSETMOVE_H
