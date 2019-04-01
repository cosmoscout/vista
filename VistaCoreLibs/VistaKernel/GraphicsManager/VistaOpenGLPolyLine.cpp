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


#include <GL/glew.h>

#include "VistaOpenGLPolyLine.h"

#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>

#include <VistaBase/VistaStreamUtils.h>

#include <iostream>
#include <cstring>
#include <algorithm>

#if defined(WIN32)
// include _before_ gl.h
#include <Windows.h>
#endif

#if defined(DARWIN)
#include <OpenGL/OpenGL.h>
#else
#include <GL/gl.h>
#endif 

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/
using namespace std;

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VistaOpenGLPolyLine::VistaOpenGLPolyLine(VistaSceneGraph * pVistaSceneGraph, VistaGroupNode* pParent)
: m_pDrawInterface(NULL)
, m_pOglNode(NULL)
, m_bRemoveFromSGOnDelete(true)
{
	m_pDrawInterface = new COpenGLPolyLineDraw;
	m_pOglNode = pVistaSceneGraph->NewOpenGLNode(pParent, m_pDrawInterface);	
	m_pOglNode->SetName("VistaOpenGLPolyLine");
}


VistaOpenGLPolyLine::~VistaOpenGLPolyLine()
{
	// ogl node was deleted by VistaPrimitive, so
	// discard the draw interface
	if(GetRemoveFromSGOnDelete())
		delete m_pDrawInterface;
}


/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/
bool VistaOpenGLPolyLine::GetIsClosedLine() const
{
	return (m_pDrawInterface->GetLineMode() == COpenGLPolyLineDraw::LINE_CLOSED);
}


void VistaOpenGLPolyLine::SetIsClosedLine(bool bLineClosed)
{
	m_pDrawInterface->SetLineMode( bLineClosed ? COpenGLPolyLineDraw::LINE_CLOSED : COpenGLPolyLineDraw::LINE_OPEN);
}

IVistaNode* VistaOpenGLPolyLine::GetVistaNode() const
{
	return m_pOglNode;
}

void VistaOpenGLPolyLine::SetLinePoints(const std::vector<float> &vecPoints)
{
	m_pDrawInterface->SetLinePoints(vecPoints);
}

bool VistaOpenGLPolyLine::GetLinePoints(std::vector<float> &vecPoints) const
{
	return m_pDrawInterface->GetLinePoints(vecPoints);
}

bool VistaOpenGLPolyLine::GetRemoveFromSGOnDelete() const
{
	return m_bRemoveFromSGOnDelete;
}

