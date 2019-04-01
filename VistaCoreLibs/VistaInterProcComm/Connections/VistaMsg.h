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


#ifndef _VISTAMSG_H
#define _VISTAMSG_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaBaseTypes.h>
#include <VistaAspects/VistaSerializable.h>
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

#include <vector>

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
 * @todo rename to VistaMessage
 */
class VISTAINTERPROCCOMMAPI VistaMsg : public IVistaSerializable
{
public:
	typedef std::vector<VistaType::byte> MSG;

	VistaMsg();
	VistaMsg(const VistaMsg &);
	virtual ~VistaMsg();

	MSG GetThisMsg() const;
	void        SetThisMsg(const MSG &);

	MSG &GetThisMsgRef();
	const MSG &GetThisMsgConstRef() const;


	MSG GetMsgAnswer() const;
	void        SetMsgAnswer(const MSG &);

	int GetMsgType() const;
	void SetMsgType(int iType);

	bool GetMsgSuccess() const;
	void SetMsgSuccess(bool bSuccess);

	int GetMsgTicket() const;
	void SetMsgTicket(int iTicket);

	virtual int Serialize(IVistaSerializer &) const;
	virtual int DeSerialize(IVistaDeSerializer &);
	virtual std::string GetSignature() const;

	static int AssignMsgByString(const std::string &sString, MSG &);
	static int AssignStringByMsg(const MSG &rMsg, std::string &sStorage);

protected:
private:
	MSG m_veMsg;
	MSG m_veAnswer;
	int         m_iType;
	int         m_iTicket;
	bool        m_bSuccess;
};
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAMSG_H

