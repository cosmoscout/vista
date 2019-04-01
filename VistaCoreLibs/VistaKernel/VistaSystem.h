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


#ifndef _VISTASYSTEM_H
#define _VISTASYSTEM_H

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
// system includes
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaAspects/VistaPropertyList.h>
#include <VistaKernel/EventManager/VistaEventHandler.h>

#include <list>
#include <iostream>
#include <vector>
#include <map>

// prototypes
class   VistaSystem;
VISTAKERNELAPI std::ostream & operator<< ( std::ostream &, const VistaSystem & );


/*============================================================================*/
/*  MACROS AND DEFINES                                                        */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
//class VistaDisplay;

class VistaSensor;
class VistaGraphicsManager;
class VistaEventManager;
class VistaDisplayManager;
class VistaInteractionManager;
class VistaFrameLoop;
class VistaFramerateDisplay;

class VistaClusterMode;

class VistaKernelMsgPort;
class VistaExternalMsgEvent;
class VistaInputEvent;
class VistaCommandEvent;
class DLVistaDataSink;
class IVistaSystemClassFactory;
class VistaKeyboardSystemControl;
class VistaClusterMode;
class VistaDriverMap;

class VistaDriverPropertyConfigurator;
class VdfnObjectRegistry;
class VistaUserPlatform;
class VistaDisplaySystem;
class VistaInteractionContext;
class VistaRuntimeLimiter;
class VistaFrameSeriesCapture;

class VdfnGraph;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaSystem is the baseclass which handles all functions from Vista. When
 * this class is created an the init- and run-function are called the simulation
 * scenegraph is display an the 3D-universe lives. If tracker or displaydevices
 * are specified in the applicaton/configfiles/vista.ini they are used to
 * generate the actuall configuration. But you can also start vista without
 * settings in the vista.ini, But then you to create the classes on your own.
 *
 */
class VISTAKERNELAPI VistaSystem : public VistaEventHandler
{
public:
	VistaSystem();
	virtual ~VistaSystem();

	// ############################################################################
	// USER CONFIGURABLE INI FILE STRUCTURES API
	// ############################################################################
	bool        SetIniFile( const std::string& sConfigNew );
	std::string GetIniFile() const;

	bool        SetDisplayIniFile( const std::string& sDisplayConfig );
	std::string GetDisplayIniFile() const;

	bool        SetGraphicsIniFile( const std::string& sGraphicsConfig );
	std::string GetGraphicsIniFile() const;

	bool        SetInteractionIniFile( const std::string& sInterConfig );
	std::string GetInteractionIniFile() const;

	bool        SetClusterIniFile( const std::string& sClusterConfig );
	std::string GetClusterIniFile() const;

	const std::list<std::string>& GetIniSearchPaths() const;
	bool                   SetIniSearchPaths( const std::list<std::string> &liSearchPaths );
	static std::string     GetSystemSectionName();
	const std::string&     GetApplicationName() const;


	// ############################################################################
	// RUN-TIME USER API
	// ############################################################################
	bool IntroMsg( std::ostream* pStream = NULL ) const;
	bool Init( int argc, char *argv[] );
	bool Run();
	bool Quit();

	// ##########################################################
	// GENERAL INTERACTION API
	// ##########################################################
	VistaKeyboardSystemControl* GetKeyboardSystemControl() const;
	VdfnObjectRegistry* GetDfnObjectRegistry() const;
	VistaUserPlatform* GetPlatformFor( VistaDisplaySystem *pDisplaySystem ) const;

	// ############################################################################
	// ACCESS TO INTERNAL STRUCTURES API
	// ############################################################################
	/**
	 * Returns the vista event manager. It serves as an entry point into the
	 * system for newly generated events. In addition it is responsible for
	 * registering and managing event types, event handlers, and event
	 * observers.
	 * The EventManager is present right after the creation of a VistaSystem
	 * instance (which is not true for the other manager instances).
	 * It defines the heart-beat of a ViSTA application.
	 */
	VistaEventManager* GetEventManager() const;
	VistaGraphicsManager* GetGraphicsManager () const;
	VistaDisplayManager* GetDisplayManager () const;
	VistaInteractionManager* GetInteractionManager() const;
	VistaDriverMap* GetDriverMap() const;
	VistaDriverPropertyConfigurator* GetDriverConfigurator() const;
	IVistaSystemClassFactory* GetSystemClassFactory() const;

	VistaFramerateDisplay* GetFramerateDisplay() const;

	VistaFrameLoop* GetFrameLoop() const;
	/**
	 * Replaces the current frame loop with a new one, allowing to change
	 * the actions/event order. Can be called before or after the
	 * VistaSystem's initialization. However, if called after VistaSystem
	 * initialization, it has to be manually initialized, and inconsistencies
	 * with framecount, framerate, and profiling may arise
	 */
	void SetFrameLoop( VistaFrameLoop* pLoop, bool bDeleteExisting );

	// ############################################################################
	// CLUSTER API
	// ############################################################################
	VistaClusterMode* GetClusterMode() const;
	bool GetIsClusterLeader() const;
	bool GetIsClusterFollower() const;

	double GetFrameClock() const;
	double GetStartUpTime() const;
	int GenerateRandomNumber( int iLowerBound, int iMaxBound ) const;
	void SetRandomSeed( int iSeed );

