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


#ifndef _VISTARINGBUFFER_H
#define _VISTARINGBUFFER_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
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
 * TVistaRingBuffer is an array with cyclic access, it is not a ring buffer
 * in a classical sense (double-linked list of entries or something like that).
 * The main code was taken from a questionable implementation found on the
 * internet, so maybe it is better to re-write this structure from scratch.
 * Most of this API is simple to understand, accessors in const and non-const
 * form as well as traversal routines. But use with care, many side-effects.
 */
template <typename T, class TSize_type = typename std::vector<T>::size_type >
class TVistaRingBuffer
{
public:
	typedef std::vector<T>            container_type;
	typedef TSize_type                size_type;
	typedef T&                        reference;
	typedef const T&                  const_reference;
	typedef T                         value_type;


	class iterator
	{
	public:
		iterator()
			: m_npItem(NULL),  m_nIndex(0)
		{}

		iterator(container_type* pcc, size_type ii)
			: m_npItem(pcc), m_nIndex(ii)
		{}

		iterator(const iterator& rhs)
			: m_npItem(rhs.m_npItem), m_nIndex(rhs.m_nIndex)
		{}

		iterator operator=(const iterator& rhs)
		{
			m_npItem = rhs.m_npItem;
			m_nIndex = rhs.m_nIndex;
			return *this;
		}

		reference operator*() const
		{
			return m_npItem->at(m_nIndex);
		}

		reference operator+(int nAhead) const
		{
			int nIndex = m_nIndex + nAhead;
			if( nIndex < 0 ) // nAhead possibly negative
			{
				return m_npItem->at( m_npItem->size()-1 + nIndex );
			}
			else
				return m_npItem->at( nIndex % m_npItem->size() );
		}

		reference operator-( int nBefore) const
		{
			nBefore = nBefore % m_npItem->size();

			int nIndex = m_nIndex - nBefore;
			if( nIndex < 0 )
			{
				return m_npItem->at( nIndex + m_npItem->size() ); // get from back
			}
			else
				return m_npItem->at( nIndex % m_npItem->size() );
		}


		iterator operator++()
		{
			m_nIndex = !(m_nIndex<m_npItem->size()-1) ? 0 : m_nIndex+1;
			return *this;
		}

		iterator operator--()
		{
			m_nIndex = 0<m_nIndex ? m_nIndex-1 : m_npItem->size()-1;
			return *this;
		}

		bool operator==(const iterator& rhs) const
		{
			return (m_npItem == rhs.m_npItem) && (m_nIndex == rhs.m_nIndex);
		}

		bool operator!=(const iterator& rhs) const
		{
			return (m_npItem != rhs.m_npItem) || (m_nIndex != rhs.m_nIndex);
		}
	private:
		container_type* m_npItem;
		typename container_type::size_type m_nIndex;
	};

	class const_iterator
	{
	public:
		const_iterator(const container_type *pcc, size_type ii)
			: m_npItem(pcc), m_nIndex(ii)
		{}

		const_iterator(const const_iterator& rhs)
			: m_npItem(rhs.m_npItem), m_nIndex(rhs.m_nIndex)
		{}

		const_reference operator*() const
		{
			return m_npItem->at(m_nIndex);
		}

		const_reference operator+( int nAhead) const
		{
			int nIndex = m_nIndex + nAhead;
			if( nIndex < 0 ) // nAhead possibly negative
			{
				return m_npItem->at( int(m_npItem->size()-1) + nIndex );
			}
			else
			{
				return m_npItem->at( nIndex % m_npItem->size() );
			}
		}

		const_reference operator-( int nBefore) const
		{
			nBefore = nBefore % (int)m_npItem->size();

			int nIndex = (int)m_nIndex - nBefore;
			if( nIndex < 0 )
			{
				return m_npItem->at( nIndex + m_npItem->size() ); // get from back
			}
			else
				return m_npItem->at( nIndex % m_npItem->size() );
		}


		const_iterator operator++()
		{
			m_nIndex = !(m_nIndex<m_npItem->size()-1) ? 0 : m_nIndex+1;
			return *this;
		}

		const_iterator operator--()
		{
			m_nIndex = 0<m_nIndex ? m_nIndex-1 : (m_npItem->size() ? m_npItem->size()-1 : 0);
			return *this;
		}

		bool operator==(const const_iterator& rhs) const
		{
			return (m_npItem == rhs.m_npItem) && (m_nIndex == rhs.m_nIndex);
		}