void VistaOpenGLPolyLine::SetRemoveFromSGOnDelete(bool bRemove)
{
	m_bRemoveFromSGOnDelete = bRemove;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaOpenGLPolyLine::COpenGLPolyLineDraw::COpenGLPolyLineDraw()
: m_iDispId(-1), m_bDlistDirty(false), m_eMode(LINE_OPEN)
{
}

VistaOpenGLPolyLine::COpenGLPolyLineDraw::~COpenGLPolyLineDraw()
{
	if(m_iDispId == -1)
		glDeleteLists(m_iDispId, 1); // remove old list
}

//bool VistaOpenGLPolyLine::COpenGLPolyLineDraw::DrawDisplayObjects()
bool VistaOpenGLPolyLine::COpenGLPolyLineDraw::Do()
{
	if(m_bDlistDirty)
		if(!UpdateDisplayList())
			return false;

	glMatrixMode(GL_MODELVIEW);

	// draw the 
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glCallList(m_iDispId);
	glPopAttrib();
	glPopMatrix();


	return true;
}

bool VistaOpenGLPolyLine::COpenGLPolyLineDraw::UpdateDisplayList()
{
	if(m_iDispId != -1)
	{
		// delete old display list
		glDeleteLists(m_iDispId,1);
		m_iDispId = -1; // reset list id
	}

	if(m_iDispId == -1)
	{
		// ok, no disp created
		if((m_iDispId = glGenLists(1))==-1) // create _one_ display list
			return false; // or bail out
	}

	// create a new list @ index 0
	// and compile it
	glNewList(m_iDispId, GL_COMPILE);
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);

	float fDiff[4];
	std::memset(fDiff, 0, 4);

	m_oMat.GetDiffuseColor(fDiff);
  //  if(m_oMat.GetOpacity() > 0)
  //  {
  //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //      glEnable(GL_BLEND);
  //  }
  //  else
  //      glDisable(GL_BLEND);

	glColor3fv(fDiff);

	float fMin[3], fMax[3];
	fMin[0] = fMin[1] = fMin[2] = 0.0f;
	fMax[0] = fMax[1] = fMax[2] = 0.0f;


	// single line
	glBegin((m_eMode == LINE_OPEN ? GL_LINE_STRIP : GL_LINE_LOOP));
	for(unsigned int i=0; i<m_vecPoints.size(); i=i+3)
	{
		glVertex3f(m_vecPoints[i], m_vecPoints[i+1], m_vecPoints[i+2]);
#if defined(WIN32) && defined(_MSC_VER) && (_MSC_VER<1300)
#if !defined(MIN)
#define MIN(A,B) A>B?B:A
#endif
#if !defined(MAX)
#define MAX(A,B) A>B?A:B
#endif

		fMin[0] = MIN(m_vecPoints[i]  , fMin[0]);
		fMin[1] = MIN(m_vecPoints[i+1], fMin[1]);
		fMin[2] = MIN(m_vecPoints[i+2], fMin[2]);
		fMax[0] = MAX(m_vecPoints[i]  , fMax[0]);
		fMax[1] = MAX(m_vecPoints[i+1], fMax[1]);
		fMax[2] = MAX(m_vecPoints[i+2], fMax[2]);

#else
		fMin[0] = std::min<float>(m_vecPoints[i]  , fMin[0]);
		fMin[1] = std::min<float>(m_vecPoints[i+1], fMin[1]);
		fMin[2] = std::min<float>(m_vecPoints[i+2], fMin[2]);
		fMax[0] = std::max<float>(m_vecPoints[i]  , fMax[0]);
		fMax[1] = std::max<float>(m_vecPoints[i+1], fMax[1]);
		fMax[2] = std::max<float>(m_vecPoints[i+2], fMax[2]);
#endif

	}
	glEnd();
	glPopAttrib();
	glEndList();

	m_oBBox.SetBounds(fMin, fMax);
	
	// tell the SG to fetch a new BBox

	m_bDlistDirty = false;
	return true;
}



bool VistaOpenGLPolyLine::COpenGLPolyLineDraw::GetBoundingBox(VistaBoundingBox &bb)
{
	bb = m_oBBox;
	return true;
}

void VistaOpenGLPolyLine::COpenGLPolyLineDraw::SetLinePoints(const std::vector<float> &vecPoints)
{
	if(vecPoints.size() % 3 != 0)
		vstr::outi() << "COpenGLPolyLineDraw::SetLinePoints() -- vecPoints.size() %3 != 0" << std::endl;
	m_vecPoints = vecPoints;
	m_bDlistDirty = true;
}

bool VistaOpenGLPolyLine::COpenGLPolyLineDraw::GetLinePoints(std::vector<float> &vecPoints) const
{
	vecPoints = m_vecPoints;
	return true;
}

VistaOpenGLPolyLine::COpenGLPolyLineDraw::eMode VistaOpenGLPolyLine::COpenGLPolyLineDraw::GetLineMode() const
{
	return m_eMode;
}

void  VistaOpenGLPolyLine::COpenGLPolyLineDraw::SetLineMode(eMode eMd)
{
	if(eMd != m_eMode)
	{
		m_eMode = eMd;
		m_bDlistDirty = true;
	}
}

VistaMaterial VistaOpenGLPolyLine::COpenGLPolyLineDraw::GetMaterial() const
{
	return m_oMat;
}

void VistaOpenGLPolyLine::COpenGLPolyLineDraw::SetMaterial(const VistaMaterial &mat)
{
	m_oMat = mat;
	m_bDlistDirty = true;
}

bool VistaOpenGLPolyLine::SetMaterial(const VistaMaterial & oMat) const
{
	m_pDrawInterface->SetMaterial(oMat);
	return true;
}

bool VistaOpenGLPolyLine::SetColor (const VistaColor  & color)
{
	m_pDrawInterface->SetMaterial(VistaMaterial( VistaColor::BLACK,
												 VistaColor(color),
												 VistaColor::BLACK,
												 VistaColor::BLACK,
												 1,1,""));

	return true;
}


