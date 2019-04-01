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


#ifndef _VISTAFILESYSTEMFILE_H
#define _VISTAFILESYSTEMFILE_H

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/


/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaToolsConfig.h"

#include "VistaFileSystemNode.h"

#include <iostream>
#include <string>
#include <vector>

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Encapsulates simple access to a file on the filesystem on various
 * operating systems
 * @DATE   July 2003
 *
 */

class VISTATOOLSAPI VistaFileSystemFile : public VistaFileSystemNode
{

public:

	//VistaFileSystemFile();
	explicit VistaFileSystemFile( const std::string& sFileName) ;
	virtual ~VistaFileSystemFile();


	/* extended functionality */
	virtual bool Create();
	virtual bool Delete();
	virtual bool Exists() const;

	/* Gets size of file
	*
	*/
	virtual long GetSize();
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAFILESYSTEMFILE_H


