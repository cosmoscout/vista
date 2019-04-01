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


#include "VistaPhantomServerDriver.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverWorkspaceAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverEnableAspect.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaInterProcComm/Connections/VistaConnection.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaByteBufferSerializer.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
static const int CONTROLCHANNEL=2;
static const int FORCECHANNEL  =1;
static const int DATACHANNEL   =0;


class VistaPhantomServerControlAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	VistaPhantomServerControlAttachSequence(VistaPhantomServerDriver *pDriver)
		: m_pDriver(pDriver)
	{
	}

	virtual bool operator()(VistaConnection *pCon)
	{
		if(!pCon->GetIsOpen())
			pCon->Open();

		bool bBlocking = pCon->GetIsBlocking();

		pCon->SetIsBlocking(true);

		int iSize = 0;
		std::string vendor;
		std::string deviceType;
		std::string visualizerIP;

		try
		{
			pCon->ReadInt32(iSize);
			pCon->ReadString(visualizerIP, iSize);
			pCon->ReadInt32(iSize);
			pCon->ReadString(vendor, iSize);
			pCon->ReadInt32(iSize);
			pCon->ReadString(deviceType, iSize);
			pCon->ReadInt32(m_pDriver->m_inputDOF);
			pCon->ReadInt32(m_pDriver->m_outputDOF);
			pCon->ReadFloat32(m_pDriver->m_maxStiffness);
			pCon->ReadFloat32(m_pDriver->m_maxForce);
			pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMin[0]);
			pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMin[1]);
			pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMin[2]);
			pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMax[0]);
			pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMax[1]);
			pCon->ReadFloat32(m_pDriver->m_maxWorkspaceMax[2]);
			pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMin[0]);
			pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMin[1]);
			pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMin[2]);
			pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMax[0]);
			pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMax[1]);
			pCon->ReadFloat32(m_pDriver->m_maxUsableWorkspaceMax[2]);

			vstr::outi() << "Found phantom device with vendor: [" 
						<< vendor << "], device: [" << deviceType << "]\n";
		}
		catch(VistaExceptionBase &x)
		{
			x.PrintException();
		}

		pCon->SetIsBlocking(bBlocking);
		pCon->SetIsBuffering(false);

		m_pDriver->m_pWorkSpace->SetWorkspace( "MAXWORKSPACE", VistaBoundingBox( &m_pDriver->m_maxWorkspaceMin[0], &m_pDriver->m_maxWorkspaceMax[0] ) );
		m_pDriver->m_pWorkSpace->SetWorkspace( "USABLEWORKSPACE", VistaBoundingBox( &m_pDriver->m_maxUsableWorkspaceMin[0], &m_pDriver->m_maxUsableWorkspaceMax[0] ) );


		return true;
	}

private:
	VistaPhantomServerDriver *m_pDriver;
};

class VistaPhantomServerAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	virtual bool operator()(VistaConnection *pCon)
	{
		if(!pCon->GetIsOpen())
			if(!pCon->Open())
				return false;

		pCon->SetIsBuffering(false);
		pCon->SetIsBlocking(false);

		return true;
	}
};


class VistaPhantomServerControlDetachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	virtual bool operator()(VistaConnection *pCon)
	{
		if(pCon->GetIsOpen())
		{
			pCon->WriteInt32('Q'); // send a quit statement
			pCon->WaitForSendFinish(); // really flush that
			pCon->Close(); // close connection
		}
		return true;
	}
};

typedef TVistaDriverEnableAspect< VistaPhantomServerDriver > VistaPhantomServerEnableAspect;


