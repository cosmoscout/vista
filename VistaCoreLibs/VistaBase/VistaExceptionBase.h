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


#ifndef _VISTAEXCEPTIONSBASE_H
#define _VISTAEXCEPTIONSBASE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
#include "VistaBaseConfig.h"

#include <string>
#include <exception>


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTABASEAPI VistaExceptionBase : public virtual std::exception
{
public:
	VistaExceptionBase( const char* pcExMsg, const char* pcExSource,
						int iExLine, int iExNum ) throw();
	//VistaExceptionBase( const std::string& sExMsg, const std::string& sExSource,
	//					int iExLine, int iExNum ) throw();
	virtual ~VistaExceptionBase() throw();
	
	/// std::exception interface
	virtual const char* what() const throw();


	int         GetExceptionNumber() const;
	std::string GetExceptionText() const;
	int         GetExceptionLine() const;
	std::string GetExceptionSource() const;

	void        PrintException() const;
	void        PrintException( std::ostream& oStream, bool bAutoIndent = false ) const;
	std::string GetPrintStatement() const;
	std::string GetBacktraceString() const;
	void        PrintBacktrace() const;
protected:
	VistaExceptionBase();
private:
	std::string m_sExceptionText;
	std::string m_sExceptionSource;
	std::string m_sBacktrace;
	int         m_iExceptionLine;
	int         m_iExceptionNumber;
};

#define VISTA_THROW(MSG,EXITNUMBER) {VistaExceptionBase _rexX(MSG, __FILE__, __LINE__, EXITNUMBER); throw _rexX;}
#define VISTA_THROW_NOT_IMPLEMENTED {VistaExceptionBase _rexX( "FUNCTION NOT IMPLEMENTED", __FILE__, __LINE__, -1); throw _rexX;}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAEXCEPTIONSBASE_H
