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


#include "VistaFastrakDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverProtocolAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverReferenceFrameAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverInfoAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>

#include <VistaInterProcComm/Connections/VistaConnection.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cstring>
#include <cstdio>

#include <string>
#include <stdlib.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


// ############################################################################
// FASTRAK COMMANDSET VARIANTS INTERFACE
// ############################################################################
class IVistaFastrakCommandSet
{
public:
	virtual ~IVistaFastrakCommandSet() {}

	virtual int  CmdGetActiveStations() = 0;
	virtual int  CmdGetTotalNumberOfStations() = 0;
	virtual bool CmdEnableUnit(unsigned int iUnit) = 0;
	virtual bool CmdDisableUnit(unsigned int iUnit) = 0;

	virtual bool CmdEnableBinaryMode() = 0;
	virtual bool CmdEnableAsciiMode() = 0;

	virtual bool CmdEnableMetricMode() const = 0;
	virtual bool CmdEnableInchMode() = 0;

	virtual bool CmdEnableContinuousMode() = 0;
	virtual bool CmdEnablePollMode(bool bForce) = 0;

	virtual bool CmdPause() = 0;
	virtual bool CmdResume() = 0;

	virtual bool CmdReset() = 0;
	virtual bool CmdSetStreamMode() = 0;
	virtual bool CmdSetPollMode(bool bForce) = 0;

	virtual bool CmdStylusButtonMode(unsigned int iMode, int iUnit) = 0;
	virtual bool CmdSetPosQuaternionMode() = 0;
	virtual bool CmdSetPosQuaternionMode(unsigned int iUnit) = 0;

	virtual bool CmdResetReferenceFrame(unsigned int iUnit) = 0;
	virtual bool CmdSetHemisphere(int nHemisphereCode) = 0;
	virtual bool CmdSetHemisphere(unsigned int iUnit, float x = 1, float y = 0, float z = 0) = 0;

	virtual bool CmdPollValues() = 0;

	virtual bool CmdSetUpdateRate() = 0;

	virtual std::string GetDeviceInfoString( std::vector<std::string> &vecStore ) = 0;

	/**
	 * @param iStation 0xFFFFFFFF for all stations
	 */
	virtual bool CmdSetAlignmentReferenceFrame(int iStation,
				float fOx = 0.0f, float fOy = 0.0f, float fOz= 0.0f,
				float fXx = 1.0f, float fXy = 0.0f, float fXz = 0.0f,
				float fYx = 0.0f, float fYy = 1.0f, float fYz = 0.0f) = 0;


	virtual bool CmdSetBoresight(int iStation, float fAzimuth, float fElevation, float fRoll, bool bResetOrigin) = 0;
	virtual bool CmdSetSourceMountingFrame(float fAzimuth, float fElevation, float fRoll) = 0;
	virtual bool CmdSetCompensation(int iMap) = 0;

	virtual long CmdGetActiveStationMask() const = 0;
	virtual long CmdGetStationsMask() const = 0;


	virtual float GetDataFromBinary(unsigned char *source) = 0;
	virtual float GetDataFromAscii(unsigned char *source, int length) = 0;


	virtual void Flush() const = 0;

	enum eMode
	{
		MD_NONE=-1,
		MD_ASCII=0,
		MD_BINARY
	};

	eMode GetMode() const;
	virtual eMode CmdGetCurrentMode() const = 0;

	virtual bool CmdInitSequence() = 0;
	virtual std::vector<std::string> GetInfoStringSet() const = 0;
protected:
	IVistaFastrakCommandSet(VistaDriverConnectionAspect *pCon)
		: m_pConnection(pCon),
		  m_eMode(MD_NONE),
		  m_nMessageLength(0)
	{
	}


	VistaDriverConnectionAspect     *m_pConnection;
	eMode m_eMode;
	int   m_nMessageLength;
};

class CommonCommandSet : public IVistaFastrakCommandSet
{
public:
	CommonCommandSet(VistaDriverConnectionAspect *pCon,
		               IVistaDriverReferenceFrameAspect *pRefFrame)
		: IVistaFastrakCommandSet(pCon),
		 m_bPollMode(true),
		 m_lActiveStations(0),
		 m_lStations(0),
		 m_pRefFrame(pRefFrame)
	{
	}

	virtual ~CommonCommandSet()
	{
	}


	virtual bool SetDeviceName(std::string sName)
	{
		m_sDeviceName = sName;
		return true;
	}

	virtual std::string GetDeviceName() const
	{
		return m_sDeviceName;
	}

	virtual int  CmdGetActiveStations() { return m_lActiveStations; }

	virtual int  CmdGetTotalNumberOfStations() { return 0; }

