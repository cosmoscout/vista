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


#include "VistaDriverMeasureHistoryAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"
#include <cassert>

#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverMeasureHistoryAspect::m_nAspectId  = -1;
unsigned int VistaDriverMeasureHistoryAspect::INVALID_READING = ~0;

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverMeasureHistoryAspect::VistaDriverMeasureHistoryAspect(unsigned int nDefSize)
: IVistaDeviceDriver::IVistaDeviceDriverAspect(),
  m_nDefHistSize(nDefSize)
{
	if(VistaDriverMeasureHistoryAspect::GetAspectId() == -1) // unregistered
		VistaDriverMeasureHistoryAspect::SetAspectId(
		VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("HISTORY"));

	SetId(VistaDriverMeasureHistoryAspect::GetAspectId());
}

VistaDriverMeasureHistoryAspect::~VistaDriverMeasureHistoryAspect()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int  VistaDriverMeasureHistoryAspect::GetAspectId()
{
	return VistaDriverMeasureHistoryAspect::m_nAspectId;
}

void VistaDriverMeasureHistoryAspect::SetAspectId(int nId)
{
	assert(m_nAspectId == -1);
	m_nAspectId = nId;

}

bool VistaDriverMeasureHistoryAspect::RegisterSensor(VistaDeviceSensor *pSen, IVistaDeviceDriver::AllocMemoryFunctor *amf
																			, IVistaDeviceDriver::ClaimMemoryFunctor *cmf
																			, bool bReRegister )
{
	_sL l( &pSen->GetDriverMeasures(), amf, cmf, bReRegister );
	m_mapHistories[pSen] = l;

	// set a default history size
	if(m_nDefHistSize != ~0u)
		SetHistorySize(pSen, 2, 2, m_nDefHistSize);

	if( amf )
	{
		VistaType::uint32 nUserSize, nDriverSize;
		nUserSize   = pSen->GetDriverMeasures().m_nClientReadSize;
		nDriverSize = pSen->GetDriverMeasures().m_nDriverWriteSize;
		for(unsigned int n=0; n < nUserSize+nDriverSize; ++n)
		{
			(*amf)( *(pSen->GetDriverMeasures().m_rbHistory.index(n)) );
		}
	}
	return true;
}

bool  VistaDriverMeasureHistoryAspect::SetAllocAndClaimFunctor( VistaDeviceSensor *pSensor,
		                                        IVistaDeviceDriver::AllocMemoryFunctor *amf,
		                                        IVistaDeviceDriver::ClaimMemoryFunctor *cmf )
{
	HISTORY::iterator it = m_mapHistories.find(pSensor);
	if(it == m_mapHistories.end())
		return false; // not registered

	(*it).second.m_amf = amf;
	(*it).second.m_cmf = cmf;

	return true;
}

bool VistaDriverMeasureHistoryAspect::UnregisterSensor(VistaDeviceSensor *pSen)
{
	HISTORY::iterator it = m_mapHistories.find(pSen);
	if(it == m_mapHistories.end())
		return true; // not registered

	if( (*it).second.m_cmf )
	{
		VistaType::uint32 nUserSize, nDriverSize;
		nUserSize   = pSen->GetDriverMeasures().m_nClientReadSize;
		nDriverSize = pSen->GetDriverMeasures().m_nDriverWriteSize;

		for(unsigned int n=0; n < nUserSize+nDriverSize; ++n)
		{
			VistaSensorMeasure &m = *(pSen->GetDriverMeasures().m_rbHistory.index(n));
				(*(*it).second.m_cmf)( m ); // apply
		}
	}

	m_mapHistories.erase(it);
	return true;
}


void VistaDriverMeasureHistoryAspect::MeasureStart(VistaDeviceSensor*pSen,
													VistaType::microtime dTs)
{
	HISTORY::const_iterator cit = m_mapHistories.find(pSen);
	if(cit == m_mapHistories.end())
		return; // not registered

	// statistics

	// create an entry
	_sL l( &pSen->GetDriverMeasures(), (*cit).second.m_amf, (*cit).second.m_cmf, (*cit).second.m_bInitialized );

	if(l.m_pMeasures->m_nDriverWriteSize == 0)
		return; // malconfigured history!

	pSen->GetWindowTimer().RecordTime();

	// fill it with 'local' information
	(*l.m_nIt).m_nMeasureTs = dTs;
	(*l.m_nIt).m_nMeasureIdx = pSen->GetMeasureCount();

	// copy into the history map for this sensor
	m_mapHistories[pSen] = l; // replace
}

void  VistaDriverMeasureHistoryAspect::MeasureStop(VistaDeviceSensor *pSen)
{
	HISTORY::const_iterator it = m_mapHistories.find(pSen);
	if(it != m_mapHistories.end())
	{
		if( (*it).second.m_pMeasures->m_nDriverWriteSize == 0 )
			return; // malconfigured history

		// advance in measure index for sensor
		pSen->AdvanceMeasure();
	}
}


bool VistaDriverMeasureHistoryAspect::SetHistorySize(VistaDeviceSensor *pSen,
							unsigned int nUserSize,
							unsigned int nDriverSize,
							unsigned int nSlotLength)
{
	HISTORY::iterator it = m_mapHistories.find(pSen);
	if(it != m_mapHistories.end())
	{
		pSen->GetDriverMeasures().Resize( nUserSize, nDriverSize, nSlotLength );
		pSen->GetWindowTimer().ResetAveraging();
		return true;
	}
	return false;
}

unsigned int VistaDriverMeasureHistoryAspect::GetHistorySize(VistaDeviceSensor *pSen) const
{
	return pSen->GetMeasureHistorySize();
}

unsigned int   VistaDriverMeasureHistoryAspect::GetDriverWriteHistorySize( VistaDeviceSensor *pSen ) const
{
	HISTORY::const_iterator it = m_mapHistories.find(pSen);
	if(it != m_mapHistories.end())
	{
		return (*it).second.m_pMeasures->m_nDriverWriteSize;
	}
	return INVALID_READING;
}

unsigned int VistaDriverMeasureHistoryAspect::GetUserReadHistorySize( VistaDeviceSensor *pSen ) const
{
	HISTORY::const_iterator it = m_mapHistories.find(pSen);
	if(it != m_mapHistories.end())
	{
		return (*it).second.m_pMeasures->m_nClientReadSize;
	}
	return INVALID_READING;
}

VistaSensorMeasure *VistaDriverMeasureHistoryAspect::GetCurrentSlot(VistaDeviceSensor*pSen) const
{
	HISTORY::const_iterator it = m_mapHistories.find(pSen);
	if(it == m_mapHistories.end())
		return NULL;

	if((*it).second.m_pMeasures->m_nDriverWriteSize == 0)
		return NULL; // malconfigured history!

	return &(*(*it).second.m_pMeasures->m_rbHistory.GetCurrent());
}

bool VistaDriverMeasureHistoryAspect::GetIsRegistered(VistaDeviceSensor *pSen) const
{
	HISTORY::const_iterator it = m_mapHistories.find(pSen);
	if(it == m_mapHistories.end())
		return false;
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


