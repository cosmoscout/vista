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


#ifndef _VISTAOPENGLDRAW_H
#define _VISTAOPENGLDRAW_H

// it is not necessary to implement this interface
// thus the methods are not pure!

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaAspects/VistaExplicitCallbackInterface.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaBoundingBox;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI IVistaOpenGLDraw : public IVistaExplicitCallbackInterface
{
public:
	IVistaOpenGLDraw();
	virtual ~IVistaOpenGLDraw();

	/**
	 * overloaded from IVistaExplicitCallbackInterface.
	 * This method may be omited on a traversal if the underlying renderer
	 * classified this node as invisible. If you change shape/Bounding box
	 * Notify the sytem by calling MarkBoundsAsDirty which will invalidate
	 * your parent node's Volume and thus trigger a re-evaluation of underneath
	 * Volumes.
	 */
	virtual bool Do () = 0;


	/**
	 * @brief	Gets the bounding box.
	 * @return	true if it succeeds, false if it fails.
	 * If it returns false, the parent's bounding volume is extendet by the
	 * local origin (0,0,0).
	 */
	virtual bool GetBoundingBox(VistaBoundingBox &bb) = 0;

	/**
	 * @brief	Mark bounding box as dirty.
	 * This will result in an invalidation of the parent node's bounding volume
	 * before the next render traversal. Thereby the next cull-traversal will
	 * call GetBoundingBox to update the node's volume.
	 */
	void MarkBoundingBoxAsDirty();
	
	/**
	 * for internal use only.
	 */
	virtual bool GetAndResetDirtyFlag();

private:
	bool m_bBoundsDirty;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTAOPENGLDRAW_H
