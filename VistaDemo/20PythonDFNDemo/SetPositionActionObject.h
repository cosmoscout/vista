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


#ifndef _SETPOSITIONACTIONOBJECT_H
#define _SETPOSITIONACTIONOBJECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaDataFlowNet/VdfnActionObject.h>

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaTransformNode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Getting data (like, in our case, a position) into our
 * own, developer-controlled source code, we need an appropriate interface.
 * It would be possible to create a new DfnNode whose NodeCreator passes it a
 * pointer to an instance of out code, and then calls the appropriate 
 * functions during DoEvalNode. However, this would require writing a node,
 * node creator, and your own class for each use-case. Therefore, the
 * ActionObject provides a generic solution: Using the Reflectionable interface,
 * one can specify what get and set functions the class owns, and these are then
 * used to automatically create an appropriate ActionNode with ports named
 * like the functors. Once an inport changes or an outport requests data,
 * the corresponding functors are called. 
 * Note that of course, it is still possible that multiple ActionNodes are
 * created, but they all reference the same ActionObject.
 */
class SetPositionActionObject : public IVdfnActionObject
{
public:
	SetPositionActionObject( VistaTransformNode* pTransformNode );
	~SetPositionActionObject();

	bool SetPosition( const VistaVector3D& v3Color ); 
	VistaVector3D GetPosition() const; 

	//REFL_DECLARE declares functions to be inherited from IVIstaReflectionable
	REFL_DECLARE;

private:
	VistaTransformNode*		m_pTransformNode;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //_SETPOSITIONACTIONOBJECT_H
