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

#include "VistaAutoBuffer.h"
#include "VistaAtomicCounter.h"

#include <cstddef>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

// -----------------------------------------------------------------------------
// VistaAutoBuffer
// -----------------------------------------------------------------------------

VistaAutoBuffer::~VistaAutoBuffer() {
  if (m_cnt && (*m_cnt).DecAndTestNull()) {
    delete m_cnt;
    delete m_vecBuffer;
    //		std::cout << "VistaAutoBuffer::~VistaAutoBuffer(" << this << ") -- delete" <<
    //std::endl;
  }
}

VistaAutoBuffer::VistaAutoBuffer(bool bValid)
    : m_vecBuffer(bValid ? new bufferType : NULL)
    , m_cnt(bValid ? new VistaSigned32Atomic(1) : NULL) {
}

VistaAutoBuffer::VistaAutoBuffer()
    : m_vecBuffer(new bufferType)
    , m_cnt(new VistaSigned32Atomic(1)) {
  //	std::cout << "VistaAutoBuffer::VistaAutoBuffer(" << this << ")" << std::endl;
}

VistaAutoBuffer::VistaAutoBuffer(const VistaAutoBuffer& other) {
  m_cnt = other.m_cnt; // there is no race here: other lives on stack, will stay on stack and the
                       // copy on stack has a ref-count != 0
  if (m_cnt) // prevent crash when copying from an invalid buffer
    ++(*m_cnt);

  m_vecBuffer =
      other.m_vecBuffer; // invalid buffers are NULL, copy of pointer is legal in that case
}

VistaAutoBuffer& VistaAutoBuffer::operator=(const VistaAutoBuffer& other) {
  if (&other == this)
    return *this;

  if (m_cnt && m_cnt->DecAndTestNull()) // we are the last owner of this buffer, dump it
  {
    delete m_cnt;
    delete m_vecBuffer;
  }

  m_cnt = other.m_cnt;

  if (m_cnt)
    ++(*m_cnt);

  m_vecBuffer = other.m_vecBuffer;

  return *this;
}

// -----------------------------------------------------------------------------
// VistaAutoWriteBuffer
// -----------------------------------------------------------------------------

VistaAutoWriteBuffer::VistaAutoWriteBuffer()
    : VistaAutoBuffer(false) {
}

VistaAutoWriteBuffer::VistaAutoWriteBuffer(size_type nSize, const value_type& v)
    : VistaAutoBuffer() {
  if (nSize)
    resize(nSize, v);
}

VistaAutoWriteBuffer::VistaAutoWriteBuffer(const VistaAutoWriteBuffer& other)
    : VistaAutoBuffer(other) {
}

