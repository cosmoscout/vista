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

#ifndef _VISTAOPENSGPHONGSHADER_H_
#define _VISTAOPENSGPHONGSHADER_H_

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaKernelOpenSGExtConfig.h"

#include "VistaOpenSGPerMaterialShader.h"
#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaGeometry;
class IVistaNode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELOPENSGEXTAPI VistaOpenSGPhongShader : public VistaOpenSGPerMaterialShader {
 public:
  VistaOpenSGPhongShader();
  VistaOpenSGPhongShader(
      const std::string& sVertexShaderFile, const std::string& sFragmentShaderFile);
  ~VistaOpenSGPhongShader();

  bool GetUseFog() const;
  void SetUseFog(const bool bSet);
  int  GetFogType() const;
  void SetFogType(const int nGLFogType);

  virtual bool ApplyToOSGMaterial(osg::ChunkMaterialPtr& pGeometry);
  virtual bool ApplyToOSGMaterial(
      osg::ChunkMaterialPtr& pMaterial, const std::vector<CShaderUniformParam>& vecUniformParams);

 private:
  static const std::string s_sVertexShader;
  static const std::string s_sFragmentShader;

  bool m_bUseFog;
  int  m_nFogType;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGPHONGSHADER_H_
