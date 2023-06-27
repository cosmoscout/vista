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

#if !defined(_VISTADISPLAY_H)
#define _VISTADISPLAY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaDisplayEntity.h"
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaAspects/VistaReflectionable.h>

#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaDisplayManager;
class VistaWindow;
class IVistaDisplayBridge;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * VistaDisplay is ...
 */
class VISTAKERNELAPI VistaDisplay : public VistaDisplayEntity {
  friend class IVistaDisplayBridge;
  friend class VistaDisplayManager;

 public:
  virtual ~VistaDisplay();

  VistaDisplayManager* GetDisplayManager() const;

  std::list<std::string> GetWindowNames() const;
  unsigned int           GetNumberOfWindows() const;

  VistaWindow*               GetWindow(unsigned int iIndex) const;
  std::vector<VistaWindow*>& GetWindows();

  virtual void Debug(std::ostream& out) const;

  /**
   * Set/get display properties. The following keys are understood:
   *
   * DISPLAY_STRING       -   [string]
   * NUMBER_OF_WINDOWS    -   [int][read only]
   * WINDOW_NAMES         -   [list of strings][read only]
   */

  class VISTAKERNELAPI VistaDisplayProperties : public IVistaDisplayEntityProperties {
    friend class VistaDisplay;

   public:
    enum { MSG_DISPLAYSTRING_CHANGE = IVistaDisplayEntityProperties::MSG_LAST, MSG_LAST };

    bool SetName(const std::string& sName);

    std::string GetDisplayString() const;
    bool        SetDisplayString(const std::string& sDispString);

    virtual std::string GetReflectionableType() const;

   protected:
    virtual int AddToBaseTypeList(std::list<std::string>& rBtList) const;

   private:
    VistaDisplayProperties(VistaDisplayEntity*, IVistaDisplayBridge*);
    virtual ~VistaDisplayProperties();

    std::string m_strDisplayString;
  };

  VistaDisplayProperties* GetDisplayProperties() const;

 protected:
  virtual IVistaDisplayEntityProperties* CreateProperties();

  VistaDisplay(
      VistaDisplayManager* pDMgr, IVistaDisplayEntityData* pData, IVistaDisplayBridge* pBridge);

 private:
  std::vector<VistaWindow*> m_vecWindows;
  VistaDisplayManager*      m_pDisplayManager;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif // !defined(_VISTADISPLAY_H)