		bool operator!=(const const_iterator& rhs) const
		{
			return (m_npItem != rhs.m_npItem) || (m_nIndex != rhs.m_nIndex);
		}

		const_iterator& operator=(const const_iterator& rhs)
		{
			m_npItem = rhs.m_npItem;
			m_nIndex = rhs.m_nIndex;
			return *this;
		}
	private:
		const    container_type* m_npItem;
		typename container_type::size_type m_nIndex;
	};


	TVistaRingBuffer(size_type nSize)
		: m_nFirst(0), m_nNext(0)
		// m_nCurrent is written on call to Reset() -- not nice
		// (side-effect), but again no need to write the value twice
	{
		Reset();
		m_vecContainer.resize(nSize);
	}
	
	bool GetIsEmpty() const
	{
		return m_vecContainer.empty();
	}

	bool GetIsFull() const
	{
		return m_vecContainer.capacity() == m_vecContainer.size();
	}

	size_type GetBufferSize() const
	{
		return m_vecContainer.size();
	}


	const_iterator begin() const
	{
		return const_iterator(&m_vecContainer,m_nFirst);
	}

	const_iterator index(size_type nIndex) const
	{
		return const_iterator(&m_vecContainer, nIndex);
	}

	iterator index(size_type nIndex)
	{
		return iterator(&m_vecContainer, nIndex);
	}

	iterator begin()
	{
		return iterator(&m_vecContainer,m_nFirst);
	}

	const_iterator end() const
	{
		return const_iterator(&m_vecContainer,m_nNext);
	}

	iterator end()
	{
		return iterator(&m_vecContainer,m_nNext);
	}

	const_iterator GetCurrentConst() const
	{
		return const_iterator(&m_vecContainer, m_nCurrent);
	}

	iterator GetCurrent()
	{
		return iterator(&m_vecContainer, m_nCurrent);
	}

	bool Advance()
	{
		m_nCurrent = succ(m_nCurrent);
		return true;
	}

	bool Stepback()
	{
		if(!m_vecContainer.empty())
			m_nCurrent = pred(m_nCurrent);
		return true;
	}

	void Add(const value_type &val)
	{
		if (m_nNext == m_vecContainer.size() )
			m_vecContainer.push_back(val);
		else
			m_vecContainer[m_nNext] = val;
		
		m_nNext  = succ(m_nNext);
	}

	TVistaRingBuffer& operator<<(value_type val)
	{
		Add(val);
		return *this;
	}

	void Reset()
	{
		m_nCurrent = m_nNext = m_nFirst = 0;
	}

	value_type RetrieveValue()
	{
		size_type temp = m_nCurrent;
		if((m_nCurrent = succ(m_nCurrent))==m_nNext)
			m_nCurrent = m_nFirst;
		return m_vecContainer.at(temp);
	}

	TVistaRingBuffer& operator>>(reference ref)
	{
		ref = RetrieveValue();
		return *this;
	}

	void ResizeBuffer(size_type nNewSize)
	{
		m_vecContainer.resize(nNewSize);
		Reset();
	}

	void SetBufferSize(size_t nNewSize)
	{
		m_vecContainer.resize(nNewSize);
		Reset();
	}

	const container_type &GetRawAccess() const
	{
		return m_vecContainer;
	}

	size_type &GetCurrentRef()
	{
		return m_nCurrent;
	}

	size_type GetCurrentVal() const
	{
		return m_nCurrent;
	}

	const size_type &GetCurrentConstRef() const
	{
		return m_nCurrent;
	}

	size_type succ(size_type nIndex) const
	{
		return ((++nIndex) % (m_vecContainer.size()));
	}

	size_type pred(size_type nIndex) const
	{
		return (nIndex ? --nIndex  : (size_type)m_vecContainer.size()-1);
	}

	size_type GetFirst() const
	{
		return m_nFirst;
	}

	size_type GetNext() const
	{
		return m_nNext;
	}

	void SetFirst(size_type nFirst)
	{
		m_nFirst = nFirst;
	}

	void SetNext(size_type nNext)
	{
		m_nNext = nNext;
	}

	void SetCurrent(size_type nCurrent)
	{
		m_nCurrent = nCurrent;
	}
private:
	size_type
		m_nFirst,   // oldest element
		m_nNext,    // m_nNext writeable element
		m_nCurrent; // write head
	container_type m_vecContainer; // storage
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTARINGBUFFER_H

