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


#include "VistaVersion.h"

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

const char *VistaVersion::GetReleaseName()
{ 
	return VISTA_RELEASE_NAME;
}

const char *VistaVersion::GetVersion() 
{ 
	return VISTA_VERSION; 
}

int VistaVersion::GetMajor() 
{ 
	return VISTA_MAJOR; 
}

int VistaVersion::GetMinor() 
{ 
	return VISTA_MINOR; 
}

int VistaVersion::GetRevision() 
{ 
	return VISTA_REVISION; 
}

const char *VistaVersion::GetBuildTimeString()
{
	return __TIME__;
}

const char *VistaVersion::GetBuildDateString()
{
	return __DATE__;
}

