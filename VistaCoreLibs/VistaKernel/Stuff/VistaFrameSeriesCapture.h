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


#ifndef _VISTAFRAMESERIESCAPTURE_H
#define _VISTAFRAMESERIESCAPTURE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaBase/VistaBaseTypes.h>

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaWindow;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaFrameSeriesCapturer repeatedly captures screenshots
 * can be configured to capture every frame, every Nth frame, after an elapsed
 * time, or with a fixed framerate
 */
class VISTAKERNELAPI VistaFrameSeriesCapture : public VistaEventHandler
{
public:
	VistaFrameSeriesCapture( VistaSystem* pSystem, VistaWindow* pWindow = NULL,
								const bool bAutoRegisterUnregisterAsEventHandler = false );
	virtual ~VistaFrameSeriesCapture();

	enum CaptureMode
	{
		CM_INVALID = -1,
		CM_EVERY_FRAME,
		CM_EVERY_NTH_FRAME,
		CM_PERIODICALLY,
		CM_FIXED_FRAMERATE,
	};

	/**
	 * Initializes a capture with the desired mode. The location specifies the directory where
	 * the screenshots will be created, and the filename pattern (with or without extension)
	 * specifies the individual filename. In both the Location and Filename pattern, the tags
	 * %D% and %T% can be used, which will be replaced by date (YYMMDD) or time (HHMMSS),
	 * %M% will be replaced by the milliseconds (3 digits), and %N% which is the cluster node name
	 * FilePatterns can also have %F% and %S%, which will be replaced by the frame and screenshot count
	 * For compatibility, you can also use $$ instead of %%
	 */
	bool InitCaptureEveryFrame( const std::string& sLocation, const std::string& sFilenamePattern );
	bool InitCaptureEveryNthFrame( const std::string& sLocation, const std::string& sFilenamePattern,
								const int nFrame );
	bool InitCapturePeriodically( const std::string& sLocation, const std::string& sFilenamePattern,
								const VistaType::microtime nPeriod );
	bool InitCaptureWithFramerate( const std::string& sLocation, const std::string& sFilenamePattern,
								const float nFramerate );

	CaptureMode GetCaptureMode();

	std::string GetFileLocation();
	std::string GetFilePattern();

	virtual void HandleEvent( VistaEvent* pEvent );

private:
	void ResetReplaceFalgs();
	bool StringReplace( std::string& sString, const std::string& sFind, const std::string& sReplace );
	void ReplaceDate( std::string& sName, const VistaType::microtime nTime );
	void ReplaceTime( std::string& sName, const VistaType::microtime nTime );
	void ReplaceMilliSeconds( std::string& sName, const VistaType::microtime nTime );
	void ReplaceFrameCount( std::string& sName );
	void ReplaceScreenshotCount( std::string& sName );
	void ReplaceNodeName( std::string& sName );

	void MakeScreenshot( const VistaType::microtime nTime );

private:
	VistaSystem* m_pSystem;
	VistaWindow* m_pWindow;
	bool m_bAutoRegisterUnregisterAsEventHandler;
	bool m_bIsRegistered;

	std::string m_sFileLocation;
	std::string m_sFilePattern;

	int m_nScreenshotCount;

	CaptureMode m_eCaptureMode;

	int m_nFrameCylce;
	VistaType::microtime m_nPeriod;

	int m_nLastCaptureFrameIndex;
	VistaType::microtime m_nLastCaptureTime;

	bool m_bNeedsDateReplace;
	bool m_bNeedsTimeReplace;
	bool m_bNeedsMilliReplace;
	bool m_bNeedsFrameReplace;
	bool m_bNeedsScreenshotReplace;
	bool m_bNeedsNodeNameReplace;	
};

#endif //_VistaFrameSeriesCapture_H

