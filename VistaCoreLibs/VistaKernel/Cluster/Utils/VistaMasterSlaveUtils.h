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


#ifndef _VISTAMASTERSLAVEUTILS_H
#define _VISTAMASTERSLAVEUTILS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaSerializable.h>

#include <map>
#include <list>
#include <string>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaEvent;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

namespace VistaMasterSlave
{	
	typedef	std::pair<int, int> PortRange;
	typedef std::list<PortRange> FreePortList;

	enum
	{
		DATASYNC_DUMMY,
		DATASYNC_TCP,
		DATASYNC_ZEROMQ,
		DATASYNC_INTERPROC,
	};

	enum
	{
		BARRIER_TCP,
		BARRIER_BROADCAST,
		BARRIER_INTERPROC,
		BARRIER_ZEROMQ,
		BARRIER_DUMMY,
	};

	enum
	{
		SWAPSYNC_NONE,
		SWAPSYNC_DEFAULTBARRIER,
		SWAPSYNC_TCP,
		SWAPSYNC_BROADCAST,
		SWAPSYNC_INTERPROC,
		SWAPSYNC_ZEROMQ,
		SWAPSYNC_DUMMY,
		SWAPSYNC_GSYNCGROUP,
	};

	std::string GetDataSyncModeName( const int m_nDataSyncMethod );
	std::string GetSwapSyncModeName( const int m_nSwapSyncMethod );
	std::string GetBarrierModeName( const int m_nBarrierMethod );

	void FillPortListFromIni( const std::string& sEntry,
							FreePortList& vecPorts );
	int GetFreePortFromPortList( FreePortList& vecPorts );
	
	
	class VISTAKERNELAPI Message : public IVistaSerializable
	{
	public:
		Message();
		virtual ~Message();

		// Setup: register allowed Events for receiving
		bool RegisterEventType( VistaEvent* pEvent );

		// Write
		void SetFrameCount( int nCount );
		void SetEventMsg( const VistaEvent* pEvent );
		void SetStartFrameMsg( VistaType::systemtime nTime );
		void SetEndFrameMsg( VistaType::systemtime nTime );
		void SetQuitMsg( VistaType::systemtime nTime );

		// Read	
		enum CMSG_TYPE
		{
			CMSG_INVALID = -1,
			CMSG_STARTFRAME,
			CMSG_EVENT,
			CMSG_ENDFRAME,
			CMSG_QUIT,
		};
		CMSG_TYPE GetType() const;
		int GetFrameCount() const;
		VistaType::systemtime GetClock() const;
		VistaEvent* GetEvent() const;

		virtual int Serialize( IVistaSerializer& ) const;
		virtual int DeSerialize( IVistaDeSerializer& );
		virtual std::string GetSignature() const;

	private:
		CMSG_TYPE				m_nType;
		int						m_nFrameCount;
		VistaType::systemtime	m_nClock;

		int						m_nEventType;
		const VistaEvent*		m_pSendEvent;
		std::map<int, VistaEvent*>	m_mapRegisteredEvents;
	};
}




/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMASTERSLAVEUTILS_H

