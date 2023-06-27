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

#ifndef _VISTADFNSIMPLETEXTNODE_H
#define _VISTADFNSIMPLETEXTNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnHistoryPort.h>
#include <VistaDataFlowNet/VdfnNodeCreators.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnShallowNode.h>

#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDisplayManager;
class VistaSimpleTextOverlay;
class IVistaTextEntity;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Overlays all  inputs on the main display system, as long as they offer a to-string
 * conversion.
 *
 * @ingroup VdfnNodes
 * @inport{TextEnabled, bool, optional, toggles text visibility on or off}
 * @inport{-any-, -any-, optional, any in-port that offers to-string conversion will be displayed}
 */
class VISTAKERNELAPI VistaDfnSimpleTextNode : public VdfnShallowNode {
 public:
  VistaDfnSimpleTextNode(VistaDisplayManager* pDisplayManager);
  ~VistaDfnSimpleTextNode();

  virtual void OnActivation(double dTs);
  virtual void OnDeactivation(double dTs);

  bool PrepareEvaluationRun();

  int        GetTextSize() const;
  void       SetTextSize(const int& oValue);
  VistaColor GetColor() const;
  void       SetColor(const VistaColor& oValue);
  int        GetCaptionColumnWidth() const;
  void       SetCaptionColumnWidth(const int& oValue);

 protected:
  bool DoEvalNode();

  virtual bool SetInPort(const std::string& sName, IVdfnPort* pPort);

 private:
  VistaDisplayManager* m_pDisplayManager;
  TVdfnPort<bool>*     m_pEnablePort;
  struct PortTextData;
  std::vector<PortTextData*> m_vecPortTexts;
  VistaSimpleTextOverlay*    m_pOverlay;

  VistaColor m_oColor;
  int        m_nTextSize;
  int        m_nCaptionColumnWidth;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFNSIMPLETEXTNODE_H
