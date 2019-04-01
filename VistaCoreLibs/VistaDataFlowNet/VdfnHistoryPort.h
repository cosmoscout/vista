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


#ifndef _VDFNHISTORYPORT_H
#define _VDFNHISTORYPORT_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VdfnConfig.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaBase/VistaBaseTypes.h>

#include "VdfnPort.h"
#include "VdfnPortFactory.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaMeasureHistory;
class VistaSensorReadState;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * The basic data to look at when using a history port. It gathers the history,
 * a transcode to access the data and a sensor for administrative purposes.
 * The VdfnHistoryPortData defines the basic data type the is propagated through the
 * data flow net when talking to devices. Along with the data there is utility
 * information as well, which is handy when writing a history processor node:
 * - the IVistaMeasureTranscode that can be queried for access on the history
     using its symbolic interface
 * - the VistaDeviceSensor source of this history
 * - the count of new measures since the last update of the history in
     count of measures.
 * - the current safe-read update index for readers
 */
class VISTADFNAPI VdfnHistoryPortData
{
public:
	VdfnHistoryPortData( const VistaMeasureHistory &oHistory,
			  IVistaMeasureTranscode *pTranscode,
			  VistaDeviceSensor *pSensor,
			  VistaSensorReadState *pReadstate );

	const VistaMeasureHistory  &m_oHistory;   /**< the history to look at */

	unsigned int				m_nNewMeasures; /**< experimental: the number of new measures since last evaluate */
	unsigned int				m_nRealNewMeasures; /**< experimental: the actual (unskimmed) number of new measures */
	unsigned int                m_nMeasureCount;
	VistaType::microtime        m_nAvgDriverUpdTime;
	double                      m_nAvgUpdFreq;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// CLIENT SIDE RESOURCES
	// Pointer resources that have to be setup on each client side in a distributed setup

	IVistaMeasureTranscode     *m_pTranscode; /**< the transcode to access the history */
	VistaDeviceSensor          *m_pSensor;    /**< the sensor that this history belongs to
												   this is a system specific attribute. user code
												   is discouraged to use this attribute, as it might
												   not be consistent on all nodes in a cluster. */
	VistaSensorReadState       *m_pReadState;

	unsigned int                MAGIC_COOKIE;
	static unsigned int         MAGIC_COOKIE_VALUE;
};

/**
 * a deserializer. reads the <b>complete</b> history! not only the newest values.
 * @todo optimize me (read only the differential, e.g. only the new values)
 */
VISTADFNAPI IVistaDeSerializer &operator>>(IVistaDeSerializer &oDeSer,   VdfnHistoryPortData *pPort );

/**
 * a serializer. writes the <b>complete</b> history! not only the newest values.
 * @toto optimize me (write only the differential, e.g., only the new values)
 */
VISTADFNAPI IVistaSerializer   &operator<<(IVistaSerializer &oSer, const VdfnHistoryPortData *pPort );

/**
 * Convenience typedef: port for histories. Writing HistoryPort
 * is shorter than TVdfnPort<CHistoryData>.
 */
typedef TVdfnPort<VdfnHistoryPortData*>                  HistoryPort;

/**
 * a port-type-compare for history ports. short-hand notation.
 */
typedef TVdfnPortTypeCompare<HistoryPort>             HistoryPortCompare;

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/**
 * template class to implement a transfer from 'history' to a 'port value' of
 * type F. It uses the transcoder API to do this polymorphic.
 * @param F the template parameter F defines the return type of the transcoder and the port-type
            (both must match for this template to work)
 */
template<class F>
class TVdfnTranscodePortSet : public VdfnPortFactory::CPortSetFunctor
{
public:
	bool Set( const VistaSensorMeasure *pMeasure,
				  IVistaPropertyGetFunctor *pGet,
				  IVdfnPort *pPort,
				  double nTs,
				  unsigned int nIndex)
	{
		// this marks that the property is not an indexed get!
		if(nIndex == ~0u)
		{
			// that's why this downcast will probably work ;)
			// note that we cast to TTranscodeValueGet<F>* !!
			typename IVistaMeasureTranscode::TTranscodeValueGet<F> *getF
				= dynamic_cast<IVistaMeasureTranscode::TTranscodeValueGet<F>*>( pGet );

			// same here: transcode return type and port type <b>must</b> match
		    // for this template. If this is not desired, design your own CPortSetFunctor
			TVdfnPort<F> *pF = dynamic_cast<TVdfnPort<F>*>(pPort);
			if(!pF)
				return false; // sanity check, should not happen

			// set value from transcode return to port
			pF->SetValue( getF->GetValue( pMeasure ), nTs );
		}
		else
		{
			// this is an indexed get!
			// note that we downcast to the TTranscodeIndexedGet<F>* !!
			typename IVistaMeasureTranscode::TTranscodeIndexedGet<F> *getF
				= dynamic_cast<IVistaMeasureTranscode::TTranscodeIndexedGet<F>*>( pGet );

			TVdfnPort<F> *pF = dynamic_cast<TVdfnPort<F>*>(pPort);
			if(!pF)
				return false; // sanity check, should not happen

			// avoid a copy, get the memory location
			F &val = pF->GetValueRef();
			// copy from measure to val, using the transcode
			getF->GetValueIndexed( pMeasure, val, nIndex );

			// we are just lazy: use the set value API to set both: the update
			// trigger and the timestamp. The below would be two lines, else.
			pF->SetValue(val, nTs);
		}
		return true;
	}
};


#endif //_VDFNHISTORYPORT_H
