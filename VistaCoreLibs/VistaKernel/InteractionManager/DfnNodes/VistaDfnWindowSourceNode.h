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

#ifndef _VISTADFNWINDOWSOURCENODE_H
#define _VISTADFNWINDOWSOURCENODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <map>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaWindow;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * Retrieves window dimensions of the associated window.
 *
 * @ingroup VdfnNodes
 * @outport{win_x, int, window x position on screen}
 * @outport{win_y, int, window y position on screen}
 * @outport{win_w, int, window width}
 * @outport{win_h, int, window height}
 */
class VISTAKERNELAPI VistaDfnWindowSourceNode : public IVdfnNode {
 public:
  VistaDfnWindowSourceNode(VistaWindow* pWindow);

  ~VistaDfnWindowSourceNode();

  virtual bool GetIsValid() const;

 protected:
  virtual bool DoEvalNode();

  virtual unsigned int CalcUpdateNeededScore() const;

 private:
  TVdfnPort<int>*m_pX, *m_pY, *m_pW, *m_pH;

  class WindowObserver;
  VistaWindow*    m_pWindow;
  WindowObserver* m_pObs;

  mutable unsigned int m_nUpdateCount;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNWINDOWSOURCENODE_H
