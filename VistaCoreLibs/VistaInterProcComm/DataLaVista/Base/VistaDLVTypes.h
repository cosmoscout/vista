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


#if !defined(DLVISTATYPES_H)
#define DLVISTATYPES_H

#if !defined(DLV_INT64)
#if defined(WIN32)
#define DLV_INT64 __int64
#elif defined(LINUX) || defined(SUNOS) || defined(HPUX) || defined(IRIX) || defined(DARWIN)
#define DLV_INT64 long long int
#else
#error DLV_INT64 NOT defined for this platform.
#endif // WIN32
#endif // DLV_INT64

#if !defined(DLV_INT32)
#if defined(WIN32)
#define DLV_INT32 __int32
#elif defined(LINUX) || defined(SUNOS) || defined(HPUX) || defined(IRIX) || defined(DARWIN)
#define DLV_INT32 long
#else
#error DLV_INT64 NOT defined for this platform.
#endif // WIN32
#endif // DLV_INT32

#endif // DLVISTATYPES_H

