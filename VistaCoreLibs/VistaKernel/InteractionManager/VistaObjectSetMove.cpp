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


#include "VistaObjectSetMove.h"

#include <VistaAspects/VistaTransformable.h>

#include <algorithm>
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaObjectSetMove::VistaObjectSetMove()
: m_pMoveFilter(NULL)
{
}

VistaObjectSetMove::~VistaObjectSetMove()
{
	delete m_pMoveFilter;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaObjectSetMove::StartMove( const VistaVector3D &v3Pos,
				const VistaQuaternion &qOri )
{
	for( SELECTVEC::iterator it = m_vecRegisteredObjects.begin();
		 it != m_vecRegisteredObjects.end(); ++it)
	{
		_sLookup &lookup = *it;

		lookup.m_pObj->GetTranslation(&lookup.m_v3Trans[0]);
		lookup.m_pObj->GetRotation(&lookup.m_qOri[0]);

		// retrieve context frame (parent's frame)
		float mt4x4[16];
		VistaVector3D wcPos;
		VistaQuaternion wcOri;
		if(lookup.m_pObj->GetParentWorldTransform(mt4x4))
		{
			VistaTransformMatrix m(mt4x4);
			VistaTransformMatrix mLc(lookup.m_qOri, lookup.m_v3Trans);
			m = mLc * m;

			m.GetTranslation(wcPos);
			wcOri = VistaQuaternion(m);
		}
		else
		{
			// else: use identity
			lookup.m_pObj->GetTranslation(&wcPos[0]);
			lookup.m_pObj->GetRotation(&wcOri[0]);
		}
		lookup.m_nXForm.Init(v3Pos, qOri,wcPos, wcOri);
	}
	return true;
}


bool VistaObjectSetMove::Move( const VistaVector3D &v3Pos,
			const VistaQuaternion &qOri )
{
	VistaVector3D childPOut;
	VistaQuaternion childQOut;

	for( SELECTVEC::iterator it = m_vecRegisteredObjects.begin();
		 it != m_vecRegisteredObjects.end(); ++it)
	{
		_sLookup &lookup = *it;
		lookup.m_nXForm.Update(v3Pos, qOri, childPOut, childQOut);

		if(m_pMoveFilter)
		{
			VistaVector3D v3Out;
			VistaQuaternion qOut;

			if(m_pMoveFilter->Move( lookup.m_nXForm.GetInitialParentPos(),
					                lookup.m_nXForm.GetInitialParentOri(),
					                childPOut, childQOut,
					                v3Out, qOut ) )
			{
				// apply
				childPOut = v3Out;
				childQOut = qOut;
			}
		}

		lookup.m_pObj->SetTranslation(&childPOut[0]);
		lookup.m_pObj->SetRotation(&childQOut[0]);
	}
	return true;
}

bool VistaObjectSetMove::RegisterObject(IVistaTransformable *pTrans)
{
	if(!GetIsRegistered(pTrans))
	{
		m_vecRegisteredObjects.push_back( _sLookup(pTrans) );
		return true;
	}
	return false;
}

bool VistaObjectSetMove::GetIsRegistered(IVistaTransformable *pTrans) const
{
	for(SELECTVEC::const_iterator cit = m_vecRegisteredObjects.begin();
		cit != m_vecRegisteredObjects.end(); ++cit)
	{
		if( (*cit).m_pObj == pTrans )
			return true;
	}
	return false;
}

bool VistaObjectSetMove::UnregisterObject(IVistaTransformable *pTrans)
{
	SELECTVEC::iterator it = std::remove( m_vecRegisteredObjects.begin(),
										  m_vecRegisteredObjects.end(),
										  pTrans );
	if( it == m_vecRegisteredObjects.end() )
		return false; // not found
	m_vecRegisteredObjects.erase(it, m_vecRegisteredObjects.end() );
	return true;
}


VistaObjectSetMove::IMoveFilter *VistaObjectSetMove::GetMoveFilter() const
{
	return m_pMoveFilter;
}

void VistaObjectSetMove::SetMoveFilter(IMoveFilter *pFilter)
{
	m_pMoveFilter = pFilter;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


