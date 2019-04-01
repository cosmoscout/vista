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
// $Id: VistaAutoBuffer.h 31862 2012-08-31 22:54:08Z ingoassenmacher $

#ifndef _VISTAIMAGEANDTEXTURECOREFACTORY_H_
#define _VISTAIMAGEANDTEXTURECOREFACTORY_H_

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>

#include "VistaImage.h"
#include "VistaGLTexture.h"

#include <map>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaMutex;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI IVistaImageAndTextureCoreFactory
{
public:
	static void SetSingleton( IVistaImageAndTextureCoreFactory* pFactory );
	static IVistaImageAndTextureCoreFactory* GetSingleton();

	virtual IVistaImageCore* CreateImageCore() = 0;
	virtual IVistaTextureCore* CreateTextureCore() = 0;

	VistaImage* GetCachedImage( const std::string& sFilename );
	void StoreCachedImage( const std::string& sFilename, const VistaImage& oImage );

protected:
	IVistaImageAndTextureCoreFactory();
	virtual ~IVistaImageAndTextureCoreFactory();

	std::map<std::string, VistaImage> m_mapCachedImages;
	VistaMutex* m_pCachedImagesMutex;
};



/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif /* _VISTAIMAGEANDTEXTURECOREFACTORY_H_ */
