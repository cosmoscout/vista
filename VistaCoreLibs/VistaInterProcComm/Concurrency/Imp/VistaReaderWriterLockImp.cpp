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


#include "VistaReaderWriterLockImp.h" 
#include "../VistaMutex.h"
#include "../VistaThreadCondition.h"

#if defined(_USE_PTHREAD_RWLOCK)
#include "VistaPthreadReaderWriterLockImp.h"
#endif

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

class VistaVanillaReaderWriterLockImp : public IVistaReaderWriterLockImp
{
	public:
	VistaVanillaReaderWriterLockImp()
	{
	m_nReadersReading = 0;
		m_nWritersWriting = 0;
	m_nWritersPending = 0;
	m_pLock      = new VistaMutex;
	m_pCondition = new VistaThreadCondition;
	}
	
	
	virtual ~VistaVanillaReaderWriterLockImp()
	{
	// we should test for locked mutexes here...
	delete m_pLock;
	delete m_pCondition;
	}
	
	virtual bool ReaderLock()
	{   
		m_pLock->Lock();
		while(m_nWritersWriting)
		{
			m_pCondition->WaitForCondition(*m_pLock);
		}
		
		++m_nReadersReading;
	m_pLock->Unlock();
		return true;
	}
	
	virtual bool ReaderUnlock()
	{
	m_pLock->Lock();
		if (m_nReadersReading == 0) 
	{
		m_pLock->Unlock();	
				return false;
		} 
	else 
	{
		--m_nReadersReading;
		if (m_nReadersReading == 0)
		{
			m_pCondition->SignalCondition();
		}
		m_pLock->Unlock();
		return true;
	}
	}
	
	virtual bool WriterLock()
	{
	m_pLock->Lock();
		while(m_nWritersWriting) 
	{
		m_pCondition->WaitForCondition(*m_pLock);
		}
		++m_nWritersWriting;
	m_pLock->Unlock();
		return true;
	}
	
	virtual bool WriterUnlock()
	{
	m_pLock->Lock();
		if (m_nWritersWriting == 0) 
	{
			m_pLock->Unlock();
			return false;
		} 
	else 
	{
			m_nWritersWriting = 0;
			m_pCondition->BroadcastCondition();
			m_pLock->Unlock();
			return true;
		}
	}
	
	
	protected:
	private:
	VistaMutex * m_pLock;
	VistaThreadCondition * m_pCondition;
	int m_nReadersReading;
	int m_nWritersWriting;
	int m_nWritersPending;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
IVistaReaderWriterLockImp::IVistaReaderWriterLockImp()
{
}

IVistaReaderWriterLockImp::~IVistaReaderWriterLockImp()
{
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
IVistaReaderWriterLockImp *IVistaReaderWriterLockImp::CreateReaderWriterLock()
{
	#if defined(VISTA_THREADING_POSIX) && defined(_USE_PTHREAD_RWLOCK)
	return new VistaPthreadReaderWriterLockImp;
	#else
	return new VistaVanillaReaderWriterLockImp;
	#endif
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


