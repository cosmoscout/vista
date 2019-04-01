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


#ifndef DLVISTACHECKERROR_H
#define DLVISTACHECKERROR_H


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <string>
#include <VistaInterProcComm/VistaInterProcCommConfig.h>

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAINTERPROCCOMMAPI DLVistaCheckError
{

private:

	std::string m_SErrorName, m_SErrorText;

protected:

	bool m_bIsNullError;


public:
	DLVistaCheckError(const std::string &SErrorName,
					   const std::string &SErrorText);

	/**
	 * Pure virtual destructor. Does nothing.
	 */
	virtual ~DLVistaCheckError();


	std::string GetErrorName() const { return m_SErrorName; };

	std::string GetErrorText() const { return m_SErrorText; };

	void SetErrorText(const std::string &SErrorText);

	bool IsNullError() const { return m_bIsNullError; };

};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


#endif //DLVISTAACTIVECOMPONENT_H


