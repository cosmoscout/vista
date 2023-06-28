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
// $Id: VistaAutoBuffer.cpp 31862 2012-08-31 22:54:08Z ingoassenmacher $

#include "VistaImage.h"

#include "VistaNativeGLImageAndTextureFactory.h"

#include <VistaInterProcComm/Concurrency/VistaMutex.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

namespace {
static IVistaImageAndTextureCoreFactory* g_pSingleton = NULL;
};
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaImageAndTextureCoreFactory::IVistaImageAndTextureCoreFactory()
    : m_pCachedImagesMutex(new VistaMutex) {
}

IVistaImageAndTextureCoreFactory::~IVistaImageAndTextureCoreFactory() {
  delete m_pCachedImagesMutex;
}

void IVistaImageAndTextureCoreFactory::SetSingleton(IVistaImageAndTextureCoreFactory* pFactory) {
  delete g_pSingleton;
  g_pSingleton = pFactory;
}

IVistaImageAndTextureCoreFactory* IVistaImageAndTextureCoreFactory::GetSingleton() {
  if (g_pSingleton == NULL) {
    g_pSingleton = new VistaNativeOpenGLImageAndTextureCoreFactory();
  }
  return g_pSingleton;
}

VistaImage* IVistaImageAndTextureCoreFactory::GetCachedImage(const std::string& sFilename) {
  VistaMutexLock                              oLock(*m_pCachedImagesMutex);
  std::map<std::string, VistaImage>::iterator itCached = m_mapCachedImages.find(sFilename);
  if (itCached != m_mapCachedImages.end())
    return &(*itCached).second;
  else
    return NULL;
}

void IVistaImageAndTextureCoreFactory::StoreCachedImage(
    const std::string& sFilename, const VistaImage& oImage) {
  VistaMutexLock oLock(*m_pCachedImagesMutex);
  m_mapCachedImages[sFilename] = oImage;
}

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
