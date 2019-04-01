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


#ifndef _VISTAOPENSGNORMALMAPMATERIAL_H__
#define _VISTAOPENSGNORMALMAPMATERIAL_H__

#include <string>
#include "VistaKernelOpenSGExtConfig.h"
#include <VistaKernel/EventManager/VistaEventObserver.h>
#include "VistaBase/VistaColor.h"
class VistaGeometry;
class VistaLightNode;
class VistaEventManager;
/**
 * @todo
 *     - differ light and material colors? (necessary?)
 *     - support for multiple lights? (necessary?)
 */
class VISTAKERNELOPENSGEXTAPI VistaOpenSGNormalMapMaterial
	: public VistaEventObserver
{
public:

	/**
	 * pEvtMgr: is needed for the light-position updates
	 * (not needed if you do light updates explicitly)
	 */
	VistaOpenSGNormalMapMaterial( VistaEventManager *pEvtMgr );
	virtual ~VistaOpenSGNormalMapMaterial();

	/**
	 * pFileName: the image to use as normal map.
	 * convertFrombumpMap: if true, the image is converted to greyscale (if not already)
	 * and treated as bump map -> a normal map is generated from this by calculating the gradients
	 * default: a perlin noise texture is used as bumpmap
	 */
	bool SetNormalMap(const char *pFileName, bool convertFromBumMap = false) const;

	/**
	 * if you give NULL here for the filename or never call the method
	 * no base-texture is used
	 */
	bool SetBaseMap  (const char *pFileName) const;

	void SetAmbientColor (const float &r, const float &g, const float &b,
						  const float &a = 1.0f) const;
	void SetDiffuseColor (const float &r, const float &g, const float &b,
						  const float &a = 1.0f) const;
	void SetSpecularColor(const float &r, const float &g, const float &b,
						  const float &a = 1.0f) const;
	void SetSpecularPower(const float &ex) const;

	/**
	 * explicitly set the light position (in eye space!!!).
	 */
	void SetLightPosition(const float &x, const float &y, const float &z) const;

	/**
	 * get and update the light positon from the given VistaLight object
	 */
	void SetVistaLight   (VistaLightNode *pObj);

	bool GetBaseMapEntry( const float nTexCoordX, const float nTexCoordY, VistaColor& oResult );
	bool GetNormalMapEntry( const float nTexCoordX, const float nTexCoordY, VistaColor& oResult );

	/**
	 * enabling the generatingTangents converts the geometry to single-indexed!
	 */
	void ApplyToGeometry(VistaGeometry *pGeo, bool generateTangents = true);

	//// DEBUG
	//void OverrideShaders(const char *vpFile, const char *fpFile) const;

	/**
	 * lightposition update callback
	 */
	virtual void Notify(const VistaEvent *pEvent);

protected:
	class NativeData;

private:
	VistaEventManager *m_pEvMgr;
	NativeData *m_pData;
	VistaLightNode *m_pTrackedLight;

	static std::string _vs_normalmap;
	static std::string _fs_normalmap;
	static std::string _fs_texturednormalmap;
};

#endif
