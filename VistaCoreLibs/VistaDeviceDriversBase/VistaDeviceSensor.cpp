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


#include "VistaDeviceSensor.h"

#include <VistaAspects/VistaPropertyFunctorRegistry.h>
#include <VistaAspects/VistaAspectsUtils.h>


#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaSerializingToolset.h>

#include <VistaAspects/VistaSerializer.h>
#include <VistaAspects/VistaDeSerializer.h>
#include <VistaAspects/VistaSerializable.h>


#include <VistaTools/VistaCPUInfo.h>

#include <cassert>

unsigned int IVistaMeasureTranscode::ITranscodeIndexedGet::UNKNOWN_NUMBER_OF_INDICES = ~0;

#if defined( VISTA_USE_ATOMICS ) && ( defined( WIN32 ) || defined( LINUX ) )
	#define USE_32BIT_ATOMICS
	#if !defined( WIN32 ) || !defined( VISTA_NATIVE64BITATOMICS_NOT_AVAILABLE )
		#define USE_64BIT_ATOMICS
	#endif
#endif


#if !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )
#include <VistaInterProcComm/Concurrency/VistaSemaphore.h>
#else
	#if defined(WIN32)
	#include <Windows.h>
	#elif defined(LINUX) && defined(__GNUC__) //&& defined(__i386__)
	// up to now, we are using hand-crafted assembler...
    // up-coming unix kernels have atomics in user-land, so we might switch one day
	#endif
#endif // !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )

/**
 * @todo move atomics to IPC (IAR: would not move this code! go and copy/modify)
 */
class VistaAtomics
{
public:
	typedef void* SWAPADDRESS;

	/**
	 * @return the prior-value of v1
	 */
	VistaType::uint32 AtomicSet(SWAPADDRESS v1, VistaType::uint32 v2)
	{
#if !defined( USE_32BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		VistaType::uint32 vOld = *((VistaType::uint32*)v1); // assign old value
		*((VistaType::uint32*)v1) = v2; // store new one, we assume that this works using
							 // a semaphore
		return vOld; // return the prior-value.
#else
	#if defined(WIN32)
			/** LONG is always 32bit, even on win64 */
			return InterlockedExchange((LONG*)v1, (LONG)v2);
	#elif defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) || defined( __LP32__) || defined(__LP64__) )
			return intel_gnuc_atomicswap((VistaType::uint32*)v1, (VistaType::uint32)v2); //atomic_xchg(v1, v2);
	#else
		VistaSemaphoreLock l(m_lock);
		VistaType::uint32 vOld = *((VistaType::uint32*)v1); // assign old value
		*((VistaType::uint32*)v1) = v2; // store new one, we assume that this works using
							 // a semaphore
		return vOld; // return the prior-value.
	#endif
#endif // USE_32BIT_ATOMICS
	}

	VistaType::uint64 AtomicSet64(SWAPADDRESS v1, VistaType::uint64 v2)
	{
#if !defined( USE_64BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		VistaType::uint64 vOld = *((VistaType::uint64*)v1); // assign old value
		*((VistaType::uint64*)v1) = v2; // store new one, we assume that this works using
							 // a semaphore
		return vOld; // return the prior-value.
#else
	#if defined(WIN32)
		#if _MSC_VER < 1900 // old versions have a different prototype
				return InterlockedExchange64((LONGLONG*)v1, (LONGLONG)v2);
		#else
				return InterlockedExchange64((unsigned __int64*)v1, (unsigned __int64)v2);
		#endif // _MSC_VER
	#elif defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) || defined( __LP32__) || defined(__LP64__) )
			return intel_gnuc_atomicswap64((VistaType::uint64*)v1, (VistaType::uint64)v2); //atomic_xchg(v1, v2);
	#else
		VistaSemaphoreLock l(m_lock);
		VistaType::uint64 vOld = *((VistaType::uint64*)v1); // assign old value
		*((VistaType::uint64*)v1) = v2; // store new one, we assume that this works using
							 // a semaphore
		return vOld; // return the prior-value.
	#endif
#endif // USE_64BIT_ATOMICS
	}

	#if defined(__GNUC__) && ( defined(__i386__) || defined(__x86_64__) || defined( __LP32__) || defined(__LP64__) )
	// only swaps 32bit values
	static VistaType::uint32 intel_gnuc_atomicswap(volatile VistaType::uint32 *mem, VistaType::uint32 val)
						{
						VistaType::uint32 prev = val;
						asm volatile ("lock; xchgl %0, %1"
						: "=r" (prev)
						: "m" (*(mem)), "0"(prev)
						: "memory");
						return prev;
						}