	virtual bool CmdEnableUnit(unsigned int iUnit) { return false; }
	virtual bool CmdDisableUnit(unsigned int iUnit) { return false; }

	virtual bool CmdEnableBinaryMode() { return false; }

	virtual bool CmdEnableAsciiMode()
	{
		char data[3];
		data[0] = 'F';
		data[1] = '0';
		data[2] = 0x0D;
		return m_pConnection->SendCommand( 0, &data[0], 3, 350 );
	}

	virtual bool CmdEnableMetricMode()  const
	{
		char data[3];
		data[0] = 'U';
		data[1] = '1';
		data[2] = 0x0D;

		return m_pConnection->SendCommand(0, &data[0], 3, 350 );
	}

	virtual bool CmdEnableInchMode()
	{
		char data[3];
		data[0] = 'U';
		data[1] = '0';
		data[2] = 0x0D;

		return m_pConnection->SendCommand(0, &data[0], 3, 350 );
	}

	virtual bool CmdEnableContinuousMode()
	{
		return false;
	}

	virtual bool CmdEnablePollMode(bool bForce)
	{
		return false;
	}

	virtual bool CmdPause()
	{
		if(m_bPollMode)
			return true; // we do not object here...
		// seem to be in continous mode
		// set to poll mode again
		return (m_bPollMode = CmdPollValues());
	}

	virtual bool CmdResume()
	{
		if(!m_bPollMode)
			return true; // seem to be streaming, ok...

		// we are not streaming, go to streaming mode...
		char data[2];
		data[0] = 'C';
		data[1] = 0x0D;

		return (m_bPollMode = !m_pConnection->SendCommand( 0, &data[0], 2, 0 ));
	}

	virtual bool CmdReset()
	{

		if(!m_bPollMode)
			CmdPause();

		char data[3];
		// write ^Y to the device (restart)
		data[0] = 0x19;
		data[1] = 0x0D;
		if(m_pConnection->SendCommand( 0, &data[0], 2, 100 ))
		{
			VistaConnection *pCon = m_pConnection->GetConnection(0);

			std::string strAnswer;
			pCon->ReadDelimitedString( strAnswer, 0x0A ); // liberty resets
			//vstr::outi() << strAnswer << std::endl;
			pCon->ReadDelimitedString( strAnswer, 0x0A ); // gives an endline
			//vstr::outi() << strAnswer << std::endl;
			pCon->ReadDelimitedString( strAnswer, 0x0A ); // says 'liberty restarted'
			//vstr::outi() << strAnswer << std::endl;


			// wait a sec to let the device reset
			VistaTimeUtils::Sleep(1000);
			return true;
		}
		return false;
	}

	virtual bool CmdSetStreamMode()
	{
		return CmdResume();
	}

	virtual bool CmdSetPollMode(bool bForce)
	{
		return CmdPause();
	}

	virtual bool CmdStylusButtonMode(unsigned int iMode, int iUnit)  { return false; }


	virtual bool CmdSetPosQuaternionMode()
	{
		char buffer[32];
		sprintf(buffer,"O*,2,7,10,8,9,10,11,1%c", 0x0D);
		m_pConnection->SendCommand( 0, buffer, (unsigned int)strlen(buffer), 150 );

		if(m_pConnection->GetConnection()->HasPendingData())
		{
			// ? error ?
			std::string sResp;
			m_pConnection->GetConnection()->ReadDelimitedString( sResp, 0x0A );
			vstr::outi() << sResp << std::endl;
		}
		return true;
	}

	virtual bool CmdSetPosQuaternionMode(unsigned int iUnit)
	{
		return false;
	}

	virtual bool CmdResetReferenceFrame(unsigned int iUnit)
	{
		char data[4];

		sprintf( data, "%c%d%c", 0x12, (int)iUnit, 0x0D );
		return m_pConnection->SendCommand(0, data, 3, 100);
	}

	virtual bool CmdSetHemisphere(int nHemisphereCode)
	{
		VistaVector3D pntHemisphere;
		switch(nHemisphereCode)
		{
		case IVistaDriverReferenceFrameAspect::HS_UP:
			{
				// Upper hemisphere is tracker's -Z
				pntHemisphere[2] = -1;
				break;
			}
		case IVistaDriverReferenceFrameAspect::HS_DOWN:
			{
				// Down hemisphere is tracker's Z
				pntHemisphere[2] = 1;
				break;
			}
		case IVistaDriverReferenceFrameAspect::HS_FRONT:
			{
				// Forward means away from the cable, so it's X
				pntHemisphere[0] = 1;
				break;
			}
		case IVistaDriverReferenceFrameAspect::HS_AFT:
			{
				// Aft is direction cable, so -X
				pntHemisphere[0] = -1;
				break;
			}
		case IVistaDriverReferenceFrameAspect::HS_LEFT:
			{
				// Right is left from front, Y
				pntHemisphere[1] = 1;
				break;
			}
		case IVistaDriverReferenceFrameAspect::HS_RIGHT:
			{
				// Left means right from front view
				pntHemisphere[1] = -1;
				break;
			}
		default:
			return false;
		}

		long nActiveStationMask = CmdGetActiveStationMask();

		for(unsigned int i=0; i<sizeof(long)*8; ++i)
		{
			long n = 1<<i;
			if(nActiveStationMask & n)
			{
				CmdSetHemisphere( i+1, pntHemisphere[0],pntHemisphere[1],pntHemisphere[2] );
			}
		}

		return true;
	}