/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaPhantomServerDriver::VistaPhantomServerDriver(IVistaDriverCreationMethod *crm)
: IVistaDeviceDriver(crm),
  m_pConAspect(new VistaDriverConnectionAspect),
  m_pWorkSpace( new VistaDriverWorkspaceAspect ),
  m_inputDOF(0), m_outputDOF(0),
  m_maxForce(0.0f), m_maxStiffness(0.0f)
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);

	RegisterAspect( new VistaPhantomServerEnableAspect( this, &VistaPhantomServerDriver::PhysicalEnable ) );

	// create connections
	RegisterAspect(m_pConAspect);

	m_pConAspect->SetConnection(DATACHANNEL, NULL, "DATACHANNEL", VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT);
	m_pConAspect->SetConnection(FORCECHANNEL, NULL, "FORCECHANNEL", VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT);
	m_pConAspect->SetConnection(CONTROLCHANNEL, NULL, "CONTROLCHANNEL", VistaDriverConnectionAspect::ALLOW_CONNECTION_REPLACEMENT);

	m_pConAspect->SetAttachSequence( DATACHANNEL,    new VistaPhantomServerAttachSequence );
	m_pConAspect->SetAttachSequence( FORCECHANNEL,   new VistaPhantomServerAttachSequence );
	m_pConAspect->SetAttachSequence( CONTROLCHANNEL, new VistaPhantomServerControlAttachSequence(this) );
	m_pConAspect->SetDetachSequence( CONTROLCHANNEL, new VistaPhantomServerControlDetachSequence );

	// create and register a sensor for the phantom
	VistaDeviceSensor *pSensor = new VistaDeviceSensor;
	pSensor->SetTypeHint( "" );
	AddDeviceSensor(pSensor);

	// new VistaPhantomServerDriverMeasureTranscode
	pSensor->SetMeasureTranscode( GetFactory()->GetTranscoderFactoryForSensor("")->CreateTranscoder() );


	// register force feedback aspect
	RegisterAspect( new VistaPhantomServerForceFeedbackAspect(this) );

    // register workspace aspect
    RegisterAspect( m_pWorkSpace );

}