	static VistaType::uint64 intel_gnuc_atomicswap64(volatile VistaType::uint64 *mem, VistaType::uint64 val)
						{
						VistaType::uint64 prev = val;
						asm volatile ("lock; xchgq %0, %1"
						: "=r" (prev)
						: "m" (*(mem)), "0"(prev)
						: "memory");
						return prev;
						}
	#endif

	inline VistaType::uint32 AtomicRead(const VistaType::uint32 &ref)
	{
#if !defined( USE_32BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		return ref;
#else
		return ref;
#endif
	}

	inline VistaType::uint64 AtomicRead64(const VistaType::uint64 &ref)
	{
#if !defined( USE_64BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		return ref;
#else
		return ref;
#endif
	}

	template<class T>
	T AtomicReadT( const T &ref )
	{
#if !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )
		VistaSemaphoreLock l(m_lock);
		return ref;
#else
		return ref;
#endif
	}

#if !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )
	VistaSemaphore m_lock;
#endif


	VistaAtomics()
#if !defined( USE_32BIT_ATOMICS ) || !defined( USE_64BIT_ATOMICS )
	: m_lock(1, VistaSemaphore::SEM_TYPE_FASTEST)
#endif
	{

	}

	static std::string GetAtomicState()
	{
#if defined( USE_32BIT_ATOMICS ) && defined( USE_64BIT_ATOMICS )
		return "VISTA_USE_ATOMICS SET!";
#elif defined( USE_32BIT_ATOMICS )
		return "VISTA_USE_ATOMICS SET (32BIT) PLUS SEMAPHORE (64BIT)!";
#else
		return "SEMAPHORE LOCKED ACCESS";
#endif
	}
};


// *****************************************************************************
// helper classes
// *****************************************************************************

IVistaTranscoderFactoryFactory::~IVistaTranscoderFactoryFactory() {}

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


const VistaSensorMeasure* VistaMeasureHistory::GetMeasure( const VistaType::uint32 nReadHead ) const
{
	if( nReadHead == ~0u )
		return GetMostCurrent();

	const VistaSensorMeasure& pMeasure = (*m_rbHistory.index( nReadHead ));
	return &pMeasure;
}


const VistaSensorMeasure *VistaMeasureHistory::GetPast( unsigned int nNum, const VistaType::uint32 nReadHead ) const
{
	if( nReadHead == ~0u )
	{
		return GetCurrentPast( nNum );
	}
	else	
	{
		Buffer::const_iterator itHead = m_rbHistory.index( nReadHead );
		const VistaSensorMeasure& pMeasure = itHead - nNum;
		return &pMeasure;
	}
}

const VistaSensorMeasure *VistaMeasureHistory::GetMostCurrent() const
{
	// return measure at current read head
	VistaMeasureHistory::Buffer::const_iterator cit( m_rbHistory.index( GetCurrentReadHead() ) );
	const VistaSensorMeasure& pMeasure = (*cit);
	return &pMeasure;
}



const VistaSensorMeasure *VistaMeasureHistory::GetCurrentPast(unsigned int nNum) const
{
	VistaMeasureHistory::Buffer::const_iterator cit( m_rbHistory.index( GetCurrentReadHead() ) );
	const VistaSensorMeasure& pMeasure = cit - nNum;
	return &pMeasure;
}



const VistaMeasureHistory::Buffer &VistaMeasureHistory::GetReadBuffer() const
{
	return m_rbHistory;
}



VistaMeasureHistory::VistaMeasureHistory( VistaAtomics *pAtomics, IVistaDeviceDriver::AllocMemoryFunctor* pAlloc,
										IVistaDeviceDriver::ClaimMemoryFunctor* pFree )
: m_state_cookie(0),
  m_rbHistory(1024),
  m_nDriverWriteSize(0),
  m_nClientReadSize(0),
  m_pAtomics(pAtomics),
  m_nMeasureSize(0),
  m_bIsInitialized(false),
  m_pAllocMemFunctor( pAlloc ),
  m_pFreeMemFunctor( pFree )
{
}

