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


#ifndef _VISTASYSTEMCOMMANDS_H
#define _VISTASYSTEMCOMMANDS_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaAspects/VistaExplicitCallbackInterface.h>
#include <VistaBase/VistaStreamUtils.h>

#include <map>
#include <string>
#include <iostream>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaFramerateDisplay;
class VistaGraphicsManager;
class VistaDisplayManager;
class VistaKeyboardSystemControl;
class VistaEventManager;
class VistaInteractionManager;
class VistaInteractionContext;
class VistaClusterMode;
class VistaEyeTester;
class VistaWindow;
class IVistaClusterDataCollect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/


class VISTAKERNELAPI VistaQuitCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaQuitCommand(VistaSystem *pSys);
	virtual bool Do();

	VistaSystem *m_pSys;
};

class VISTAKERNELAPI VistaToggleFramerateCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleFramerateCommand( VistaFramerateDisplay* pFramerateDisplay );

	virtual bool Do();

	VistaFramerateDisplay* m_pFramerateDisplay;
};

class VISTAKERNELAPI VistaToggleCursorCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleCursorCommand( VistaDisplayManager *pDisMgr );
	virtual bool Do();

	VistaDisplayManager *m_pDisMgr;
};

class VISTAKERNELAPI VistaShowAvailableCommands : public IVistaExplicitCallbackInterface
{
public:
	VistaShowAvailableCommands(VistaKeyboardSystemControl *pSysCtrl,
								VistaSystem *pSys);
	virtual bool Do();

	VistaKeyboardSystemControl *m_pCtrl;
	VistaSystem *m_pSys;
};


class VISTAKERNELAPI VistaReloadContextGraphCommand : public IVistaExplicitCallbackInterface
{
public:
    VistaReloadContextGraphCommand( VistaSystem *pSys,
                                     VistaInteractionContext *pCtx,
                                     const std::string &strRoleId,
                                     bool bDumpGraph, bool bWritePorts );

    virtual bool Do();
private:
    VistaInteractionContext    *m_pInteractionContext;
    VistaSystem                *m_pSys;
    std::string m_sRoleId;
    bool m_bDumpGraph,
         m_bWritePorts;
};

class VISTAKERNELAPI VistaDebugContextGraphCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaDebugContextGraphCommand( VistaInteractionContext *pCtx );

	virtual bool Do();
private:
	VistaInteractionContext    *m_pInteractionContext;
};


template<class T>
class TVistaDebugToConsoleCommand : public IVistaExplicitCallbackInterface
{
public:
	TVistaDebugToConsoleCommand( T *pObj )
	: IVistaExplicitCallbackInterface(),
	  m_pObj(pObj)
	  {

	  }

	bool Do()
	{
		(*m_pObj).Debug( vstr::out() );
		return true;
	}

	T *m_pObj;
};

class VISTAKERNELAPI VistaPrintProfilerOutputCommand : public IVistaExplicitCallbackInterface
{
public:
    virtual bool Do();
private:   
};
class VISTAKERNELAPI VistaPrintProfilerOutputForAllNodesCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaPrintProfilerOutputForAllNodesCommand( VistaClusterMode* pClusterMode );
    virtual bool Do();
private:  
	VistaClusterMode* m_pClusterMode;
	IVistaClusterDataCollect* m_pCollect;
};
class VISTAKERNELAPI VistaResetProfilerCommand : public IVistaExplicitCallbackInterface
{
public:
    virtual bool Do();
private:   
};


class VistaToggleFrustumCullingCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleFrustumCullingCommand( VistaGraphicsManager *pGfxMgr );

	bool Do();
private:
	VistaGraphicsManager *m_pGfxMgr;
};

class VistaToggleOcclusionCullingCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleOcclusionCullingCommand( VistaGraphicsManager *pGfxMgr );

	bool Do();
private:
	VistaGraphicsManager *m_pGfxMgr;

};

class VistaToggleBBoxDrawingCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleBBoxDrawingCommand(VistaGraphicsManager *pGfxMgr);

	bool Do();
private:
	VistaGraphicsManager *m_pGfxMgr;
};

class VISTAKERNELAPI VistaChangeEyeDistanceCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaChangeEyeDistanceCommand( const float fChangeValue,
									VistaDisplayManager* pDisplayManager );
	virtual bool Do();
private:
	float					m_fChangeValue;
	VistaDisplayManager*	m_pDisplayManager;
};

class VISTAKERNELAPI VistaToggleEyeTesterCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleEyeTesterCommand( VistaSystem* pSystem );
	~VistaToggleEyeTesterCommand();
	virtual bool Do();
private:
	VistaSystem*			m_pSystem;
	VistaEyeTester*			m_pTester;
};

class VISTAKERNELAPI VistaToggleVSyncCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleVSyncCommand( VistaDisplayManager* pDisplayManager );
	virtual bool Do();
private:
	VistaDisplayManager*	m_pDisplayManager;
};

class VISTAKERNELAPI VistaToggleFullscreenCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaToggleFullscreenCommand( VistaDisplayManager* pDisplayManager );
	virtual bool Do();
private:
	VistaDisplayManager*	m_pDisplayManager;
};

class VISTAKERNELAPI VistaMakeScreenshotCommand : public IVistaExplicitCallbackInterface
{
public:
	VistaMakeScreenshotCommand( VistaDisplayManager* pDisplayManager,
								VistaClusterMode* pCluster,
								const std::string& m_sExtension = "png",
								const std::string& sScreenshotDir = "screenshots" );
	virtual bool Do();
private:
	VistaDisplayManager*	m_pDisplayManager;
	VistaClusterMode*		m_pCluster;
	std::string				m_sExtension;
	std::string				m_sScreenshotDir;
};



/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEMCOMMANDS_H
