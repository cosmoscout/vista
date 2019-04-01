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


#include "Vdfn3DNormalizeNode.h"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
Vdfn3DNormalizeNode::Vdfn3DNormalizeNode()
: IVdfnNode(),
	m_pOut( new TVdfnPort<VistaTransformMatrix> ),
	m_pFlipX(NULL),
	m_pFlipY(NULL),
	m_pFlipZ(NULL),
	m_pMinX(NULL),
	m_pMinY(NULL),
	m_pMinZ(NULL),
	m_pTgW(NULL),
	m_pTgH(NULL),
	m_pTgD(NULL),
	m_pSrW(NULL),
	m_pSrH(NULL),
	m_pSrD(NULL),
	m_minx(0.0f),
	m_miny(0.0f),
	m_minz(0.0f),
	m_targetw(1.0f),
	m_targeth(1.0f),
	m_targetd(1.0f),
	m_flipx(false),
	m_flipy(false),
	m_flipz(false)
{
	RegisterInPortPrototype( "min_x", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "min_y", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "min_z", new TVdfnPortTypeCompare<TVdfnPort<float> > );

	RegisterInPortPrototype( "target_w", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "target_h", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "target_d", new TVdfnPortTypeCompare<TVdfnPort<float> > );

	RegisterInPortPrototype( "source_w", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "source_h", new TVdfnPortTypeCompare<TVdfnPort<float> > );
	RegisterInPortPrototype( "source_d", new TVdfnPortTypeCompare<TVdfnPort<float> > );

	RegisterInPortPrototype( "flip_x", new TVdfnPortTypeCompare<TVdfnPort<bool> > );
	RegisterInPortPrototype( "flip_y", new TVdfnPortTypeCompare<TVdfnPort<bool> > );
	RegisterInPortPrototype( "flip_z", new TVdfnPortTypeCompare<TVdfnPort<bool> > );

	RegisterOutPort( "transform", m_pOut );

}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
bool Vdfn3DNormalizeNode::GetIsValid() const
{
	return (m_pSrW && m_pSrH && m_pSrD);
}

bool Vdfn3DNormalizeNode::PrepareEvaluationRun()
{
	m_pTgW = dynamic_cast<TVdfnPort<float>*>( GetInPort("target_w") );
	m_pTgH = dynamic_cast<TVdfnPort<float>*>( GetInPort("target_h") );
	m_pTgD = dynamic_cast<TVdfnPort<float>*>( GetInPort("target_d") );

	m_pSrW = dynamic_cast<TVdfnPort<float>*>( GetInPort("source_w") );
	m_pSrH = dynamic_cast<TVdfnPort<float>*>( GetInPort("source_h") );
	m_pSrD = dynamic_cast<TVdfnPort<float>*>( GetInPort("source_d") );

	m_pMinX = dynamic_cast<TVdfnPort<float>*>( GetInPort("min_x") );
	m_pMinY = dynamic_cast<TVdfnPort<float>*>( GetInPort("min_y") );
	m_pMinZ = dynamic_cast<TVdfnPort<float>*>( GetInPort("min_z") );

	m_pFlipX = dynamic_cast<TVdfnPort<bool>*>( GetInPort("flip_x") );
	m_pFlipY = dynamic_cast<TVdfnPort<bool>*>( GetInPort("flip_y") );
	m_pFlipZ = dynamic_cast<TVdfnPort<bool>*>( GetInPort("flip_z") );

	return GetIsValid();
}

bool Vdfn3DNormalizeNode::DoEvalNode()
{
	VistaTransformMatrix &v = m_pOut->GetValueRef();
	m_minx = (m_pMinX ? m_pMinX->GetValue() : m_minx ); // optional
	m_miny = (m_pMinY ? m_pMinY->GetValue() : m_miny ); // optional
	m_minz = (m_pMinZ ? m_pMinZ->GetValue() : m_minz ); // optional

	// default is to scale to [0;1]
	m_targetw =  (m_pTgW ? m_pTgW->GetValue() : m_targetw ); // optional
	m_targeth =  (m_pTgH ? m_pTgH->GetValue() : m_targeth ); // optional
	m_targetd =  (m_pTgD ? m_pTgD->GetValue() : m_targetd ); // optional

	m_flipx =  (m_pFlipX ? m_pFlipX->GetValue() : m_flipx ); // optional
	m_flipy =  (m_pFlipY ? m_pFlipY->GetValue() : m_flipy ); // optional
	m_flipz =  (m_pFlipZ ? m_pFlipZ->GetValue() : m_flipz ); // optional

	// we calc: min_x + ( target_w * ( 1 - (source_w-pos_x)/source_w ) )
	// for all three dimensions, where pos_x is input to a deferred mult-matrix-vector
	// operation, e.g., normalize within [-1;1] from a 640/480 window means:
	// min_x = -1; target_w = 2; source_w = 640 (anvdfnlogously for height)

	float xN =( m_pSrW->GetValue() ? m_targetw/m_pSrW->GetValue() : 0.f);
	float yN =( m_pSrH->GetValue() ? m_targeth/m_pSrH->GetValue() : 0.f);
	float zN =( m_pSrD->GetValue() ? m_targetd/m_pSrD->GetValue() : 0.f);

	// plain write for the matrix (nicer to read in a debugger)
	v[0][0] = xN; v[0][1] =  0; v[0][2] =  0; v[0][3] = m_minx;
	v[1][0] =  0; v[1][1] = yN; v[1][2] =  0; v[1][3] = m_miny;
	v[2][0] =  0; v[2][1] =  0; v[2][2] = zN; v[2][3] = m_minz;
	v[3][0] =  0; v[3][1] =  0; v[3][2] =  0; v[3][3] = 1;

	// flip...
	if( m_flipx )
	{
		v[0][0] = -v[0][0];
		v[0][3] = -v[0][3];
	}
	if( m_flipy )
	{
		v[1][1] = -v[1][1];
		v[1][3] = -v[1][3];
	}
	if( m_flipz )
	{
		v[2][2] = -v[2][2];
		v[2][3] = -v[2][3];
	}

    m_pOut->SetUpdateTimeStamp(GetUpdateTimeStamp());
	m_pOut->IncUpdateCounter();

	return true;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


