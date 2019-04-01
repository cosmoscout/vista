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


#include "VistaDriverThreadAspect.h"
#include "VistaDeviceDriverAspectRegistry.h"
#include <VistaInterProcComm/Concurrency/VistaThreadLoop.h>
#include <VistaInterProcComm/Concurrency/VistaPriority.h>

#include <cassert>

namespace 
{
	class VistaDriverPollThread : public VistaThreadLoop
	{
	public:
		VistaDriverPollThread(IVistaDeviceDriver *pDriver)
			: VistaThreadLoop(),
			  m_pPoller(NULL),
			  m_pDriver(pDriver)
		{
		}

		virtual ~VistaDriverPollThread()
		{
		}

		VistaDriverThreadAspect::IDriverUpdatePrepare *GetDriverUpdate() const
		{
			return m_pPoller;
		}

		void SetDriverUpdate(VistaDriverThreadAspect::IDriverUpdatePrepare *pPoller)
		{
			if(IsRunning())
			{
				PauseThread();
				m_pPoller = pPoller;
				UnpauseThread();
			}
			else
				m_pPoller = pPoller;
		}

		/**
		 * Method that is to be performed BEFORE departed fork starts execution
		 */
		virtual void PreRun()
		{
			VistaDriverThreadAspect *ta = m_pDriver->GetAspectAs<VistaDriverThreadAspect>( VistaDriverThreadAspect::GetAspectId() );
			if( ta->GetThreadOnce() )
				ta->GetThreadOnce()->InitOnceMethod();
		}

		/**
		 * Method that is to be performed AFTER forked work is done
		 */
		virtual void PostRun()
		{
			VistaDriverThreadAspect *ta = m_pDriver->GetAspectAs<VistaDriverThreadAspect>( VistaDriverThreadAspect::GetAspectId() );
			if( ta->GetThreadOnce() )
				ta->GetThreadOnce()->ExitOnceMethod();
		}

	protected:
		bool LoopBody()
		{
			if(m_pPoller)
				m_pPoller->PrePoll();

			m_pDriver->PreUpdate();
			m_pDriver->Update();
			m_pDriver->PostUpdate();

			if(m_pPoller)
				m_pPoller->PostPoll();

			return false; // do not yield
		}
	private:
		VistaDriverThreadAspect::IDriverUpdatePrepare *m_pPoller;
		IVistaDeviceDriver *m_pDriver;
	};


	class ThreadLoopActivationContext : public VistaDriverThreadAspect::IActivationContext
	{
	public:
		ThreadLoopActivationContext( IVistaDeviceDriver *pDriver )
			: m_pThread( new VistaDriverPollThread(pDriver) )
			, m_nProcAffinity(-1)
		{
		}

		virtual ~ThreadLoopActivationContext()
		{
			if(m_pThread->IsRunning())
				m_pThread->StopGently(true); // this could deadlock?!
			delete m_pThread;
		}

		virtual int GetProcessorAffinity() const
		{
			return m_nProcAffinity;
		}

		virtual bool SetProcessorAffinity( int proc_affinity )
		{
			if(m_pThread->IsRunning())
				return false;

			if(m_pThread->SetProcessorAffinity(proc_affinity))
			{
				m_nProcAffinity = proc_affinity;
				return true;
			}
			return false;
		}

		virtual bool SetPriority( const VistaPriority &oPrio )
		{
			return m_pThread->SetPriority(oPrio);
		}

		virtual VistaPriority GetPriority() const
		{
			VistaPriority oPrio;
			m_pThread->GetPriority(oPrio);
			return oPrio;
		}

		virtual bool Activate()
		{
			if(!m_pThread->IsRunning())
				return m_pThread->Run();
			return true; // started threads remain running
		}

		virtual bool IsActive() const
		{
			return m_pThread->IsRunning() && !m_pThread->IsPausing();
		}

		virtual bool DeActivate() 
		{
			if( m_pThread->IsRunning() ) {
				return m_pThread->StopGently(true);
			}
			return true;
		}

		virtual VistaDriverThreadAspect::IDriverUpdatePrepare *GetDriverUpdatePrepare() const
		{
			return m_pThread->GetDriverUpdate();
		}

