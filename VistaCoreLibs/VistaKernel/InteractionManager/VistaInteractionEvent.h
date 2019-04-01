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


#ifndef _VISTAINTERACTIONEVENT_H
#define _VISTAINTERACTIONEVENT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/EventManager/VistaEvent.h>

#include <list>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaInteractionContext;
class VistaInteractionManager;

class IVdfnNode;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaInteractionEvent : public VistaEvent
{
public:
	typedef std::list<std::string> PortList;

	enum
	{
		VEID_FIRST = VistaEvent::VEID_LAST,
		VEID_CONTEXT_CHANGE = VistaEvent::VEID_LAST,
		VEID_CONTEXT_GRAPH_UPDATE, /**< indicates that this context needs an update.
		                                this is a system message. */
		VEID_GRAPH_INPORT_CHANGE, /**< indicates the this context performed an update that
		                               lead to a state change, e.g., the value of the graph
		                               has changed */
		VEID_LAST
	};

	VistaInteractionEvent(VistaInteractionManager *pMgr);
	virtual ~VistaInteractionEvent();

	VistaInteractionContext *GetInteractionContext() const;
	void  SetInteractionContext(VistaInteractionContext *pCtx);
	unsigned int GetRoleId() const;


	PortList &GetPortMapWrite();
	const PortList &GetPortMapRead() const;
	const IVdfnNode *GetEventNode() const;
	void SetEventNode(IVdfnNode *pNode);


	void SetTime(double dTs);
	// ######################################################################
	// STATIC EVENT API
	// ######################################################################
	static int GetTypeId();
	static void SetTypeId(int nId);
	static std::string GetIdString(int nId);

	// ######################################################################
	// SERIALIZER API
	// ######################################################################
	virtual int Serialize(IVistaSerializer &) const;
	virtual int DeSerialize(IVistaDeSerializer &);
	virtual std::string GetSignature() const;


	virtual void  Debug(std::ostream & out) const;
protected:
private:
	int        m_nUpdateMsg;
	static int m_nEventId;
	VistaInteractionContext *m_pContext;
	VistaInteractionManager *m_pMgr;

	IVdfnNode *m_pEventNode;
	PortList m_mpPortChangeMap;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAINTERACTIONEVENT_H

