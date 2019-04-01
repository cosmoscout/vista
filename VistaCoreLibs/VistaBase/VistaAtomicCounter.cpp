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


#include <cerrno>

#include "VistaAtomicCounter.h"
#include "VistaExceptionBase.h"

#if defined(WIN32)
	#include <windows.h>
#endif

#ifdef  VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE

namespace
{
	struct _private;

	_private *towp( void *p ) { return reinterpret_cast<_private*>( p ); }

	class _scopedLock
	{
	public:
		_scopedLock( _private &Cs );
		~_scopedLock();

		_private &m_cs;
	};

	#if defined(WIN32)

		struct _private
		{
			_private()
			{
				InitializeCriticalSection(&m_cs);
			}

			~_private()
			{
				DeleteCriticalSection(&m_cs);
			}

			void Lock()
			{
				EnterCriticalSection( &m_cs );
			}

			void Unlock()
			{
				LeaveCriticalSection( &m_cs );
			}

			CRITICAL_SECTION m_cs;
		};



	#else // if WIN32

		#include <semaphore.h>
		#include <unistd.h>
		// this is for 32bit gnu-based systems, that do not expose the {q} qualifier for assembly construction
		struct _private
		{
			_private()
			{
				if(sem_init(&m_semaphore, 0, 1) == -1)
				{
					VISTA_THROW("[AtomicCounter::_private]: sem_init() failed", 0L);
				}
			}

			~_private()
			{
				sem_destroy(&m_semaphore);
			}

			void Lock()
			{
				if( TEMP_FAILURE_RETRY( sem_wait(&m_semaphore) ) != 0 )
					VISTA_THROW("[AtomicCounter::_private::Lock()]: sem_wait() failed", 0L);
			}

			void Unlock()
			{
				if( sem_post(&m_semaphore) )
					VISTA_THROW("[AtomicCounter::_private::Lock()]: sem_post() failed", 0L);
			}

			sem_t m_semaphore;
		};
	#endif // WIN32


		// implementation part for scoped lock
		_scopedLock::_scopedLock( _private &Cs )
		: m_cs(Cs)
		{
			m_cs.Lock();
		}

		_scopedLock::~_scopedLock()
		{
			m_cs.Unlock();
		}
}


#endif // VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
VistaSigned32Atomic::VistaSigned32Atomic( VistaType::sint32 initialValue )
: TVistaAtomicCounter<VistaType::sint32>(initialValue)
{

}


VistaSigned64Atomic::VistaSigned64Atomic( VistaType::sint64 initialValue )
: TVistaAtomicCounter<VistaType::sint64>( initialValue )
, m_pPrivate()
{
#ifdef VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE
	m_pPrivate.m_pPrivate = (void*) new _private;
#endif
}

VistaSigned64Atomic::~VistaSigned64Atomic()
{
#ifdef VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE
	delete towp( m_pPrivate.m_pPrivate ) ;
#endif
}

VistaType::sint64 VistaSigned64Atomic::Get() const
{
#ifdef VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE
	_scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
	return m_nValue;
#else
	return m_nValue;
#endif

}

// __LP64__ accounts for mac 64 snow leopard and above.
#if defined(__GNUC__) && ( defined(i386) || defined(__i386__) || defined(__x86_64__) || defined(__LP64__) )

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// VistaSigned32Atomic
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// hand-crafted assembler for 32bit singed integer on intel machines / gnu-c compilers,
	// though the assembler instructions are supported by a number of other compilers, too.

	void VistaSigned32Atomic::Add( VistaType::sint32 nValue)
	{
		__asm__ __volatile__ ("lock; add{l} {%1,%0|%0,%1}"
				  	        : "+m" (m_nValue)
				  	        : "ir" (nValue)
				  	        : "memory");
	}

	void VistaSigned32Atomic::Sub( VistaType::sint32 nValue )
	{
		__asm__ __volatile__ ("lock; sub{l} {%1,%0|%0,%1}"
				  : "+m" (m_nValue)
				  : "ir" (nValue)
				  : "memory");
	}

	void VistaSigned32Atomic::Inc()
	{
		__asm__ __volatile__ ("lock; inc{l} %0"
				  : "+m" (m_nValue)
				  :
				  : "memory");
	}


	void VistaSigned32Atomic::Dec()
	{
		__asm__ __volatile__ ("lock; dec{l} %0"
				  : "+m" (m_nValue)
				  :
				  : "memory");
	}

	bool VistaSigned32Atomic::DecAndTestNull()
	{
		unsigned char c;
		__asm__ __volatile__("lock; dec{l} %0; sete %1"
				 :"+m" (m_nValue), "=qm" (c)
				 :
				 : "memory");
		return c != 0;
	}

	bool VistaSigned32Atomic::AddAndTestNegative( VistaType::sint32 nValue )
	{
		unsigned char c;
		__asm__ __volatile__("lock; add{l} {%2,%0|%0,%2}; sets %1"
				 :"+m" (m_nValue), "=qm" (c)
				 :"ir" (nValue)
				 : "memory");
		return c != 0;
	}

	VistaType::sint32 VistaSigned32Atomic::ExchangeAndAdd( VistaType::sint32 nValue )
	{
		VistaType::sint32 res;
		__asm__ __volatile__ ("lock; xadd{l} {%0,%1|%1,%0}"
				: "=r" (res), "+m" (m_nValue)
							: "0" (nValue)
							: "memory");
		return res;
	}


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// VistaSigned64Atomic
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if defined( VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE )
	 inline void VistaSigned64Atomic::Add( VistaType::sint64 nValue)
	 {
		 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
		 m_nValue += nValue;
	 }

	 inline void VistaSigned64Atomic::Sub( VistaType::sint64 nValue )
	 {
		 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
		 m_nValue -= nValue;
	 }

	 void VistaSigned64Atomic::Inc()
	 {
		 Add(1);
	 }

	 void VistaSigned64Atomic::Dec()
	 {
		 Sub(1);
	 }

	 bool VistaSigned64Atomic::DecAndTestNull()
	 {
		 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
		 return (--m_nValue == 0);
	 }

	 bool VistaSigned64Atomic::AddAndTestNegative( VistaType::sint64 nValue )
	 {
		 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
		 m_nValue -= nValue;
		 return m_nValue < 0;

	 }

	 VistaType::sint64 VistaSigned64Atomic::ExchangeAndAdd( VistaType::sint64 nValue )
	 {
		 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
		 VistaType::sint64 old = m_nValue;
		 m_nValue -= nValue;
		 return old;
	 }
#else

	 inline void VistaSigned64Atomic::Add( VistaType::sint64 nValue)
	 {
			__asm__ __volatile__ ("lock; add{q} {%1,%0|%0,%1}"
					  : "+m" (m_nValue)
					  : "ir" (nValue)
					  : "memory");
	 }

	 inline void VistaSigned64Atomic::Sub( VistaType::sint64 nValue )
	 {
			__asm__ __volatile__ ("lock; sub{q} {%1,%0|%0,%1}"
					  : "+m" (m_nValue)
					  : "ir" (nValue)
					  : "memory");
	 }

	 void VistaSigned64Atomic::Inc()
	 {
			__asm__ __volatile__ ("lock; inc{q} %0"
					  : "+m" (m_nValue)
					  :
					  : "memory");
	 }

	 void VistaSigned64Atomic::Dec()
	 {
			__asm__ __volatile__ ("lock; dec{q} %0"
					  : "+m" (m_nValue)
					  :
					  : "memory");
	 }

	 bool VistaSigned64Atomic::DecAndTestNull()
	 {
			unsigned char c;
			__asm__ __volatile__("lock; dec{q} %0; sete %1"
					 :"+m" (m_nValue), "=qm" (c)
					 :
					 : "memory");
			return c != 0;
	 }

	 bool VistaSigned64Atomic::AddAndTestNegative( VistaType::sint64 nValue )
	 {
			unsigned char c;
			__asm__ __volatile__("lock; add{q} {%2,%0|%0,%2}; sets %1"
					 :"+m" (m_nValue), "=qm" (c)
					 :"ir" (nValue)
					 : "memory");
			return c != 0;
	 }

	 VistaType::sint64 VistaSigned64Atomic::ExchangeAndAdd( VistaType::sint64 nValue )
	 {
		 long res;
		__asm__ __volatile__ ("lock; xadd{q} {%0,%1|%1,%0}"
				: "=r" (res), "+m" (m_nValue)
							: "0" (nValue)
							: "memory");
		return res;
	 }
#endif // VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE

#elif defined(WIN32) // we use that for win64, too as it is *always* defined according to msdn on windows
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// VistaSigned32Atomic
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		inline void VistaSigned32Atomic::Add( VistaType::sint32 nValue)
		{
			InterlockedExchangeAdd( (LONG*)&m_nValue, (LONG)nValue);
		}

		inline void VistaSigned32Atomic::Sub( VistaType::sint32 nValue )
		{
			InterlockedExchangeAdd( (LONG*)&m_nValue, (LONG)-nValue);
		}

		void VistaSigned32Atomic::Inc()
		{
			Add(1);
		}


		void VistaSigned32Atomic::Dec()
		{
			Sub(1);
		}

		bool VistaSigned32Atomic::DecAndTestNull()
		{
			// test whether the old nValue was 1, so a decrease would render m_nValue to 0
			return ( InterlockedExchangeAdd((LONG*)&m_nValue, -1) == 1 );
		}

		bool VistaSigned32Atomic::AddAndTestNegative( VistaType::sint32 nValue )
		{
			return ( InterlockedExchangeAdd((LONG*)&m_nValue, (LONG)-nValue) < nValue );
		}

		VistaType::sint32 VistaSigned32Atomic::ExchangeAndAdd( VistaType::sint32 nValue )
		{
			return InterlockedExchangeAdd( (LONG*)&m_nValue, (LONG)nValue );
		}


		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// VistaSigned64Atomic
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE
		 inline void VistaSigned64Atomic::Add( VistaType::sint64 nValue)
		 {
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 m_nValue += nValue;
		 }

		 inline void VistaSigned64Atomic::Sub( VistaType::sint64 nValue )
		 {
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 m_nValue -= nValue;
		 }

		 void VistaSigned64Atomic::Inc()
		 {
			 Add(1);
		 }

		 void VistaSigned64Atomic::Dec()
		 {
			 Sub(1);
		 }

		 bool VistaSigned64Atomic::DecAndTestNull()
		 {
 			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 return (--m_nValue == 0);
		 }

		 bool VistaSigned64Atomic::AddAndTestNegative( VistaType::sint64 nValue )
		 {
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 m_nValue -= nValue;
			 return m_nValue < 0;

		 }

		 VistaType::sint64 VistaSigned64Atomic::ExchangeAndAdd( VistaType::sint64 nValue )
		 {
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 VistaType::sint64 old = m_nValue;
			 m_nValue -= nValue;
			 return old;
		 }
#else
		 inline void VistaSigned64Atomic::Add( VistaType::sint64 nValue)
		 {
			 InterlockedExchangeAdd64( (LONGLONG*)&m_nValue, (LONGLONG)nValue);
		 }

		 inline void VistaSigned64Atomic::Sub( VistaType::sint64 nValue )
		 {
			 InterlockedExchangeAdd64( (LONGLONG*)&m_nValue, (LONGLONG)-nValue);
		 }

		 void VistaSigned64Atomic::Inc()
		 {
			 Add(1);
		 }

		 void VistaSigned64Atomic::Dec()
		 {
			 Sub(1);
		 }

		 bool VistaSigned64Atomic::DecAndTestNull()
		 {
			 // test whether the old nValue was 1, so a decrease would render m_nValue to 0
			 return ( InterlockedExchangeAdd64((LONGLONG*)&m_nValue, (LONGLONG)-1) == 1 );
		 }

		 bool VistaSigned64Atomic::AddAndTestNegative( VistaType::sint64 nValue )
		 {
			 return ( InterlockedExchangeAdd64((LONGLONG*)&m_nValue, (LONGLONG)-nValue) < nValue );
		 }

		 VistaType::sint64 VistaSigned64Atomic::ExchangeAndAdd( VistaType::sint64 nValue )
		 {
			 return InterlockedExchangeAdd64( (LONGLONG*)&m_nValue, (LONGLONG)nValue );
		 }
#endif // WINVER

#else

#if defined(VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE)

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// VistaSigned32Atomic
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		void VistaSigned32Atomic::Add( VistaType::sint32 nValue)
		{
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 m_nValue += nValue;
		}

		void VistaSigned32Atomic::Sub( VistaType::sint32 nValue )
		{
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 m_nValue -= nValue;
		}

		void VistaSigned32Atomic::Inc()
		{
			 Add(1);
		}


		void VistaSigned32Atomic::Dec()
		{
			 Sub(1);
		}

		bool VistaSigned32Atomic::DecAndTestNull()
		{
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 return (--m_nValue == 0);
		}

		bool VistaSigned32Atomic::AddAndTestNegative( VistaType::sint32 nValue )
		{
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 m_nValue -= nValue;
			 return m_nValue < 0;
		}

		VistaType::sint32 VistaSigned32Atomic::ExchangeAndAdd( VistaType::sint32 nValue )
		{
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 VistaType::sint64 old = m_nValue;
			 m_nValue -= nValue;
			 return old;
		}


		 inline void VistaSigned64Atomic::Add( VistaType::sint64 nValue)
		 {
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 m_nValue += nValue;
		 }

		 inline void VistaSigned64Atomic::Sub( VistaType::sint64 nValue )
		 {
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 m_nValue -= nValue;
		 }

		 void VistaSigned64Atomic::Inc()
		 {
			 Add(1);
		 }
	

		 void VistaSigned64Atomic::Dec()
		 {
			 Sub(1);
		 }

		 bool VistaSigned64Atomic::DecAndTestNull()
		 {
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 return (--m_nValue == 0);
		 }

		 bool VistaSigned64Atomic::AddAndTestNegative( VistaType::sint64 nValue )
		 {
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 m_nValue -= nValue;
			 return m_nValue < 0;

		 }

		 VistaType::sint64 VistaSigned64Atomic::ExchangeAndAdd( VistaType::sint64 nValue )
		 {
			 _scopedLock l( *towp( m_pPrivate.m_pPrivate ) );
			 VistaType::sint64 old = m_nValue;
			 m_nValue -= nValue;
			 return old;
		 }
#else
	#error atomics not decided on this platform
#endif // VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE

#endif
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