	virtual bool CmdSetHemisphere(unsigned int iUnit, float x , float y , float z )
	{
		char data[25];
		sprintf(data, "H%d,%+2.3f,%+2.3f,%+2.3f%c", (int)iUnit, x, y, z, 0x0d);
		return m_pConnection->SendCommand( 0, data, (unsigned int)strlen(data), 100 );
	}

	virtual bool CmdPollValues()
	{
		char data[1];
		data[0] = 'P';
		//data[1] = 0x0D;
		//data[2] = 0x0A;
		return m_pConnection->SendCommand( 0, &data[0], 1, 0 );
	}

	virtual bool CmdSetAlignmentReferenceFrame(int iStation,
					float fOx = 0.0f, float fOy = 0.0f, float fOz= 0.0f,
					float fXx = 1.0f, float fXy = 0.0f, float fXz = 0.0f,
					float fYx = 0.0f, float fYy = 1.0f, float fYz = 0.0f)
	{
		if(CmdResetReferenceFrame(iStation))
		{
			char data[59];
			int nCount = sprintf(data, "A%d,%+3.2f,%+3.2f,%+3.2f,%+3.2f,%+3.2f,%+3.2f,%+3.2f,%+3.2f,%+3.2f%c",
				   iStation, fOx, fOy, fOz, fXx, fXy,fXz, fYx,fYy,fYz, 0x0d);

			return m_pConnection->SendCommand(0, data, nCount, 100);
		}
		return false;

	}


	virtual bool CmdSetAlignmentReferenceFrameForAll(
					float fOx = 0.0f, float fOy = 0.0f, float fOz= 0.0f,
					float fXx = 1.0f, float fXy = 0.0f, float fXz = 0.0f,
					float fYx = 0.0f, float fYy = 1.0f, float fYz = 0.0f)
	{
		for(int n=0; n < m_lStations; ++n)
		{
			CmdSetAlignmentReferenceFrame(n+1,
					fOx, fOy, fOz,
					fXx, fXy, fXz,
					fYx, fYy, fYz);
		}
		return true;
	}

	virtual bool CmdSetBoresight(int iStation, float fAzimuth, float fElevation, float fRoll, bool bResetOrigin)
	{
		char data[33];
		int nCount = sprintf( data, "B%d,%3.2f,%3.2f,%3.2f,%d%c",
			iStation, fAzimuth, fElevation, fRoll, bResetOrigin ? 1:0, 0x0d );
		return m_pConnection->SendCommand(0, data, nCount, 100);
	}

	virtual bool CmdSetSourceMountingFrame(float fAzimuth, float fElevation, float fRoll)
	{
		char data[25];
		int nCount = sprintf( data, "G%3.2f,%3.2f,%3.2f,%c",
			fAzimuth, fElevation, fRoll, 0x0d );
		return m_pConnection->SendCommand(0, data, nCount, 100);
	}

	virtual bool CmdSetCompensation(int iMap) { return false; }

	virtual float GetDataFromBinary(unsigned char *source) { return false; }
	virtual float GetDataFromAscii(unsigned char *source, int length)  { return false; }
	

	virtual long CmdGetActiveStationMask() const
	{
		Flush();
		char data[3];
		data[0] = 0x15;
		data[1] = '0';
		data[2] = 0x0D;

		struct _StationResponse
		{
			char station_dg1;
			char station_dg2;
			char dummy;
			char error;
			char blank;
			char hexBm[4];
			char hexBmActive[4];
			char crlf[2];
		};

		if(m_pConnection->SendCommand( 0, &data[0], 3, 200 ))
		{
			_StationResponse res;
			m_pConnection->GetConnection()->Receive( &res, sizeof(_StationResponse) );
			char hexBm[5];
			hexBm[0] = res.hexBmActive[0];
			hexBm[1] = res.hexBmActive[1];
			hexBm[2] = res.hexBmActive[2];
			hexBm[3] = res.hexBmActive[3];
			hexBm[4] = 0x0; // null-terminate
			return strtol( hexBm, NULL, 16 ); // to the base of 16
		}
		return 0;
	}

