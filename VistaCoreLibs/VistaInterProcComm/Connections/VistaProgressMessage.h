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


#ifndef _VISTAPROGRESSMESSAGE_H
#define _VISTAPROGRESSMESSAGE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <string>

#include <VistaAspects/VistaSerializable.h>

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
 * @todo check why is this not derived from VistaMessage (VistaMsg)?
 */
class VISTAINTERPROCCOMMAPI VistaProgressMessage : public IVistaSerializable
{
public:
	VistaProgressMessage();
	virtual ~VistaProgressMessage();

	std::string GetApplicationName() const;
	void        SetApplicationName(const std::string &sApplicationName);

	/**
	 * 0-GetIconStateMax
	 */
	int         GetIconState() const;
	void        SetIconState(int iState);
	int         GetIconStateMax() const;
	void        SetIconStateMax(int iStateMax);

	/**
	 * 0-100
	 */
	int         GetTotalProgress() const;
	void        SetTotalProgress(int iTotalProgress);
	std::string GetTotalProgressLabel() const;
	void        SetTotalProgressLabel(const std::string &sTotalProgressLabel);

	int         GetSubtaskProgress() const;
	void        SetSubtaskProgress(int iSubtaskProgress);
	std::string GetSubtaskProgressLabel() const;
	void        SetSubtaskProgressLabel(const std::string &sSubtaskLabel);

	bool        GetHideFlag() const;
	void        SetHideFlag(bool bFlag);

	std::string GetPictureName() const;
	void        SetPictureName(const std::string &sPicName);

	/**
	* SERIALIZABLE-INTERFACE
	*/
	virtual int Serialize(IVistaSerializer &) const;
	virtual int DeSerialize(IVistaDeSerializer &);
	virtual std::string GetSignature() const;

protected:
private:
	std::string m_sApplicationName;
	std::string m_sProgressLabel;
	std::string m_sSubtaskLabel;
	std::string m_sPictureName;

	int m_iTotalProgress,
		m_iSubtaskProgress,
		m_iIconStateMax;
	bool m_bHideFlag;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

