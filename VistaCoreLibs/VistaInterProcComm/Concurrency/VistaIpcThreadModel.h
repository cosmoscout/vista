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


#ifndef _VISTAIPCTHREADMODEL_H
#define _VISTAIPCTHREADMODEL_H


// set threadmodel flags, default to posix/pthreads
#if defined(WIN32)
#define VISTA_THREADING_WIN32
#elif defined(IRIX) && !defined(VISTA_THREADING_POSIX)
#define VISTA_THREADING_SPROC
#elif defined(SUNOS) || defined(HPUX) || defined(LINUX) || defined(IRIX) || defined(DARWIN)
#define VISTA_THREADING_POSIX
#else
#error "NO THREADMODEL FOR THIS PLATFORM DEFINED"
#endif

#endif //_VISTAIPCTHREADMODEL_H
