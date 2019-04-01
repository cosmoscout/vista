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


#ifndef _VISTAREADERWRITERLOCK_H
#define _VISTAREADERWRITERLOCK_H
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
//#include <pthread.h>

class VistaThreadCondition;
class VistaMutex;
class IVistaReaderWriterLockImp;

class VISTAINTERPROCCOMMAPI VistaReaderWriterLock
{

public:

	VistaReaderWriterLock();
	virtual ~VistaReaderWriterLock();

	bool ReaderLock();

	bool WriterLock();

	bool ReaderUnlock();

	bool WriterUnlock();

private:

		IVistaReaderWriterLockImp *m_pImp;
};
#endif //_VISTAREADERWRITERLOCK_H

