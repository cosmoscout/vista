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

#ifndef _VISTAOPENSGMULTIMATERIAL_H
#define _VISTAOPENSGMULTIMATERIAL_H

#include "VistaKernelOpenSGExtConfig.h"

#include <VistaAspects/VistaNameable.h>
#include <VistaAspects/VistaUncopyable.h>
#include <VistaBase/VistaColor.h>
#include <string>

class IVistaNode;
class VistaGeometry;
class VistaPropertyList;
class VistaOpenSGTextureLoader;

namespace osg {
template <class BasePtrTypeT, class FieldContainerTypeT>
class FCPtr;

class AttachmentContainerPtr;
class NodeCore;
class Drawable;
class MaterialDrawable;
class Geometry;
class Material;
class AttachmentPtr;
class StateChunk;
class TextureChunk;
class Image;

typedef FCPtr<AttachmentContainerPtr, NodeCore> NodeCorePtr;
typedef FCPtr<NodeCorePtr, Drawable>            DrawablePtr;
typedef FCPtr<DrawablePtr, MaterialDrawable>    MaterialDrawablePtr;
typedef FCPtr<MaterialDrawablePtr, Geometry>    GeometryPtr;
typedef FCPtr<AttachmentContainerPtr, Material> MaterialPtr;
typedef FCPtr<AttachmentPtr, StateChunk>        StateChunkPtr;
typedef FCPtr<StateChunkPtr, TextureChunk>      TextureChunkPtr;
typedef FCPtr<AttachmentContainerPtr, Image>    ImagePtr;
} // namespace osg

class VISTAKERNELOPENSGEXTAPI VistaOpenSGMultiMaterial {
  VISTA_UNCOPYABLE(VistaOpenSGMultiMaterial)
 public:
  VistaOpenSGMultiMaterial(VistaOpenSGTextureLoader* pTexLoader);
  virtual ~VistaOpenSGMultiMaterial();

  virtual VistaOpenSGMultiMaterial* Clone();

  enum BlendingMode {
    BM_AUTO,
    BM_NO_BLENDING,
    BM_BLENDING,
  };

  // @TODO: Reflectionable?
  bool Configure(const VistaPropertyList& oConfig);

  bool SetBaseMap(const std::string& sImageFile);
  bool SetBaseMap(osg::TextureChunkPtr pTexture);
  bool SetBaseMap(osg::ImagePtr pImage);
  bool GetHasBaseMap() const;

  bool  SetBlendMap(const std::string& sImageFile);
  bool  SetBlendMap(const std::string& sImageFile, const float nBlendFactor);
  bool  GetHasBlendMap() const;
  bool  SetBlendFactor(const float nBlendFactor);
  float GetBlendFactor() const;

  bool  SetSpecularMap(const std::string& sImageFile);
  bool  GetHasSpecularMap() const;
  bool  SetObjectEnvironmentMap(const std::string& sFolder);
  bool  SetObjectEnvironmentMap(const std::string& sTopImage, const std::string& sBottomImage,
       const std::string& sLeftImage, const std::string& sRightImage, const std::string& sFrontImage,
       const std::string& sBackImage);
  bool  GetHasObjectEnvironmentMap() const;
  bool  SetWorldEnvironmentMap(const std::string& sFolder);
  bool  SetWorldEnvironmentMap(const std::string& sTopImage, const std::string& sBottomImage,
       const std::string& sLeftImage, const std::string& sRightImage, const std::string& sFrontImage,
       const std::string& sBackImage);
  bool  GetHasWorldEnvironmentMap() const;
  bool  SetReflectionFactor(const float nReflectionFactor);
  float GetReflectionFactor() const;
  bool  SetReflectionBlur(const float nBlur);
  float GetReflectionBlur() const;

  bool SetGlossMap(const std::string& sImageFile);
  bool GetHasGlossMap() const;

  bool SetBumpMap(
      const std::string& sImageFile, const float nDepthMagnitude, const bool bSaveNormalMap = true);
  bool SetNormalMap(const std::string& sImageFile);
  bool GetHasNormalMap() const;

  void  SetTextureScale(const float fScaleS, const float fScaleT);
  void  SetTextureScaleS(const float fScale);
  void  SetTextureScaleT(const float fScale);
  float GetTextureScaleS() const;
  float FetTextureScaleT() const;

  void       SetColor(const VistaColor& oColor, const float nRelativeDiffuse = 1.0f,
            const float nRelativeSpecular = 1.0f, const float nRelativeAmbient = 1.0f,
            const float nRelativeEmissive = 0.0f);
  void       SetAmbientColor(const VistaColor& oColor);
  VistaColor GetAmbientColor() const;
  void       SetDiffuseColor(const VistaColor& oColor);
  VistaColor GetDiffuseColor() const;
  void       SetSpecularColor(const VistaColor& oColor);
  VistaColor GetSpecularColor() const;
  void       SetEmissiveColor(const VistaColor& oColor);
  VistaColor GetEmissiveColor() const;
  void       SetShininess(const float nShininess);
  float      GetShininess() const;
  void       SetOpacity(const float nOpacity);
  float      GetOpacity() const;

  int  GetBlendingMode() const;
  void SetBlendingMode(const int eMode);
  bool GetIsUsingBlending() const;

  void SetSortKey(const int nKey);
  int  GetSortKey() const;

  void SetBackfaceCulling(const bool bSet);
  bool GetBackfaceCulling() const;
  void SetFrontfaceCulling(const bool bSet);
  bool GetFrontfaceCulling() const;

  void SetIsLit(const bool bSet);
  bool GetIsLit() const;

  void SetUseNativeGLPipeline(const bool bSet);
  bool GetUseNativeGLPipeline() const;
  void SetDrawAsWireFrame(const bool bSet);
  bool GetDrawAsWireFrame() const;

  void SetUseFog(const bool bSet);
  bool GetUseFog() const;
  int  GetFogType() const;
  void SetFogType(const int nGLFogType);

  osg::MaterialPtr GetOSGMaterial();
  bool             ApplyTo(osg::MaterialDrawablePtr pDrawable);
  bool             ApplyTo(osg::GeometryPtr pGeometry, const bool bReplaceVertexColors);
  bool             ApplyTo(VistaGeometry* pGeometry, const bool bReplaceVertexColors);

  bool ApplyToGeomNodesInSubtree(IVistaNode* pNode, const bool bReplaceVertexColors);

 protected:
  virtual void CopyPropertiesToOtherMaterial(VistaOpenSGMultiMaterial* pOther) const;

 private:
  class Internals;
  Internals* m_pInternals;
};

#endif // _VISTAOPENSGMULTIMATERIAL_H