	virtual long CmdGetStationsMask() const
	{
		Flush();
		char data[3];
		data[0] = 0x15;
		data[1] = '0';
		data[2] = 0x0D;

		struct _StationResponse
		{
			char station_dg1;
			char station_dg2;
			char dummy;
			char error;
			char blank;
			char hexBm[4];
			char hexBmActive[4];
			char crlf[2];
		};

		if(m_pConnection->SendCommand( 0, &data[0], 3, 200 ))
		{
			_StationResponse res;
			m_pConnection->GetConnection()->Receive( &res, sizeof(_StationResponse) );
			char hexBm[5];
			hexBm[0] = res.hexBm[0];
			hexBm[1] = res.hexBm[1];
			hexBm[2] = res.hexBm[2];
			hexBm[3] = res.hexBm[3];
			hexBm[4] = 0x0; // null-terminate
			return strtol( hexBm, NULL, 16 ); // to the base of 16
		}

		return 0;
	}


	virtual bool CmdInitSequence()
	{
		VistaConnection *pCon = m_pConnection->GetConnection();
		if(pCon->HasPendingData())
			Flush();

		// write ^W0 (factory default)
		char data[3];
		data[0] = 0x18;
		data[1] = '0';
		data[2] = 0x0D;
		if(pCon->Send( data, 3 ) == 3)
		{
			VistaTimeUtils::Sleep( 100 );
			// write ^Y to the device (restart)
			data[0] = 0x19;
			data[1] = 0x0D;
			if(pCon->Send( data, 2 ) == 2)
			{
				std::string strAnswer;
				pCon->ReadDelimitedString( strAnswer, 0x0A ); // liberty resets
				//vstr::outi() << "answer : " << strAnswer << std::endl;
				pCon->ReadDelimitedString( strAnswer, 0x0A ); // gives an endline
				//vstr::outi() << "answer : " << strAnswer << std::endl;
				pCon->ReadDelimitedString( strAnswer, 0x0A ); // says 'liberty restarted'
				//vstr::outi() << "answer : " << strAnswer << std::endl;

				// wait a sec to let the device reset
				//VistaTimeUtils::Sleep(1000);
				// write F0 (ascii mode)
				data[0] = 'F';
				data[1] = '0'; // data[2] is still 0x0D!
				if(pCon->Send( data, 3 ) == 3)
				{
					VistaTimeUtils::Sleep(100);

					// ok, we should be in ASCII mode
					// get the device information to set up the device
					std::string strAnswer = GetDeviceInfoString(m_vecInfoStrings);
					//vstr::outi() << strAnswer << std::endl;

				}

				CmdSetUpdateRate();

				if(CmdEnableMetricMode())
				{
					vstr::outi() << "[FastrackDriver]: switched to metric mode" << std::endl;
				}

				long nMask = CmdGetStationsMask();
				m_lStations = 0;
				for(unsigned int n = 0; n < sizeof(long)*8; ++n )
				{
					if( nMask & (1<<n) )
						++m_lStations;
				}
				vstr::outi() << "[FastrackDriver]: this device has " << m_lStations << " stations" << std::endl;

				nMask = CmdGetActiveStationMask();
				m_lActiveStations = 0;
				for(unsigned int n = 0; n < sizeof(long)*8; ++n )
				{
					if( nMask & (1<<n) )
						++m_lActiveStations;
				}

				vstr::outi() << "[FastrackDriver]:this device has " << m_lActiveStations << " active stations" << std::endl;

				CmdSetPosQuaternionMode();

				//use the following reference frame
				//
				//            +Y ^    /-Z
				//               |   /
				//               |  /
				//               | /
				//    -X         |/         +X
				//   ------------------------>
				//              /|
				//             / |
				//            /  |
				//           /   |
				//      +Z  /    |
				//         v     |-Y
				//

				//VistaEulerAngles euang(0,0,0);
				//if(m_pRefFrame->GetEmitterAlignment(euang))
				//{
				//	vstr::outi() << "Set RefFrame to: a[" << VistaRadToDeg(euang.a)
				//		      << "] ; e[" << VistaRadToDeg(euang.b)
				//			  << "] ; r[" << VistaRadToDeg(euang.c) << "]" << std::endl;
				//	CmdSetSourceMountingFrame( VistaRadToDeg(euang.a),
				//		                       VistaRadToDeg(euang.b),
				//							   VistaRadToDeg(euang.c) );
				//}


				for(unsigned int k = 0; k < sizeof(long)*8; ++k )
				{
					// nMask is assumed to be active station mask
					if( nMask & (1<<k) )
					{
						VistaTransformMatrix mt;
						if(m_pRefFrame->GetSensorAlignment( mt, k+1 ))
						{
							VistaVector3D origin;
							mt.GetTranslation(origin);
							/** @todo fix setup */
							// x-axis has to be given in relation to the original coord-system of the tracker device
							CmdSetAlignmentReferenceFrame(k+1, origin[0], origin[1], origin[2], -1,0,0, 0,0,-1);
						}
					}
				}

				IVistaDriverReferenceFrameAspect::eHemisphereCode eCode;
				m_pRefFrame->GetHemisphere(eCode);
				CmdSetHemisphere( eCode );

				Flush();
			}

		}
		return true;
	}

