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


#ifndef _VDFNAPPLYTRANSFORMNODE_H
#define _VDFNAPPLYTRANSFORMNODE_H


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
 * The VdfnApplyTransformNode can be used to <b>apply</b> an absolute transformation
 * to one IVistaTransformable interface. The new matrix is multiplied from
 * right-hand-side
 * The node offers a set for three independent channels
 * - VistaTransformMatrix to apply the affine 4x4 transformation matrix as the transformable's
     transform. The matrix is right handed and expected to be in row major.
 * - VistaVector3D to apply the transformable's translation
 * - VistaQuaternion to apply the transformable's rotation
 *
 * Each channel is optional, but when set, each channel is evaluated!
 * Execpt when transform is set, then only transform will be evaluated.
 *
 *
 * @ingroup VdfnNodes
 * @inport{transform, VistaTransformMatrix, row-major transform matrix}
 *
 * @todo check the evaluation mode of this node, it will evaluate unconditional of an
         incoming state change, and it will evaluate on and inport set. This can be
         confusing.
 */
class VISTADFNAPI VdfnApplyTransformNode : public IVdfnNode
{
public:
	/**
	 * empty constructor, use ApplyTransformTarget() to make this node work.
	 */
	VdfnApplyTransformNode();

	/**
	 * delayed construction constructor, give an object registry to look for
	 * a transformable upon the call to PrepareEvaluationRun().
	 * @param a non NULL object registry
	 * @param strKey the name to look for in the registry, case sensitive
	 * @param applyLocal set true if you want to apply the changes in local coordsystem (right hand side)
	 */
	VdfnApplyTransformNode( VdfnObjectRegistry *, const std::string &strKey, const bool bApplyLocal = false );

	/**
	 * use when the transformable is known already.
	 * @param pObj the transformable
	 * @param applyLocal set true if you want to apply the changes in local coordsystem (right hand side)
	 */
	VdfnApplyTransformNode(IVistaTransformable *pObj, const bool bApplyLocal = false );

	/**
	 * does nothing.
	 */
	~VdfnApplyTransformNode();

	/**
	 * is valid when GetTransformTarget() != NULL and at least one inport is set
	 */
	virtual bool GetIsValid() const;

	/**
	 * in case a registry was provided, the name key is not empty and no transform
	 * target was set, the method tries to claim a transform target from the
	 * registry and caches the node ports
	 * @return GetIsValid()
	 */
	virtual bool PrepareEvaluationRun();

	/**
	 * @return the current transform target
	 */
	IVistaTransformable *GetTransformTarget() const;

	/**
	 * set the current transfom target.
	 */
	void SetTransformTarget( IVistaTransformable * );

	static const std::string STransformInPortName;
protected:
	virtual bool DoEvalNode();

private:
	void RegisterInPrototypes();

	TVdfnPort<VistaTransformMatrix> *m_pInTransform;

	unsigned int m_nTCount; // update count for transform inport

	IVistaTransformable *m_pOutTransform;
	VdfnObjectRegistry *m_pObjRegistry;
	std::string          m_strKey;
	bool				 m_bApplyLocal;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNAPPLYTRANSFORMNODE_H

