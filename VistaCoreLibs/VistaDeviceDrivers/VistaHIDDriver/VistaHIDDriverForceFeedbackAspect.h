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


#ifndef _VISTAHIDDRIVERFORCEFEEDBACKASPECT_H
#define _VISTAHIDDRIVERFORCEFEEDBACKASPECT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#if defined(LINUX)

#include <linux/input.h>

#include <VistaDeviceDriversBase/DriverAspects/VistaDriverForceFeedbackAspect.h>

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaConnection;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * aspect for force feedback implementation based on the HID layer of
 * the Linux event subsystem.
 *
 * @todo add documentation for this PLEAZE
 */
class VistaHIDDriverForceFeedbackAspect : public IVistaDriverForceFeedbackAspect
{
public:
	VistaHIDDriverForceFeedbackAspect(VistaConnection *pCon);
	virtual ~VistaHIDDriverForceFeedbackAspect();

	virtual bool SetForce( const VistaVector3D & force,
						   const VistaVector3D & v3Ignored);
	using IVistaDriverForceFeedbackAspect::SetForce; //to make overload available

	int GetNumInputDOF() const;
	int GetNumOutputDOF() const;

    /**
     * Returns the maximum stiffness of this device.
     * @return 0 when this parameter is not present, > 0 else
     */
    virtual float GetMaximumStiffness() const;

    /**
     * Returns the maximal force to be output by the device.
     * @return 0 for not applicable, > 0 else
     */
    virtual float GetMaximumForce() const;

	virtual bool SetForcesEnabled(bool bEnabled);
	virtual bool GetForcesEnabled() const;

protected:

private:
	ff_effect m_effect;

	VistaConnection *m_pCon;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // LINUX
#endif //_VISTAHIDDRIVERFORCEFEEDBACKASPECT_H