VistaAutoWriteBuffer& VistaAutoWriteBuffer::operator=(const VistaAutoWriteBuffer& other) {
  if (&other == this)
    return *this;

  VistaAutoBuffer::operator=(other);

  return *this;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

// ----------------------------------------------------------------------------
// VistaAutoBuffer
// ----------------------------------------------------------------------------

VistaAutoBuffer::const_iterator VistaAutoBuffer::begin() const {
  return (*m_vecBuffer).begin();
}

VistaAutoBuffer::const_iterator VistaAutoBuffer::end() const {
  return (*m_vecBuffer).end();
}

VistaAutoBuffer::const_reverse_iterator VistaAutoBuffer::rbegin() const {
  return (*m_vecBuffer).rbegin();
}

VistaAutoBuffer::const_reverse_iterator VistaAutoBuffer::rend() const {
  return (*m_vecBuffer).rend();
}

VistaAutoBuffer::const_reference VistaAutoBuffer::back() const {
  return (*m_vecBuffer).back();
}

VistaAutoBuffer::const_reference VistaAutoBuffer::front() const {
  return (*m_vecBuffer).front();
}

VistaAutoBuffer::const_reference VistaAutoBuffer::operator[](size_type n) const {
  return (*m_vecBuffer)[n];
}

VistaAutoBuffer::const_reference VistaAutoBuffer::at(size_type n) const {
  return (*m_vecBuffer).at(n);
}

VistaAutoBuffer::const_pointer VistaAutoBuffer::data() const {
#if defined(WIN32)
  return &(*m_vecBuffer)[0];
#else
  return (*m_vecBuffer).data();
#endif
}

VistaAutoBuffer::size_type VistaAutoBuffer::size() const {
  return (*m_vecBuffer).size();
}

bool VistaAutoBuffer::empty() const {
  return (*m_vecBuffer).empty();
}

// operators
bool VistaAutoBuffer::operator==(const VistaAutoBuffer& other) const {
  return m_vecBuffer == other.m_vecBuffer;
}

bool VistaAutoBuffer::operator!=(const VistaAutoBuffer& other) const {
  return !(*this == other);
}

// -----------------------------------------------------------------
// additional interface
// -----------------------------------------------------------------

bool VistaAutoBuffer::HasOneOwner() const {
  return ((*m_cnt).Get() == 1);
}

VistaType::sint32 VistaAutoBuffer::GetCount() const {
  return (*m_cnt).Get();
}

bool VistaAutoBuffer::IsValid() const {
  return (m_vecBuffer != NULL) && (m_cnt != NULL);
}

// -----------------------------------------------------------------------------
// VistaAutoWriteBuffer
// -----------------------------------------------------------------------------
VistaAutoWriteBuffer::iterator VistaAutoWriteBuffer::begin() {
  return (*m_vecBuffer).begin();
}

VistaAutoWriteBuffer::iterator VistaAutoWriteBuffer::end() {
  return (*m_vecBuffer).end();
}

VistaAutoWriteBuffer::reverse_iterator VistaAutoWriteBuffer::rbegin() {
  return (*m_vecBuffer).rbegin();
}

VistaAutoWriteBuffer::reverse_iterator VistaAutoWriteBuffer::rend() {
  return (*m_vecBuffer).rend();
}

VistaAutoWriteBuffer::reference VistaAutoWriteBuffer::back() {
  return (*m_vecBuffer).back();
}

VistaAutoWriteBuffer::reference VistaAutoWriteBuffer::front() {
  return (*m_vecBuffer).front();
}

VistaAutoWriteBuffer::reference VistaAutoWriteBuffer::operator[](size_type n) {
  return (*m_vecBuffer)[n];
}

VistaAutoWriteBuffer::reference VistaAutoWriteBuffer::at(size_type n) {
  return (*m_vecBuffer).at(n);
}

VistaAutoWriteBuffer::pointer VistaAutoWriteBuffer::data() {
#if defined(WIN32)
  return &(*m_vecBuffer)[0];
#else
  return (*m_vecBuffer).data();
#endif
}

VistaAutoWriteBuffer::iterator VistaAutoWriteBuffer::erase(iterator position) {
  return (*m_vecBuffer).erase(position);
}

VistaAutoWriteBuffer::iterator VistaAutoWriteBuffer::erase(iterator first, iterator last) {
  return (*m_vecBuffer).erase(first, last);
}

void VistaAutoWriteBuffer::swap(VistaAutoWriteBuffer& other) {
  (*m_vecBuffer).swap(*other.m_vecBuffer);
}

void VistaAutoWriteBuffer::clear() {
  (*m_vecBuffer).clear();
}

VistaAutoWriteBuffer::iterator VistaAutoWriteBuffer::insert(
    iterator position, const value_type& v) {
  return (*m_vecBuffer).insert(position, v);
}

void VistaAutoWriteBuffer::insert(iterator position, size_type num, const value_type& v) {
  (*m_vecBuffer).insert(position, num, v);
}

void VistaAutoWriteBuffer::resize(size_type n, value_type c) {
  if (n == size())
    (*m_vecBuffer).assign(n, c);
  else
    (*m_vecBuffer).resize(n, c);
}

void VistaAutoWriteBuffer::reserve(size_type n) {
  if (n != size())
    (*m_vecBuffer).reserve(n);
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
