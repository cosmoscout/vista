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


#include "VistaAutoBufferPool.h"

#include <VistaBase/VistaStreamUtils.h>

#include <algorithm>
#include <functional>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
namespace
{
	class _findUnique : public std::unary_function<const VistaAutoWriteBuffer, bool>
	{
	public:
		bool operator()( const VistaAutoWriteBuffer &b ) const
		{
			// if there is only one owner, it is the pool (since we created it, we own it)
			return b.HasOneOwner();
		}
	};

#ifdef DEBUG
	class _checkCount : public std::unary_function<const VistaAutoWriteBuffer, void>
	{
	public:
		void operator()( const VistaAutoWriteBuffer &b ) const
		{
			if( b.HasOneOwner() == false )
				vstr::outi() << "VistaAutoWriteBuffer @ " << &b
				                    << " seems still to be owned be someone else than this pool, "
				                    << "but was create by bufferpool that is now destroyed."
				                    << std::endl;
		}
	};
#endif
}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

VistaAutoBufferPool::VistaAutoBufferPool( size_t nNumBufsPreAlloced, size_t nBufDefSize, bool resizeOnNeed )
: m_pool()
, m_bAdaptSize( resizeOnNeed )
{
	for( size_t n=0 ; n < nNumBufsPreAlloced; ++n )
		m_pool.push_back( VistaAutoWriteBuffer(nBufDefSize) );
}

VistaAutoBufferPool::~VistaAutoBufferPool()
{
#if defined(DEBUG)
	std::for_each( m_pool.begin(), m_pool.end(), _checkCount() );
#endif
}


VistaAutoWriteBuffer VistaAutoBufferPool::GetBufferWrite()
{
	std::vector<VistaAutoWriteBuffer>::const_iterator cit = SearchNextAvailable();
	if( cit == m_pool.end() )
	{
		if( m_bAdaptSize )
		{
			m_pool.push_back( VistaAutoWriteBuffer(0) );
			return m_pool.back();
		}
		else
			return VistaAutoWriteBuffer(); // invalid buffer
	}

	return *cit;
}

size_t VistaAutoBufferPool::GetCurrentNumOfBuffers() const
{
	return m_pool.size();
}

std::vector<VistaAutoWriteBuffer>::const_iterator VistaAutoBufferPool::SearchNextAvailable() const
{
	return std::find_if( m_pool.begin(), m_pool.end(), _findUnique() );
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
