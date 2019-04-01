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


#ifndef _VISTAOPENGLDEBUG_H
#define _VISTAOPENGLDEBUG_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <iostream>
#include <string>
#include <VistaKernel/VistaKernelConfig.h>
//#include "VistaStd.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaOpenGLDebug
{
public:
	VistaOpenGLDebug();
	virtual ~VistaOpenGLDebug();

	virtual bool DebugMsg (bool);
	virtual bool DebugMsg (double);
	virtual bool DebugMsg (int);
	virtual bool DebugMsg (const char*);
	virtual bool DebugMsg (std::string);

	void DebugOglError ();
	void DebugProjection ();
	void DebugModelMatrix ();
	void DebugProjectionMatrix ();
	void DebugShading ();
	void DebugBlending ();
	void DebugLighting ();
	void DebugLight (int lightNum);

private:
	std::ostream *    m_pOutputStream;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // _VISTAOPENGLDEBUG_H
