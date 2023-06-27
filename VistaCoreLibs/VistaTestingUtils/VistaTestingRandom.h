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

#ifndef _VISTATESTINGRANDOM_H
#define _VISTATESTINGRANDOM_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#ifndef VISTATESTINGRANDOM_NUMBER_RANGE
#define VISTATESTINGRANDOM_NUMBER_RANGE 1e9
#endif

#include <VistaAspects/VistaConversion.h>
#include <VistaAspects/VistaPropertyList.h>
#include <VistaTools/VistaRandomNumberGenerator.h>

#include <time.h> // we use time to init the random seed

#include <gtest/gtest.h>

#include <limits>

/*============================================================================*/
/* HELPERS                                                                    */
/*============================================================================*/

namespace VistaTesting {
VistaRandomNumberGenerator* GetRNG() {
  return VistaRandomNumberGenerator::GetStandardRNG();
}

inline int InitializeRandomSeed() {
  unsigned int nSeed = (unsigned int)(100000.0 * time(NULL));
  GetRNG()->SetSeed(nSeed);
  return nSeed;
}

inline int RandomInt(const int nMax) {
  return (int)GetRNG()->GenerateInt32(0, nMax);
}

template <typename T>
inline T GenerateRandom() {
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable : 4127) // disable warning about static if statement
#endif
  if (std::numeric_limits<T>::is_signed)
    return T(GetRNG()->GenerateDouble(
        -VISTATESTINGRANDOM_NUMBER_RANGE, VISTATESTINGRANDOM_NUMBER_RANGE));
  else
    return T(GetRNG()->GenerateDouble(0, VISTATESTINGRANDOM_NUMBER_RANGE));
#ifdef WIN32
#pragma warning(pop)
#endif
}

template <>
inline bool GenerateRandom<bool>() {
  if (GetRNG()->GenerateDouble(0, 1) > 0.5)
    return true;
  else
    return false;
}

template <>
inline char GenerateRandom<char>() {
  return char(GetRNG()->GenerateInt32(45, 45 + 81));
}

template <>
inline std::string GenerateRandom<std::string>() {
  std::string oVal((std::size_t)GetRNG()->GenerateInt32(0, 1024), ' ');
  for (std::size_t i = 0; i < oVal.size(); ++i)
    oVal[i] = GenerateRandom<char>();
  return oVal;
}

template <>
inline VistaVector3D GenerateRandom<VistaVector3D>() {
  return VistaVector3D(GenerateRandom<float>(), GenerateRandom<float>(), GenerateRandom<float>());
}

template <>
inline VistaQuaternion GenerateRandom<VistaQuaternion>() {
  VistaQuaternion qRand(GenerateRandom<float>(), GenerateRandom<float>(), GenerateRandom<float>(),
      GenerateRandom<float>());
  qRand.Normalize();
  return qRand;
}

template <>
inline VistaTransformMatrix GenerateRandom<VistaTransformMatrix>() {
  return VistaTransformMatrix(GenerateRandom<float>(), GenerateRandom<float>(),
      GenerateRandom<float>(), GenerateRandom<float>(), GenerateRandom<float>(),
      GenerateRandom<float>(), GenerateRandom<float>(), GenerateRandom<float>(),
      GenerateRandom<float>(), GenerateRandom<float>(), GenerateRandom<float>(),
      GenerateRandom<float>(), GenerateRandom<float>(), GenerateRandom<float>(),
      GenerateRandom<float>(), GenerateRandom<float>());
}

inline VistaPropertyList GenerateRandomProplist(int nMaxRecursionDepth = -1) {
  VistaPropertyList oPropList;
  int               nNumEntries = GetRNG()->GenerateInt32(3, 20);
  if (nMaxRecursionDepth == -1)
    nMaxRecursionDepth = GetRNG()->GenerateInt32(1, 5);
  for (int i = 0; i < nNumEntries; ++i) {
    std::string sName = GenerateRandom<std::string>();
    int         nType = GetRNG()->GenerateInt32(0, 7);
    switch (nType) {
    case 0:
      oPropList.SetValue(sName, GenerateRandom<std::string>());
      break;
    case 1:
      oPropList.SetValue(sName, GenerateRandom<int>());
      break;
    case 2:
      oPropList.SetValue(sName, GenerateRandom<bool>());
      break;
    case 3:
      oPropList.SetValue(sName, GenerateRandom<float>());
      break;
    case 4:
      oPropList.SetValue(sName, GenerateRandom<VistaVector3D>());
      break;
    case 5:
      oPropList.SetValue(sName, GenerateRandom<VistaQuaternion>());
      break;
    case 6:
      if (nMaxRecursionDepth > 0)
        oPropList.SetValue(sName, GenerateRandomProplist(--nMaxRecursionDepth));
      break;
    }
  }
  return oPropList;
}

template <>
inline VistaPropertyList GenerateRandom<VistaPropertyList>() {
  return GenerateRandomProplist();
}
} // namespace VistaTesting

/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

#endif //_VISTATESTINGRANDOM_H
