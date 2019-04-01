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
/* Ingo Assenmacher (2008), Patric Schmitz (2010)                             */
/*                                                                            */
/*============================================================================*/


#ifndef _VDFNDELAYNODE_H
#define _VDFNDELAYNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnPort.h>

#include <deque>

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
 *
 * @ingroup VdfnNodes
 */
template<class T>
class VdfnDelayNode : public IVdfnNode
{
public:
	VdfnDelayNode(unsigned int iDelay) :
		IVdfnNode(),
		m_pOutValue( new TVdfnPort<T> ),
		m_iDelay(iDelay),
		m_pInPort( NULL )
	{
		RegisterInPortPrototype( "in", new TVdfnPortTypeCompare<TVdfnPort<T> > );
		RegisterOutPort( "out", m_pOutValue );
	}

	virtual ~VdfnDelayNode()
	{
	}


	bool PrepareEvaluationRun()
	{
		m_pInPort  = dynamic_cast<TVdfnPort<T>*>(GetInPort( "in" ));
		m_pOutValue->SetValue( T(), 0 );
		return GetIsValid();
	}

protected:
	bool DoEvalNode()
	{
		// store value
		m_vecBuffer.push_back( m_pInPort->GetValue() );

		if(m_vecBuffer.size() == m_iDelay+1)
		{
			m_pOutValue->SetValue( m_vecBuffer.front(), GetUpdateTimeStamp() );
			m_vecBuffer.pop_front();
		}
		return true;
	}

private:
	TVdfnPort<T> *m_pOutValue,
		         *m_pInPort;

	unsigned int  m_iDelay;
	std::deque<T> m_vecBuffer;
};

template<class T>
class VdfnDelayNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		try
		{
			const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
			unsigned int delay = subs.GetValueOrDefault<int>( "delay", 1 );
			
			return new VdfnDelayNode<T>(delay);
		}
		catch(VistaExceptionBase &x)
		{
			x.PrintException();
		}
		return NULL;
		
	}
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //_VDFNDELAYNODE_H

