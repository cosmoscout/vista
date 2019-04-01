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


#include "OSGVistaOpenGLDrawCore.h"

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>
#include <VistaMath/VistaBoundingBox.h>

#include <VistaBase/VistaStreamUtils.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif

#include <OpenSG/OSGConfig.h>

#include <OpenSG/OSGDrawAction.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGNodeFields.h>

#ifdef WIN32
#pragma warning(pop)
#endif

#include <cstdlib>
#include <cstdio>
#include <iostream>
using namespace std;

OSG_BEGIN_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::VistaOpenGLDrawCore
A NodeCore that references a Vista::IVistaOpenGLDraw Object to draw some plain OpenGL. As the referenced object is no OpenSG FieldContainer, we don't name it here but  manually add it in the implementation!
*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/
VistaOpenGLDrawCore::DrawObjectsMap VistaOpenGLDrawCore::s_mapOpenGLDrawObjects;


/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void VistaOpenGLDrawCore::initMethod (void)
{
#if (OSG_VERSION <= 0x010800)
	DrawAction::registerEnterDefault( getClassType(),
		osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, MaterialDrawablePtr,
			  CNodePtr, Action *>(&MaterialDrawable::drawActionHandler));

	RenderAction::registerEnterDefault( getClassType(),
		osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, MaterialDrawablePtr,
			  CNodePtr, Action *>(&MaterialDrawable::renderActionHandler));
#else
	DrawAction::registerEnterDefault( getClassType(),
		osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, MaterialDrawablePtr,
			  CNodePtr, Action *>(&MaterialDrawable::drawActionEnterHandler));

	RenderAction::registerEnterDefault( getClassType(),
		osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE, MaterialDrawablePtr,
			  CNodePtr, Action *>(&MaterialDrawable::renderActionEnterHandler));
#endif
}

void VistaOpenGLDrawCore::InvalidateMarkedVolumes() // STATIC!!
{
	DrawObjectsMap::iterator it(s_mapOpenGLDrawObjects.begin());
	const DrawObjectsMap::iterator itEnd(s_mapOpenGLDrawObjects.end());
	while(it != itEnd)
	{
		if(it->second && it->second->GetAndResetDirtyFlag())
		{
			const VistaOpenGLDrawCore *pObj = it->first;
			for(osg::UInt32 i = 0; i < pObj->_parents.size(); ++i)
			{
				pObj->_parents[i]->invalidateVolume();
			}
		}
		it++;
	}
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

IVistaOpenGLDraw* VistaOpenGLDrawCore::GetOpenGLDraw() const
{
	return s_mapOpenGLDrawObjects[this];
}

void VistaOpenGLDrawCore::SetOpenGLDraw(IVistaOpenGLDraw *ptr) const
{
	s_mapOpenGLDrawObjects[this] = ptr;
}

Action::ResultE VistaOpenGLDrawCore::drawPrimitives(DrawActionBase * action)
{
	s_mapOpenGLDrawObjects[this]->Do();
	return Action::Continue;
}

void VistaOpenGLDrawCore::adjustVolume(Volume & volume)
{
    volume.setValid(true);
	VistaBoundingBox bb;
	if(!s_mapOpenGLDrawObjects[this]->GetBoundingBox(bb))
	{
		/** @todo is this an infinite or an empty volume?
		 * setInfinite seems not to work as expected with frustumn culling
		 * as the "infinite" state is not evaluated in opensg's box/frustum
		 * intersection tests :-(
		 */
		volume.setEmpty();
		osg::Pnt3f pMin(0,0,0);
		volume.extendBy(pMin);
#ifdef DEBUG
		vstr::warnp() << "[VistaOpenGLDrawCore]: got no bounding box from OpenGLDraw object! Creating zero-volume around the node's origin..." << std::endl;
#endif
	}
	else
	{
		volume.setEmpty();
		osg::Pnt3f pMin, pMax;
		bb.GetBounds(&pMin[0], &pMax[0]);
		volume.extendBy(pMin);
		volume.extendBy(pMax);
	}

	//m_pOpenGLDraw->GetBoundingBox();

	// will this be called every frame, when we set the volume to invalid?
}


/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

VistaOpenGLDrawCore::VistaOpenGLDrawCore(void) :
    Inherited()
{
//	setMaterial( osg::getDefaultMaterial() );
	s_mapOpenGLDrawObjects[this] = NULL;
}

VistaOpenGLDrawCore::VistaOpenGLDrawCore(const VistaOpenGLDrawCore &source) :
    Inherited(source)
{
//	setMaterial( osg::getDefaultMaterial() );
	s_mapOpenGLDrawObjects[this] = NULL;
}

VistaOpenGLDrawCore::~VistaOpenGLDrawCore(void)
{
	s_mapOpenGLDrawObjects.erase(this);
}

/*----------------------------- class specific ----------------------------*/

void VistaOpenGLDrawCore::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

void VistaOpenGLDrawCore::dump(      UInt32    ,
                         const BitVector ) const
{
    SLOG << "Dump VistaOpenGLDrawCore NI" << std::endl;
}


/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id$";
    static Char8 cvsid_hpp       [] = OSGVistaOpenGLDrawCoreBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGVistaOpenGLDrawCoreBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGVistaOpenGLDrawCoreFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

OSG_END_NAMESPACE