VistaMeasureHistory::VistaMeasureHistory( const VistaMeasureHistory& oHistory, const bool bCopyData )
: m_state_cookie(0),
  m_rbHistory(1024),
  m_nDriverWriteSize(0),
  m_nClientReadSize(0),
  m_pAtomics( oHistory.m_pAtomics ),
  m_nMeasureSize(0),
  m_bIsInitialized(false),
  m_pAllocMemFunctor( oHistory.m_pAllocMemFunctor ),
  m_pFreeMemFunctor( oHistory.m_pFreeMemFunctor )
{
	if( bCopyData )
	{
		m_rbHistory = oHistory.m_rbHistory;
		m_nDriverWriteSize = oHistory.m_nDriverWriteSize;
		m_nClientReadSize = oHistory.m_nClientReadSize;
		m_nMeasureSize = oHistory.m_nMeasureSize;
	}
	else
	{
		Resize( oHistory.m_nClientReadSize, oHistory.m_nDriverWriteSize, oHistory.m_nMeasureSize );
	}
}


VistaMeasureHistory::~VistaMeasureHistory()
{
	// if a free functor is given, clean up the existing history
	if( m_pFreeMemFunctor && m_bIsInitialized )
	{
		for( unsigned int i = 0; i < ( m_nClientReadSize + m_nDriverWriteSize); ++i )
		{
			VistaSensorMeasure& oMeasure = (*m_rbHistory.index(i));
			(*m_pFreeMemFunctor)( oMeasure );
		}
	}
}


VistaType::uint32 VistaMeasureHistory::GetDriverWriteSize() const
{
	return m_nDriverWriteSize;
}

VistaType::uint32 VistaMeasureHistory::GetClientReadSize() const
{
	return m_nClientReadSize;
}

VistaMeasureHistory::Buffer & VistaMeasureHistory::GetWriteBuffer()
{
	return m_rbHistory;
}

VistaType::uint32 VistaMeasureHistory::GetMeasureCount() const
{
	const VistaSensorMeasure* pMeasure = GetMostCurrent();
	if( pMeasure->GetIsValid() == false )
		return 0;
	else
		return pMeasure->m_nMeasureIdx + 1;
}

VistaType::microtime VistaMeasureHistory::GetUpdateTimeStamp() const
{
	const VistaSensorMeasure* pMeasure = GetMostCurrent();
	if( pMeasure->GetIsValid() == false )
		return 0;
	else
		return pMeasure->m_nMeasureTs;
}

VistaType::uint32 VistaMeasureHistory::GetCurrentWriteHead() const
{
	const VistaMeasureHistory::Buffer::size_type& nVar = m_rbHistory.GetCurrentConstRef();
	return (VistaType::uint32)m_pAtomics->AtomicRead( nVar );
}

VistaType::uint32 VistaMeasureHistory::GetCurrentReadHead() const
{
	return (VistaType::uint32)m_rbHistory.pred( GetCurrentWriteHead() );
}

bool VistaMeasureHistory::Resize( unsigned int nClientReadSize, unsigned int nDriverWriteSize, unsigned int nMeasureSize )
{
	if( nMeasureSize == 0 )
		return false; // illegal to set measure size to 0

	unsigned int nNewSize = nClientReadSize + nDriverWriteSize;

	if( nNewSize == 0 )
		return false; // not allowed

	// check if the new bounds fit into the old bounds (we may shift the client/driver boundary
	// also make sure that the measure size was not reset to a different size
	if( (nNewSize == ( m_nDriverWriteSize + m_nClientReadSize )) && nMeasureSize == m_nMeasureSize )
	{
		m_nDriverWriteSize = nDriverWriteSize;
		m_nClientReadSize  = nClientReadSize;
		return true;
	}

	// if a free functor is given, clean up the existing history
	if( m_pFreeMemFunctor && m_bIsInitialized )
	{
		for( unsigned int i = 0; i < ( m_nClientReadSize + m_nDriverWriteSize); ++i )
		{
			VistaSensorMeasure& oMeasure = (*m_rbHistory.index(i));
			(*m_pFreeMemFunctor)( oMeasure );
		}
	}

	m_rbHistory = VistaMeasureHistory::Buffer( nNewSize );

	for( unsigned int n = 0; n < nNewSize; ++n )
	{
		VistaSensorMeasure m( ~0u, 0, nMeasureSize );
		m_rbHistory.Add( m );
		if( m_pAllocMemFunctor )
		{
			(*m_pAllocMemFunctor)( *m_rbHistory.index(n) );
			m_bIsInitialized = true; // flip to steady true
		}
	}

	m_rbHistory.Reset();

	m_nDriverWriteSize = nDriverWriteSize;
	m_nClientReadSize  = nClientReadSize;
	m_nMeasureSize     = nMeasureSize;

	return true;
}

