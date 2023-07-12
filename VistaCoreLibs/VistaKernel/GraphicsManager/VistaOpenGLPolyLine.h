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

#ifndef _VISTAOPENGLPOLYLINE_H
#define _VISTAOPENGLPOLYLINE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/GraphicsManager/VistaGeometry.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaMath/VistaBoundingBox.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaNode;
class VistaGroupNode;
class VistaSceneGraph;
class VistaOpenGLNode;
class IVistaNode;
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaOpenGLPolyLine {
 public:
  class VISTAKERNELAPI COpenGLPolyLineDraw : public IVistaOpenGLDraw {
   public:
    COpenGLPolyLineDraw();
    virtual ~COpenGLPolyLineDraw();

    // IVistaOpenGLDraw interface
    virtual bool Do();
    virtual bool GetBoundingBox(VistaBoundingBox& bb);

    void SetLinePoints(const std::vector<float>& vecPoints);
    bool GetLinePoints(std::vector<float>& vecPoints) const;

    enum eMode { LINE_OPEN = 0, LINE_CLOSED };

    eMode GetLineMode() const;
    void  SetLineMode(eMode eMd);

    VistaMaterial GetMaterial() const;
    void          SetMaterial(const VistaMaterial& mat);

   protected:
    bool UpdateDisplayList();

   private:
    int  m_iDispId;
    bool m_bDlistDirty;

    std::vector<float> m_vecPoints;
    VistaBoundingBox   m_oBBox;
    eMode              m_eMode;
    VistaMaterial      m_oMat;
  };

 public:
  VistaOpenGLPolyLine(VistaSceneGraph* pVistaSceneGraph, VistaGroupNode* pParent);

  virtual ~VistaOpenGLPolyLine();

  IVistaNode* GetVistaNode() const;

  void SetLinePoints(const std::vector<float>& vecPoints);
  bool GetLinePoints(std::vector<float>& vecPoints) const;

  bool GetRemoveFromSGOnDelete() const;
  void SetRemoveFromSGOnDelete(bool bRemove);

  bool GetIsClosedLine() const;
  void SetIsClosedLine(bool bLineClosed);

  bool SetMaterial(const VistaMaterial& oMat) const;
  bool SetColor(const VistaColor& color);

 private:
  VistaOpenGLNode*     m_pOglNode;
  COpenGLPolyLineDraw* m_pDrawInterface;
  /** default is true */
  bool m_bRemoveFromSGOnDelete;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTALINE_H
