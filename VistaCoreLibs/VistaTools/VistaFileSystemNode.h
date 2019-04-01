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


#ifndef _VISTAFILESYSTEMNODE_H
#define _VISTAFILESYSTEMNODE_H

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

#include<string>

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Abstract superclass of all filesystem access classes.
 *
 * @DATE   July 2003
 *
 */
class VISTATOOLSAPI VistaFileSystemNode
{

public:

  enum FILE_TYPE
  {
	FT_UNKNOWN     = 0,
	FT_FILE        = 1,
	FT_DIRECTORY   = 2,
	FT_LINK        = 3
  };

public:

  //   VistaFileSystemNode();
   VistaFileSystemNode( const std::string& sNodeName );
   virtual ~VistaFileSystemNode();


   /****** interface methods *************************************************/

   /**
	* Creates a new node
	*/
   virtual bool Create() = 0;

   /**
    * Creates the node, as well as all parent directories that dont exist yet
    */
   bool CreateWithParentDirectories();

   /**
	* Deletes this node
	*/
   virtual bool Delete() = 0;


   /**
	* Checks for existing of a node using operating system functions
	*/
   virtual bool Exists() const = 0;


   /**
	* Checks if node is readonly
	*   (different in Windows for files and directories, so handle it by
	*    more specific classes)
	*/
   virtual bool IsReadOnly() const;


   /**
	* Returns node type as enum FILE_TYPE (see above)
	*/
   FILE_TYPE GetType() const;

   /**
	* Gets size of Node (file or directory)
	*/
   virtual long GetSize() = 0;

   /**
	* Gets date of creation of node
	*/
   virtual double GetCreationDate();


   /**
	* Gets date of last access to node
	*/
   virtual double GetLastAccessDate();

   /**
	* Gets date of last modify of node
	*/
   virtual double GetLastModifiedDate();

   /***** final methods ******************************************************/


   std::string GetName() const;   // relativ oder absolut oder ... ?
   std::string GetLocalName() const; // just the name, without prefix
   std::string GetParentDirectory() const; // Parent directory without preceeding "/"

   void SetName(const std::string &strName);

   bool IsDirectory() const;
   bool IsFile() const;

private:
	std::string m_sName;
	std::string m_sLocalName;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAFILESYSTEMNODE_H

