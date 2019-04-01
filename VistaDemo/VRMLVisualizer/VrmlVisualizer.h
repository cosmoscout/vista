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


#ifndef _VRMLVISUALIZER_H
#define _VRMLVISUALIZER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <string>
#include <vector>

#include "VrmlEventHandler.h"

#include <VistaKernel/VistaSystem.h>

#include <VistaAspects/VistaPropertyAwareable.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IVistaNode;
class VistaTransformNode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VrmlVisualizer
{
public:
	VrmlVisualizer( int argc = 0, char  *argv[] = NULL );
	virtual ~VrmlVisualizer();

	void Run ();
	void Update();
	void NextStep();
	void PrevStep();
	void IncreaseStepSpeed();
	void DecreaseStepSpeed();
	void ParseParameters();

	inline VistaPropertyList &GetPropertyList(){ return m_vrmlProps; }

private:

	void PrintUsage();
	bool AddByFilename( const std::string& strFilename );
	bool LoadData();

	VistaSystem							m_vistaSystem;
	VistaPropertyList					m_vrmlProps;
	VistaTransformNode					*FirstNodeAsTransformNode( IVistaNode * );
	std::vector<VistaTransformNode*>	m_loadedNodes;
	int									m_iCurrentStep;
	double								m_dLastTime;
	double								m_dStepTime;
	VrmlEventHandler					m_graphicsEventHandler;
	std::vector<std::string>			m_vecArgs;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif // _VRMLVISUALIZER_H
