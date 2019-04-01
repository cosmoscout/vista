/*============================================================================*/
/*                    ViSTA VR toolkit - OpenCV2 driver                  */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published         */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/


#ifndef __VISTAOPENCVFACETRACKDRIVER_H
#define __VISTAOPENCVFACETRACKDRIVER_H

#if defined(WIN32) && !defined(VISTAOPENCVFACETRACKDRIVER_STATIC)
#ifdef VISTAOPENCVFACETRACKDRIVER_EXPORTS
#define VISTAOPENCVFACETRACKDRIVERAPI __declspec(dllexport)
#else
#define VISTAOPENCVFACETRACKDRIVERAPI __declspec(dllimport)
#endif
#else // no Windows or static build
#define VISTAOPENCVFACETRACKDRIVERAPI
#endif


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverGenericParameterAspect.h>

#include <VistaBase/VistaTimer.h>

#include <string>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverThreadAspect;
class VistaDriverInfoAspect;

namespace cv
{
	class VideoCapture;
	class CascadeClassifier;
}
struct _IplImage;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAOPENCVFACETRACKDRIVERAPI VistaOpenCVFaceTrackDriver : public IVistaDeviceDriver
{
public:

	struct FacePoseMeasure 
	{
		float	m_a3fPosition[3];
	};

	VistaOpenCVFaceTrackDriver( IVistaDriverCreationMethod* pCreationMethod );
	virtual ~VistaOpenCVFaceTrackDriver();


	class FaceTrackParameters : public VistaDriverGenericParameterAspect::IParameterContainer
	{
		REFL_DECLARE
	public:
		FaceTrackParameters( VistaOpenCVFaceTrackDriver* pDriver );

		enum
		{
			MSG_CAPTUREDEVICE_CHG = VistaDriverGenericParameterAspect::IParameterContainer::MSG_LAST,
			MSG_FRAMERATE_CHG,
			MSG_HEIGHT_CHG,
			MSG_WIDTH_CHG,
			MSG_EYEDISTANCCE_CHG,
			MSG_NORMALIZEDEYEDISTANCE_CHG,
			MSG_SHOWDEBUGWINDOW_CHG,
			MSG_CLASSIFIERFILE_CHG,
			MSG_LAST
		};

		int GetCaptureDevice() const;
		bool SetCaptureDevice( int nDeviceID );

		int GetWidth() const;
		bool SetWidth( int nWidth );

		int GetHeight() const;
		bool SetHeight( int nHeight );	

		float GetFrameRate() const;
		bool SetFrameRate( float nFrameRate );

		float GetEyeDistance() const;
		bool SetEyeDistance( float fEyeDistance );

		float GetNormalizedEyeDistance() const;
		bool SetNormalizedEyeDistance( float nNormailzedDistance );

		bool GetShowDebugWindow() const;
		bool SetShowDebugWindow( bool bShowDebug );

		std::string GetClassifierFile() const;
		bool SetClassifierFile( const std::string& sFilename );
	private:
		VistaOpenCVFaceTrackDriver *m_pDriver;

		int m_nDeviceID;
		int m_nWidth;
		int m_nHeight;
		float m_nFramerate;
		float m_nEyeDistance;
		float m_nNormalizedEyeDistance;
		bool m_bShowDebugWindow;
		std::string m_sClassifierFile;
	};

	void UpdateEyeDistanceFactor();
	void ConnectToOpenCVDevice();
	void ReadCascadeClassifier();
	void ChangeShowDebugWindow();
protected:
	
	virtual bool DoConnect();
	virtual bool DoDisconnect();

	virtual bool PhysicalEnable( bool bEnable );
	bool DoSensorUpdate( VistaType::microtime nTs );
private:
	VistaDriverInfoAspect*		m_pInfo;
	VistaDriverThreadAspect*	m_pThread;
	VistaDriverGenericParameterAspect* m_pParams;

	cv::VideoCapture*			m_pCapture;
	cv::CascadeClassifier*		m_pCascade;
	bool						m_bDebugWindow;
	float						m_nEyeDistanceFactor;
	float						m_nPosNormalizeFactor;
	int							m_nNormalizeWidth;
	int							m_nNormalizeHeight;
	bool						m_bPreConnect;

	int							m_nLastSizeX;
	int							m_nLastSizeY;
	int							m_nLastPosX;
	int							m_nLastPosY;
	float						m_fSizeTolerance;

	VistaWeightedAverageTimer	m_oTimer;

	void*						m_pCLEyeCamera;
	_IplImage*					m_pImage;
};


class VISTAOPENCVFACETRACKDRIVERAPI VistaOpenCVFaceTrackDriverCreationMethod : public IVistaDriverCreationMethod
{
public:
	VistaOpenCVFaceTrackDriverCreationMethod(IVistaTranscoderFactoryFactory *pFac );
	virtual IVistaDeviceDriver *CreateDriver();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

#endif //__VISTAOPENCVFACETRACK_H

