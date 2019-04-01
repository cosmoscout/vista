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


#ifndef _VDFNPROJECTVECTORNODE_H
#define _VDFNPROJECTVECTORNODE_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VdfnConfig.h"

#include "VdfnSerializer.h"
#include "VdfnNode.h"
#include "VdfnPort.h"
#include "VdfnNodeFactory.h"


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
 * Project a VistaVector3D to a selected component (x,y or z)
 * @ingroup VdfnNodes
 * @inport{vec,VistaVector3D,mandatory,the vector v to decompose}
 * @outport{out,float,the component x\,y or z\,depending on the configuration of
          the node}
 */
class VISTADFNAPI VdfnProjectVectorNode : public IVdfnNode
{
public:
    enum eProject
    {
        PRJ_X = 0, /**< project X */
        PRJ_Y,     /**< project Y */
        PRJ_Z,     /**< project Z */
        PRJ_NONE
    };

    /**
     * @param ePrj the component to project, behavior of setting PRJ_NONE is undefined
     */
    VdfnProjectVectorNode( eProject ePrj );

    /**
     * @return true when a proper project mode was set and the inport is valid
     */
    bool GetIsValid() const;

    /**
     * @return GetIsValid()
     */
    bool PrepareEvaluationRun();

protected:
    bool DoEvalNode();

private:
    TVdfnPort<VistaVector3D> *m_pVecIn;
    TVdfnPort<float>          *m_pValOut;
    eProject                   m_ePrj;
};

/**
 * creates a VdfnProjectVectorNode
 */
class VISTADFNAPI VdfnProjectVectorNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	/**
	 * accepts
	 * - component: 'X', 'Y' or 'Z' (case-insensitive)
	 * default is PRJ_NONE
	 * @return a VdfnProjectVectorNode
	 */
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

