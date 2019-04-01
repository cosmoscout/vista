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


#ifndef _VDFNFORCEFEEDBACKNODE_H
#define _VDFNFORCEFEEDBACKNODE_H


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
class VistaDriverMap;
class IVistaDriverForceFeedbackAspect;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * This is a very simple force feedback node. It does pass its in arguments
 * to the IVistaDriverForceFeedbackAspect::SetForce() interface. This might
 * be enough for simple devices, such as FF joysticks or other "plausible"
 * interfaces. It does however, only pass the force upon evaluation, which
 * might not be enough for a good feedback, e.g., for the phantom with a
 * 1000Hz required update.
 * Whenever the node is in a network, it will call forces enable upon
 * a call to OnActivation() and it will disable the forces upon a call to
 * OnDeactivation(). These APIs are called from the VdfnGraph.
 *
 * You can use this node for the simple enabling and disabling of force
 * feedback in your application using
 * - an explicit state set on the 'enabled' inport of this node
 * - disabling or enabling of the complete graph where this node is part of
 *
 *
 * @ingroup VdfnNodes
 * @inport{force, VistaVector3D, optional, a force vector to set on evaluation}
 * @inport{torque,VistaQuaternion,optional,a torque value to forward to the device upon evaluation}
 * @inport{enabled,bool,optional,the enable/disable state to forward to the device upon change}
 *
 * @todo check with the force feedback API, velocity was introduced.
 */
class VISTADFNAPI VdfnForceFeedbackNode : public IVdfnNode
{
public:
	VdfnForceFeedbackNode( IVistaDriverForceFeedbackAspect *pAspect );

	/**
	 *  enables forces upon activation
	 */
	virtual void OnActivation( double dTs );

	/**
	 * disables forces opon deactivation
	 */
	virtual void OnDeactivation( double dTs );

	/**
	 * this node need a force-feedback aspect to work on
	 * @return true when the force-feedback aspect was set during construction
	 */
	bool GetIsValid() const;

	/**
	 * @return GetIsValid()
	 */
	bool PrepareEvaluationRun();
protected:

	bool DoEvalNode();
private:
	IVistaDriverForceFeedbackAspect *m_pAspect;

	TVdfnPort<VistaVector3D>   *m_pForceVec;
	TVdfnPort<VistaVector3D> *m_pTorque;
	TVdfnPort<bool>             *m_pEnabled;
};


/**
 * force feedback construction node, needs a driver map to operate.
 * the driver map is queried for a device given the sole parameter
 * to the construction 'device_id'. If the driver with this id
 * exists and this driver exports a IVistaDriverForceFeedbackAspect,
 * than the node will be created.
 */
class VISTADFNAPI VdfnForceFeedbackNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
{
public:
	VdfnForceFeedbackNodeCreate( VistaDriverMap *pMap );
	virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;
private:
	VistaDriverMap *m_pMap;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VDFNFORCEFEEDBACKNODE_H

