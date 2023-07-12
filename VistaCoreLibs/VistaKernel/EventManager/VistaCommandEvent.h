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

#ifndef _VISTACOMMANDEVENT_H
#define _VISTACOMMANDEVENT_H

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaMsg;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaCommandEvent : public VistaEvent, public IVistaPropertyAwareable {
 public:
  enum { VEIDC_CMD = 0, VEIDC_LAST };

  VistaCommandEvent();
  virtual ~VistaCommandEvent();

  virtual int Serialize(IVistaSerializer&) const;
  virtual int DeSerialize(IVistaDeSerializer&);

  virtual std::string GetSignature() const;

  int  GetMethodToken() const;
  void SetMethodToken(int iMethodToken);

  int  GetMessageTicket() const;
  void SetMessageTicket(int iMethodTicket);

  VistaPropertyList GetPropertyList() const;
  bool              UsesPropertyListAnswer() const;
  void              ClearPropertyList();

  virtual int           SetProperty(const VistaProperty&);
  virtual int           GetProperty(VistaProperty&);
  virtual VistaProperty GetPropertyByName(const std::string& sPropName);
  virtual bool SetPropertyByName(const std::string& sPropName, const std::string& sPropValue);
  virtual int  SetPropertiesByList(const VistaPropertyList&);
  virtual int  GetPropertiesByList(VistaPropertyList&);

  virtual int         GetPropertySymbolList(std::list<std::string>& rStorageList);
  virtual std::string GetPropertyDescription(const std::string& sPropName);

  // ###############################################

  void      SetCommandMsg(VistaMsg*);
  VistaMsg* GetCommandMsg() const;

  static int         GetTypeId();
  static void        SetTypeId(int nId);
  static std::string GetIdString(int nId);

 protected:
 private:
  int m_iMethodToken;
  int m_iMessageTicket;

  VistaPropertyList m_mpPropertyList;
  VistaMsg*         m_pMsg;

  static int m_nEventId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACOMMANDEVENT_H
