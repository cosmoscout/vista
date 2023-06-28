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

#ifndef _VISTAKERNELMSGTAB_H
#define _VISTAKERNELMSGTAB_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

#define UNDEFINEDMSG 0xFFFF
#define SYSTEMMSG 0x0000
#define DISPLAYMSG 0x0001
#define DISPLAYSYSTEMMSG 0x0002
#define INTERACTIONMSG 0x0003
#define INTERACTIONDEVICEMSG 0x0004
#define INTERACTIONTRACKERSENSOR 0x0005
#define GRAPHICSMSG 0x0006
#define PICKMSG 0x0007
#define EVENTMSG 0x0008

// defines for SYSTEMMSG
#define ISCLUSTERLEADER 0x0001
#define ISCLUSTERFOLLOWER 0x0002
#define GETCLUSTERMODE 0x0003
#define GETINIFILE 0x0004
#define SETINIFILE 0x0005
#define GETVISTAVERSION 0x0006
#define DISCONNECT 0x0007
#define GETAPPLICATIONNAME 0x0008
#define GETOSTYPE 0x0009
#define GETNUMBEROFPROCESSORS 0x000A
#define GETCPUTYPE 0x000B
#define GETMEMORY 0x000C
#define QUITAPP 0x000D

// defines for DISPLAYMSG
#define GETROOT 0x0001
#define GETDISPLAYSYSTEM 0x0002
#define SETDISPLAYSYSTEM 0x0003

// defines for INTERACTIONMSG
#define GETAVAILABLETRACKER 0x0001
#define GETAVAILABLEDEVICES 0x0002
#define GETCURRENTTRACKER 0x0003
#define GETCURRENTDEVICES 0x0004
#define GETDEVICEBYROLE 0x0005

// defines for INTERACTIONDEVICEMSG
#define GETDEVICE 0x0001
#define SETDEVICE 0x0002

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAKERNELMSGTAB_H