VistaType::uint32 VistaMeasureHistory::GetSensorMeasureSize() const
{
	return m_nMeasureSize;
}

VistaType::uint32 VistaMeasureHistory::GetHistorySize() const
{
	assert( ( m_nClientReadSize + m_nDriverWriteSize ) == m_rbHistory.GetBufferSize() );
	return ( m_nClientReadSize + m_nDriverWriteSize );
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool IVistaMeasureTranscoderFactory::OnUnload()
{
#if defined(DEBUG)
	vstr::outi() << "IVistaMeasureTranscoderFactory::OnUnload("
			<< typeid( *this ).name()
			<< ")" << std::endl;
#endif
	VistaPropertyFunctorRegistry *pReg = VistaPropertyFunctorRegistry::GetSingleton();

	// get all getter symbols from prop fac with the co-responding type tag.
	std::list<IVistaPropertyGetFunctor*> liGetterByClass = pReg->GetGetterByClass( GetTranscoderName() );
	std::list<IVistaPropertySetFunctor*> liSetterByClass = pReg->GetSetterByClass( GetTranscoderName() );

	// unregister all getters first
	for( std::list<IVistaPropertyGetFunctor*>::iterator git = liGetterByClass.begin(); git != liGetterByClass.end(); ++git )
		delete *git;

	for( std::list<IVistaPropertySetFunctor*>::iterator sit = liSetterByClass.begin(); sit != liSetterByClass.end(); ++sit )
		delete *sit;

	return true;
}



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


 int DefaultBinaryDumpSerializeAdapter::Serialize( IVistaSerializer &output ) const
{
	const VistaSensorMeasure *output_measure = GetReadOnlySensorMeasure();
	if( output_measure )
	{
		int nret = SerializeMeasureHeader( output, *output_measure );

		VistaType::uint32 size = static_cast<VistaType::uint32>((*output_measure).m_vecMeasures.size());
		nret += output.WriteInt32( size );

		if( size )
			nret += output.WriteRawBuffer( &(*output_measure).m_vecMeasures[0], (int)size );

		return nret;
	}
	return 0;
}

 int DefaultBinaryDumpSerializeAdapter::DeSerialize( IVistaDeSerializer &input )
{
	VistaSensorMeasure *input_measure = GetWriteableSensorMeasure();
	if( input_measure )
	{
		int nret = DeSerializeMeasureHeader( input, *input_measure );

		VistaType::uint32 size=0;
		input.ReadInt32( size );

		(*input_measure).m_vecMeasures.resize(size);

		if( size )
			input.ReadRawBuffer( &(*input_measure).m_vecMeasures[0], size );

		return nret;
	}
	return 0;
}


 std::string DefaultBinaryDumpSerializeAdapter::GetSignature() const
{
	return "DefaultBinaryDumpSerializeAdapter";
}

 int DefaultBinaryDumpSerializeAdapter::SerializeMeasureHeader( IVistaSerializer &output, const VistaSensorMeasure &output_measure )
 {
 	int nret = 0;

 	nret += output.WriteRawBuffer( &output_measure.m_nBusWidth, sizeof( VistaType::byte ) );
 	nret += output.WriteRawBuffer( &output_measure.m_nEndianess, sizeof( VistaType::byte ) );

 	nret += output.WriteInt32( output_measure.m_nMeasureIdx );
 	nret += output.WriteDouble( output_measure.m_nMeasureTs );
 	nret += output.WriteDouble( output_measure.m_nSwapTime );
 	nret += output.WriteDouble( output_measure.m_nDeliverTs );
 	nret += output.WriteDouble( output_measure.m_nAverageDeliverTime );
 	return nret;
 }

 int DefaultBinaryDumpSerializeAdapter::DeSerializeMeasureHeader( IVistaDeSerializer &input, VistaSensorMeasure &input_measure)
 {
 	int nret = 0;

 	nret += input.ReadRawBuffer( &input_measure.m_nBusWidth, sizeof( VistaType::byte) );
 	nret += input.ReadRawBuffer( &input_measure.m_nEndianess, sizeof( VistaType::byte) );

 #if defined(DEBUG)
 	if( input_measure.m_nEndianess != VistaSerializingToolset::GetPlatformEndianess()
 		&& input.GetByteorderSwapFlag() == false )
 	{
 		vstr::errp() << "Endianess mismatch!";
 	}
 #endif

 	nret += input.ReadInt32( input_measure.m_nMeasureIdx );
 	nret += input.ReadDouble( input_measure.m_nMeasureTs );
 	nret += input.ReadDouble( input_measure.m_nSwapTime );
 	nret += input.ReadDouble( input_measure.m_nDeliverTs );
 	nret += input.ReadDouble( input_measure.m_nAverageDeliverTime );
 	return nret;
 }


IVistaMeasureTranscode::IVistaMeasureTranscode()
: m_pMeasure(NULL)
, m_nNumberOfScalars(0)
{
}


IVistaMeasureTranscode::~IVistaMeasureTranscode()
{
}


bool IVistaMeasureTranscode::SetSensorMeasure(const VistaSensorMeasure *pMeasure)
{
	m_pMeasure = pMeasure;
	return true;
}

const VistaSensorMeasure *IVistaMeasureTranscode::GetSensorMeasure() const
{
	return m_pMeasure;
}

unsigned int IVistaMeasureTranscode::GetNumberOfScalars() const
{
	return m_nNumberOfScalars;
}

void IVistaMeasureTranscode::SetNumberOfScalars(unsigned int uiNumScalars)
{
	m_nNumberOfScalars = uiNumScalars;
}

std::string IVistaMeasureTranscode::GetTypeString()
{
	return "IVistaMeasureTranscode";
}

IVistaMeasureTranscode::ITranscodeGet *IVistaMeasureTranscode::GetMeasureProperty(const std::string &strPropName) const
{
	VistaPropertyFunctorRegistry *pReg = VistaPropertyFunctorRegistry::GetSingleton();
	// ok, ok... it is a const-getter, but we might need to cache the
	// base-class list, as this is unlikely to change, so we check
	// whether we have to build it
	if(m_BtList.empty())
	{
		// ... ah, yes
		// cache it
		m_BtList = GetBaseTypeList();
	}

	if( m_MpList.empty() )
	{
		// we have a m_BtList here,
		// we need to have all symbols starting at IVistaMeasureTranscode 

		// first: copy the base-class list
		m_MpList = m_BtList;
		
		// remove any type class sting belonging to out base class (leave in the rest)
		m_MpList.remove_if( VistaAspectsComparisonStuff::ObjEqualsCompare<std::string>( SuperClass::GetReflectionableType() ) );
	}


	// use the cached version for the get-functor
	return dynamic_cast<ITranscodeGet*>(pReg->GetGetFunctor(strPropName, GetReflectionableType(), m_MpList));
}

std::set<std::string> IVistaMeasureTranscode::GetMeasureProperties() const
{
	VistaPropertyFunctorRegistry *pReg = VistaPropertyFunctorRegistry::GetSingleton();
		// ok, ok... it is a const-getter, but we might need to cache the
		// base-class list, as this is unlikely to change, so we check
		// whether we have to build it
		if(m_BtList.empty())
		{
			// ... ah, yes
			// cache it
			m_BtList = GetBaseTypeList();
		}

		if( m_MpList.empty() )
		{
			// we have a m_BtList here,
			// we need to have all symbols starting at IVistaMeasureTranscode 

			// first: copy the base-class list
			m_MpList = m_BtList;
			
			// remove any type class sting belonging to out base class (leave in the rest)
			m_MpList.remove_if( VistaAspectsComparisonStuff::ObjEqualsCompare<std::string>( SuperClass::GetReflectionableType() ) );
		}

		// get all symbols belonging to these base-classes
		std::set<std::string> oSet;
		pReg->GetGetterSymbolSet( oSet, m_MpList );
		return oSet;
}




namespace
{
	const std::string sSReflectionTypeVSD("IVistaMeasureTranscode");

	class VistaMeasureIndexTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<VistaType::uint32>
	{
	public:
		VistaMeasureIndexTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<unsigned int>("MEASUREINDEX", sSReflectionTypeVSD, "absolute index of measurement")
		{
		}

		virtual VistaType::uint32 GetValue(const VistaSensorMeasure *pMeasure ) const
		{
			return pMeasure->m_nMeasureIdx;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure,
								VistaType::uint32 &dIndex ) const
		{
			dIndex = pMeasure->m_nMeasureIdx;
			return true;
		}
	};

	class VistaMeasureTimeStampTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<VistaType::microtime>
	{
	public:
		VistaMeasureTimeStampTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<VistaType::microtime>("TIMESTAMP", sSReflectionTypeVSD, "timestamp of raw measurement")
		{
		}

		virtual VistaType::microtime GetValue(const VistaSensorMeasure *pMeasure ) const
		{
			return pMeasure->m_nMeasureTs;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure,
				VistaType::microtime &dStamp ) const
		{
			dStamp = pMeasure->m_nMeasureTs;
			return true;
		}
	};

	class VistaMeasureTimeStampSecTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<double>
	{
	public:
		VistaMeasureTimeStampSecTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<double>("TIMESTAMP", sSReflectionTypeVSD, "timestamp of raw measurement"),
			m_nSpeed(1)
		{
			VistaCPUInfo info;
			m_nSpeed = info.GetSpeed();
		}

		virtual double GetValue(const VistaSensorMeasure *pMeasure ) const
		{
			return double(pMeasure->m_nMeasureTs) / double(m_nSpeed);
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure,
									 double &dStamp ) const
		{
			dStamp = GetValue(pMeasure);
			return true;
		}
	private:
		VistaType::uint64 m_nSpeed;
	};

	class VistaMeasureDeliveryTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<double>
	{
	public:
		VistaMeasureDeliveryTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<double>("DELIVERY_TIMESTAMP", sSReflectionTypeVSD, "the timestamp of delivery to the history")
		{
		}
		virtual double GetValue(const VistaSensorMeasure *pMeasure ) const
		{
			return double(pMeasure->m_nDeliverTs);
		}
		virtual bool GetValue(const VistaSensorMeasure *pMeasure,
									 double &dStamp ) const
		{
			dStamp = GetValue(pMeasure);
			return true;
		}
	private:
	};

	class VistaMeasureSwapTimeTranscode : public IVistaMeasureTranscode::TTranscodeValueGet<double>
	{
	public:
		VistaMeasureSwapTimeTranscode()
			: IVistaMeasureTranscode::TTranscodeValueGet<double>("SWAPTIME", sSReflectionTypeVSD, "time in between locks (lock latency)")
		{
		}

		virtual double GetValue(const VistaSensorMeasure *pMeasure ) const
		{
			return pMeasure->m_nSwapTime;
		}

		virtual bool GetValue(const VistaSensorMeasure *pMeasure,
				double &dStamp ) const
		{
			dStamp = pMeasure->m_nSwapTime;
			return true;
		}
	};

	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new VistaMeasureIndexTranscode,
		new VistaMeasureTimeStampTranscode,
		new VistaMeasureSwapTimeTranscode,
		new VistaMeasureTimeStampSecTranscode,
		NULL
	};
}

