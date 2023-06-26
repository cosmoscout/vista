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

#include "VistaBase64.h"
#include <VistaBase/VistaExceptionBase.h>
#include <cassert>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

namespace {

char EncodeSextet(VistaType::byte sextet) {
  if (sextet >= 0 && sextet <= 25)
    return static_cast<char>(sextet + 65); // to ASCII 65 ('A')
  else if (sextet >= 26 && sextet <= 51)
    return static_cast<char>(sextet + 71); // to ASCII 97 ('a')
  else if (sextet >= 52 && sextet <= 61)
    return static_cast<char>(sextet - 4); // to ASCII 48 ('0')
  else if (sextet == 62)
    return '+';
  else if (sextet == 63)
    return '/';

  VISTA_THROW("Invalid sextet! Cannot encode this byte stream. \
				  Something must be wrong with the Encode() function.",
      -1);
  // Intentionally no 'general' return statement here as compiler detects it
  // as 'unreachable code', producing a warning.
}

VistaType::byte DecodeCharacter(char character) {
  if (character >= 65 && character <= 90)
    return static_cast<VistaType::byte>(character - 65);
  else if (character >= 97 && character <= 122)
    return static_cast<VistaType::byte>(character - 71);
  else if (character >= 48 && character <= 57)
    return static_cast<VistaType::byte>(character + 4);
  else if (character == '+')
    return static_cast<VistaType::byte>(62);
  else if (character == '/')
    return static_cast<VistaType::byte>(63);
  else if (character == '=')
    VISTA_THROW("Padding character detected at wrong position. \
					  Check input to have 0, 1, or 2 padding characters ('=') at the _END_,",
        -1);

  VISTA_THROW("Invalid character! Is input string really base64 encoded?", -1);
  // Intentionally no 'general' return statement here as compiler detects it
  // as 'unreachable code', producing a warning.
}

} // namespace

namespace VistaBase64 {

/**
 * @TODO Optimize this function. A lot of code should be easily eliminated
 *		 by compactifying the if-branches.
 */
std::string Encode(const VistaType::byte* pInput, std::size_t nNumBytes) {
  std::size_t            nBytesRemaining = nNumBytes;
  const VistaType::byte* pCursor         = pInput;
  std::string            strResult;

  while (nBytesRemaining > 0) {
    if (nBytesRemaining > 2) // at least 3 bytes remaining
    {
      // bits 0..5
      VistaType::byte a = (pCursor[0] >> 2) & 0x3F;
      // bits 6..11
      VistaType::byte b = ((pCursor[0] << 4) & 0x30) | ((pCursor[1] >> 4) & 0x0F);
      // bits 12..17
      VistaType::byte c = ((pCursor[1] << 2) & 0x3C) | ((pCursor[2] >> 6) & 0x03);
      // bits 18..23
      VistaType::byte d = pCursor[2] & 0x3F;

      nBytesRemaining -= 3;
      pCursor += 3;

      strResult += ::EncodeSextet(a);
      strResult += ::EncodeSextet(b);
      strResult += ::EncodeSextet(c);
      strResult += ::EncodeSextet(d);
    } else // either 0, 1 or 2 bytes remaining
    {
      if (nBytesRemaining == 2) {
        // bits 0..5
        VistaType::byte a = (pCursor[0] >> 2) & 0x3F;
        // bits 6..11
        VistaType::byte b = ((pCursor[0] << 4) & 0x30) | ((pCursor[1] >> 4) & 0x0F);
        // bits 12..17
        VistaType::byte c = (pCursor[1] << 2) & 0x3C;

        strResult += ::EncodeSextet(a);
        strResult += ::EncodeSextet(b);
        strResult += ::EncodeSextet(c);
        strResult += "=";

        nBytesRemaining -= 2;
        // final bytes, no cursor movement required
      } else if (nBytesRemaining == 1) {
        // bits 0..5
        VistaType::byte a = (pCursor[0] >> 2) & 0x3F;
        // bits 6..11
        VistaType::byte b = (pCursor[0] << 4) & 0x30;

        strResult += ::EncodeSextet(a);
        strResult += ::EncodeSextet(b);
        strResult += "==";

        nBytesRemaining -= 1;
        pCursor += 1;
        // final byte, no cursor movement required
      }
    }
  }

  return strResult;
}

std::string Encode(const std::vector<VistaType::byte>& vecInput) {
  if (vecInput.empty())
    return "";
  return Encode(&vecInput[0], vecInput.size());
}

bool Decode(const std::string& strInput, VistaType::byte*& pOutput, std::size_t& nNumBytes) {
  std::size_t nCursor = 0;

  // nothing to decode
  if (strInput.empty())
    return true;

  // must always be a multiple of 4, or string is malformed (missing filling '='?)
  if (strInput.length() % 4 != 0)
    return false;

  // determine padding size
  std::size_t nPaddingSize = 0;
  if (strInput[strInput.length() - 1] == '=' && strInput[strInput.length() - 2] == '=')
    nPaddingSize = 2; // final block contains only 1 byte
  else if (strInput[strInput.length() - 1] == '=')
    nPaddingSize = 1; // final block contains only 2 bytes

  nNumBytes                      = strInput.length() / 4 * 3 - nPaddingSize;
  pOutput                        = new VistaType::byte[nNumBytes];
  VistaType::byte* pOutputCursor = pOutput;

  while (nCursor < strInput.length()) {
    VistaType::byte a = DecodeCharacter(strInput[nCursor + 0]);
    VistaType::byte b = DecodeCharacter(strInput[nCursor + 1]);

    // decode full blocks
    // second condition ensure complete final blocks are also handled here
    if (nCursor + 4 < strInput.length() || nPaddingSize == 0) {
      VistaType::byte c = DecodeCharacter(strInput[nCursor + 2]);
      VistaType::byte d = DecodeCharacter(strInput[nCursor + 3]);

      // bits 0..7
      pOutputCursor[0] = ((a << 2) & 0xFC)    // top 6 bits
                         | ((b >> 4) & 0x03); // bottom 2 bits
      // bits 8..15
      pOutputCursor[1] = ((b << 4) & 0xF0)    // top 4 bits
                         | ((c >> 2) & 0x0F); // bottom 4 bits
      // bits 16..23
      pOutputCursor[2] = ((c << 6) & 0xC0) // top 2 bits
                         | (d & 0x3F);     // bottom 6 bits

      pOutputCursor += 3; // only here, as not required for final (incomplete) block
    } else                // in case final block is incomplete
    {
      // in any case we have one byte
      // bits 0..7
      pOutputCursor[0] = ((a << 2) & 0xFC)    // top 6 bits
                         | ((b >> 4) & 0x03); // bottom 2 bits

      if (nPaddingSize == 1) // two bytes are left
      {
        VistaType::byte c = DecodeCharacter(strInput[nCursor + 2]);
        // bits 8..15
        pOutputCursor[1] = ((b << 4) & 0xF0)    // top 4 bits
                           | ((c >> 2) & 0x0F); // bottom 4 bits
      }
    }

    nCursor += 4;
  }

  return true;
}

bool Decode(const std::string& strInput, std::vector<VistaType::byte>& vecOutput) {
  vecOutput.clear();
  VistaType::byte* pOutput   = NULL;
  std::size_t      nNumBytes = 0;

  const bool bResult = Decode(strInput, pOutput, nNumBytes);
  if (bResult) {
    vecOutput.assign(pOutput, pOutput + nNumBytes);
    delete[] pOutput;
  }

  return bResult;
}

} // namespace VistaBase64
