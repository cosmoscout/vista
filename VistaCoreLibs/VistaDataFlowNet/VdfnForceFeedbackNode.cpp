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


#include "VdfnForceFeedbackNode.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverForceFeedbackAspect.h>
#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaDeviceDriver.h>
#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnForceFeedbackNode::VdfnForceFeedbackNode( IVistaDriverForceFeedbackAspect *pAspect )
 : IVdfnNode(),
   m_pAspect(pAspect),
   m_pForceVec(NULL),
   m_pTorque(NULL),
   m_pEnabled(NULL)
   {
		RegisterInPortPrototype( "force", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> >);
		RegisterInPortPrototype( "torque", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> >);
		RegisterInPortPrototype( "enabled", new TVdfnPortTypeCompare<TVdfnPort<bool> >);
   }

bool VdfnForceFeedbackNode::GetIsValid() const
{
	return (m_pAspect != NULL);
}

bool VdfnForceFeedbackNode::PrepareEvaluationRun()
{
	m_pForceVec = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("force"));
	m_pTorque = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("torque"));
	m_pEnabled = dynamic_cast<TVdfnPort<bool>*>(GetInPort("enabled"));

	return GetIsValid();
}

bool VdfnForceFeedbackNode::DoEvalNode()
{
	if(m_pEnabled)
		m_pAspect->SetForcesEnabled(m_pEnabled->GetValue());

    // check whether this is a simple control node
	// in that case both (force and torque) are NULL
    if(!m_pForceVec && !m_pTorque)
        return true;

	return m_pAspect->SetForce( (m_pForceVec ? m_pForceVec->GetValue() : VistaVector3D()),
								(m_pTorque ? m_pTorque->GetValue() : VistaVector3D()) );
}

void VdfnForceFeedbackNode::OnActivation( double dTs )
{
	if(m_pAspect)
		m_pAspect->SetForcesEnabled(true);
}

void VdfnForceFeedbackNode::OnDeactivation( double dTs )
{
	if(m_pAspect)
		m_pAspect->SetForcesEnabled(false);
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


// ############################################################################


VdfnForceFeedbackNodeCreate::VdfnForceFeedbackNodeCreate( VistaDriverMap *pMap )
: VdfnNodeFactory::IVdfnNodeCreator(),
  m_pMap(pMap)
  {

  }

IVdfnNode *VdfnForceFeedbackNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strDriverId = subs.GetValue<std::string>( "device_id" );
		if(strDriverId.empty())
			return NULL;

		// try to claim driver
		IVistaDeviceDriver *pDriver = m_pMap->GetDeviceDriver( strDriverId );
		if(!pDriver)
			return NULL;

		// ok, driver there, get force-feedback aspect
		IVistaDriverForceFeedbackAspect *pAspect =
			dynamic_cast<IVistaDriverForceFeedbackAspect*>(pDriver->GetAspectById( IVistaDriverForceFeedbackAspect::GetAspectId() ));
		if(!pAspect)
			return NULL;

		// ok, force-feedback aspect there, create node and return it.
		return new VdfnForceFeedbackNode( pAspect );

	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
	return NULL;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


