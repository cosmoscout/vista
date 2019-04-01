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


#ifndef _VISTACRC32_H
#define _VISTACRC32_H


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"

#include <VistaBase/VistaBaseTypes.h>

#include <string>

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
 * \brief Calculates CRC-Codes for verification and identification purposes.
 *
 * A cyclic redundancy check (CRC) is a type of hash function used to produce a
 * checksum - which is a small, fixed number of bits - against a block of data,
 * such as a packet of network traffic or a block of a computer file. The checksum
 * is used to detect and correct errors after transmission or storage. A CRC is
 * computed and appended before transmission or storage, and verified afterwards
 * by recipient to confirm that no changes occurred on transit. Correction can
 * also be done if information lost is lower than information held by the checksum.
 *
 * This implementation supports polynoms of degree 32 for the checksum generation,
 * hence the name CRC-32. The constructor has a popular default polynom given by the
 * key 0x04c11db7.
 *
 * The typical usage is to first call the constructor, which builds a lookup-table.
 * ProcessString or ProcessByteBuffer can be called to compute the checksum.
 * Alternatively, AddString or AddBuffer can be called multiple times and the
 * result can be returned by GetResult. Please note that GetResult() resets the
 * internal checksum.
 *
 * Literature: Sarwate, D.V., "Computation of Cyclic Redundancy Checks via
 *             Table Look-Up", Communications of the ACM, 31(8), pp.1008-1013, 1988.
 *
 */
class VISTATOOLSAPI VistaCRC32
{
public:

	VistaCRC32						(unsigned long ulKey = 0x04c11db7);
	virtual ~VistaCRC32			();

	/// Computes and returns checksum
	unsigned long	ProcessString	(const std::string &sText);
	/// Computes and returns checksum

	unsigned long	ProcessByteBuffer(VistaType::byte* pByteBuffer, int iLength);

	/// Adds successively to checksum
	void			AddString		(const std::string &sText);

	/// Adds successively to checksum
	void			AddByteBuffer	(VistaType::byte* pByteBuffer, int iLength);

	/// Returns accumulated checksum and resets internal checksum
	unsigned long	GetResult		();

private:
	void			Init			(unsigned long ulKey);
	void			AddByte			(VistaType::byte cByte);

	unsigned long m_ulLUTElements[256];
	unsigned long m_ulRegister;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTACRC32_H
