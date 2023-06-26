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

#include "VdfnTypeConvertNodeCreator.h"

#include "VdfnTypeConvertNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace VistaConversion {
template <>
struct ConvertTypeObject<VistaVector3D, VistaTransformMatrix> {
  VistaVector3D operator()(const VistaTransformMatrix& oFrom) {
    return oFrom.GetTranslation();
  }
};

template <typename To, typename From>
struct ConvertTypeObject<std::vector<To>, std::vector<From>> {
  typename std::vector<To> operator()(const typename std::vector<From>& vecFrom) {
    typename std::vector<To> vecTo(vecFrom.size());
    for (std::size_t i = 0; i < vecFrom.size(); ++i)
      vecTo[i] = VistaConversion::ConvertType<To, From>(vecFrom[i]);
    return vecTo;
  }
};
} // namespace VistaConversion

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnTypeConvertNodeCreators::RegisterTypeConvertNodeCreators(VdfnNodeFactory* pFac) {
  pFac->SetNodeCreator("TypeConvert[double,bool]", new VdfnTypeConvertNodeCreate<double, bool>);
  pFac->SetNodeCreator("TypeConvert[double,int]", new VdfnTypeConvertNodeCreate<double, int>);
  pFac->SetNodeCreator(
      "TypeConvert[double,unsigned int]", new VdfnTypeConvertNodeCreate<double, unsigned int>);
  pFac->SetNodeCreator("TypeConvert[double,float]", new VdfnTypeConvertNodeCreate<double, float>);
  pFac->SetNodeCreator(
      "TypeConvert[double,string]", new VdfnTypeConvertNodeCreate<double, std::string>);
  pFac->SetNodeCreator("TypeConvert[VistaType::microtime,string]",
      new VdfnTypeConvertNodeCreate<VistaType::microtime, std::string>);

  pFac->SetNodeCreator("TypeConvert[int,bool]", new VdfnTypeConvertNodeCreate<int, bool>());
  pFac->SetNodeCreator(
      "TypeConvert[int,unsigned int]", new VdfnTypeConvertNodeCreate<int, unsigned int>());
  pFac->SetNodeCreator("TypeConvert[int,float]", new VdfnTypeConvertNodeCreate<int, float>());
  pFac->SetNodeCreator("TypeConvert[int,double]", new VdfnTypeConvertNodeCreate<int, double>());
  pFac->SetNodeCreator(
      "TypeConvert[int,string]", new VdfnTypeConvertNodeCreate<int, std::string>());

  pFac->SetNodeCreator("TypeConvert[bool,int]", new VdfnTypeConvertNodeCreate<bool, int>());
  pFac->SetNodeCreator(
      "TypeConvert[bool,unsigned int]", new VdfnTypeConvertNodeCreate<bool, unsigned int>());
  pFac->SetNodeCreator("TypeConvert[bool,float]", new VdfnTypeConvertNodeCreate<bool, float>());
  pFac->SetNodeCreator("TypeConvert[bool,double]", new VdfnTypeConvertNodeCreate<bool, double>());
  pFac->SetNodeCreator(
      "TypeConvert[bool,string]", new VdfnTypeConvertNodeCreate<bool, std::string>());

  pFac->SetNodeCreator("TypeConvert[float,bool]", new VdfnTypeConvertNodeCreate<float, bool>());
  pFac->SetNodeCreator("TypeConvert[float,int]", new VdfnTypeConvertNodeCreate<float, int>());
  pFac->SetNodeCreator(
      "TypeConvert[float,unsigned int]", new VdfnTypeConvertNodeCreate<float, unsigned int>());
  pFac->SetNodeCreator("TypeConvert[float,double]", new VdfnTypeConvertNodeCreate<float, double>());
  pFac->SetNodeCreator(
      "TypeConvert[float,string]", new VdfnTypeConvertNodeCreate<float, std::string>());

  pFac->SetNodeCreator(
      "TypeConvert[unsigned int,bool]", new VdfnTypeConvertNodeCreate<unsigned int, bool>());
  pFac->SetNodeCreator(
      "TypeConvert[unsigned int,int]", new VdfnTypeConvertNodeCreate<unsigned int, int>());
  pFac->SetNodeCreator(
      "TypeConvert[unsigned int,float]", new VdfnTypeConvertNodeCreate<unsigned int, float>());
  pFac->SetNodeCreator(
      "TypeConvert[unsigned int,double]", new VdfnTypeConvertNodeCreate<unsigned int, double>());
  pFac->SetNodeCreator("TypeConvert[unsigned int,string]",
      new VdfnTypeConvertNodeCreate<unsigned int, std::string>());

  pFac->SetNodeCreator("TypeConvert[VistaVector3D,string]",
      new VdfnTypeConvertNodeCreate<VistaVector3D, std::string>());
  pFac->SetNodeCreator("TypeConvert[VistaQuaternion,string]",
      new VdfnTypeConvertNodeCreate<VistaQuaternion, std::string>());
  pFac->SetNodeCreator("TypeConvert[VistaTransformMatrix,string]",
      new VdfnTypeConvertNodeCreate<VistaTransformMatrix, std::string>());

  pFac->SetNodeCreator("TypeConvert[VistaVector3D,VistaTransformMatrix]",
      new VdfnTypeConvertNodeCreate<VistaVector3D, VistaTransformMatrix>());
  pFac->SetNodeCreator("TypeConvert[VistaQuaternion,VistaTransformMatrix]",
      new VdfnTypeConvertNodeCreate<VistaQuaternion, VistaTransformMatrix>());
  pFac->SetNodeCreator("TypeConvert[VistaTransformMatrix,VistaQuaternion]",
      new VdfnTypeConvertNodeCreate<VistaTransformMatrix, VistaQuaternion>());
  //	pFac->SetNodeCreator( "TypeConvert[VistaTransformMatrix,VistaVector3D]", new
  //VdfnTypeConvertNodeCreate<VistaTransformMatrix,VistaVector3D>() );

  pFac->SetNodeCreator("TypeConvert[vector[double],string]",
      new VdfnTypeConvertNodeCreate<std::vector<double>, std::string>());

  pFac->SetNodeCreator("TypeConvert[vector[bool],vector[int]]",
      new VdfnTypeConvertNodeCreate<std::vector<bool>, std::vector<bool>>());
  pFac->SetNodeCreator("TypeConvert[vector[bool],vector[unsigned int]]",
      new VdfnTypeConvertNodeCreate<std::vector<bool>, std::vector<unsigned int>>());
  pFac->SetNodeCreator("TypeConvert[vector[bool],vector[float]]",
      new VdfnTypeConvertNodeCreate<std::vector<bool>, std::vector<float>>());
  pFac->SetNodeCreator("TypeConvert[vector[bool],vector[double]]",
      new VdfnTypeConvertNodeCreate<std::vector<bool>, std::vector<double>>());
  pFac->SetNodeCreator("TypeConvert[vector[int],vector[bool]]",
      new VdfnTypeConvertNodeCreate<std::vector<int>, std::vector<bool>>());
  pFac->SetNodeCreator("TypeConvert[vector[int],vector[unsigned int]]",
      new VdfnTypeConvertNodeCreate<std::vector<int>, std::vector<unsigned int>>());
  pFac->SetNodeCreator("TypeConvert[vector[int],vector[float]]",
      new VdfnTypeConvertNodeCreate<std::vector<int>, std::vector<float>>());
  pFac->SetNodeCreator("TypeConvert[vector[int],vector[double]]",
      new VdfnTypeConvertNodeCreate<std::vector<int>, std::vector<double>>());
  pFac->SetNodeCreator("TypeConvert[vector[float],vector[bool]]",
      new VdfnTypeConvertNodeCreate<std::vector<float>, std::vector<bool>>());
  pFac->SetNodeCreator("TypeConvert[vector[float],vector[int]]",
      new VdfnTypeConvertNodeCreate<std::vector<float>, std::vector<int>>());
  pFac->SetNodeCreator("TypeConvert[vector[float],vector[unsigned int]]",
      new VdfnTypeConvertNodeCreate<std::vector<float>, std::vector<unsigned int>>());
  pFac->SetNodeCreator("TypeConvert[vector[float],vector[double]]",
      new VdfnTypeConvertNodeCreate<std::vector<float>, std::vector<double>>());
  pFac->SetNodeCreator("TypeConvert[vector[double],vector[bool]]",
      new VdfnTypeConvertNodeCreate<std::vector<double>, std::vector<bool>>());
  pFac->SetNodeCreator("TypeConvert[vector[double],vector[int]]",
      new VdfnTypeConvertNodeCreate<std::vector<double>, std::vector<int>>());
  pFac->SetNodeCreator("TypeConvert[vector[double],vector[unsigned int]]",
      new VdfnTypeConvertNodeCreate<std::vector<double>, std::vector<unsigned int>>());
  pFac->SetNodeCreator("TypeConvert[vector[double],vector[float]]",
      new VdfnTypeConvertNodeCreate<std::vector<double>, std::vector<float>>());

  return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
