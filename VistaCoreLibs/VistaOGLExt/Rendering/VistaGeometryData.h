/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2016 RWTH Aachen University               */
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

#ifndef __VistaGeometryData_h
#define __VistaGeometryData_h

#include "../VistaOGLExtConfig.h"

#include <vector>

class VistaBufferObject;
class VistaVertexArrayObject;

class VISTAOGLEXTAPI VistaGeometryData {
 public:
  VistaGeometryData();
  virtual ~VistaGeometryData();
  /**************************************************************************/
  /* GETTER                                                                 */
  /**************************************************************************/
  unsigned int GetPolygonOffset() const;
  unsigned int GetPolygonCount() const;
  unsigned int GetStripOffset() const;
  unsigned int GetStripCount() const;
  unsigned int GetTriangleOffset() const;
  unsigned int GetTriangleCount() const;
  unsigned int GetLineOffset() const;
  unsigned int GetLineCount() const;

  VistaVertexArrayObject* GetVertexArrayObject() const;
  VistaBufferObject*      GetInderBufferObject() const;
  VistaBufferObject*      GetVertexBufferObjekt(unsigned int uiIndex) const;
  /**************************************************************************/
  /* SETTER                                                                 */
  /**************************************************************************/
  void SetPolygonOffset(unsigned int uiPolygonOffset);
  void SetPolygonCount(unsigned int uiPolygonCount);
  void SetStripOffset(unsigned int uiStripOffset);
  void SetStripCount(unsigned int uiStripCount);
  void SetTriangleOffset(unsigned int uiStripOffset);
  void SetTriangleCount(unsigned int uiStripCount);
  void SetLineOffset(unsigned int uiLineOffset);
  void SetLineCount(unsigned int uiLineCount);

  void SpecifyFloatAttribute(
      unsigned int uiIndex, unsigned int uiNumComponents, unsigned int uiDataType);
  void SpecifyIntegerAttribute(
      unsigned int uiIndex, unsigned int uiNumComponents, unsigned int uiDataType);
  void SpecifyDoubleAttribute(
      unsigned int uiIndex, unsigned int uiNumComponents, unsigned int uiDataType);

  bool UpdateAttributeData(unsigned int uiIndex, unsigned int uiSizeInByte, const void* pData);

 protected:
  void SpecifyAttribute(unsigned int uiIndex);

 private:
  typedef std::vector<VistaBufferObject*> VboVector;

  VboVector               m_vecVBOs;
  VistaBufferObject*      m_pIBO;
  VistaVertexArrayObject* m_pVAO;

  unsigned int m_uiPolygonOffset;
  unsigned int m_uiPolygonCount;
  unsigned int m_uiStripOffset;
  unsigned int m_uiStripCount;
  unsigned int m_uiTriangleOffset;
  unsigned int m_uiTriangleCount;
  unsigned int m_uiLineOffset;
  unsigned int m_uiLineCount;
};

#endif // Include guard.
/******************************************************************************/
/* END OF FILE                                                                */
/******************************************************************************/
