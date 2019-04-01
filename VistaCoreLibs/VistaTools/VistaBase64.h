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


#ifndef _VISTABASE64_H
#define _VISTABASE64_H

#include "VistaToolsConfig.h"
#include <VistaBase/VistaBaseTypes.h>
#include <string>
#include <vector>
#include <cstddef>


namespace VistaBase64
{

	/**
	 * Encodes an arbitrary byte stream into a base64 string.
	 * 
	 * @param pInput Pointer to the beginning of the byte stream to be encoded.
	 * @param nNumBytes The number of bytes to be encoded.
	 * @return A base64 string that represents the input.
	 */
	std::string VISTATOOLSAPI Encode( const VistaType::byte* pInput, std::size_t nNumBytes );
	/**
	 * Encodes an arbitrary byte vector into a base64 string.
	 * 
	 * @param vecInput The vector that contains the bytes to be encoded.
	 * @return A base64 string that represents the input vector.
	 */
	std::string VISTATOOLSAPI Encode( const std::vector< VistaType::byte >& vecInput );

	/**
	 * Decodes a base64 string into a byte stream.
	 * 
	 * @param strInput The base64 string to be decoded.
	 * @param pOutput An uninitialized pointer that will be initialized by the
	 *		  decode function and hold the output byte stream. Will be NULL if
	 *		  an error occurs.
	 * @param nNumBytes Will hold the number of bytes in the byte stream.
	 * @return Will return 'true' iff decode was successful, 'false' otherwise.
	 *		   In case 'false' is returned, the output pointer will be set to
	 *		   NULL. In case 'true' is returned, users will have to delete the
	 *		   output pointer (use delete []).
	 *		   
	 * @TODO Will return true and pOutput=NULL iff strInput is "". Is this ok?
	 *		 Shouldn't the pointer pOutput be valid in that case?
	 */
	bool VISTATOOLSAPI Decode( const std::string& strInput,
							   VistaType::byte*& pOutput,
							   std::size_t& nNumBytes );
	/**
	 * Decodes a base64 string into a byte stream.
	 * 
	 * @param strInput The base64 string to be decoded.
	 * @param vecOutput Will hold the decodes byte stream. Will be emptied by
	 *		  a call to this function in any case. Will remain empty if an error
	 *		  was encountered.
	 * @return Will return 'true' iff decode was successful, 'false' otherwise.
	 *		   In case 'false' is returned, the output vector will be empty.
	 */
	bool VISTATOOLSAPI Decode( const std::string& strInput,
							   std::vector< VistaType::byte >& vecOutput );

} // namespace VistaBase64

#endif // Include guard