		virtual void SetDriverUpdatePrepare(VistaDriverThreadAspect::IDriverUpdatePrepare *pUpdate) 
		{
			m_pThread->SetDriverUpdate( pUpdate );
		}

		virtual bool Pause()
		{
			if( m_pThread->IsRunning() )
				return m_pThread->PauseThread();
			return false;
		}

		virtual bool IsPaused() const
		{
			return m_pThread->IsPausing();
		}

		virtual bool UnPause()
		{
			if( m_pThread->IsRunning() )
				return m_pThread->UnpauseThread();
			else
				return m_pThread->Run();
		}

		VistaDriverPollThread *m_pThread;		
		int                    m_nProcAffinity;
	};
} // namespace

VistaDriverThreadAspect::IDriverUpdatePrepare::~IDriverUpdatePrepare()
{
}

VistaDriverThreadAspect::IDriverUpdatePrepare::IDriverUpdatePrepare()
{
}



/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

int VistaDriverThreadAspect::m_nAspectId  = -1;
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDriverThreadAspect::VistaDriverThreadAspect(IVistaDeviceDriver *pDriver, IActivationContext *activation_context )
: IVistaDeviceDriver::IVistaDeviceDriverAspect()
  , m_pOnce(NULL)
  , m_activation_context( activation_context )
  , m_own_activation_context( activation_context == 0 ? true : false )
{
	if(VistaDriverThreadAspect::GetAspectId() == -1) // unregistered
		VistaDriverThreadAspect::SetAspectId( 
		VistaDeviceDriverAspectRegistry::GetSingleton()->RegisterAspect("THREAD"));

	SetId(VistaDriverThreadAspect::GetAspectId());

	if( m_activation_context == 0 )
		m_activation_context = new ThreadLoopActivationContext(pDriver);
}

VistaDriverThreadAspect::~VistaDriverThreadAspect()
{
	if( m_own_activation_context )
		delete m_activation_context;

	delete m_pOnce;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

// #########################################
// OVERWRITE IN SUBCLASSES
// #########################################
int  VistaDriverThreadAspect::GetAspectId()
{
	return VistaDriverThreadAspect::m_nAspectId;
}

void VistaDriverThreadAspect::SetAspectId(int nId)
{
	assert(VistaDriverThreadAspect::m_nAspectId == -1);
	VistaDriverThreadAspect::m_nAspectId = nId;

}

VistaDriverThreadAspect::IDriverUpdatePrepare *VistaDriverThreadAspect::GetDriverUpdatePrepare() const
{
	return m_activation_context->GetDriverUpdatePrepare();
}

void VistaDriverThreadAspect::SetDriverUpdatePrepare(IDriverUpdatePrepare *pUpdate)
{
	m_activation_context->SetDriverUpdatePrepare(pUpdate);
}

bool VistaDriverThreadAspect::StopProcessing()
{
	return m_activation_context->DeActivate();
}

bool VistaDriverThreadAspect::StartProcessing()
{
	return m_activation_context->Activate();
}

bool VistaDriverThreadAspect::PauseProcessing()
{
	return m_activation_context->Pause();
}

bool VistaDriverThreadAspect::UnpauseProcessing()
{
	return m_activation_context->UnPause();
}

bool VistaDriverThreadAspect::GetIsProcessing() const
{
	return m_activation_context->IsActive();
}

void VistaDriverThreadAspect::GetPriority(VistaPriority &oPrio) const
{
	oPrio = m_activation_context->GetPriority();
}

void VistaDriverThreadAspect::SetPriority( const VistaPriority &oPrio )
{
	m_activation_context->SetPriority( oPrio );
}

void VistaDriverThreadAspect::SetProcessorAffinity(int nProcessorNum)
{
	m_activation_context->SetProcessorAffinity( nProcessorNum );
}

int VistaDriverThreadAspect::GetProcessorAffinity() const
{
	return m_activation_context->GetProcessorAffinity();
}

VistaDriverThreadAspect::IThreadOnce *VistaDriverThreadAspect::GetThreadOnce() const
{
	return m_pOnce;
}

void VistaDriverThreadAspect::SetThreadOnce( VistaDriverThreadAspect::IThreadOnce *once )
{
	m_pOnce = once;
}


VistaDriverThreadAspect::IActivationContext *VistaDriverThreadAspect::GetActivationContext() const
{
	return m_activation_context;
}
