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


#ifndef _VDFNBINARYOPNODE_H
#define _VDFNBINARYOPNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnSerializer.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"
#include "VdfnBinaryOps.h"

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
 * template to simplify the construction of nodes that compute on two arguments
 * f(a,b) = x, where a is of type T, b is of type T2, and x is of type K.
 * The function f is formulated as a specialization of CBinOp and the definition
 * of the operator. The nice thing about this node is that is unifies the
 * in- and outports for all binary operations.
 * The node operates only on a non-NULL CBinOp().
 *
 * @ingroup VdfnNodes
 * @inport{first,type T, mandatory, the LEFT argument to CBinOp(left\,right)}
 * @inport{second,type T2,mandatory, the RIGHT argument to CBinOp(left\,right)}
 * @outport{out, type K, the return result of CBinOp(first\,second)}
 *
 */
template<class T, class T2, class K>
class TVdfnBinaryOpNode : public IVdfnNode
{
public:
	/**
	 * construct and determine the operation.
	 */
	TVdfnBinaryOpNode( VdfnBinaryOps::BinOp<T,T2,K> *pBinOp)
	: IVdfnNode(), m_pBinOp(pBinOp),
	  m_pFirst(NULL),
	  m_pSecond(NULL),
	  m_pOut( new TVdfnPort<K> )
	  {
		RegisterInPortPrototype( "first", new TVdfnPortTypeCompare<TVdfnPort<T> >);
		RegisterInPortPrototype( "second", new TVdfnPortTypeCompare<TVdfnPort<T2> >);
		RegisterOutPort("out", m_pOut);
	  }

	/**
	 * @return true when a bin op was specified upon construction, and all inports
	           are assigned.
	 *
	 */
	bool GetIsValid() const
	{
		return m_pBinOp && (IVdfnNode::GetIsValid());
	}

	/**
	 * @return GetIsValid()
	 */
	bool PrepareEvaluationRun()
	{
		m_pFirst = dynamic_cast<TVdfnPort<T>*>(GetInPort("first"));
		m_pSecond = dynamic_cast<TVdfnPort<T2>*>(GetInPort("second"));

		return GetIsValid();
	}
protected:

	/**
	 * sets the outport value determined on the calculation of CBinOp().
	 * @return true
	 */
	bool DoEvalNode()
	{
		m_pOut->SetValue( (*m_pBinOp)( m_pFirst->GetValueConstRef(),
				                       m_pSecond->GetValueConstRef() ),
				          GetUpdateTimeStamp() );

		return true;
	}
private:
	TVdfnPort<T> *m_pFirst;
	TVdfnPort<T2> *m_pSecond;
    TVdfnPort<K> *m_pOut;
	VdfnBinaryOps::BinOp<T,T2,K> *m_pBinOp;
};


/**
 * creates a TBinaryOpNode for a given CBinOp instance.
 */
template<class T, class T2, class K>
class TVdfnBinOpCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	/**
	 * defines the binary operation by the argument given.
	 * @param pOp the binary operation. The memory of pOp belongs to the
	          TVdfnBinOpCreate now and will be deleted upon destruction
	 */
	TVdfnBinOpCreate( typename VdfnBinaryOps::BinOp<T,T2,K> *pOp )
	: VdfnNodeFactory::IVdfnNodeCreator(), m_pOp(pOp)
	{

	}

	/**
	 * deletes the CBinOp instance.
	 */
	~TVdfnBinOpCreate() { delete m_pOp; }

	/**
	 * simply creates TBinaryOpNode for the CBinOp as given during
	 * the construction of the creator.
	 */
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		return new TVdfnBinaryOpNode<T,T2,K>(m_pOp);
	}
private:
	typename VdfnBinaryOps::BinOp<T,T2,K> *m_pOp;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

