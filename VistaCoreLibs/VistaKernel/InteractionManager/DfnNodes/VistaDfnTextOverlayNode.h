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

#ifndef _VISTADFNTEXTOVERLAYNODE_H
#define _VISTADFNTEXTOVERLAYNODE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaBase/VistaExceptionBase.h>

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnNodeFactory.h>
#include <VistaDataFlowNet/VdfnPort.h>
#include <VistaDataFlowNet/VdfnUtil.h>
#include <map>

#include <VistaKernel/DisplayManager/Vista2DDrawingObjects.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <VistaAspects/VistaPropertyAwareable.h>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * A very simple text label that is updated when the inport changes.
 * The conversion is done using VistaConversion::ToString() on the input type.
 *
 *
 * @ingroup VdfnNodes
 * @inport{value, any that can be converted using VistaConversion, mandatory, the value to display
 * as overlay text}
 */
template <class T>
class VistaDfnTextOverlayNode : public IVdfnNode {
 public:
  VistaDfnTextOverlayNode(Vista2DText* pText)
      : m_pText(pText)
      , m_pValue(NULL) {
    RegisterInPortPrototype("value", new TVdfnPortTypeCompare<TVdfnPort<T>>);
  }

  ~VistaDfnTextOverlayNode() {
    m_pText->SetEnabled(false);
    // delete m_pText;
  }

  virtual void OnActivation(double dTs) {
    IVdfnNode::OnActivation(dTs);
    m_pText->SetEnabled(true);
  }

  virtual void OnDeactivation(double dTs) {
    IVdfnNode::OnDeactivation(dTs);
    m_pText->SetEnabled(false);
  }

  bool GetIsValid() const {
    return (m_pValue != NULL);
  }
  bool PrepareEvaluationRun() {
    m_pValue = VdfnUtil::GetInPortTyped<TVdfnPort<T>*>("value", this);
    return GetIsValid();
  }

  std::string GetPrefix() const {
    return m_strPrefix;
  }
  void SetPrefix(const std::string& strPrefix) {
    m_strPrefix = strPrefix;
  }

  std::string GetPostfix() const {
    return m_strPostfix;
  }
  void SetPostfix(const std::string& strPostfix) {
    m_strPostfix = strPostfix;
  }

 protected:
  bool DoEvalNode() {
    const T& value = m_pValue->GetValueConstRef();

    std::string sValue = VistaConversion::ToString(value);
    if (!m_strPrefix.empty())
      sValue = m_strPrefix + sValue;

    if (!m_strPostfix.empty())
      sValue = sValue + m_strPostfix;

    m_pText->SetText(sValue);

    return true;
  }

 private:
  TVdfnPort<T>* m_pValue;
  Vista2DText*  m_pText;

  std::string          m_strPrefix, m_strPostfix;
  VistaDisplayManager* m_pDm;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTADFOVERLAYTEXTLABELNODE_H