namespace VistaDeviceSensorUtil
{
	void ReleaseProperties()
	{
		IVistaPropertyGetFunctor **git = SaGetter;

		while( *git )
			delete *git++;
	}
}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDeviceSensor::VistaDeviceSensor( IVistaDeviceDriver::AllocMemoryFunctor* pAlloc,
						IVistaDeviceDriver::ClaimMemoryFunctor* pFree)
: m_pAtomics(new VistaAtomics),
  m_rbSensorHistory( m_pAtomics, pAlloc, pFree ),
  m_pMeasureTranscode(NULL),
  m_nSwap1FailCount(0),
  m_nSwap2FailCount(0),
  m_pTickStop( new VistaWindowAverageTimer ),
  m_pParent(NULL)
{
}

VistaDeviceSensor::~VistaDeviceSensor()
{
	delete m_pAtomics;
	delete m_pTickStop;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

IVistaDeviceDriver *VistaDeviceSensor::GetParent() const
{
	return m_pParent;
}

void VistaDeviceSensor::SetParent( IVistaDeviceDriver *pParent )
{
	m_pParent = pParent;
}

VistaType::microtime VistaDeviceSensor::GetUpdateTimeStamp() const
{
	return m_rbSensorHistory.GetUpdateTimeStamp();
}

unsigned int VistaDeviceSensor::GetMeasureCount() const
{
	// protected read of measure count
	return m_rbSensorHistory.GetMeasureCount();
}


unsigned int VistaDeviceSensor::GetMeasureHistorySize() const
{
	return (unsigned int)m_rbSensorHistory.GetReadBuffer().GetBufferSize();
}


bool VistaDeviceSensor::AddMeasure(const VistaSensorMeasure &oMeasure)
{
	m_rbSensorHistory.m_rbHistory.Add(oMeasure);
	return true;
}

bool VistaDeviceSensor::AdvanceMeasure()
{
	VistaMeasureHistory::Buffer& oBuffer = m_rbSensorHistory.GetWriteBuffer();
	// get a reference to the memory where the current ref (write head) is
	// stored (mark it as volatile). size_type is typically sizeof(long) (bus-width)
	volatile VistaMeasureHistory::Buffer::size_type &cur = oBuffer.GetCurrentRef();

	// get next slot index and save in next (using RingBuffer.succ())
	// succ() does not change the value, it merely reads it and increases it in the return value
	// while it aboyes the array bounds
	VistaMeasureHistory::Buffer::size_type next = oBuffer.succ(cur);

	// first... some statistics...
	VistaType::microtime nPre = VistaTimeUtils::GetStandardTimer().GetMicroTime();


	// note: checking equality directly within the if condition issues a warning ...
	bool bIs64bit = (sizeof(TVistaRingBuffer<VistaSensorMeasure>::size_type) == sizeof(VistaType::uint64));
	if( bIs64bit )
	{
		while(m_pAtomics->AtomicSet64((VistaAtomics::SWAPADDRESS)&cur, next) == cur)
			++m_nSwap1FailCount;
	}
	else
	{
		while(m_pAtomics->AtomicSet((VistaAtomics::SWAPADDRESS)&cur, (VistaType::uint32)next) == (VistaType::uint32)cur )
			++m_nSwap1FailCount;
	}

	// finally... more statistics...
	VistaType::microtime nPost = VistaTimeUtils::GetStandardTimer().GetMicroTime();

	// write the swap-time to the "last" written value!
	// this is, of course, a race, as the "last" value is possibly under
	// access... but currently, the swap-time is for debugging and likely to
	// be removed in the future.

	// so, fetch the current slot
	VistaMeasureHistory::Buffer::iterator it( oBuffer.index( m_rbSensorHistory.GetCurrentReadHead() ) ); // of the current slot being written


	// write swap-time as the difference to post / pre
	(*it).m_nSwapTime = nPost - nPre;

	// return true, iff this swap really worked
	return ( oBuffer.GetCurrentRef() == next );
}


const VistaMeasureHistory &VistaDeviceSensor::GetMeasures() const
{
	return m_rbSensorHistory;
}

VistaMeasureHistory &VistaDeviceSensor::GetDriverMeasures()
{
	return m_rbSensorHistory;
}

IVistaMeasureTranscode *VistaDeviceSensor::GetMeasureTranscode() const
{
	return m_pMeasureTranscode;
}

void VistaDeviceSensor::SetMeasureTranscode(IVistaMeasureTranscode *pTranscode)
{
	m_pMeasureTranscode = pTranscode;
}


unsigned int VistaDeviceSensor::GetSwap1CountFail() const
{
	return m_nSwap1FailCount;
}

unsigned int VistaDeviceSensor::GetSwap2CountFail() const
{
	return m_nSwap2FailCount;
}

std::string VistaDeviceSensor::GetTypeHint() const
{
	return m_sTypeHint;
}

void VistaDeviceSensor::SetTypeHint(const std::string &sTypeHint)
{
	m_sTypeHint = sTypeHint;
}

std::string VistaDeviceSensor::GetSensorName() const
{
	return m_nSensorName;
}

void VistaDeviceSensor::SetSensorName( const std::string &strName )
{
	m_nSensorName = strName;
}

VistaWindowAverageTimer &VistaDeviceSensor::GetWindowTimer() const
{
	return *m_pTickStop;
}

unsigned int VistaDeviceSensor::GetNewMeasureCount( unsigned int &lastRead ) const
{
	unsigned int nMIndex = GetMeasureCount();

	// skim: the user can only read what he decided to read, even if there are more
	// entries in the current history.
	// Example: user said, n slots are interesting, n+m (m>0) are there, the new measure count
	// is limited to n.
	unsigned int nNewMeasureCount = std::min<unsigned int>( nMIndex - lastRead,
			m_rbSensorHistory.m_nClientReadSize);

	// write back to user given reference value
	lastRead = nMIndex;

	// return the number of new measures available for read
	return nNewMeasureCount;
}


IVistaSerializer   &operator<<(IVistaSerializer &oSer,
							   const VistaSensorMeasure &oMeasure )
{
	oSer << oMeasure.m_nEndianess
		 << oMeasure.m_nBusWidth
		 << oMeasure.m_nMeasureIdx
		 << oMeasure.m_nMeasureTs
		 << oMeasure.m_nSwapTime
		 << oMeasure.m_nDeliverTs
		 << oMeasure.m_nAverageDeliverTime
		 << static_cast<VistaType::uint32>(oMeasure.m_vecMeasures.size());

	// we are writing the measure vec as a raw buffer
	// this will lead to problems, once this data is transferred between machines
	// with a different endianess. One solution could be: send an endianess flag
	// along and let the transcoders read off the memory using a ByteBufferSerializer
	// with the sending endianess. But there might be a devil in the details then...
	oSer.WriteRawBuffer( &oMeasure.m_vecMeasures[0], (int)oMeasure.m_vecMeasures.size() );

	return oSer;
}

IVistaDeSerializer &operator>>(IVistaDeSerializer &oDeSer,
							   VistaSensorMeasure &oMeasure)
{
	oDeSer >> oMeasure.m_nEndianess
		   >> oMeasure.m_nBusWidth;

#if defined(DEBUG)
	if( oMeasure.m_nEndianess != VistaSerializingToolset::GetPlatformEndianess() )
	{
		vstr::warnp() << "[VistaSensorMeasure]: Endianess mismatch during deserialization" << std::endl;
	}

	if( oMeasure.m_nBusWidth != VistaSerializingToolset::GetPlatformBusWidth() )
	{
		vstr::warnp() << "[VistaSensorMeasure]: BusWidth mismatch during deserialization" << std::endl;
	}
#endif

	oDeSer	 >> oMeasure.m_nMeasureIdx
		 >> oMeasure.m_nMeasureTs
		 >> oMeasure.m_nSwapTime
		 >> oMeasure.m_nDeliverTs
		 >> oMeasure.m_nAverageDeliverTime;

	unsigned int nSize = 0;
	oDeSer >> nSize;
	oMeasure.m_vecMeasures.resize(nSize);

	// reading the raw buffer as is assumes at this stage that the content
	// here has a good endianess.
	oDeSer.ReadRawBuffer( &oMeasure.m_vecMeasures[0], nSize );
	return oDeSer;
}

IVistaSerializer   &operator<<(IVistaSerializer &oSer,
							   const VistaMeasureHistory &oHist )
{
	// it is possible that a driver is writing concurrently to this history.
	// that means that some values might be crashed during the read off this
	// history. The current past should be ok to read off, so we ignore this
	// here. However, we have to think about concurrent device access in a cluster
	// environment. Does not seem to be easy.
	oSer << oHist.m_nClientReadSize
		 << oHist.m_nDriverWriteSize
		 << static_cast<VistaType::uint32>(oHist.GetReadBuffer().GetBufferSize())
		 << static_cast<VistaType::uint32>(oHist.GetReadBuffer().GetFirst())
		 << static_cast<VistaType::uint32>(oHist.GetReadBuffer().GetNext())
		 << static_cast<VistaType::uint32>(oHist.GetReadBuffer().GetCurrentVal());

	const TVistaRingBuffer<VistaSensorMeasure>::container_type &vCont = oHist.GetReadBuffer().GetRawAccess();
	TVistaRingBuffer<VistaSensorMeasure>::container_type::const_iterator begin = vCont.begin();
	TVistaRingBuffer<VistaSensorMeasure>::container_type::const_iterator end   = vCont.end();

	for(TVistaRingBuffer<VistaSensorMeasure>::container_type::const_iterator cit = begin; cit != end; ++cit)
	{
		oSer << *cit;
	}
	return oSer;
}


IVistaDeSerializer &operator>>(IVistaDeSerializer &oDeSer,
							   VistaMeasureHistory &oHist)
{
	/** FIXME hmm... this effectively can f*ck up all concurrent */
	// readers to this history, as it is part of a sensor that
	// might be attached to the sensor of this history.
	oDeSer >> oHist.m_nClientReadSize
		   >> oHist.m_nDriverWriteSize;


	VistaType::uint32 nFirst, nNext, nCurrent, nSize;
	nFirst = nNext = nCurrent = nSize = 0;
	oDeSer >> nSize
		   >> nFirst
		   >> nNext
		   >> nCurrent;


	// this should not really happen, but I am lazy now, so if it
	// happens, the next line will force it to work properly
	if(oHist.m_rbHistory.GetRawAccess().size() != nSize)
	{
		oHist.m_rbHistory.ResizeBuffer(nSize);
	}

	oHist.m_rbHistory.SetFirst(nFirst);
	oHist.m_rbHistory.SetNext(nNext);
	oHist.m_rbHistory.SetCurrent(nCurrent);

	// read off the single measures index by index
	// we should think about optimizing that and only transferring
	// the different slots and not all of them
	for(unsigned int n=0; n < nSize; ++n)
	{
		oDeSer >> *(oHist.m_rbHistory.index(n));
	}

	return oDeSer;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


