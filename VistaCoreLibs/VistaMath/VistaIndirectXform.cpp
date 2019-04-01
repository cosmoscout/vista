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


#include "VistaIndirectXform.h"

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
// always put this line below your constant definitions
// to avoid problems with HP's compiler
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaIndirectXform::VistaIndirectXform(int iMode /*= XFORM_MODE_ATTACH */ )
{
	if (iMode == XFORM_MODE_MIRROR)
	{
		m_enMode = XFORM_MODE_MIRROR;
	}
	else
	{
		m_enMode = XFORM_MODE_ATTACH;
	}
}

VistaIndirectXform::~VistaIndirectXform()
{
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Init                                                        */
/*                                                                            */
/*============================================================================*/
bool VistaIndirectXform::Init(const VistaVector3D &vParentPosIn,
							   const VistaQuaternion &qParentOrientIn,
							   const VistaVector3D &vChildPosIn,
							   const VistaQuaternion &qChildOrientIn)
{
	m_vInitParentPos		=	vParentPosIn;
	m_qInitParentInvOrient	=	qParentOrientIn.GetComplexConjugated();
	m_vInitChildPos			=	vChildPosIn;
	m_qInitChildOrient		=	qChildOrientIn;

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Update                                                      */
/*                                                                            */
/*============================================================================*/
bool VistaIndirectXform::Update(const VistaVector3D &vParentPosIn,
								 const VistaQuaternion &qParentOrientIn,
								 VistaVector3D &vChildPosOut,
								 VistaQuaternion &qChildOrientOut)
{
	// first: compute the new orientation of the child
	// i.e. modify child's orientation by undoing parent's initial orientation
	// and applying parent's current rotation
	VistaQuaternion qParentRotation = qParentOrientIn * m_qInitParentInvOrient;
	qChildOrientOut = qParentRotation * m_qInitChildOrient;

	// make this stuff a little more efficient:
	if (m_enMode == XFORM_MODE_ATTACH)
	{
		vChildPosOut = qParentRotation.Rotate(m_vInitChildPos-m_vInitParentPos)+vParentPosIn;
	}
	else
	{
		vChildPosOut = m_vInitChildPos + vParentPosIn - m_vInitParentPos;
	}

	return true;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Get/SetMode                                                 */
/*                                                                            */
/*============================================================================*/
VistaIndirectXform::EnXformMode VistaIndirectXform::GetMode() const
{
	return m_enMode;
}

bool VistaIndirectXform::SetMode(int iMode)
{
	switch (iMode)
	{
	case XFORM_MODE_ATTACH:
		m_enMode = XFORM_MODE_ATTACH;
		break;
	case XFORM_MODE_MIRROR:
		m_enMode = XFORM_MODE_MIRROR;
		break;
	default:
		return false;
	}

	return true;
}


VistaVector3D VistaIndirectXform::GetInitialParentPos() const
{
	return m_vInitParentPos;
}

VistaQuaternion VistaIndirectXform::GetInitialParentOri() const
{
	return m_qInitParentInvOrient.GetComplexConjugated();
}

