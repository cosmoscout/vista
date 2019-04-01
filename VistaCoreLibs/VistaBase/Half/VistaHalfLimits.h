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


///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////


// Primary authors:
//     Florian Kainz <kainz@ilm.com>
//     Rod Bogart <rgb@ilm.com>


#ifndef INCLUDED_VISTAHALF_LIMITS_H
#define INCLUDED_VISTAHALF_LIMITS_H


//------------------------------------------------------------------------
//
//	C++ standard library-style numeric_limits for class half
//
//------------------------------------------------------------------------

#include <limits>
#include "VistaHalf.h"

namespace std {

template <>
class numeric_limits <VistaHalf>
{
  public:

	static const bool is_specialized = true;

	static VistaHalf min () throw () {return VISTAHALF_NRM_MIN;}
	static VistaHalf max () throw () {return VISTAHALF_MAX;}

	static const int digits = VISTAHALF_MANT_DIG;
	static const int digits10 = VISTAHALF_DIG;
	static const bool is_signed = true;
	static const bool is_integer = false;
	static const bool is_exact = false;
	static const int radix = VISTAHALF_RADIX;
	static VistaHalf epsilon () throw () {return VISTAHALF_EPSILON;}
	static VistaHalf round_error () throw () {return VISTAHALF_EPSILON / 2;}

	static const int min_exponent = VISTAHALF_MIN_EXP;
	static const int min_exponent10 = VISTAHALF_MIN_10_EXP;
	static const int max_exponent = VISTAHALF_MAX_EXP;
	static const int max_exponent10 = VISTAHALF_MAX_10_EXP;

	static const bool has_infinity = true;
	static const bool has_quiet_NaN = true;
	static const bool has_signaling_NaN = true;
	static const float_denorm_style has_denorm = denorm_present;
	static const bool has_denorm_loss = false;
	static VistaHalf infinity () throw () {return VistaHalf::posInf();}
	static VistaHalf quiet_NaN () throw () {return VistaHalf::qNan();}
	static VistaHalf signaling_NaN () throw () {return VistaHalf::sNan();}
	static VistaHalf denorm_min () throw () {return VISTAHALF_MIN;}

	static const bool is_iec559 = false;
	static const bool is_bounded = false;
	static const bool is_modulo = false;

	static const bool traps = true;
	static const bool tinyness_before = false;
	static const float_round_style round_style = round_to_nearest;
};


} // namespace std

#endif