	virtual void Flush() const
	{
		char dTmp;
		while(m_pConnection->GetConnection()->HasPendingData())
		{
			m_pConnection->GetCommand(0, &dTmp, sizeof(char) );		
		}
	}

	virtual std::vector<std::string> GetInfoStringSet() const
	{
		return m_vecInfoStrings;
	}

protected:


	std::vector<std::string> m_vecInfoStrings;
	bool m_bPollMode;
	long m_lStations;
	long m_lActiveStations;
	std::string m_sDeviceName;
	IVistaDriverReferenceFrameAspect *m_pRefFrame;
};

class LibertyCommandSet : public CommonCommandSet
{
public:
	LibertyCommandSet(VistaDriverConnectionAspect *pCon,
		               IVistaDriverReferenceFrameAspect *pRefFrame)
	: CommonCommandSet( pCon, pRefFrame )
	{
	}

	static std::string GetCommandSetId() { return "LIBERTY"; }
	virtual eMode CmdGetCurrentMode() const { return MD_NONE; }

	virtual bool CmdSetUpdateRate()
	{
		char data[3];
		data[0] = 'R';
		data[1] = '3'; // set to 120Hz output
		data[2] = 0x0D;
		if(m_pConnection->SendCommand( 0, data, 3, 150 ))
		{
			vstr::outi() << "[FastrackDriver]: switched to 120Hz update rate." << std::endl;
			return true;
		}
		return false;		
	}

	virtual std::string GetDeviceInfoString(std::vector<std::string> &vecStore )
	{
		Flush();
		VistaConnection *pCon = m_pConnection->GetConnection(0);

		char data[2];

		data[0] = 0x16; // ^V : WhoAmI ?
		data[1] = 0x0D;
		if(pCon->Send(data, 2) == 2)
		{
			VistaTimeUtils::Sleep(1000); // wait a sec
			std::string strAnswer;

			std::string strPart;
			for(int i=0; i < 11; ++i)
			{
				if(pCon->ReadDelimitedString( strPart, 0x0A )==-1)
					break; // leave loop
				vecStore.push_back(strPart);
				strAnswer += strPart + std::string("\n"); // we swallowd the trailing endline
			}

			return strAnswer;
		}

		return "";
	}
};

class PatriotCommandSet : public LibertyCommandSet
{
public:
	PatriotCommandSet(VistaDriverConnectionAspect *pCon,
		               IVistaDriverReferenceFrameAspect *pRefFrame)
		: LibertyCommandSet( pCon, pRefFrame )
	{
	}

	static std::string GetCommandSetId() { return "PATRIOTS"; }
	virtual eMode CmdGetCurrentMode() const { return MD_NONE; }

	virtual bool CmdSetUpdateRate()
	{
		return true; // nothing to be done
	}
	
	virtual std::string GetDeviceInfoString(std::vector<std::string> &vecStore )
	{
		Flush();
		VistaConnection *pCon = m_pConnection->GetConnection(0);

		char data[2];

		data[0] = 0x16; // ^V : WhoAmI ?
		data[1] = 0x0D;
		if(pCon->Send(data, 2) == 2)
		{
			VistaTimeUtils::Sleep(1000); // wait a sec
			std::string strAnswer;

			std::string strPart;
			for( int i = 0; i < 6; ++i )
			{
				if(pCon->ReadDelimitedString( strPart, 0x0A )==-1)
					break; // leave loop
				vecStore.push_back(strPart);
				strAnswer += strPart + std::string("\n"); // we swallowd the trailing endline
			}

			return strAnswer;
		}

		return "";
	}
};


// ######################################################################
// PROTOCOL
// ######################################################################

class VistaFastrackProtocolAspect : public IVistaDriverProtocolAspect
{
public:
	VistaFastrackProtocolAspect(VistaFastrakDriver *pDriver)
		: IVistaDriverProtocolAspect(),
		  m_pDriver(pDriver)
	{
	}

