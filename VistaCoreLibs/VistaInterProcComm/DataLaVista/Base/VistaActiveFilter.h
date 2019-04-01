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


#ifndef DLVISTAACTIVEFILTER_H
#define DLVISTAACTIVEFILTER_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include "VistaFilter.h"
#include "VistaActiveComponent.h"
#include <string>
#include <list>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaThreadLoop;
class VistaPriority;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaActiveFilter : public IDLVistaFilter
												 , public IDLVistaActiveComponent
{
public:
	DLVistaActiveFilter(IDLVistaFilter *);

	virtual ~DLVistaActiveFilter();

	virtual bool InitPacketMgmt();

	virtual IDLVistaDataPacket *FilterPacketL(IDLVistaDataPacket * pPacket);

	virtual void ConsumePacket(IDLVistaDataPacket * pPacket);

	virtual IDLVistaDataPacket *PullPacket(bool bBlock);

	virtual IDLVistaDataPacket * GivePacket(bool bBlock);

	virtual IDLVistaDataPacket *CreatePacket();

	virtual void DeletePacket(IDLVistaDataPacket *pPacket);


	virtual bool AttachInputComponent(IDLVistaPipeComponent * pComp);

	virtual bool AttachOutputComponent(IDLVistaPipeComponent * pComp);

	virtual bool RecycleDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	virtual bool AcceptDataPacket(IDLVistaDataPacket *pPacket, IDLVistaPipeComponent *pSender, bool bBlock=false);

	virtual int GetInputPacketType() const;
	virtual int GetOutputPacketType() const;

	virtual std::list<IDLVistaPipeComponent *> GetInputComponents() const;
	virtual std::list<IDLVistaPipeComponent *> GetOutputComponents() const;

   /**
	 * This is a method for constraint checking.
	 * @param pComp the component to see whether this is registered as input to this component
	 * @return true if pComp is a valid input to this component.
	 */
	virtual bool IsInputComponent(IDLVistaPipeComponent *pComp) const;

	/**
	 * This is a method for constraint checking.
	 * @param pComp the component to see whether this is registered as input to this component
	 * @return true if pComp is a valid input to this component.
	 */
	virtual bool IsOutputComponent(IDLVistaPipeComponent *pComp) const;

	bool StartComponent();

	bool PauseComponent(bool bJoin);
	bool UnPauseComponent(bool bJoin);

	bool StopComponent(bool bJoin);

	bool StopComponentGently(bool bJoin);

	bool HaltComponent();

	/**
	 * tell the filter that active filtering is about to be stopped.
	 * Use this to indicate termination whenever the filter may
	 * be in a blocking wait internally
	 */
	void IndicateFilteringEnd();

	virtual bool IsActiveComponent() const { return true;}

	bool IsComponentRunning() const;

	virtual int  SetComponentPriority(const VistaPriority &pPrio);

	void SetThreadName(const std::string& strName);

	std::string GetThreadName() const;

	bool GetStopOnDestruct() const;
	void SetStopOnDestruct(bool bStop);

	virtual IDLVistaDataPacket *ReturnPacket();

private:
	VistaThreadLoop *m_pThread;

	IDLVistaFilter *m_pRealFilter;
	bool m_bStopOnDestruct;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTAACTIVEFILTER_H