	// ############################################################################
	// MSGPORT API
	// ############################################################################
	/**
	 * Set the external message port. Returns the formerly set
	 * message port. By making this call, ownership of the port
	 * goes to the system, i.e. the port is being destroyed upon
	 * system destruction.
	 */
	VistaKernelMsgPort* SetExternalMsgPort( VistaKernelMsgPort *pPort );

	void DisconnectExternalMsgPort();
	bool HasExternalMsgPort() const;
	bool HasExternalConnection() const;


	bool                 RegisterMsgPort(VistaKernelMsgPort *pPort);
	VistaKernelMsgPort *UnregisterMsgPort(VistaKernelMsgPort *);


	// ############################################################################
	// USER FEEDBACK PROGRESS API
	// ############################################################################
	bool IndicateApplicationProgress(int iAppProgress,
									 const std::string& sProgressMessage);

	// INHERITED FROM EVENTHANDLER API
	virtual void HandleEvent(VistaEvent *pEvent);

	virtual void Debug( std::ostream & out ) const;
	bool PrintMsg( const std::string& strMsg, std::ostream* pStream = 0 ) const;
	bool PrintMsg( const char* pMsg, std::ostream* pStream = 0 ) const;
	
private:
	bool ArgParser( int argc, char *argv[] );
	bool ArgHelpMsg( const std::string& sAppName, std::ostream * pStream = NULL ) const;

	bool DoInit( int argc, char** argv );

	void RegisterSigTermCallbacks();
	void UnregisterSigTermHandlers();

	void RegisterEventTypes();

	bool LoadIniFiles();
	void CreateClusterMode();
	bool SetupCluster();
	bool SetupMessagePort();
	bool SetupEventManager();
	bool SetupDisplayManager();
	bool SetupGraphicsManager();
	bool SetupInteractionManager();

	bool SetupOutputStreams();
	bool SetupBasicInteraction();
	VistaInteractionContext* SetupInteractionContext( const std::string& strContextSec );
	void BindKeyboardActions();
	void CreateFramerateDisplay();
	void RegisterConfigurators();
	void CreateDeviceDrivers();
	bool LoadDriverPlugin( const std::string& sDriverType,
							const std::string& sPluginName = "",
							const std::string& sTranscoderName = "" );

	bool IndicateSystemProgress( const std::string& sProgressMessageText,
								bool bDone );

	std::string FindFileInIniSearchPath(const std::string& file,
										std::ostream *logger = NULL) const;
private:
	IVistaSystemClassFactory*			m_pSystemClassFactory;

	VistaEventManager*					m_pEventManager;

	VistaExternalMsgEvent*				m_pExternalMsg;
	int									m_iInitProgressIndicator;
	VistaKernelMsgPort*					m_pMessagePort;
	std::list<VistaKernelMsgPort*>		m_liAppMsgPorts;

	VistaDisplayManager*				m_pDisplayManager;
	VistaInteractionManager*			m_pInteractionManager;
	VistaGraphicsManager*				m_pGraphicsManager;
	VistaCommandEvent*					m_pCommandEvent;
	VistaKeyboardSystemControl*			m_pKeyboardSystemControl;
	VistaDriverMap*						m_pDriverMap;
	VdfnObjectRegistry*					m_pDfnObjects;
	VistaDriverPropertyConfigurator*	m_pConfigurator;
	VistaClusterMode*					m_pClusterMode;
	VistaFrameLoop*						m_pFrameLoop;
	VistaFramerateDisplay*				m_pFramerateDisplay;

	VistaRuntimeLimiter*				m_pRuntimeLimiter;

	bool								m_bAllowStreamColors;

	std::map<VistaDisplaySystem*, VistaUserPlatform*> m_mapUserPlatforms;
	std::list<VistaInteractionContext*> m_liCreateCtxs;

	struct DllHelper*					m_pDllHlp;

	bool								m_bInitialized;

	// store the absolute pathes of the ini-files
	std::string                 m_sVistaConfigFile;
	std::string					m_sGraphicsConfigFile;
	std::string					m_sDisplayConfigFile;
	std::string					m_sInteractionConfigFile;
	std::string					m_sClusterConfigFile;
	
	std::string					m_sModelFile;
	float						m_nModelScale;

	bool						m_bUseNewClusterMaster;
	int							m_nClusterNodeType;
	std::string					m_sClusterNodeName;
	bool						m_bRecordOnlyMaster;
	std::string					m_sRecordFile;
	std::string					m_sApplicationName;

	int							m_eFrameCaptureMode;
	double						m_nFrameCaptureParameter;
	std::string					m_sCaptureFramesFilePattern;
	std::vector< VistaFrameSeriesCapture* >	m_vecFrameCaptures;

	VistaPropertyList			m_oVistaConfig;
	VistaPropertyList			m_oGraphicsConfig;
	VistaPropertyList			m_oDisplayConfig;
	VistaPropertyList			m_oInteractionConfig;
	VistaPropertyList			m_oClusterConfig;

	std::list<std::string>		m_liSearchPathes;
	bool						m_bLockSearchPath;

	std::list<std::string>		m_liDriverPluginPathes;

	VistaType::systemtime		m_dStartClock;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

VISTAKERNELAPI VistaSystem * GetVistaSystem();

#endif // _VISTASYSTEM_H
