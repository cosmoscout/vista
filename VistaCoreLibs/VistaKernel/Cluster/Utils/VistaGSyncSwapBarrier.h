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

#ifndef _VISTAGSYNCSWAPBARRIER_H
#define _VISTAGSYNCSWAPBARRIER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplayManager;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaGSyncSwapBarrier uses nvidia-specific OpenGL-Extensions to configure
 * a swap barrier that ensures that all nodes perform their glSwapBuffers()
 * simultaneously. Note that this requires a properly configured GSync setup.
 * VistaGSyncSwapBarrier is not a specialization of IVistaClusterBarrier since
 * it can only be used to barrier-wait at glSwapBuffer-calls, not at any time.
 */
namespace VistaGSyncSwapBarrier {
VISTAKERNELAPI bool JoinSwapBarrier(VistaDisplayManager* pDisplayManager);
VISTAKERNELAPI bool LeaveSwapBarrier(VistaDisplayManager* pDisplayManager);
}; // namespace VistaGSyncSwapBarrier
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAGSYNCSWAPBARRIER_H
