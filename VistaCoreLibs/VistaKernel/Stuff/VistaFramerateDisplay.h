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


#ifndef _VISTAFRAMERATEDISPLAY_H
#define _VISTAFRAMERATEDISPLAY_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/EventManager/VistaEventObserver.h>

#include <VistaBase/VistaColor.h>
#include <VistaBase/VistaTimer.h>

#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplayManager;
class Vista2DText;
class VistaFrameLoop;
class VistaPropertyList;
class VistaVector3D;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaFramerateDisplay : public VistaEventObserver
{
public:
	VistaFramerateDisplay( VistaDisplayManager* pDisplayManager,
							VistaFrameLoop* pFrameLoop,
							const bool bOnAllViewports = false );
	VistaFramerateDisplay( VistaDisplayManager* pDisplayManager,
							VistaFrameLoop* pFrameLoop,
							const VistaPropertyList& oConfig );
	virtual ~VistaFramerateDisplay();

	virtual void Notify( const VistaEvent *pEvent );

	VistaColor GetColor() const;
	void SetColor( const VistaColor& oValue );
	
	int GetTextSize() const;
	void SetTextSize( const int oValue );

	void GetPosition( float afPosition[2] ) const;
	VistaVector3D GetPosition() const;
	void SetPosition( const float anPosition[2] );
	void SetPosition( const VistaVector3D& v3Position );

	VistaType::microtime GetUpdateFrequency() const;
	void SetUpdateFrequency( const VistaType::microtime& oValue );

	std::string GetPrefix() const;
	void SetPrefix( const std::string& oValue );
	std::string GetPostfix() const;
	void SetPostfix( const std::string& oValue );

	bool GetIsEnabled() const;
	void SetIsEnabled( const bool& oValue );

private:
	void PrepareText();
	void SetText();

private:
	VistaFrameLoop* m_pFrameLoop;

	VistaColor m_oColor;
	int m_nSize;
	float m_anPosition[2];
	bool m_bEnabled;
	VistaType::microtime m_nUpdateFrequency;
	std::string m_sPrefix;
	std::string m_sPostfix;
	std::vector<Vista2DText*> m_vecDisplayTexts;
	std::string m_sText;
	VistaType::systemtime m_nLastUpdate;
};

#endif //_VISTAFRAMERATEDISPLAY_H

