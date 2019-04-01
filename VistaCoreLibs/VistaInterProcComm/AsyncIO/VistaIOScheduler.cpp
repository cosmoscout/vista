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


#include "VistaIOScheduler.h"
#include "VistaIORequest.h"

#include "VistaIOMultiplexer.h"
#include <VistaInterProcComm/Concurrency/VistaThreadPool.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>

#include <cstdio>
#include <iostream>
using namespace std;

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI VistaMultiplexerJob : public IVistaThreadPoolWorkInstance
{
public:
	VistaMultiplexerJob(VistaIOScheduler *pSched, VistaIOMultiplexer *pPlexer);
	virtual ~VistaMultiplexerJob();

protected:
	void DefinedThreadWork();
private:
	VistaIOMultiplexer *m_pMultiplexer;
	VistaIOScheduler   *m_pScheduler;
};

VistaMultiplexerJob::VistaMultiplexerJob(VistaIOScheduler *pSched,VistaIOMultiplexer *pPlexer)
{
	m_pMultiplexer = pPlexer;
	m_pScheduler = pSched;
}


VistaMultiplexerJob::~VistaMultiplexerJob()
{
}

void VistaMultiplexerJob::DefinedThreadWork()
{
	int iRet = 0;
	while((iRet=(*m_pMultiplexer).Demultiplex())!=-1)
	{
		// iRet  is the id, != -1, hmm...

	   // first of all, we will create a job of our own for this
		// so we will remove the handle from the global multiplexer
  //      printf ("Remove and process %d .\n", iRet);
		(*m_pMultiplexer).RemMultiplexPointByTicket(iRet);

		// we pass the ticket to the scheduler
		(*m_pScheduler).HandleRequest(iRet);

		// and simply continue, 
		// this job (iRet) is none of our  business anymore
	   // scheduler stuff: remove done jobs
	   (*m_pScheduler).Cleanup ();
	}
	printf ("Multiplexer exited.\n");
}

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaIOScheduler::VistaIOScheduler(int iRequestThreads, VistaIOMultiplexer *pMultiplexer)
: m_pMultiplexer (pMultiplexer)
{

	// we need one job for the multiplexer job
	m_pPool = new VistaThreadPool(iRequestThreads+1);
	m_pPlexer = new VistaMultiplexerJob(this, m_pMultiplexer);
	m_iPlexerId = -1;
}

VistaIOScheduler::~VistaIOScheduler()
{
	m_pPool->StopPoolWork();
	if(!m_pPool->WaitForAllJobsDone())
	{
		// hmm... panic would be a solution
	}
	delete m_pPool;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

void VistaIOScheduler::StartMultiplexing()
{
	if(!(*m_pPool).GetIsPoolRunning())
	{
		if(!(*m_pPool).StartPoolWork())
			return; /** @todo this should be an error */
	}

	if(!(*m_pPlexer).GetIsProcessed())
		m_iPlexerId = (*m_pPool).AddWork(m_pPlexer);
}


void VistaIOScheduler::StopMultiplexing()
{
	VISTA_THROW("CALLED A NON WORKING API", 0x0000000);
//	if(m_iPlexerId != -1)
//   {
//	   m_pMultiplexer->Shutdown();
////       (*m_pPool).WaitForJob(m_pPlexer);
//	   //(*m_pPool).WaitForJob(m_iPlexerId); // this should finish, soon!
//	   m_iPlexerId = -1;
//   }
}


void VistaIOScheduler::Cleanup ()
{
	// clear done task pool jobs
	while ((*m_pPool).GetNumberOfPendingDoneJobs() > 0)
	{
		IVistaThreadPoolWorkInstance* pJob = (*m_pPool).RemoveNextDoneJob ();
		if (pJob)
			printf ("[IOScheduler] Remove done job %d",pJob->GetJobId());

	}

}

void VistaIOScheduler::AddRequestToMultiplexer (int iTicket, IVistaIORequest *pRequest)
{
	switch((*pRequest).GetRequestDir())
	{
	case IVistaIORequest::RD_IN:
		{
			//(*m_pMultiplexer).AddMultiplexPoint((*pRequest).GetMultiplexHandle(), iTicket, VistaIOMultiplexer::MP_IOIN);
			pRequest->RegisterAtMultiplexer (m_pMultiplexer, iTicket, VistaIOMultiplexer::MP_IOIN);
			break;
		}
	case IVistaIORequest::RD_OUT:
		{
			//(*m_pMultiplexer).AddMultiplexPoint((*pRequest).GetMultiplexHandle(), iTicket, VistaIOMultiplexer::MP_IOOUT);
			pRequest->RegisterAtMultiplexer (m_pMultiplexer, iTicket, VistaIOMultiplexer::MP_IOOUT);
			break;
		}
	case IVistaIORequest::RD_ERR:
		{
			//(*m_pMultiplexer).AddMultiplexPoint((*pRequest).GetMultiplexHandle(), iTicket, VistaIOMultiplexer::MP_IOERR);
			pRequest->RegisterAtMultiplexer (m_pMultiplexer, iTicket, VistaIOMultiplexer::MP_IOERR);
			break;
		}
	case IVistaIORequest::RD_INOUT:
		{
//            (*m_pMultiplexer).AddMultiplexPoint((*pRequest).GetMultiplexHandle(), iTicket, VistaIOMultiplexer::MP_IOIN);
			pRequest->RegisterAtMultiplexer (m_pMultiplexer, iTicket, VistaIOMultiplexer::MP_IOIN);
//            (*m_pMultiplexer).AddMultiplexPoint((*pRequest).GetMultiplexHandle(), iTicket, VistaIOMultiplexer::MP_IOOUT);
			pRequest->RegisterAtMultiplexer (m_pMultiplexer, iTicket, VistaIOMultiplexer::MP_IOOUT);
			break;
		}
	case IVistaIORequest::RD_NONE:
	default:
		break;
	}
}

int VistaIOScheduler::AddRequest(IVistaIORequest *pRequest)
{
	// should be thread safe
	int iTicket = (*m_pMultiplexer).GetNextTicket();
	m_mpRequestMap.insert(REQUESTMAP::value_type(iTicket, pRequest));
	AddRequestToMultiplexer (iTicket, pRequest);
	return iTicket;
}

void VistaIOScheduler::RemRequest(int iTicket)
{
//    int iTicket = (*m_pMultiplexer).GetTicketForHandle(pReq->GetMultiplexHandle());
	REQUESTMAP::iterator it = m_mpRequestMap.find(iTicket);
	if(it != m_mpRequestMap.end())
	{
		// ok, this is our request
		if(!(*m_pPool).RemoveDoneJob((*it).second->GetJobId()))
		{
			// ok, job was not done
			//if(!(*it).second->WaitForJobFinish())
			//{
				// panic would be a solution now
			//}
		}

		// remove from pool
		(*m_pPool).RemoveDoneJob((*it).second->GetJobId());

		// remove from multiplexer
		(*m_pMultiplexer).RemMultiplexPointByTicket(iTicket);

		// remove from our internal structure
		m_mpRequestMap.erase(it); 
	}
}


void VistaIOScheduler::HandleRequest(int iTicket)
{
	REQUESTMAP::iterator it = m_mpRequestMap.find(iTicket);
	if(it != m_mpRequestMap.end())
	{
		vstr::outi() << "VistaIOScheduler::HandleRequest() - Handle request for ticket" 
						<< iTicket << std::endl;
		(*m_pPool).AddWork((*it).second);
	}
	else
	{
		vstr::outi() << "VistaIOScheduler::HandleRequest() - No work for this ticket." << std::endl;
	}
}


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


