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


#ifndef _VISTATESTROTATIONS_H
#define _VISTATESTROTATIONS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <gtest/gtest.h>

#include <VistaBase/VistaQuaternion.h>

#include <VistaTestingUtils/VistaTestingCompare.h>

namespace VistaTestRotations
{
	const double s_a9dRotX180[] = { 1, 0, 0,
								0, -1, 0,
								0, 0, -1 };
	const VistaQuaternion s_qRotX180( VistaAxisAndAngle( Vista::XAxis, Vista::DegToRad( 180 ) ) );
	const double s_a9dRotXm90[] = { 1, 0, 0,
								0, 0, 1,
								0, -1, 0 };
	const VistaQuaternion s_qRotXm90( VistaAxisAndAngle( Vista::XAxis, Vista::DegToRad( -90 ) ) );
	const double s_a9dRotX25[] = { 1, 0, 0,
								0, 0.90630778703665, -0.422618261740699,
								0, 0.422618261740699, 0.90630778703665 };
	const VistaQuaternion s_qRotX25( VistaAxisAndAngle( Vista::XAxis, Vista::DegToRad( 25 ) ) );
	const double s_a9dRotY90[] = { 0, 0, 1,
								0, 1, 0,
								-1, 0, -0 };
	const VistaQuaternion s_qRotY90( VistaAxisAndAngle( Vista::YAxis, Vista::DegToRad( 90 ) ) );
	const double s_a9dRotYm45[] = { 0.707106781186548, 0, -0.707106781186548,
								0, 1, 0,
								0.707106781186548, 0, 0.707106781186548 };
	const VistaQuaternion s_qRotYm45( VistaAxisAndAngle( Vista::YAxis, Vista::DegToRad( -45 ) ) );
	const double s_a9dRotZ45[] = { 0.707106781186548, -0.707106781186548, 0,
								0.707106781186548, 0.707106781186548, 0,
								0, 0, 1 };
	const VistaQuaternion s_qRotZ45( VistaAxisAndAngle( Vista::ZAxis, Vista::DegToRad( 45 ) ) );
	const double s_a9dRotZm12[] = { 0.978147600733806,	0.207911690817759, 0,
								-0.207911690817759,	0.978147600733806, 0,
								0, 0, 1 };
	const VistaQuaternion s_qRotZm12( VistaAxisAndAngle( Vista::ZAxis, Vista::DegToRad( -12 ) ) );

	
	const double s_a9dRotY61Z42[] = { 0.365890464984075,	-0.874619707139396,	0.318063728704068,
									0.660083872029737,	0.484809620246337,	0.573802155801834,
									-0.656059028990507,	0,	0.754709580222772, };
	const VistaQuaternion s_qRotY61Z42 = VistaQuaternion( VistaAxisAndAngle( Vista::YAxis, Vista::DegToRad( 61 ) ) )
											* VistaQuaternion( VistaAxisAndAngle( Vista::ZAxis, Vista::DegToRad( 42 ) ) );

	const double s_a9dRotX32Ym78Zm3[] = { 0.207626755071376,	0.010881257153035,	0.978147600733806,
								-0.562012299007777,	0.819758095046477,	-0.110176410223629,
								-0.800645555999984,	0.572606552397492,	0.176319113566664 };
	const VistaQuaternion s_qRotX32Ym78Zm3 = VistaQuaternion( VistaAxisAndAngle( Vista::ZAxis, Vista::DegToRad( 32 ) ) )
											* VistaQuaternion( VistaAxisAndAngle( Vista::YAxis, Vista::DegToRad( -78 ) ) )
											* VistaQuaternion( VistaAxisAndAngle( Vista::XAxis, Vista::DegToRad( -3 ) ) );


	const double s_a9dRotZ13Xm56Y111[] = { -0.523289100964658,	-0.125791033217352,	0.842819751058144,
										0.673521118204854,	0.544860825582235,	-0.499495729789483,
										-0.522051549316428,	-0.829037572555042,	-0.200396814217095 };
	const VistaQuaternion s_qRotZ13Xm56Y111 = VistaQuaternion( VistaAxisAndAngle( Vista::YAxis, Vista::DegToRad( 13 ) ) )
											* VistaQuaternion( VistaAxisAndAngle( Vista::XAxis, Vista::DegToRad( -56 ) ) )
											* VistaQuaternion( VistaAxisAndAngle( Vista::ZAxis, Vista::DegToRad( 111 ) ) );

	double s_a16dTestMatrix1[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	double s_a16dTestMatrix2[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	double s_a16dTestMatrix3[16] = { 1e23, -0.2, 1e12, 2.2, 1, -2.3, 0.0, 0.1e12, 0.5, 42e23, 5.55, 0.0001, 0, 0, 0, 1 };
	double s_a16dTestMatrix4[16] = { 1, -1, 2, -2, 3, -4, 6, 13, 23, 42, 128, 666, 0, 0, 0.5, 1 };
}

#endif
