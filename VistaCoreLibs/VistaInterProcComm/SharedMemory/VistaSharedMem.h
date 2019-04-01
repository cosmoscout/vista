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


#ifndef _VISTASHAREDMEM_H
#define _VISTASHAREDMEM_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaInterProcComm/VistaInterProcCommConfig.h>
#include <string>

// ============================================================================
// ============================================================================
// ============================================================================
// ============================================================================
class VISTAINTERPROCCOMMAPI VistaSharedMemory
{
//METHODS
public:
	VistaSharedMemory();
	virtual ~VistaSharedMemory();

	// IMPLEMENTATION
	virtual  bool    CreateSharedMem  (std::string & sharedMem, long size = 0);
	virtual  void *  GetSharedMemPointer (std::string & sharedMem);
	virtual  bool    DestroySharedMem (std::string & sharedMem);
	virtual  bool    SetSharedMem (std::string & sharedMem, void * pBufferSrc,
									long size, long offset = 0);
	virtual  bool    GetSharedMem  (std::string & sharedMem, void * pBufferTar,
									long size, long offset = 0);
	virtual  bool    SetSharedMemSize (long size);
	virtual  long    GetSharedMemSize ();

protected:

// MEMBERS
private:
	std::string         m_strMapFileName;
	void *              m_pSharedMem;
	long                m_nSharedMemSize;
};

#endif // _VISTASHAREDMEM_H
