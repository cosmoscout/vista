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

/*============================================================================*/
/* DEFINITIONS                                                                */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include "VistaFileSystemFile.h"

#include <VistaBase/VistaStreamUtils.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <fstream>
#ifdef WIN32
#include <io.h>
#else
#include <cstdio>
#endif

using namespace std;
/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

VistaFileSystemFile::VistaFileSystemFile(const string& file_name)
    : VistaFileSystemNode(file_name) {
}

VistaFileSystemFile::~VistaFileSystemFile() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

bool VistaFileSystemFile::Create() {
  if (Exists())
    return true;
  std::ofstream oStream(GetName().c_str());
  bool          bSuccess = oStream.good();
  oStream.close();
  return bSuccess;
}

bool VistaFileSystemFile::Delete() {
  vstr::outi() << "VistaFileSystemFile::Delete() -- deleting file [" << GetName() << "]"
               << std::endl;
  return (remove(GetName().c_str()) == 0) ? true : false;
}

bool VistaFileSystemFile::Exists() const {
#ifdef WIN32 // ##### WINDOWS VERSION ########################################

  struct _stat attributes;

  if (_stat(GetName().c_str(), &attributes) != 0)
    return false;

  if (attributes.st_mode & _S_IFREG)
    return true;
  else
    return false;

#else // ##### UNIX VERSION ############################################

  struct stat attributes;

  if (stat(GetName().c_str(), &attributes) != 0)
    return false;

  if (attributes.st_mode & S_IFREG)
    return true;
  else
    return false;

#endif
}

long VistaFileSystemFile::GetSize() {
#ifdef WIN32 // ##### WINDOWS VERSION ########################################

  struct _stat attributes;

  if (_stat(GetName().c_str(), &attributes) != 0)
    return 0;

  return ((long)attributes.st_size);

#else // ##### UNIX VERSION ############################################

  struct stat attributes;

  if (stat(GetName().c_str(), &attributes) != 0)
    return 0;

  return ((long)attributes.st_size);

#endif
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
