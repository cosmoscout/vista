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


#ifndef _VISTAXES_H
#define _VISTAXES_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaGeometry.h"
#include <vector>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaSceneGraph;
class IVistaNode;
class VistaGroupNode;

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Creates coordinate axes rendering
 */
class VISTAKERNELAPI VistaAxes
{
public:

	/**
	 * VistaAxes constructor
	 *
	 * @param pParent
	 * The parent group node. Pass NULL here to create the axes below
	 * the root node.
	 */
	VistaAxes( VistaSceneGraph * pVistaSceneGraph,
			   VistaGroupNode *pParent = NULL,
			   float fSizeX = 1.0f, float fSizeY = 1.0f, float fSizeZ = 1.0f);
	virtual ~VistaAxes();

	IVistaNode * GetVistaNode();

private:

	std::vector<VistaGeometry*>	m_vecAxes;
	std::vector<VistaGeometry*>	m_vecTips;
	VistaGeometry*				m_pCenter;
	VistaGroupNode*				m_pGroup;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAXES_H