	bool SetProtocol( const _cVersionTag &oTag )
	{
		if(oTag.m_strProtocolName == "FASTRAK")
		{
			vstr::errp() << "[FastrackDriver]: Protocol [FASTRAK] known, but not supported" << std::endl; 
			return false;
		}
		else if(oTag.m_strProtocolName == "LIBERTY")
		{
			vstr::outi() << "[FastrackDriver]: Using the LIBERTY protocol" << std::endl;
			VistaDriverConnectionAspect *pConAsp
				= dynamic_cast<VistaDriverConnectionAspect*>(
				     m_pDriver->GetAspectById(
						VistaDriverConnectionAspect::GetAspectId() ) );

			IVistaDriverReferenceFrameAspect *pRefFrame
				= dynamic_cast<IVistaDriverReferenceFrameAspect *>(
				     m_pDriver->GetAspectById(
						IVistaDriverReferenceFrameAspect ::GetAspectId() ) );

			LibertyCommandSet *pSet = new LibertyCommandSet(pConAsp, pRefFrame);
			m_pDriver->SetCommandSet(pSet);
			return true;
		}
		else if(oTag.m_strProtocolName == "PATRIOT")
		{
			vstr::outi() << "[FastrackDriver]: Using the PATRIOT protocol" << std::endl;
			VistaDriverConnectionAspect *pConAsp
				= dynamic_cast<VistaDriverConnectionAspect*>(
				     m_pDriver->GetAspectById(
						VistaDriverConnectionAspect::GetAspectId() ) );

			IVistaDriverReferenceFrameAspect *pRefFrame
				= dynamic_cast<IVistaDriverReferenceFrameAspect *>(
				     m_pDriver->GetAspectById(
						IVistaDriverReferenceFrameAspect ::GetAspectId() ) );

			PatriotCommandSet *pSet = new PatriotCommandSet(pConAsp, pRefFrame);
			m_pDriver->SetCommandSet(pSet);
			return true;
		}

		return false;
	}

private:
	VistaFastrakDriver *m_pDriver;
};

// ######################################################################
// ######################################################################

class VistaFastrackReferenceFrameAspect : public IVistaDriverReferenceFrameAspect
{
public:
	VistaFastrackReferenceFrameAspect()
		: m_eHemisphere( HS_AFT ),
		  m_Angles(0,0,0)
	{
	}

	virtual bool SetHemisphere ( eHemisphereCode eHs )
	{
		m_eHemisphere = eHs;
		return true;
	}

	virtual bool GetHemisphere ( eHemisphereCode &eHs )
	{
		eHs = m_eHemisphere;
		return true;
	}

	virtual bool SetEmitterAlignment( const VistaTransformMatrix &mat )
	{
		m_nEmitterFrame = mat;
		return true;
	}

	virtual bool GetEmitterAlignment( VistaTransformMatrix &mat )
	{
		mat = m_nEmitterFrame;
		return true;
	}


	virtual bool SetSensorAlignment( const VistaTransformMatrix &mat,
		                             unsigned int nSensorId )
	{
		m_SensorFrames[nSensorId] = mat;
		return true;
	}

	virtual bool SetEmitterAlignment( const VistaEulerAngles &angles )
	{
		m_Angles = angles;
		return true;
	}

	virtual bool GetEmitterAlignment( VistaEulerAngles &angles )
	{
		angles = m_Angles;
		return true;
	}

	virtual bool GetSensorAlignment( VistaTransformMatrix &mat,
		                             unsigned int nSensorId )
	{
		std::map<int, VistaTransformMatrix>::const_iterator cit = m_SensorFrames.find(nSensorId);
		if(cit != m_SensorFrames.end())
		{
			mat = (*cit).second;
			return true;
		}
		return false;
	}

private:
	eHemisphereCode m_eHemisphere;
	VistaTransformMatrix m_nEmitterFrame;

	std::map<int, VistaTransformMatrix> m_SensorFrames;
	VistaEulerAngles m_Angles;
};

// #############################################################################
// CONNECTION ATTACH / DETACH SEQUENCE
// #############################################################################

class VistaFastrakAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	VistaFastrakAttachSequence( VistaFastrakDriver *pDriver )
		: m_pDriver(pDriver)
	{
	}

	virtual bool operator()(VistaConnection *pCon)
	{
		if(!pCon->GetIsOpen())
			if(!pCon->Open())
				return false; // open, iff not open already


		pCon->SetIsBlocking(true);

		if(m_pDriver->GetCommandSet())
		{
			m_pDriver->GetCommandSet()->CmdPollValues(); // switch back to normal mode
			VistaTimeUtils::Sleep(250); // give it some time to relax...
			m_pDriver->GetCommandSet()->Flush(); // suck line
		}
		pCon->SetIsBlocking(false);

		return true;
	}

