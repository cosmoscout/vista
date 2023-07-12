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

#include "RandomNumberDfnNode.h"

#include <VistaTools/VistaRandomNumberGenerator.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* RandomNumberDfnNode                                                        */
/*============================================================================*/
RandomNumberDfnNode::RandomNumberDfnNode(float fMin, float fMax)
    : IVdfnNode()
    , m_fMin(fMin)
    , m_fRange(fMax - fMin)
    , m_pOut(new TVdfnPort<float>) {
  /**
   * We just create and register a single outport
   */
  RegisterOutPort("out", m_pOut);

  m_pRand = VistaRandomNumberGenerator::GetStandardRNG();

  /**
   * This node does not have any inports, and should just generate a new
   * random number each time the graph is evaluated. Since usually nodes are
   * only evaluated when at least one inport changed, we now have to specify
   * that it instead should be evaluated every graph traversal. Therefore,
   * we set the unconditional evaluation flag to true.
   */
  SetEvaluationFlag(true);
}

RandomNumberDfnNode::~RandomNumberDfnNode() {
}

/**
 * Different to the ColorCHangerDfnNode, we don't need to override GetIsValid()
 * and PrepareEvaluationRun() here, we just have one outport and are always
 * ready for work!
 * We do, however, overwrite the GetIsMasterSim routine to return true instead
 * of the usually false. This specifies that the node should be evaluated on the
 * master, and the values of the outport(s) are then transfered over network
 * to the clients, ensuring a deterministic synchronized state.
 */
bool RandomNumberDfnNode::GetIsMasterSim() const {
  return true;
}

/**
 * During Evaluation, we simply set the outport value to a random number
 * from the specified interval.
 */
bool RandomNumberDfnNode::DoEvalNode() {
  float fValue = m_fMin + m_fRange * (float)m_pRand->GenerateDouble1();
  m_pOut->SetValue(fValue, GetUpdateTimeStamp());
  return true;
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/**
 * While we don't do it here, we could pass some parameters to the ctor.
 * This would for example allow us to store a pointer to a DisplaySystem
 * or anything else, and pass it to the actual nodes when constructing them.
 */
RandomNumberDfnNodeCreate::RandomNumberDfnNodeCreate()
    : VdfnNodeFactory::IVdfnNodeCreator() {
}
RandomNumberDfnNodeCreate::~RandomNumberDfnNodeCreate() {
}

/**
 * The CreateNode routine should return a valid instance of the node, or
 * NULL if something went wrong (e.g. if mandatory parameters were not found).
 */
IVdfnNode* RandomNumberDfnNodeCreate::CreateNode(const VistaPropertyList& oParams) const {
  /**
   * Here, we configure the node from the parameters given in the params list
   * Here, we're looking for entries called 'min' and 'max'. If those are not
   * found, we default to 0 and 1.
   */
  float fMin = 0.0f;
  float fMax = 1.0f;

  /**
   * The passed parameter list oParams contains all xml parameters given for the
   * node. However, we are only interersted in those tagget 'param', thus we
   * extract the sub-PropList for these
   */
  const VistaPropertyList& oSubParams =
      oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

  if (oSubParams.HasProperty("min"))
    fMin = oSubParams.GetValue<float>("min");
  if (oSubParams.HasProperty("max"))
    fMax = oSubParams.GetValue<float>("max");
  return new RandomNumberDfnNode(fMin, fMax);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
