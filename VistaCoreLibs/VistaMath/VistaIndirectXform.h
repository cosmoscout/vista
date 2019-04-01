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


#ifndef _VISTAINDIRECTXFORM_H
#define _VISTAINDIRECTXFORM_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaVectorMath.h>
#include "VistaMathConfig.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAMATHAPI VistaIndirectXform
{
public:
	enum EnXformMode
	{
		XFORM_MODE_ATTACH,
		XFORM_MODE_MIRROR
	};

public:
	// CONSTRUCTORS / DESTRUCTOR
	VistaIndirectXform(int iMode = XFORM_MODE_ATTACH);
	virtual ~VistaIndirectXform();

	// IMPLEMENTATION
	// initialize xform object with parent and child data
	virtual	bool	Init	(	const VistaVector3D   & vParentPosIn,
								const VistaQuaternion & qParentOrientIn,
								const VistaVector3D   & vChildPosIn,
								const VistaQuaternion & qChildOrientIn	);

	// compute child data according to updated parent data
	virtual	bool	Update	(	const VistaVector3D   & vParentPosIn,
								const VistaQuaternion & qParentOrientIn,
									  VistaVector3D   & vChildPosOut,
									  VistaQuaternion & qChildOrientOut	);

	// get/set attach/mirror state
	virtual	EnXformMode	GetMode() const;
	virtual	bool		SetMode(int iMode);

	VistaVector3D GetInitialParentPos() const;
	VistaQuaternion GetInitialParentOri() const;

protected:
	VistaVector3D		m_vInitParentPos;
	VistaQuaternion	m_qInitParentInvOrient;
	VistaVector3D		m_vInitChildPos;
	VistaQuaternion	m_qInitChildOrient;
	EnXformMode			m_enMode;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAINDIRECTXFORM_H
