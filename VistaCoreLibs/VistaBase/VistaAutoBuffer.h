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


#ifndef _VISTAAUTOBUFFER_H_
#define _VISTAAUTOBUFFER_H_

#include "VistaBaseConfig.h"
#include "VistaBaseTypes.h"
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaSigned32Atomic;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTABASEAPI VistaAutoBuffer
{
public:
	VistaAutoBuffer();
	virtual ~VistaAutoBuffer();
	VistaAutoBuffer( const VistaAutoBuffer & );
	VistaAutoBuffer &operator=( const VistaAutoBuffer & );

	// ---------------------------------------------------------------
	// vector mirror interface
	// this mirrors just a subset of the vector interface, as this
	// buffer is not meant to be a ref-counted total replacement
	// of vector.
	// ---------------------------------------------------------------

	typedef VistaType::byte         value_type;
	typedef std::vector<value_type> bufferType;
	typedef bufferType::size_type   size_type;

	typedef bufferType::const_iterator         const_iterator;
	typedef bufferType::const_pointer          const_pointer;
	typedef bufferType::const_reference        const_reference;
	typedef bufferType::const_reverse_iterator const_reverse_iterator;

	const_iterator begin() const;
	const_iterator end() const;

	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;

	const_reference back() const;
	const_reference front() const;

	const_reference operator[] ( size_type ) const;

	const_reference at( size_type ) const;
	const_pointer data() const;


	size_type size() const;
	bool empty() const;


	// operators
	bool operator==( const VistaAutoBuffer & ) const;
	bool operator!=( const VistaAutoBuffer & ) const;

	// -----------------------------------------------------------------
	// additional interface
	// -----------------------------------------------------------------

	bool HasOneOwner() const;
	VistaType::sint32 GetCount() const;
	
	bool IsValid() const;
protected:
	bufferType      *m_vecBuffer;

	VistaAutoBuffer( bool bValid );
private:
	VistaSigned32Atomic        *m_cnt;
};

class VISTABASEAPI VistaAutoWriteBuffer : public VistaAutoBuffer
{
public:
	VistaAutoWriteBuffer();
	VistaAutoWriteBuffer( size_type size, const value_type &v = value_type() );
	VistaAutoWriteBuffer( const VistaAutoWriteBuffer & );
	VistaAutoWriteBuffer &operator=( const VistaAutoWriteBuffer & );

	typedef bufferType::pointer pointer;
	typedef bufferType::reference reference;
	typedef bufferType::iterator iterator;
	typedef bufferType::reverse_iterator reverse_iterator;

	iterator begin();
	reverse_iterator rbegin();

	iterator end();
	reverse_iterator rend();

	reference back();
	reference front();

	reference operator[]( size_type );
	reference at(size_type);
	pointer   data();

	iterator insert( iterator position, const value_type & );
	void     insert( iterator position, size_type num, const value_type & );

	iterator erase( iterator position );
	iterator erase( iterator first, iterator last );

	void swap( VistaAutoWriteBuffer &other );
	void clear();

	void resize( size_type, value_type c = value_type() );
	void reserve( size_type );


private:

};


#endif /* _VISTAAUTOBUFFER_H_ */

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/