private:
	VistaFastrakDriver *m_pDriver;
};

class VistaFastrakDetachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	VistaFastrakDetachSequence( VistaFastrakDriver *pDriver )
		: m_pDriver(pDriver)
	{

	}
	virtual bool operator()(VistaConnection *pCon)
	{
		pCon->SetIsBlocking(true);

		if(m_pDriver->GetCommandSet())
		{
			m_pDriver->GetCommandSet()->CmdPollValues(); // switch back to normal mode
			m_pDriver->GetCommandSet()->Flush(); // suck line
		}
		if(pCon->GetIsOpen())
			pCon->Close();

		return true;
	}
private:
	VistaFastrakDriver *m_pDriver;

};

typedef TVistaDriverEnableAspect< VistaFastrakDriver > VistaFastrakEnableAspect;


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaFastrakDriver::VistaFastrakDriver(IVistaDriverCreationMethod *crm)
: IVistaDeviceDriver(crm),
  m_pConnection( new VistaDriverConnectionAspect),
  m_pSensorMap( new VistaDriverSensorMappingAspect(crm)),
  m_pCommandSet( NULL ),
  m_pProtocol(NULL),
  m_pInfoAspect(NULL),
  m_pRefFrame(NULL)
{
	SetUpdateType( IVistaDeviceDriver::UPDATE_CONNECTION_THREADED );
	m_pProtocol = new VistaFastrackProtocolAspect(this);
	RegisterAspect(m_pProtocol);

	// setup connection aspect
	// we only have one connection to talk to
	m_pConnection->SetConnection( 0, NULL, "MAIN", VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT );
	m_pConnection->SetUpdateConnectionIndex(0);
	m_pConnection->SetAttachSequence( 0, new VistaFastrakAttachSequence(this) );
	m_pConnection->SetDetachSequence( 0, new VistaFastrakDetachSequence(this) );

	RegisterAspect( m_pConnection );

	// assume an update rate of 120Hz at max
	m_nWandType = m_pSensorMap->RegisterType( "WAND" );
	m_nBodyType = m_pSensorMap->RegisterType( "BODY" );
	RegisterAspect( m_pSensorMap );


	m_pInfoAspect = new VistaDriverInfoAspect;

	m_pRefFrame = new VistaFastrackReferenceFrameAspect();
	RegisterAspect(m_pRefFrame);

	RegisterAspect( new VistaFastrakEnableAspect( this, &VistaFastrakDriver::PhysicalEnable ) );
}

