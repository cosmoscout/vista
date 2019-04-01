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


#include "VdfnHistoryPort.h"
#include "VistaDeviceDriversBase/VistaSensorReadState.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

unsigned int VdfnHistoryPortData::MAGIC_COOKIE_VALUE = 0xCAFECAFE;

#if defined( VISTA_TRANSMIT_INCREMENTAL_HISTORIES )
	static const unsigned int IS_INCREMENTAL=1;
#else
	static const unsigned int IS_INCREMENTAL=0;
#endif

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnHistoryPortData::VdfnHistoryPortData( const VistaMeasureHistory &oHistory,
									IVistaMeasureTranscode *pTranscode,
									VistaDeviceSensor *pSensor,
									VistaSensorReadState *pReadState)
		: m_oHistory(oHistory),
		  m_nNewMeasures(0),
		  m_nRealNewMeasures(0),
		  m_nMeasureCount(0),
		  m_nAvgDriverUpdTime(0),
		  m_nAvgUpdFreq(0),
		  m_pTranscode(pTranscode),
		  m_pSensor(pSensor),
		  m_pReadState( pReadState ),
		  MAGIC_COOKIE( MAGIC_COOKIE_VALUE )
{
	// pre-condition: m_pReadState is not 0
	assert( m_pReadState );
	m_pReadState->SetUseIncrementalSerialization( IS_INCREMENTAL == 1 ? true : false );
	m_pReadState->SetUseOwnHistoryForDeSerialization(false);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


IVistaDeSerializer &operator>>(IVistaDeSerializer &oDeSer,
							   VdfnHistoryPortData *pPort )
{

	oDeSer >> (*pPort).MAGIC_COOKIE;

	if( (*pPort).MAGIC_COOKIE != VdfnHistoryPortData::MAGIC_COOKIE_VALUE ) {
		vstr::err() << "Invalid format for History Port Data. Expected: [0x"
				    << std::hex
					<< VdfnHistoryPortData::MAGIC_COOKIE_VALUE
					<< "] -- is: [0x" << (*pPort).MAGIC_COOKIE << "]" << std::dec;
		return oDeSer;
	}

	unsigned int is_incremental = 2;

	oDeSer >> (*pPort).m_nNewMeasures
		   >> (*pPort).m_nRealNewMeasures
		   >> (*pPort).m_nMeasureCount
		   >> (*pPort).m_nAvgDriverUpdTime
		   >> (*pPort).m_nAvgUpdFreq
		   >> is_incremental;

	assert( is_incremental == IS_INCREMENTAL ); // error trapping

	(*pPort).m_pReadState->DeSerialize( oDeSer );

	return oDeSer;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SERIALIZING
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


IVistaSerializer   &operator<<(IVistaSerializer &oSer,
							   const VdfnHistoryPortData *pPort )
{
	oSer << (*pPort).MAGIC_COOKIE;


	oSer << (*pPort).m_nNewMeasures
		 << (*pPort).m_nRealNewMeasures
		 << (*pPort).m_nMeasureCount
		 << (*pPort).m_nAvgDriverUpdTime
		 << (*pPort).m_nAvgUpdFreq;

	oSer << IS_INCREMENTAL;

	(*pPort).m_pReadState->Serialize( oSer );

	return oSer;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


