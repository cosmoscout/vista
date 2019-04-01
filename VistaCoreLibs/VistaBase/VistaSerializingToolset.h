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


#ifndef _VISTASERIALIZINGTOOLSET_H
#define _VISTASERIALIZINGTOOLSET_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaBaseConfig.h"

#include <VistaBase/VistaBaseTypes.h>
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
 * The VistaSerializingToolset encapsulates a number of utility methods that
 * are used during serializations. It can be used by serializers/deserializers.
 */
class VISTABASEAPI VistaSerializingToolset
{
private:

	/**
	 * We do not need instances of Toolsets, instead we use the methods
	 * directly (static-methods)
	 */
	VistaSerializingToolset();

	/**
	 * We do not create an instance, so we do not destroy it ;)
	 */
	~VistaSerializingToolset();

	// replaced by Swap(void *, int, int) below...
	//static void SwapStep(void *p, int iLength, int iCount);
protected:
public:
	enum eEndianess
	{
		VST_NONEENDIAN = 0,     /**< no assumption        */
		VST_LITTLEENDIAN = 1,   /**< low bits first       */
		VST_MIDDLEENDIAN = 2,   /**< high/low bits mixed  */
		VST_BIGENDIAN    = 3    /**< high bits first      */
	};

	enum ByteOrderSwapBehavior
	{
		DOES_NOT_SWAP_MULTIBYTE_VALUES=0,
		SWAPS_MULTIBYTE_VALUES=1
	};
	/**
	 * byte-spwaps a memory-region of size four bytes
	 * @param p a pointer to the memory-region to be swapped, ensure |p| >= 4
	 */
	static void Swap4(void *p);

	/**
	 * byte-spwaps a memory-region of size two bytes
	 * @param p a pointer to the memory-region to be swapped, ensure |p| >= 2
	 */
	static void Swap2(void *p);

	/**
	 * byte-spwaps a memory-region of size eight bytes
	 * @param p a pointer to the memory-region to be swapped, ensure |p| >= 8
	 */
	static void Swap8(void *p);

	/**
	 * byte-spwaps a memory-region, give a length argument with iLength.
	 * This is a convinience method that will call Swap8()/Swap4()/Swap2() with
	 * respect to iLength
	 * @param iLength either 2,4 or 8 everything else will be ignored IFF iLength is no power of 8,4 or 2
	 * @param p a pointer to the memory-region to be swapped, ensure |p| >= iLength
	 */
	static void Swap(void *p, VistaType::uint32 iLength);

	/**
	 * Byte-swaps a memory region of iCount tuples of length iLength bytes each.
	 * Total size of the memory region is iLength*iCount bytes.
	 * @param p           a pointer to the memory-region to be swapped, ensure |p| >= iLength
	 * @param iLength     number of bytes per tuple, has to be 2, 4 or 8
	 * @param iCount      number of tuples to be swapped
	 */
	static void Swap(void *p, VistaType::uint32 iLength, VistaType::uint32 iCount);


	/**
	 * little convinence method to inquire the endianess of this host. The method computes
	 * the numerical value of an int that is set as a byte-array and return the endianess
	 * as a result. This computation is done only ONCE; every subsequent call to
	 * this method will simply return a flag that is set with the first call.
	 */
	static eEndianess GetPlatformEndianess();

	/**
	 * @brief declarative macro to retrieve the platform bus width
	 *
	 * @return the number of bits for a void*
	 */
	static VistaType::byte GetPlatformBusWidth();

	static ByteOrderSwapBehavior GetDefaultPlatformSwapBehavior();
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEM_H

