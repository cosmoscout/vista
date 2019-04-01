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


#ifndef _VISTA_OPENSG_GLOVERLAYFOREGROUND_H_
#define _VISTA_OPENSG_GLOVERLAYFOREGROUND_H_

#ifdef __sgi
#pragma once
#endif

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/OpenSG/VistaOpenSGGLOverlayForegroundBase.h>

#if defined(WIN32)
//diable warnings from OpenSG includes
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGStatElemDesc.h>

#if defined(WIN32)
#pragma warning(pop)
#endif



OSG_BEGIN_NAMESPACE


class VISTAKERNELAPI VistaOpenSGGLOverlayForeground : public VistaOpenSGGLOverlayForegroundBase
{
  private:

	typedef VistaOpenSGGLOverlayForegroundBase Inherited;

	/*==========================  PUBLIC  =================================*/
  public:

	/*---------------------------------------------------------------------*/
	/*                             Sync                                    */
	/*---------------------------------------------------------------------*/

	virtual void changed(BitVector whichField,
						 UInt32    origin    );

	/*---------------------------------------------------------------------*/
	/*                            Output                                   */
	/*---------------------------------------------------------------------*/

	virtual void dump(      UInt32     uiIndent = 0,
					  const BitVector  bvFlags  = 0) const;

	/*---------------------------------------------------------------------*/
	/*                             Draw                                    */
	/*---------------------------------------------------------------------*/

	virtual void draw( DrawActionBase * action, Viewport * port );

	/*---------------------------------------------------------------------*/
	/*                      Convenience Functions                          */
	/*---------------------------------------------------------------------*/

	/*=========================  PROTECTED  ===============================*/
  protected:

	// Variables should all be in TextForegroundBase.

	/*---------------------------------------------------------------------*/
	/*                         Constructors                                */
	/*---------------------------------------------------------------------*/

	VistaOpenSGGLOverlayForeground(void);
	VistaOpenSGGLOverlayForeground(const VistaOpenSGGLOverlayForeground &source);

	/*---------------------------------------------------------------------*/
	/*                          Destructors                                */
	/*---------------------------------------------------------------------*/

	virtual ~VistaOpenSGGLOverlayForeground(void);


	/*==========================  PRIVATE  ================================*/
  private:


	friend class FieldContainer;
	friend class VistaOpenSGGLOverlayForegroundBase;

	static void initMethod(void);

//    static void initText(void);

	// prohibit default functions (move to 'public' if you need one)

	void operator =(const VistaOpenSGGLOverlayForeground &source);
};

typedef VistaOpenSGGLOverlayForeground *VistaOpenSGGLOverlayForegroundP;

OSG_END_NAMESPACE

#include "VistaOpenSGGLOverlayForegroundBase.inl"
#include "VistaOpenSGGLOverlayForeground.inl"

#endif
