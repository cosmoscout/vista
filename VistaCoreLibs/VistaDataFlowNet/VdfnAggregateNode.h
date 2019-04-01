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


#ifndef _VDFNAGGREGATENODE_H
#define _VDFNAGGREGATENODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnNode.h"
#include "VdfnHistoryPort.h"

#include <VistaDeviceDriversBase/VistaDeviceSensor.h>
#include <VistaDeviceDriversBase/VistaSensorReadState.h>


#include <string>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This node allows to collect a number of new samples from a history property
 * as a vector of samples at its out-port. It is more or less an example that
 * can be inspected when trying to work with history data in the dfn. This
 * node simply accumulates new values, especially, it does not save the timestamps
 * of each values, rendering it almost useless, except for simple cases (such
 * as keyboard strokes). It is defined as a templated class, as device data can
 * have arbitrary types. During construction, give the symbol name used with
 * the history's transcode to get access to new data.
 *
 * @ingroup VdfnNodes
 * @inport{history, HistoryPortCompare, mandatory, give a history of a device}
 * @outport{values, std::vector<T>, the ordered vector of the last updates in the history}
 *
 * The aggregate node is provided the <b>data name</b> the collect from the history.
 * It needs a transcoder type to access the data from the history.
 * On every update, it will read all new values in the history, and push them to
 * a vector of proper type. That vector is available as outport of the node.
 */
template <class T>
class TVdfnAggregateNode : public IVdfnNode
{
public:
	/**
	 * @param strInValue the name of the property to query the transcoder of the history for.
	 */
	TVdfnAggregateNode( const std::string &strInValue,
						const bool bStoreNewestEntryLast )
		: m_pInPort(NULL),
		m_pOutPort(new TVdfnPort<std::vector<T> >),
		m_strInValue(strInValue),
		m_pGetFunctor(NULL),
		m_bStoreNewestEntryLast( bStoreNewestEntryLast )
	{
		RegisterInPortPrototype("history", new HistoryPortCompare(&m_pInPort));
		RegisterOutPort( "values", m_pOutPort );
	}


	/**
	 * is valid when the inport is connected, a get functor for the property read
	 * was found in the history (and had the proper type)
	 */
	virtual bool GetIsValid() const
	{
		return IVdfnNode::GetIsValid() && (m_pGetFunctor != 0);
	}

	virtual bool PrepareEvaluationRun()
	{
		if(m_pInPort && m_pInPort->GetValueConstRef()->m_pTranscode )
			m_pGetFunctor = dynamic_cast<IVistaMeasureTranscode::TTranscodeValueGet<T>*>(
							   m_pInPort->GetValueConstRef()->m_pTranscode->GetMeasureProperty(m_strInValue));
		return true;
	}

protected:
	virtual bool DoEvalNode()
	{
		// get history port
		const        VdfnHistoryPortData *pData = m_pInPort->GetValueConstRef();

		// this is the "official way"(tm) to find out about new samples in the
		// history. The history took care of
		// - fixing the number of new samples for this run
		// - checking that we can not get more than we wanted (user-read size)
		// - finding the correct amount of newly incoming samples
		//
		/** @todo still check for a cleaner way to access the number */
		//       of new samples in the history (make it a method?)

		unsigned int nNewMeasureCount = pData->m_nNewMeasures;

        // get memory managed by outport (avoids a copy)
		std::vector<T> &vec = m_pOutPort->GetValueRef();

		// resize to a proper width.
		vec.resize(nNewMeasureCount);

		// now we copy in
		for(unsigned int n=0; n < nNewMeasureCount; ++n)
		{
			// claim a "past" measure (copy in from most-current to oldest)
			const VistaSensorMeasure *pMeasure = pData->m_pReadState->GetPastMeasure( n );
			if( pMeasure ) // can be 0 in case no data was delivered, yet, but driver signalled update
			{
				// get value using the transcoder, store in vector
				if( m_bStoreNewestEntryLast )
					vec[ nNewMeasureCount - n - 1 ] = m_pGetFunctor->GetValue(pMeasure);
				else
					vec[n] = m_pGetFunctor->GetValue(pMeasure);
			}
		}
		// trigger change for the outport, as we have directly written to the memory
		// managed by the m_pOutPort.
		m_pOutPort->IncUpdateCounter();

		return true;
	}

private:
	HistoryPort                                   *m_pInPort;
	IVistaMeasureTranscode::TTranscodeValueGet<T> *m_pGetFunctor;
	TVdfnPort<std::vector<T> >                    *m_pOutPort;
	std::string m_strInValue;
	bool		m_bStoreNewestEntryLast;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNAGGREGATENODE_H
