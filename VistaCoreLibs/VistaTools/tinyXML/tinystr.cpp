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

/*
www.sourceforge.net/projects/tinyxml
Original file by Yves Berquin.

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

/*
 * THIS FILE WAS ALTERED BY Tyge L�vset, 7. April 2005.
 */

/*
 * THIS FILE WAS ALTERED BY Ingo Assenmacher, 17.04.2008
 * - added namespace for the inclusion of the source files to the ViSTA
 *   Virtual Reality Toolkit, RWTH Aachen University
 * - added TIXML_USE_STL #ifdef/#define
 */

#ifndef TIXML_USE_STL

#include "tinystr.h"

namespace VistaXML {

// Error value for find primitive
const TiXmlString::size_type TiXmlString::npos = static_cast<TiXmlString::size_type>(-1);

// Null rep.
TiXmlString::Rep TiXmlString::nullrep_ = {0, 0, {'\0'}};

void TiXmlString::reserve(size_type cap) {
  if (cap > capacity()) {
    TiXmlString tmp;
    tmp.init(length(), cap);
    std::memcpy(tmp.start(), data(), length());
    swap(tmp);
  }
}

TiXmlString& TiXmlString::assign(const char* str, size_type len) {
  size_type cap = capacity();
  if (len > cap || cap > 3 * (len + 8)) {
    TiXmlString tmp;
    tmp.init(len);
    std::memcpy(tmp.start(), str, len);
    swap(tmp);
  } else {
    std::memmove(start(), str, len);
    set_size(len);
  }
  return *this;
}

TiXmlString& TiXmlString::append(const char* str, size_type len) {
  size_type newsize = length() + len;
  if (newsize > capacity()) {
    reserve(newsize + capacity());
  }
  std::memmove(finish(), str, len);
  set_size(newsize);
  return *this;
}

TiXmlString operator+(const TiXmlString& a, const TiXmlString& b) {
  TiXmlString tmp;
  tmp.reserve(a.length() + b.length());
  tmp += a;
  tmp += b;
  return tmp;
}

TiXmlString operator+(const TiXmlString& a, const char* b) {
  TiXmlString            tmp;
  TiXmlString::size_type b_len = static_cast<TiXmlString::size_type>(std::strlen(b));
  tmp.reserve(a.length() + b_len);
  tmp += a;
  tmp.append(b, b_len);
  return tmp;
}

TiXmlString operator+(const char* a, const TiXmlString& b) {
  TiXmlString            tmp;
  TiXmlString::size_type a_len = static_cast<TiXmlString::size_type>(std::strlen(a));
  tmp.reserve(a_len + b.length());
  tmp.append(a, a_len);
  tmp += b;
  return tmp;
}

} // namespace VistaXML
#endif // TIXML_USE_STL
