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


#include "ColorChangerDfnNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
ColorChangerDfnNode::ColorChangerDfnNode()
	: IVdfnNode()
	, m_pChangeRPort( NULL )
	, m_pChangeGPort( NULL )
	, m_pChangeBPort( NULL )
	, m_pColorPort( new TVdfnPort<VistaVector3D> )
	, m_v3CurrentColor( 0.5f, 0.5, 0.5f )
{
	/**
	 * First, we register our three outport, for which we already created an
	 * object of the type IVdfnPort, by using the templated TVdfnPort with
	 * VistaVector3D as template class. Note that the DataFlowNet only supports
	 * a limited set of datatypes, which is why we store our output color as
	 * VistaVector3D, instead of VistaColor.
	 * After creation of the port, we register it so that the DFN knows about
	 * it, and thereby give it a name with which we reference it in the graph file.
	 * Also note that by registering, we hand over memory management to the node
	 * itself and we don't delete m_pColorPort ourselves!
	 */
	RegisterOutPort( "color", m_pColorPort );

	/**
	 * Inports are not created directly by us (thus they are initialized to NULL),
	 * but are instead created on demand by the DFN itself when the graph is built and
	 * an edge connects to the desired port.
	 * Thus, we don't register ports, but Port Prototypes that tell the Node the name and
	 * type of the inport using the template TVdfnPortTypeCompare.
	 * We later retrieve the actual nodes, @see PrepareEvaluationRun
	 */
	RegisterInPortPrototype( "change_red", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "change_green", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "change_blue", new TVdfnPortTypeCompare<TVdfnPort<float> > );
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
/**
 * PrepareEvaluationRun is called once after the DFN graph was created, but
 * before the actual evaluation starts.
 * Here, we retrieve the actual inports for which we registered prototypes
 * in the node constructor, and store them.
 */
bool ColorChangerDfnNode::PrepareEvaluationRun()
{	
	m_pChangeRPort = dynamic_cast<TVdfnPort<float>*>( GetInPort( "change_red" ) );
	m_pChangeGPort = dynamic_cast<TVdfnPort<float>*>( GetInPort( "change_green" ) );
	m_pChangeBPort = dynamic_cast<TVdfnPort<float>*>( GetInPort( "change_blue" ) );

	/**
	 * The return value specifies whether or not the node was set up correctly.
	 * For this, we use the result of GetIsValid()
	 */
	return GetIsValid();
}

/**
 * GetIsValid specifies if the port is set up correctly and ready for use.
 * In our case, we demand that all three inports have been created, so after
 * the PrepareEvaluatioRun, they should be non-NULL
 */
bool ColorChangerDfnNode::GetIsValid() const
{
	return ( m_pChangeRPort && m_pChangeGPort && m_pChangeBPort );
}

/**
 * DoEvalNode is the main function that is called whenever the node is evaluated
 * during a graph traversal. This is where we perform our actual computation.
 * In our case, we simply add the values from the inports to our current color
 * and ensure its correctness.
 * Note: A node evaluation is usually NOT triggered each time the graph is traversed.
 * Instead, by default DoEvalNode is only called if at least one of the inports has
 * changed since the last evaluation.
 * If you want to ensure an evaluation in each graph traversal, set the
 * unconditional evaluation flag using SetEvaluationFlag( true ).
 * If you need a more elaborate scheme to determine if the node should be evaluated
 * (e.g. at least two of the four inports changed or if an external message
 * event was received or whatever), re-implement the CalcUpdateNeededScore() routine.
 */
bool ColorChangerDfnNode::DoEvalNode()
{
	m_v3CurrentColor[0] = m_pChangeRPort->GetValueConstRef();
	m_v3CurrentColor[1] = m_pChangeGPort->GetValueConstRef();
	m_v3CurrentColor[2] = m_pChangeBPort->GetValueConstRef();

	// ensure that all values are in the range [0..1]
	for( int i = 0; i < 3; ++i )
	{
		if( m_v3CurrentColor[i] < 0.0f )
			m_v3CurrentColor[i] = 0.0f;
		else if( m_v3CurrentColor[i] > 1.0f )
			m_v3CurrentColor[i] = 1.0f;
	}
	
	// now, we update our outport with the new value
	m_pColorPort->SetValue( m_v3CurrentColor, GetUpdateTimeStamp() );
	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


