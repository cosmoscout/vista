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

#include <GL/glew.h>

#include "VistaGeometryData.h"

#include <VistaOGLExt/VistaBufferObject.h>
#include <VistaOGLExt/VistaVertexArrayObject.h>

VistaGeometryData::VistaGeometryData()
    : m_pIBO(new VistaBufferObject())
    , m_pVAO(new VistaVertexArrayObject())
    , m_uiPolygonOffset(0)
    , m_uiPolygonCount(0)
    , m_uiStripOffset(0)
    , m_uiStripCount(0)
    , m_uiTriangleOffset(0)
    , m_uiTriangleCount(0)
    , m_uiLineOffset(0)
    , m_uiLineCount(0) {

  m_pVAO->SpecifyIndexBufferObject(m_pIBO);
}

VistaGeometryData::~VistaGeometryData() {
  delete m_pVAO;
  delete m_pIBO;
  for (size_t n = 0; n < m_vecVBOs.size(); ++n)
    delete m_vecVBOs[n];
}

/******************************************************************************/
/* Getters                                                                    */
/******************************************************************************/

unsigned int VistaGeometryData::GetPolygonOffset() const {
  return m_uiPolygonOffset;
}
unsigned int VistaGeometryData::GetPolygonCount() const {
  return m_uiPolygonCount;
}
unsigned int VistaGeometryData::GetStripOffset() const {
  return m_uiStripOffset;
}
unsigned int VistaGeometryData::GetStripCount() const {
  return m_uiStripCount;
}
unsigned int VistaGeometryData::GetTriangleOffset() const {
  return m_uiTriangleOffset;
}
unsigned int VistaGeometryData::GetTriangleCount() const {
  return m_uiTriangleCount;
}
unsigned int VistaGeometryData::GetLineOffset() const {
  return m_uiLineOffset;
}
unsigned int VistaGeometryData::GetLineCount() const {
  return m_uiLineCount;
}

VistaVertexArrayObject* VistaGeometryData::GetVertexArrayObject() const {
  return m_pVAO;
}
VistaBufferObject* VistaGeometryData::GetInderBufferObject() const {
  return m_pIBO;
}
VistaBufferObject* VistaGeometryData::GetVertexBufferObjekt(unsigned int uiIndex) const {
  if (uiIndex < m_vecVBOs.size())
    return m_vecVBOs[uiIndex];

  return NULL;
}
/******************************************************************************/
/* Setters                                                                    */
/******************************************************************************/

void VistaGeometryData::SetPolygonOffset(unsigned int uiPolygonOffset) {
  m_uiPolygonOffset = uiPolygonOffset;
}
void VistaGeometryData::SetPolygonCount(unsigned int uiPolygonCount) {
  m_uiPolygonCount = uiPolygonCount;
}
void VistaGeometryData::SetStripOffset(unsigned int uiStripOffset) {
  m_uiStripOffset = uiStripOffset;
}
void VistaGeometryData::SetStripCount(unsigned int uiStripCount) {
  m_uiStripCount = uiStripCount;
}
void VistaGeometryData::SetTriangleOffset(unsigned int uiTriangleOffset) {
  m_uiTriangleOffset = uiTriangleOffset;
}
void VistaGeometryData::SetTriangleCount(unsigned int uiTriangleCount) {
  m_uiTriangleCount = uiTriangleCount;
}
void VistaGeometryData::SetLineOffset(unsigned int uiLineOffset) {
  m_uiLineOffset = uiLineOffset;
}
void VistaGeometryData::SetLineCount(unsigned int uiLineCount) {
  m_uiLineCount = uiLineCount;
}

/******************************************************************************/
/* Attribute Specification                                                    */
/******************************************************************************/
void VistaGeometryData::SpecifyFloatAttribute(
    unsigned int uiIndex, unsigned int uiNumComponents, unsigned int uiDataType) {
  SpecifyAttribute(uiIndex);

  m_pVAO->SpecifyAttributeArrayFloat(
      uiIndex, uiNumComponents, uiDataType, false, 0, 0, m_vecVBOs[uiIndex]);
}
void VistaGeometryData::SpecifyIntegerAttribute(
    unsigned int uiIndex, unsigned int uiNumComponents, unsigned int uiDataType) {
  SpecifyAttribute(uiIndex);

  m_pVAO->SpecifyAttributeArrayInteger(
      uiIndex, uiNumComponents, uiDataType, 0, 0, m_vecVBOs[uiIndex]);
}
void VistaGeometryData::SpecifyDoubleAttribute(
    unsigned int uiIndex, unsigned int uiNumComponents, unsigned int uiDataType) {
  SpecifyAttribute(uiIndex);

  m_pVAO->SpecifyAttributeArrayDouble(
      uiIndex, uiNumComponents, uiDataType, 0, 0, m_vecVBOs[uiIndex]);
}

void VistaGeometryData::SpecifyAttribute(unsigned int uiIndex) {
  while (uiIndex >= m_vecVBOs.size())
    m_vecVBOs.push_back(NULL);

  if (m_vecVBOs[uiIndex] == NULL)
    m_vecVBOs[uiIndex] = new VistaBufferObject;

  m_pVAO->EnableAttributeArray(uiIndex);
}

bool VistaGeometryData::UpdateAttributeData(
    unsigned int uiIndex, unsigned int uiSizeInByte, const void* pData) {
  if (uiIndex >= m_vecVBOs.size())
    return false;

  m_vecVBOs[uiIndex]->BindAsVertexDataBuffer();
  m_vecVBOs[uiIndex]->BufferData(uiSizeInByte, pData, GL_STATIC_DRAW);
  m_vecVBOs[uiIndex]->Release();

  return true;
}

/******************************************************************************/
/* END OF FILE                                                                */
/******************************************************************************/