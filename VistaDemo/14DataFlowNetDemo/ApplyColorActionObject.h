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


#ifndef _APPLYCOLORACTIONOBJECT_H
#define _APPLYCOLORACTIONOBJECT_H

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

class VistaGeometry;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * The ApplyColorActionObject modifies the Geometry when receiving a new color.
 * In contrast to the SetPositionActionObject, it also has a getter, so that
 * a corresponding ActionNode has an outport.
 */
class ApplyColorActionObject : public IVdfnActionObject
{
public:
	ApplyColorActionObject( VistaGeometry* pTargetGeometry );
	~ApplyColorActionObject();

	bool SetColor( const VistaVector3D& v3Color ); 
	VistaVector3D GetColor( ) const;

	REFL_DECLARE;

private:
	VistaGeometry*		m_pGeometry;
	VistaVector3D		m_v3Color;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__SPATIALDFN_H
