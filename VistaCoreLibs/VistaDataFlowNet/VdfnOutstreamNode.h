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


#ifndef _VDFNOUTSTREAMNODE_H
#define _VDFNOUTSTREAMNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnSerializer.h"
#include <VistaBase/VistaVectorMath.h>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaTransformable;
class VdfnObjectRegistry;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Writes text to an outstream, enclosed in a prefix (default: empty) and
 * a postfix (default: linebreak)
 *
 * @ingroup VdfnNodes
 * @outport{ text, string, text to display}
 */
class VISTADFNAPI VdfnOutstreamNode : public IVdfnNode
{
public:
	VdfnOutstreamNode( std::ostream& oOutstream );
	~VdfnOutstreamNode();

	virtual bool PrepareEvaluationRun();

	std::string GetPrefix() const;
	void SetPrefix( const std::string& oValue );
	std::string GetPostfix() const;
	void SetPostfix( const std::string& oValue );

protected:
	virtual bool DoEvalNode();

private:
	std::string m_sPrefix;
	std::string m_sPostfix;
	std::ostream& m_oOutstream;
	TVdfnPort<std::string>* m_pTextPort;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNAPPLYTRANSFORMNODE_H

