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
// $Id: VistaSerializable.cpp 28992 2012-01-27 09:12:29Z dr165799 $

#include "VistaSharedCore.h"


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaSharedCore::IVistaSharedCore()
: m_nReferenceCount( 0 )
{

}

IVistaSharedCore::~IVistaSharedCore()
{

}

VistaType::sint32 IVistaSharedCore::GetReferenceCount() const
{
	return m_nReferenceCount.Get();
}

void IVistaSharedCore::IncReferenceCount()
{
	m_nReferenceCount.Inc();
}

void IVistaSharedCore::DecReferenceCount()
{
	if( m_nReferenceCount.DecAndTestNull() )
		delete this;
}


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/


