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


#ifndef _VISTAVERSION_H
#define _VISTAVERSION_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "VistaBaseConfig.h"

/**
 * \mainpage
 * 	<div align="center">
 * 		<table border=0>
 * 			<tr>
 * 				<td align="right">API version:</td>
 * 				<td align="left"><b>1.16</b></td>
 * 			</tr>
 * 			<tr>
 * 				<td align="right">codename:</td>
 * 				<td align="left"><b>ViSTA HUBBLE</b></td>
 * 			</tr>
 * 			<tr>
 * 				<td align="right">API release date:</td>
 * 				<td align="left"><b>May 20th, 2016</b></td>
 * 			</tr>
 * 			<tr><td colspan=2>&nbsp;</td></tr>
 * 			<tr><td align="center" colspan=2><i>This is the ViSTA release 1.16.0, codename "HUBBLE".</i></td></tr>
 * 			<tr><td align="center" colspan=2><i>It is dedicated to the Hubble Space Telescope, one of the</i></td></tr>
 * 			<tr><td align="center" colspan=2><i>largest and most versatile space telescopes, observing in the</i></td></tr>
 * 			<tr><td align="center" colspan=2><i>infrared, visible and ultraviolet spectra.</i></td></tr>
 * 			<tr><td align="center" colspan=2><i>It entered service on May 20th, 1990.</i></td></tr>
 * 			<tr><td colspan=2>&nbsp;</td></tr>
 * 			<tr>
 * 				<td align="right">full version:</td>
 * 				<td align="left"><b>1.16.0</b></td>
 * 			</tr>
 * 			<tr>
 * 				<td align="right">release date:</td>
 * 				<td align="left"><b>May 20th, 2016</b></td>
 * 			</tr>
 * 		</table>
 * 	</div>
 */



#define VISTA_HEAD
#define VISTA_RELEASE_NAME "HEAD"
//#define VISTA_RELEASE_NAME "HUBBLE"
#define VISTA_VERSION "1.16.0"
#define VISTA_MAJOR    1
#define VISTA_MINOR    16
#define VISTA_REVISION 0

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTABASEAPI VistaVersion
{
public:
	static const char *GetReleaseName();
	static const char *GetVersion();
	static int GetMajor();
	static int GetMinor();
	static int GetRevision();
	static const char *GetBuildTimeString();
	static const char *GetBuildDateString();
};

#endif //_VISTAVERSION_H
