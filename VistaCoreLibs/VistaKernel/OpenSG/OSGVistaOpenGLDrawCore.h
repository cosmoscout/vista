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

#ifndef _OSGVISTAOPENGLDRAWCORE_H_
#define _OSGVISTAOPENGLDRAWCORE_H_

#ifdef __sgi
#pragma once
#endif

#include "OSGVistaOpenGLDrawCoreBase.h"

#include <VistaKernel/VistaKernelConfig.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif
#include <OpenSG/OSGConfig.h>
#ifdef WIN32
#pragma warning(pop)
#endif


class IVistaOpenGLDraw;
OSG_BEGIN_NAMESPACE

class VISTAKERNELAPI VistaOpenGLDrawCore : public VistaOpenGLDrawCoreBase
{
private:


	typedef VistaOpenGLDrawCoreBase Inherited;
	typedef std::map<const VistaOpenGLDrawCore*, IVistaOpenGLDraw*> DrawObjectsMap;

	/*==========================  PUBLIC  =================================*/
public:

	/*---------------------------------------------------------------------*/
	/*! \name                      Sync                                    */
	/*! \{                                                                 */

	virtual void changed(BitVector  whichField, UInt32 origin);

	// get/set callback
	IVistaOpenGLDraw* GetOpenGLDraw() const;
	void SetOpenGLDraw(IVistaOpenGLDraw* ptr) const;

	// draw the object
    virtual Action::ResultE drawPrimitives(DrawActionBase * action);

    // updates
    void adjustVolume(Volume & volume);


	static void InvalidateMarkedVolumes();

	/*! \}                                                                 */
	/*---------------------------------------------------------------------*/
	/*! \name                     Output                                   */
	/*! \{                                                                 */

	virtual void dump(UInt32 uiIndent = 0, const BitVector bvFlags = 0) const;


	/*! \}                                                                 */
	/*=========================  PROTECTED  ===============================*/
protected:

	// Variables should all be in VistaOpenGLDrawCoreBase.

	/*---------------------------------------------------------------------*/
	/*! \name                  Constructors                                */
	/*! \{                                                                 */

	VistaOpenGLDrawCore(void);
	VistaOpenGLDrawCore(const VistaOpenGLDrawCore &source);

	/*! \}                                                                 */
	/*---------------------------------------------------------------------*/
	/*! \name                   Destructors                                */
	/*! \{                                                                 */

	virtual ~VistaOpenGLDrawCore(void);

	/*! \}                                                                 */

	/*==========================  PRIVATE  ================================*/
private:

	static DrawObjectsMap s_mapOpenGLDrawObjects;
	//IVistaOpenGLDraw *m_pOpenGLDraw;

	friend class FieldContainer;
	friend class VistaOpenGLDrawCoreBase;

	static void initMethod(void);

	// prohibit default functions (move to 'public' if you need one)

	void operator =(const VistaOpenGLDrawCore &source);
};

typedef VistaOpenGLDrawCore *VistaOpenGLDrawCoreP;

OSG_END_NAMESPACE

#include "OSGVistaOpenGLDrawCoreBase.inl"
#include "OSGVistaOpenGLDrawCore.inl"

#define OSGVistaOpenGLDrawCore_HEADER_CVSID "@(#)$Id$"

#endif /* _OSGVISTAOPENGLDRAWCORE_H_ */
