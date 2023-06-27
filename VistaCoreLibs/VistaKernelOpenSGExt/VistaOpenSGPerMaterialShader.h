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

#ifndef _VISTAOPENSGPERPERMATERIALSHADER_H__
#define _VISTAOPENSGPERPERMATERIALSHADER_H__

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaKernelOpenSGExtConfig.h"

#include <string>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaGeometry;
class IVistaNode;

// fwd declare of ChunkMaterialPtr
namespace osg {
template <class BasePtrTypeT, class FieldContainerTypeT>
class FCPtr;
class ChunkMaterial;
class AttachmentContainerPtr;
class Material;
typedef FCPtr<AttachmentContainerPtr, Material> MaterialPtr;
typedef FCPtr<MaterialPtr, ChunkMaterial>       ChunkMaterialPtr;
} // namespace osg

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELOPENSGEXTAPI VistaOpenSGPerMaterialShader {
 public:
  struct VISTAKERNELOPENSGEXTAPI CShaderUniformParam {
    friend class VistaOpenSGPerMaterialShader;

   public:
    enum PARAM_TYPE {
      INVALID = -1,
      BOOL    = 0,
      INT,
      FLOAT,
      VEC2,
      VEC3,
      VEC4,
      OSG_LIGHT_MASK,
      OSG_ACTIVE_LIGHTS,
      OSG_CAMERA_POSITION,
    };

    CShaderUniformParam(const std::string& sName, const bool bParam);
    CShaderUniformParam(const std::string& sName, const int iParam);
    CShaderUniformParam(const std::string& sName, const float fParam);
    CShaderUniformParam(const std::string& sName, const float afParam[], const int iVecSize);
    CShaderUniformParam(const PARAM_TYPE eType);

   private:
    std::string m_sName;
    PARAM_TYPE  m_eType;
    float       m_afData[4];
    int         m_iData;
  };

  VistaOpenSGPerMaterialShader();
  virtual ~VistaOpenSGPerMaterialShader();

  bool SetShadersFromFile(
      const std::string& sVertexShaderFile, const std::string& sFragmentShaderFile);
  bool SetShadersFromString(const std::string& sVertexShader, const std::string& sFragmentShader);

  bool SetUniformParameter(const CShaderUniformParam& oParam);

  virtual bool ApplyToGeometry(VistaGeometry* pGeometry);
  virtual bool ApplyToGeometry(
      VistaGeometry* pGeometry, const std::vector<CShaderUniformParam>& vecUniformParams);
  virtual bool RemoveFromGeometry(VistaGeometry* pGeometry);

  virtual bool ApplyToAllGeometries(IVistaNode* pNode);
  virtual bool RemoveFromAllGeometries(IVistaNode* pNode);

  /**
   * For specializations (e.g. changes based on current material), override
   * this function
   */
  virtual bool ApplyToOSGMaterial(osg::ChunkMaterialPtr& pMaterial);
  virtual bool ApplyToOSGMaterial(
      osg::ChunkMaterialPtr& pMaterial, const std::vector<CShaderUniformParam>& vecUniformParams);
  virtual bool RemoveFromOSGMaterial(osg::ChunkMaterialPtr& pMaterial);

 private:
  struct CShaderData;
  CShaderData* m_pShaderData;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGPERPERMATERIALSHADER_H__
