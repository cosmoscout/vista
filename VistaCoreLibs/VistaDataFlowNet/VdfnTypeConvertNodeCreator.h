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


#ifndef _VDFNTYPECONVERTNODECREATOR_H
#define _VDFNTYPECONVERTNODECREATOR_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnTypeConvertNode.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VdfnNodeFactory;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

namespace VdfnTypeConvertNodeCreators
{
	/**
	 * management API, registers type convert node creators with the default combinations
	 * This function is Called from VdfnNodeCreators::RegisterNodeCreators,
	 * and has mainly been separated due to .obj file size issues
	 */
	VISTADFNAPI bool RegisterTypeConvertNodeCreators( VdfnNodeFactory* pFac );
}

/**
 * templated node constructor, constructs nodes of type TVdfnTypeConvertNode
 */
template<class tFrom, class tTo>
class VdfnTypeConvertNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	typedef typename TVdfnTypeConvertNode<tFrom, tTo>::CFAssign AssignF;

	/**
	 * @param fct the conversion function to use for all instances created by
	          this creator.
	 */
	VdfnTypeConvertNodeCreate( AssignF fct = NULL )
		: VdfnNodeFactory::IVdfnNodeCreator(),
		  m_CFct( fct )
	{
		if( m_CFct == NULL )
			m_CFct = &VistaConversion::ConvertType<tTo, tFrom>;
	}

	/**
	 * creates an TVdfnTypeConvertNode, unconditional, no argument.
	 */
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const
	{
		return new TVdfnTypeConvertNode<tFrom, tTo>( m_CFct );
	}

private:
	AssignF m_CFct;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNTYPECONVERTNODECREATOR_H

