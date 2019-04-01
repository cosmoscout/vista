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


#include "VdfnPlaneConstraintNode.h"

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverForceFeedbackAspect.h>

#include <VistaDataFlowNet/VdfnSerializer.h>
#include <VistaDataFlowNet/VdfnNode.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

VdfnPlaneConstraintNode::VdfnPlaneConstraintNode( IVistaDriverForceFeedbackAspect *pAsp )
: IVdfnNode(),
  m_pForceFeedback(pAsp),
  m_pOrigin(NULL),
  m_pDamping(NULL),
  m_pNormal(NULL),
  m_pInternalForce(NULL),
  m_pStiffness(NULL),
  m_pToDriver(NULL),
  m_pOriginOut(new TVdfnPort<VistaVector3D>),
  m_pNormalOut(new TVdfnPort<VistaVector3D>),
  m_pPlaneConstraint( new VistaPlaneConstraint )
  {
	RegisterInPortPrototype( "origin", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> >);
	RegisterInPortPrototype( "normal", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> >);
	RegisterInPortPrototype( "internal_force", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> >);
	RegisterInPortPrototype( "stiffness", new TVdfnPortTypeCompare<TVdfnPort<float> >);
	RegisterInPortPrototype( "damping", new TVdfnPortTypeCompare<TVdfnPort<float> >);
    RegisterInPortPrototype( "todriver", new TVdfnPortTypeCompare<TVdfnPort<VistaTransformMatrix> >);

    RegisterOutPort( "origin_out", m_pOriginOut );
    RegisterOutPort( "normal_out", m_pNormalOut );

    m_pForceFeedback->SetForceAlgorithm( m_pPlaneConstraint );
  }

VdfnPlaneConstraintNode::~VdfnPlaneConstraintNode()
{
	if(m_pForceFeedback)
		m_pForceFeedback->SetForceAlgorithm(NULL);
	delete m_pPlaneConstraint;
}

bool VdfnPlaneConstraintNode::GetIsValid() const
{
	return (m_pForceFeedback && m_pOrigin && m_pNormal && m_pStiffness);
}

bool VdfnPlaneConstraintNode::PrepareEvaluationRun()
{
	m_pOrigin = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("origin"));
	m_pNormal = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("normal"));
	m_pInternalForce = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort("internal_force"));
	m_pStiffness = dynamic_cast<TVdfnPort<float>*>(GetInPort("stiffness"));
	m_pDamping = dynamic_cast<TVdfnPort<float>*>(GetInPort("damping"));
    m_pToDriver = dynamic_cast<TVdfnPort<VistaTransformMatrix>*>(GetInPort("todriver"));

	return GetIsValid();
}

bool VdfnPlaneConstraintNode::DoEvalNode()
{
	VistaVector3D v3Origin = m_pOrigin->GetValue();
	VistaVector3D v3Normal = m_pNormal->GetValue();
    v3Normal[3] = 0.0f; //convert to vector
	VistaVector3D v3InternalForce = (m_pInternalForce ? m_pInternalForce->GetValue() : VistaVector3D() );
    v3InternalForce[3] = 0.0f; //convert to vector

	float nStiffness = m_pStiffness->GetValue();
	float nDamping = m_pDamping->GetValue();

    if(m_pToDriver)
    {
        const VistaTransformMatrix &m = m_pToDriver->GetValueConstRef();
        VistaTransformMatrix md = m.GetTranspose();
        v3Origin = m.Transform(v3Origin);
        v3Normal = md.Transform(v3Normal);
        v3InternalForce = md.Transform(v3InternalForce);
    }

    m_pOriginOut->SetValue( v3Origin, GetUpdateTimeStamp() );
    m_pNormalOut->SetValue( v3Normal, GetUpdateTimeStamp() );

	return m_pPlaneConstraint->SetConstraintPlane( v3Origin , v3Normal, nStiffness, nDamping, v3InternalForce );
}

/*============================================================================*/
/*								C R E A T O R								  */
/*============================================================================*/

VdfnPlaneConstraintNodeCreate::VdfnPlaneConstraintNodeCreate( VistaDriverMap *pMap )
: IVdfnNodeCreator(),
  m_pMap(pMap)
  {

  }

IVdfnNode *VdfnPlaneConstraintNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
	try
	{
		const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();
		std::string strDriverId = subs.GetValueOrDefault<std::string>("device_id");
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
		return new VdfnPlaneConstraintNode( pAspect );

	}
	catch(VistaExceptionBase &x)
	{
		x.PrintException();
	}
    return NULL;
}