VistaFastrakDriver::~VistaFastrakDriver()
{
	VistaFastrakEnableAspect *enabled = GetAspectAs< VistaFastrakEnableAspect >( VistaDriverEnableAspect::GetAspectId() );
	UnregisterAspect( enabled, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete enabled;

	UnregisterAspect(m_pRefFrame, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pRefFrame;

	UnregisterAspect( m_pInfoAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pInfoAspect;

	UnregisterAspect( m_pConnection, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pConnection;
	UnregisterAspect( m_pSensorMap, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pSensorMap;

	delete m_pCommandSet;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/



bool VistaFastrakDriver::PhysicalEnable(bool bEnable)
{
	if(m_pCommandSet)
	{
		if(bEnable)
			return m_pCommandSet->CmdResume();
		else
			return m_pCommandSet->CmdPause();

	}
	else if(bEnable)
			return false;

	return true;
}

bool VistaFastrakDriver::DoSensorUpdate(VistaType::microtime dTs)
{
	VistaConnection *pCon = m_pConnection->GetConnection();

	pCon->SetIsBlocking(true);
	int n=0;
	float fPx, fPy, fPz,
		fOx, fOy, fOz, fOw;
	int nSensor, nButtonFlag;

	int nTimeStamp,
		nFrameCount,
		nStylus,
		nDistortionLevel;

	std::string sData;

	while(n < 3*GetCommandSet()->CmdGetActiveStations())
	{

		m_pConnection->GetConnection()->ReadDelimitedString( sData, 0x0A );

		int nScanned = 0;
		if((nScanned=sscanf(sData.c_str(), "%d %f %f %f %f %f %f %f %d %d %d %d %d",
				&nSensor, &fPx, &fPy, &fPz, &fOw, &fOx, &fOy, &fOz, &nButtonFlag, &nTimeStamp, &nFrameCount, &nStylus, &nDistortionLevel )) == 13 )
		{
			// detetermine by sensor mapping
			// in this case: all id's are globally numbered over all types
			unsigned int nId = m_pSensorMap->GetSensorIdByRawId( nSensor );
			if(nId == ~0u) // not found
				break; // skip this record (break-if)

			VistaDeviceSensor *pSensor = GetSensorByIndex(nId);
			// else

			// should not be NULL
			if(nStylus == 0) // simple body
			{
				MeasureStart( nId, dTs );
				// claim memory for the measure
				VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);
				VistaFastrakMeasures::sFastrakButtonSample *pMeasure = pM->getWrite< VistaFastrakMeasures::sFastrakButtonSample >();

				pMeasure->m_vPos[0] = fPx;
				pMeasure->m_vPos[1] = fPy;
				pMeasure->m_vPos[2] = fPz;

				pMeasure->m_vOri[0] = fOx;
				pMeasure->m_vOri[1] = fOy;
				pMeasure->m_vOri[2] = fOz;
				pMeasure->m_vOri[3] = fOw;
				pMeasure->m_nDistortionLevel = nDistortionLevel;
				pMeasure->m_nFrameCount = nFrameCount;
				pMeasure->m_nTimeStamp = nTimeStamp;
				pMeasure->m_bBtPress = (nStylus ? true:false);

				MeasureStop( nId );
			}
			else // has buttons?
			{
				MeasureStart( nId, dTs );
				// claim memory for the measure
				VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSensor);
				VistaFastrakMeasures::sFastrakButtonSample *pMeasure = pM->getWrite< VistaFastrakMeasures::sFastrakButtonSample >();

				pMeasure->m_vPos[0] = fPx;
				pMeasure->m_vPos[1] = fPy;
				pMeasure->m_vPos[2] = fPz;

				pMeasure->m_vOri[0] = fOx;
				pMeasure->m_vOri[1] = fOy;
				pMeasure->m_vOri[2] = fOz;
				pMeasure->m_vOri[3] = fOw;
				pMeasure->m_nDistortionLevel = nDistortionLevel;
				pMeasure->m_nFrameCount = nFrameCount;
				pMeasure->m_nTimeStamp = nTimeStamp;

				pMeasure->m_bBtPress = (nButtonFlag ? true : false);
				MeasureStop( nId );
			}




		}

		//std::cout << strData << std::endl;
		if(pCon->HasPendingData() == false)
			break;
		++n;
	}

	pCon->SetIsBlocking(false);
#if defined(WIN32)
	m_pConnection->GetConnection()->WaitForIncomingData();
#endif
	return true;
}


bool VistaFastrakDriver::DoDisconnect()
{
	VistaConnection *pCon = m_pConnection->GetConnection();
	if(pCon == NULL)
		return false; // no connection, no fun...

	pCon->Close();

	return false;
}

bool VistaFastrakDriver::DoConnect()
{
	VistaConnection *pCon = m_pConnection->GetConnection();
	if(pCon == NULL)
		return false; // no connection, no fun...

	pCon->SetIsBlocking(true);
	bool bRet = true;
	try
	{
		GetCommandSet()->Flush();

		bRet = GetCommandSet()->CmdInitSequence();
		if(bRet)
		{
			// setup info aspect
			std::vector<std::string> strInfoStrings = GetCommandSet()->GetInfoStringSet();
			VistaPropertyList &props = m_pInfoAspect->GetInfoPropsWrite();
			unsigned int n=0;
			for( std::vector<std::string>::const_iterator cit = strInfoStrings.begin();
				cit != strInfoStrings.end(); ++cit )
			{
				props.SetValue( VistaConversion::ToString( n++ ), *cit );
			}


			long nStations = GetCommandSet()->CmdGetActiveStationMask();
			for(unsigned int n=0; n < sizeof(long)*8; ++n)
			{
				if( nStations & (1<<n) )
				{
					VistaDeviceSensor *pSensor = new VistaDeviceSensor;
					IVistaMeasureTranscode *pTranscode
						= GetFactory()->GetTranscoderFactoryForSensor("WAND")->CreateTranscoder();
						
						//m_pSensorMap->GetTranscoderFactoryForType("WAND")->CreateTranscoder();
					pSensor->SetMeasureTranscode(pTranscode);
					pSensor->SetTypeHint( "WAND"  );
					unsigned int nId = AddDeviceSensor( pSensor );
					m_pSensorMap->SetSensorId( m_pSensorMap->GetTypeId("WAND"), n+1, nId );
				}
			}
		}
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
		bRet = false;
	}

	// which is default for the connection updater
	pCon->SetIsBlocking(false);

	return bRet;
}


void VistaFastrakDriver::SetCommandSet( IVistaFastrakCommandSet *pSet )
{
	if(m_pCommandSet)
		return; // no change possible
	m_pCommandSet = pSet;
}

IVistaFastrakCommandSet *VistaFastrakDriver::GetCommandSet() const
{
	return m_pCommandSet;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