VistaPhantomServerDriver::~VistaPhantomServerDriver()
{
	VistaDeviceSensor *pSensor = GetSensorByIndex(0);
	RemDeviceSensor(pSensor);
	IVistaMeasureTranscode *pTC = pSensor->GetMeasureTranscode();
	pSensor->SetMeasureTranscode(NULL);
	delete pSensor;
	delete pTC;

	UnregisterAspect( m_pConAspect, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete m_pConAspect;

	UnregisterAspect( GetAspectById( IVistaDriverForceFeedbackAspect::GetAspectId() ) );

	VistaPhantomServerEnableAspect *enabler = GetAspectAs< VistaPhantomServerEnableAspect >( VistaDriverEnableAspect::GetAspectId() );
	UnregisterAspect( enabler, IVistaDeviceDriver::DO_NOT_DELETE_ASPECT );
	delete enabler;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VistaPhantomServerDriver::DoSensorUpdate(VistaType::microtime dTs)
{
	try
	{
		VistaConnection *pIncoming = m_pConAspect->GetConnection(DATACHANNEL);
		if(pIncoming && pIncoming->GetIsOpen())
		{
			unsigned long nReadSize = pIncoming->PendingDataSize();

			if(nReadSize > 0)
			{
				std::vector<VistaType::byte> vecMsg(sizeof(VistaPhantomServerMeasures::sPhantomServerMeasure));

				// we assume a packet oriented protocol here
				// otherwise, we might end up in a state where we read off
				// the header and tail is still missing
				int nDataRead = 0;
				for(unsigned int i=0; i < nReadSize; i+=nDataRead)
				{
					nDataRead = pIncoming->ReadRawBuffer((void*)&vecMsg[0], sizeof(VistaPhantomServerMeasures::sPhantomServerMeasure));
					if(nDataRead < 0)
					{
						// TROUBLE!
						pIncoming->WaitForSendFinish();
						pIncoming->Close();
						return false;
					}
				}

				VistaByteBufferDeSerializer deSer;
				deSer.SetBuffer( &vecMsg[0], sizeof(VistaPhantomServerMeasures::sPhantomServerMeasure) );

				VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(GetSensorByIndex(0));
				if(pM == NULL)
					return false;

				// claim a new sensor measure
				MeasureStart( 0, dTs );

				VistaPhantomServerMeasures::sPhantomServerMeasure *m = (VistaPhantomServerMeasures::sPhantomServerMeasure*)(&(*pM).m_vecMeasures[0]);
				deSer.ReadFloat32(m->m_afPosition[0]);
				deSer.ReadFloat32(m->m_afPosition[1]);
				deSer.ReadFloat32(m->m_afPosition[2]);

				for(int i=0; i < 9; ++i)
					deSer.ReadFloat32(m->m_afRotMatrix[i]);

				deSer.ReadFloat32(m->m_afPosSCP[0]);
				deSer.ReadFloat32(m->m_afPosSCP[1]);
				deSer.ReadFloat32(m->m_afPosSCP[2]);

				deSer.ReadFloat32(m->m_afVelocity[0]);
				deSer.ReadFloat32(m->m_afVelocity[1]);
				deSer.ReadFloat32(m->m_afVelocity[2]);
				deSer.ReadFloat32(m->m_afForce[0]);
				deSer.ReadFloat32(m->m_afForce[1]);
				deSer.ReadFloat32(m->m_afForce[2]);
				deSer.ReadInt32(m->m_nButtonState);
				deSer.ReadFloat32(m->m_nUpdateRate);

				deSer.ReadFloat32(m->m_afOverheatState[0]);
				deSer.ReadFloat32(m->m_afOverheatState[1]);
				deSer.ReadFloat32(m->m_afOverheatState[2]);
				deSer.ReadFloat32(m->m_afOverheatState[3]);
				deSer.ReadFloat32(m->m_afOverheatState[4]);
				deSer.ReadFloat32(m->m_afOverheatState[5]);

				MeasureStop(0);
			}
		}

		VistaConnection *pControl = m_pConAspect->GetConnection(CONTROLCHANNEL); // claim control connection
		if(pControl && pControl->GetIsOpen())
		{
			unsigned int nDataSize = 0;
			if((nDataSize=pControl->PendingDataSize())>0)
			{
				unsigned int nDataRead = 0;
				int nCommand = 0;
				for (unsigned int i=0; i<nDataSize; i+=nDataRead)
				{
					nDataRead = pControl->ReadInt32(nCommand);
					if( nDataRead <= 0)
					{
						vstr::warnp() << "VistaPhantomServerDriver::DoSensorUpdate() -- "
									<< "read error" << endl;
						pControl->Close();
						return false;
					}
				}
				switch(nCommand)
				{
				case 'E': // error flag sent
					{
						int errorId = 0;
						int nSize   = 0;
						std::string sErrorMsg;

						pControl->ReadInt32(errorId);
						pControl->ReadInt32(nSize);
						if(nSize > 0)
							pControl->ReadString(sErrorMsg, nSize);
						vstr::outi() << "[VistaPhantomServer] Error: " << errorId 
									<< " " << sErrorMsg << std::endl;

						break;
					}
				default:
					break;
				}

			}

		}
		return true;
	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return false;
}

bool VistaPhantomServerDriver::PhysicalEnable(bool bEnable)
{
	VistaConnection *pCon = m_pConAspect->GetConnection(CONTROLCHANNEL);
	return (pCon->WriteInt32( bEnable ? 'S' : 's' ) == sizeof(VistaType::sint32));
}

void	VistaPhantomServerDriver::AttachMaximalBoundary() const
{
	VistaConnection *pCon = m_pConAspect->GetConnection(2);
	if(pCon)
	{
		pCon->WriteInt32('B');
	}
}

void	VistaPhantomServerDriver::DetachBoundary() const
{
	VistaConnection *pCon = m_pConAspect->GetConnection(2);
	if(pCon)
	{
		pCon->WriteInt32('b');
	}
}

void	VistaPhantomServerDriver::ResetEncoders() const
{
	VistaConnection *pCon = m_pConAspect->GetConnection(2);
	if(pCon)
	{
		pCon->WriteInt32('R');
	}
}

void	VistaPhantomServerDriver::GetMaxWorkspace(VistaVector3D & min, VistaVector3D & max) const
{
	min = m_maxWorkspaceMin;
	max = m_maxWorkspaceMax;
}

void	VistaPhantomServerDriver::GetMaxUsableWorkspace(VistaVector3D & min, VistaVector3D & max) const
{
	min = m_maxUsableWorkspaceMin;
	max = m_maxUsableWorkspaceMax;
}


// #############################################################################

VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::VistaPhantomServerForceFeedbackAspect( VistaPhantomServerDriver *pDriver )
: IVistaDriverForceFeedbackAspect(),
  m_pParent(pDriver),
  m_pSerializer(new VistaByteBufferSerializer(256)),
  m_nDefaultStiffness(1.0f),
  m_bForcesEnabled(false),
  m_bDynamicFriction(false)
{
}

VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::~VistaPhantomServerForceFeedbackAspect()
{
	delete m_pSerializer;
}


bool VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::SetForce( const VistaVector3D   & v3Force,
																	  const VistaVector3D & v3AngularForce)
{
	VistaConnection *pFC = GetChannel(FORCECHANNEL);
	m_pSerializer->SetByteorderSwapFlag( pFC->GetByteorderSwapFlag() );
	m_pSerializer->ClearBuffer();

	m_pSerializer->WriteInt32('F');
	m_pSerializer->WriteFloat32(v3Force[0]);
	m_pSerializer->WriteFloat32(v3Force[1]);
	m_pSerializer->WriteFloat32(v3Force[2]);
	m_pSerializer->WriteFloat32(v3AngularForce[0]);
	m_pSerializer->WriteFloat32(v3AngularForce[1]);
	m_pSerializer->WriteFloat32(v3AngularForce[2]);

	if(pFC->WriteRawBuffer(m_pSerializer->GetBuffer(), m_pSerializer->GetBufferSize()) == m_pSerializer->GetBufferSize())
		return true;

	return false;
}


bool VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::SetConstraint( const VistaVector3D		& contactPoint,
							const VistaVector3D	& normal,
							const float stiffness,
							const VistaVector3D	& internalForce) const
{
	VistaConnection *pFC = GetChannel(FORCECHANNEL);

	m_pSerializer->SetByteorderSwapFlag( pFC->GetByteorderSwapFlag() );
	m_pSerializer->ClearBuffer();

	m_pSerializer->WriteInt32('C');
	m_pSerializer->WriteFloat32(contactPoint[0]);
	m_pSerializer->WriteFloat32(contactPoint[1]);
	m_pSerializer->WriteFloat32(contactPoint[2]);
	m_pSerializer->WriteFloat32(normal[0]);
	m_pSerializer->WriteFloat32(normal[1]);
	m_pSerializer->WriteFloat32(normal[2]);

	float phantomStiffness = stiffness;

	if(stiffness <= 0.0f)
		phantomStiffness = m_nDefaultStiffness;
	else
		phantomStiffness = stiffness;

	if(phantomStiffness > 1.0f)
		phantomStiffness = 1.0f;

	m_pSerializer->WriteFloat32(phantomStiffness);

	m_pSerializer->WriteFloat32(internalForce[0]);
	m_pSerializer->WriteFloat32(internalForce[1]);
	m_pSerializer->WriteFloat32(internalForce[2]);

	if(pFC->WriteRawBuffer(m_pSerializer->GetBuffer(), m_pSerializer->GetBufferSize()) == m_pSerializer->GetBufferSize())
		return true;
	else
		return false;
}

bool VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::SetForcesEnabled(bool bEnabled)
{
	if(bEnabled != m_bForcesEnabled)
	{
		m_bForcesEnabled = bEnabled;

		VistaConnection *pFC = GetChannel(CONTROLCHANNEL);
		return (pFC->WriteInt32( m_bForcesEnabled ? 'F' : 'f' ) == sizeof(VistaType::sint32));
	}

	return false;
}

bool VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::SetDynamicFrictionEnabled( bool bEnabled )
{
	if(bEnabled != m_bDynamicFriction)
	{
		m_bDynamicFriction = bEnabled;

		VistaConnection *pFC = GetChannel(CONTROLCHANNEL);
		return (pFC->WriteInt32( m_bDynamicFriction ? 'D' : 'd' ) == sizeof(VistaType::sint32));
	}

	return false;
}

bool VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::GetDynamicFrictionEnabled() const
{
	return m_bDynamicFriction;
}

bool VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::GetForcesEnabled() const
{
	return m_bForcesEnabled;
}

VistaConnection *VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::GetChannel(int nIdx) const
{
	if(m_pParent)
	{
		VistaDriverConnectionAspect *pConAsp =
			static_cast<VistaDriverConnectionAspect*>(m_pParent->GetAspectById( VistaDriverConnectionAspect::GetAspectId() ) );
		return pConAsp->GetConnection( nIdx );
	}
	return NULL;
}

int VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::GetNumInputDOF() const
{
	return m_pParent->m_inputDOF;
}

int VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::GetNumOutputDOF() const
{
	return m_pParent->m_outputDOF;
}

float VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::GetMaximumStiffness() const
{
    return m_pParent->m_maxStiffness;
}

float VistaPhantomServerDriver::VistaPhantomServerForceFeedbackAspect::GetMaximumForce() const
{
    return m_pParent->m_maxForce;
}


bool VistaPhantomServerDriver::DoConnect()
{
	return true;
}

bool VistaPhantomServerDriver::DoDisconnect()
{
	return true;
}

