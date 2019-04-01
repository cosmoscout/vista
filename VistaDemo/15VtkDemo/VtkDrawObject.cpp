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


#include <vtkPolyDataMapper.h>
#include <vtkConeSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkLineSource.h>
#include <vtkActorCollection.h>
#include <vtkDataSet.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaMath/VistaBoundingBox.h> 

#include "VtkDrawObject.h"

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/
VtkDrawObject::VtkDrawObject() :
m_pVtkRenderer(NULL),
m_pActorColl(NULL)
{
	m_pVtkRenderer = vtkRenderer::New();
	this->AddRenderer (m_pVtkRenderer);

	// create vtk objects
	// 1.) The Object
	vtkConeSource * pCone = vtkConeSource::New();
	pCone->SetResolution(8);
	// 2. The Mapper
	vtkPolyDataMapper * pConeMapper = vtkPolyDataMapper::New();
#if (VTK_MAJOR_VERSION < 6)
	pConeMapper->SetInput(pCone->GetOutput());
#else
	pConeMapper->SetInputData(pCone->GetOutput());
#endif
	
	// 3. The Actor
	vtkActor * pConeActor = vtkActor::New();
	pConeActor->SetMapper (pConeMapper);
	pConeActor->GetProperty()->SetColor (0.2,0.63,0.79);
	pConeActor->GetProperty()->SetDiffuse (0.7);
	pConeActor->GetProperty()->SetSpecular (0.4);
	pConeActor->GetProperty()->SetSpecularPower (20);
	pConeActor->SetPosition (2,2,-5);

	// world coord system cross
	vtkLineSource * pXAxis = vtkLineSource::New();
	pXAxis->SetPoint1 (0,0,0);
	pXAxis->SetPoint2 (1,0,0);
	pXAxis->SetResolution (1);
	vtkPolyDataMapper * pXAxisMapper = vtkPolyDataMapper::New();
#if (VTK_MAJOR_VERSION < 6)
	pXAxisMapper->SetInput(pXAxis->GetOutput());
#else
	pXAxisMapper->SetInputData(pXAxis->GetOutput());
#endif
	vtkActor * pXAxisActor = vtkActor::New();
	pXAxisActor->SetMapper (pXAxisMapper);
	pXAxisActor->GetProperty()->SetColor (1.0,0,0);

	vtkLineSource * pYAxis = vtkLineSource::New();
	pYAxis->SetPoint1 (0,0,0);
	pYAxis->SetPoint2 (0,1,0);
	vtkPolyDataMapper * pYAxisMapper = vtkPolyDataMapper::New();
	
#if (VTK_MAJOR_VERSION < 6)
	pYAxisMapper->SetInput(pYAxis->GetOutput());
#else
	pYAxisMapper->SetInputData(pYAxis->GetOutput());
#endif
	vtkActor * pYAxisActor = vtkActor::New();
	pYAxisActor->SetMapper (pYAxisMapper);
	pYAxisActor->GetProperty()->SetColor (0,1.0,0);

	vtkLineSource * pZAxis = vtkLineSource::New();
	pZAxis->SetPoint1 (0,0,0);
	pZAxis->SetPoint2 (0,0,1);
	vtkPolyDataMapper * pZAxisMapper = vtkPolyDataMapper::New();
#if (VTK_MAJOR_VERSION < 6)
	pZAxisMapper->SetInput(pZAxis->GetOutput());
#else
	pZAxisMapper->SetInputData(pZAxis->GetOutput());
#endif
	vtkActor * pZAxisActor = vtkActor::New();
	pZAxisActor->SetMapper (pZAxisMapper);
	pZAxisActor->GetProperty()->SetColor (0.5,0.5,1.0);

	// add vtk actors
	m_pActorColl = vtkActorCollection::New();
	m_pActorColl->AddItem (pConeActor);
	m_pActorColl->AddItem (pXAxisActor);
	m_pActorColl->AddItem (pYAxisActor);
	m_pActorColl->AddItem (pZAxisActor);
}

VtkDrawObject::~VtkDrawObject()
{
	if (m_pActorColl)
	{
		vtkActor * pCurActor = NULL;
		m_pActorColl->InitTraversal();
		do
		{
			pCurActor = m_pActorColl->GetNextActor();
			if (pCurActor != NULL)
			{
				vtkPolyDataMapper*  pCurMapper = (vtkPolyDataMapper*)pCurActor->GetMapper();
				vtkDataSet* pCurData   = pCurMapper->GetInput();
				pCurActor->Delete();
				pCurMapper->Delete();
				pCurData->Delete();
			}
		}
		while (pCurActor != NULL);
		m_pActorColl->Delete();
	}
	m_pActorColl = NULL;

	if (m_pVtkRenderer)
	{
		this->RemoveRenderer (m_pVtkRenderer);
		m_pVtkRenderer->Delete();
	}
	m_pVtkRenderer = NULL;

	// At that point we have a problem with reference counters.
	// It appears even when no actors and friends are created.
	// Therfore, it seems to be a problem of vtk?OpenGLRenderWindow.
	// How to solve this problem?
	// ToDo!!!!!!!!!

	// UPDATE: the issue has been resolved by calling "m_pDrawObject->Delete()"
	// instead of "delete m_pDrawObject". (ms 030120)
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VtkDrawObject::GetBoundingBox(VistaBoundingBox &oBB)
{
	// @todo: calculate more accurate bounding box
	VistaBoundingBox oBBTemp(
		VistaVector3D(-10,-10,-10),
		VistaVector3D(10,10,10));

	oBB = oBBTemp;
		
	return true;
}

bool VtkDrawObject::Do()
{
	vtkActor *pCurActor = NULL;
	m_pActorColl->InitTraversal();
	do
	{
		pCurActor = m_pActorColl->GetNextActor();
		if ( pCurActor != NULL )
		{
			pCurActor->RenderOpaqueGeometry ((vtkViewport*)m_pVtkRenderer);
		}
	}
	while ( pCurActor != NULL );

	// start a second loop for translucent props (if any!)
	m_pActorColl->InitTraversal();
	do
	{
		pCurActor = m_pActorColl->GetNextActor();
		if (pCurActor != NULL)
		{
#if ((VTK_MAJOR_VERSION < 5) ||							\
	 (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION < 2))
			pCurActor->RenderTranslucentGeometry ((vtkViewport*)m_pVtkRenderer);
#else
			pCurActor->RenderTranslucentPolygonalGeometry ( (vtkViewport*)m_pVtkRenderer );
#endif
		}
	}
	while (pCurActor != NULL);

	return true;
}

