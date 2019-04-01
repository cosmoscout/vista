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


#ifndef _VDFNGETTRANSFORMNODE_H
#define _VDFNGETTRANSFORMNODE_H


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
 * returns the transformation of an IVistaTransformable, the user decides
 * whether this is the local (relative) or the global (world) transformation
 * of the IVistaTransformable. The default is local (relative) transformation.
 *
 * @ingroup VdfnNodes
 * @outport{transform,VistaTransformMatrix,right handed and row major transformation matrix}
 *
 *
 */
class VISTADFNAPI VdfnGetTransformNode : public IVdfnNode
{
public:
    enum eMode
    {
        TRANS_RELATIVE=0, /**< calls IVistaTransformable::GetTransform() */
        TRANS_WORLD       /**< calls IVistaTransformable::GetWorldTransform() */
    };


    /**
     * empty constructor, use SetTransformable() to set the target of this node
     * @see SetTransformable()
     */
	VdfnGetTransformNode();

	/**
	 * constructor using the object registry and a name to lookup in it, as
	 * <i>Transform</i>.
	 * @param pReg a non-NULL pointer to the registry to use
	 * @param strKey a non-empty name for a transform to look up
	 */
	VdfnGetTransformNode(VdfnObjectRegistry *pReg, const std::string &strKey);

	/**
	 * use this when you have a transformable at hand and do not want to look
	 * it up in the registry during PrepareEvaluationRun()
	 */
	VdfnGetTransformNode( IVistaTransformable *pTransform );

	/**
	 * @return GetTransformable() != NULL
	 */
	bool GetIsValid() const;

	/**
	 * when GetTransformable() == NULL and the ObjectRegistry was set, the method
	 * will try to resolve the transformable given the key during construction.
	 * @return GetIsValid()
	 */
	bool PrepareEvaluationRun();

	/**
	 * @return the transformable set/resolved
	 */
	IVistaTransformable *GetTransformable() const;

	/**
	 * set the transformable to work on for the next iteration.
	 */
	void SetTransformable( IVistaTransformable * );

	/**
	 * @return the transform mode to be used
	 */
    eMode GetTransformGetMode() const;

    /**
     * @param md sets the transform mode to be used.
     */
    void SetTransformGetMode( eMode md );
protected:
	bool DoEvalNode();

	IVistaTransformable              *m_pTransform;
	TVdfnPort<VistaTransformMatrix> *m_pOutMat;

	VdfnObjectRegistry *m_pReg;
	std::string m_strKey;
    eMode m_eMode;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNGETTRANSFORMNODE_H

