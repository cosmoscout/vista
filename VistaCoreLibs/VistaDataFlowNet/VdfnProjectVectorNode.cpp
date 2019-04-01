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


#include "VdfnProjectVectorNode.h" 
#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnProjectVectorNode::VdfnProjectVectorNode( eProject ePrj )
: IVdfnNode(), m_ePrj(ePrj),
  m_pVecIn(NULL),
  m_pValOut( new TVdfnPort<float> )
{
    RegisterInPortPrototype( "vec", new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
    RegisterOutPort("out", m_pValOut);
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool VdfnProjectVectorNode::GetIsValid() const
{
    return (m_ePrj != PRJ_NONE) && IVdfnNode::GetIsValid();
}

bool VdfnProjectVectorNode::PrepareEvaluationRun()
{
    m_pVecIn = dynamic_cast<TVdfnPort<VistaVector3D>*>(GetInPort( "vec" ));
    return GetIsValid();
}

bool VdfnProjectVectorNode::DoEvalNode()
{
    m_pValOut->SetValue( m_pVecIn->GetValueConstRef()[m_ePrj], GetUpdateTimeStamp() );
    return true;
}

// #############################################################################

IVdfnNode *VdfnProjectVectorNodeCreate::CreateNode( const VistaPropertyList &oParams ) const
{
    try
    {
        const VistaPropertyList &subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

        VdfnProjectVectorNode::eProject ePrj = VdfnProjectVectorNode::PRJ_NONE;

        std::string strProject;
		if( subs.GetValue("component", strProject ) )
		{
			if(VistaAspectsComparisonStuff::StringEquals("X", strProject, false))
				ePrj = VdfnProjectVectorNode::PRJ_X;
			else if(VistaAspectsComparisonStuff::StringEquals("Y", strProject, false))
				ePrj = VdfnProjectVectorNode::PRJ_Y;
			else if(VistaAspectsComparisonStuff::StringEquals("Z", strProject, false))
				ePrj = VdfnProjectVectorNode::PRJ_Z;
		}

        return new VdfnProjectVectorNode(ePrj);


    }
    catch( VistaExceptionBase & )
    {

    }
    return NULL;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


