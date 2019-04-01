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


#ifndef IDLVISTAPIPECHECKER_H
#define IDLVISTAPIPECHECKER_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <list>
#include <VistaInterProcComm/VistaInterProcCommConfig.h>
/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class IDLVistaPipeComponent;
class DLVistaCheckError;
class IDLVistaRegistration;


/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaPipeChecker
{
public:
	DLVistaPipeChecker();
	/**
	 * Pure virtual destructor. Does nothing.
	 */
	virtual ~DLVistaPipeChecker();

	/**
	 * Starts the pipe-checking at the given entry point into the pipe.
	 * @return an error code (!=0) to state a failure, 0 iff this pipe seems to be ok.
	 */
	virtual DLVistaCheckError CheckPipe(IDLVistaPipeComponent *pBeginning, IDLVistaPipeComponent *pEnd) const;

protected:

	DLVistaCheckError CheckInboundReference(IDLVistaPipeComponent *pComp) const;
	DLVistaCheckError CheckOutboundReference(IDLVistaPipeComponent *pComp) const;

	DLVistaCheckError CheckPacketIntegrity(IDLVistaPipeComponent *pIn, IDLVistaPipeComponent *pOut) const;

	DLVistaCheckError CheckReachability(IDLVistaPipeComponent *pBegin, IDLVistaPipeComponent *pEnd) const;

private:
	IDLVistaRegistration *m_pPacketRegistration;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTAACTIVECOMPONENT_H


