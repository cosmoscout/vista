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




#include <gtest/gtest.h>

#include <VistaBase/VistaVector3D.h>

#include <VistaTestingUtils/VistaTestingCompare.h>

/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

TEST( VistaVector3DTest, CtorDefault )
{
	VistaVector3D v3Test;
	ASSERT_EQ( v3Test[Vista::X], 0 );
	ASSERT_EQ( v3Test[Vista::Y], 0 );
	ASSERT_EQ( v3Test[Vista::Z], 0 );
	ASSERT_EQ( v3Test[Vista::W], 1 );
}

TEST( VistaVector3DTest, CtorThreeFloats )
{
	VistaVector3D v3Test( 1, 2, 3 );
	ASSERT_EQ( v3Test[Vista::X], 1 );
	ASSERT_EQ( v3Test[Vista::Y], 2 );
	ASSERT_EQ( v3Test[Vista::Z], 3 );
	ASSERT_EQ( v3Test[Vista::W], 1 );

	VistaVector3D v3Test2( 4.74f, 2.53e-17f, -1.9e32f );
	ASSERT_EQ( v3Test2[Vista::X], 4.74f );
	ASSERT_EQ( v3Test2[Vista::Y], 2.53e-17f );
	ASSERT_EQ( v3Test2[Vista::Z], -1.9e32f );
	ASSERT_EQ( v3Test2[Vista::W], 1 );
}

TEST( VistaVector3DTest, CtorFourFloats )
{
	VistaVector3D v3Test1( 6, -5, 1, 0 );
	ASSERT_EQ( v3Test1[Vista::X], 6 );
	ASSERT_EQ( v3Test1[Vista::Y], -5 );
	ASSERT_EQ( v3Test1[Vista::Z], 1 );
	ASSERT_EQ( v3Test1[Vista::W], 0 );

	VistaVector3D v3Test2( 0.0f, 4.3e12f, -5.3f, 1.0f );
	ASSERT_EQ( v3Test2[Vista::X], 0.0f );
	ASSERT_EQ( v3Test2[Vista::Y], 4.3e12f );
	ASSERT_EQ( v3Test2[Vista::Z], -5.3f );
	ASSERT_EQ( v3Test2[Vista::W], 1.0f );

	VistaVector3D v3Test3( 1, 2, 3, -0.5f );
	ASSERT_EQ( v3Test3[Vista::X], 1 );
	ASSERT_EQ( v3Test3[Vista::Y], 2 );
	ASSERT_EQ( v3Test3[Vista::Z], 3 );
	ASSERT_EQ( v3Test3[Vista::W], -0.5f );

	VistaVector3D v3Test4( -4, 2.4f, 11.0f, 0.3e-12f );
	ASSERT_EQ( v3Test4[Vista::X], -4 );
	ASSERT_EQ( v3Test4[Vista::Y], 2.4f );
	ASSERT_EQ( v3Test4[Vista::Z], 11.0f );
	ASSERT_EQ( v3Test4[Vista::W], 0.3e-12f );
}

TEST( VistaVector3DTest, CtorFloatArray )
{
	float aArray1[] = { 0.0f, -4.73e12f, 2754.3f };
	VistaVector3D v3Test1( aArray1 );
	ASSERT_EQ( v3Test1[Vista::X], aArray1[Vista::X] );
	ASSERT_EQ( v3Test1[Vista::Y], aArray1[Vista::Y] );
	ASSERT_EQ( v3Test1[Vista::Z], aArray1[Vista::Z] );
	ASSERT_EQ( v3Test1[Vista::W], 1.0f );

	float aArray2[] = { -1.4e-17f, 275.4f, 0.00001f, 1274.32f };
	VistaVector3D v3Test2( aArray2 );
	ASSERT_EQ( v3Test2[Vista::X], aArray2[Vista::X] );
	ASSERT_EQ( v3Test2[Vista::Y], aArray2[Vista::Y] );
	ASSERT_EQ( v3Test2[Vista::Z], aArray2[Vista::Z] );
	ASSERT_EQ( v3Test2[Vista::W], 1.0f );
}

TEST( VistaVector3DTest, CtorDoubleArray )
{
	double aArray1[] = { 0.0, -4.73e12, 2754.3 };
	VistaVector3D v3Test1( aArray1 );
	ASSERT_EQ( v3Test1[Vista::X], (float)aArray1[Vista::X] );
	ASSERT_EQ( v3Test1[Vista::Y], (float)aArray1[Vista::Y] );
	ASSERT_EQ( v3Test1[Vista::Z], (float)aArray1[Vista::Z] );
	ASSERT_EQ( v3Test1[Vista::W],  1.0f );

	double aArray2[] = { -1.4e-17, 275.4, 0.00001, 1274.32 };
	VistaVector3D v3Test2( aArray2 );
	ASSERT_EQ( v3Test2[Vista::X], (float)aArray2[Vista::X] );
	ASSERT_EQ( v3Test2[Vista::Y], (float)aArray2[Vista::Y] );
	ASSERT_EQ( v3Test2[Vista::Z], (float)aArray2[Vista::Z] );
	ASSERT_EQ( v3Test2[Vista::W], 1.0f );
}

TEST( VistaVector3DTest, CtorCopy )
{
	VistaVector3D v3Test1( 1, 2, 3 );
	VistaVector3D v3Copy1( v3Test1 );
	ASSERT_EQ( v3Test1[Vista::X], v3Copy1[Vista::X] );
	ASSERT_EQ( v3Test1[Vista::Y], v3Copy1[Vista::Y] );
	ASSERT_EQ( v3Test1[Vista::Z], v3Copy1[Vista::Z] );
	ASSERT_EQ( v3Test1[Vista::W], v3Copy1[Vista::W] );

	VistaVector3D v3Test2( -2, 2, -6, -204.7f );
	VistaVector3D v3Copy2( v3Test2 );
	ASSERT_EQ( v3Test2[Vista::X], v3Copy2[Vista::X] );
	ASSERT_EQ( v3Test2[Vista::Y], v3Copy2[Vista::Y] );
	ASSERT_EQ( v3Test2[Vista::Z], v3Copy2[Vista::Z] );
	ASSERT_EQ( v3Test2[Vista::W], v3Copy2[Vista::W] );
}


TEST( VistaVector3DTest, SetGetValuesFloatArray )
{
	VistaVector3D v3Test;

	float aArray1[] = { 0.0f, -4.73e12f, 2754.3f };
	v3Test.SetValues( aArray1 );
	ASSERT_EQ( v3Test[Vista::X], aArray1[Vista::X] );
	ASSERT_EQ( v3Test[Vista::Y], aArray1[Vista::Y] );
	ASSERT_EQ( v3Test[Vista::Z], aArray1[Vista::Z] );
	ASSERT_EQ( v3Test[Vista::W], 1.0f );
	
	float aArrayCheck1[] = { -1.0f, -2.0f, -3.0f, -4.0f };
	v3Test.GetValues( aArrayCheck1 );
	ASSERT_EQ( aArrayCheck1[Vista::X], aArray1[Vista::X] );
	ASSERT_EQ( aArrayCheck1[Vista::Y], aArray1[Vista::Y] );
	ASSERT_EQ( aArrayCheck1[Vista::Z], aArray1[Vista::Z] );
	ASSERT_EQ( aArrayCheck1[Vista::W], -4.0f );

	float aArray2[] = { -1.4e-17f, 275.4f, 0.00001f, 1274.32f };
	v3Test.SetValues( aArray2 );
	ASSERT_EQ( v3Test[Vista::X], aArray2[Vista::X] );
	ASSERT_EQ( v3Test[Vista::Y], aArray2[Vista::Y] );
	ASSERT_EQ( v3Test[Vista::Z], aArray2[Vista::Z] );
	ASSERT_EQ( v3Test[Vista::W], 1.0f );

	float aArrayCheck2[] = { -1.0f, -2.0f, -3.0f, -4.0f };
	v3Test.GetValues( aArrayCheck2 );
	ASSERT_EQ( aArrayCheck2[Vista::X], aArray2[Vista::X] );
	ASSERT_EQ( aArrayCheck2[Vista::Y], aArray2[Vista::Y] );
	ASSERT_EQ( aArrayCheck2[Vista::Z], aArray2[Vista::Z] );
	ASSERT_EQ( aArrayCheck2[Vista::W], -4.0f );
}

TEST( VistaVector3DTest, SetGetValuesDoubleArray )
{
	VistaVector3D v3Test;

	double aArray1[] = { 0.0, -4.73e12, 2754.3 };
	v3Test.SetValues( aArray1 );
	ASSERT_EQ( v3Test[Vista::X], (float)aArray1[Vista::X] );
	ASSERT_EQ( v3Test[Vista::Y], (float)aArray1[Vista::Y] );
	ASSERT_EQ( v3Test[Vista::Z], (float)aArray1[Vista::Z] );
	ASSERT_EQ( v3Test[Vista::W],  1.0f );
	
	double aArrayCheck1[] = { -1.0, -2.0, -3.0, -4.0 };
	v3Test.GetValues( aArrayCheck1 );
	ASSERT_EQ( (float)aArrayCheck1[Vista::X], (float)aArray1[Vista::X] );
	ASSERT_EQ( (float)aArrayCheck1[Vista::Y], (float)aArray1[Vista::Y] );
	ASSERT_EQ( (float)aArrayCheck1[Vista::Z], (float)aArray1[Vista::Z] );
	ASSERT_EQ( (float)aArrayCheck1[Vista::W], -4.0f );

	double aArray2[] = { -1.4e-17, 275.4, 0.00001, 1274.32 };
	v3Test.SetValues( aArray2 );
	ASSERT_EQ( v3Test[Vista::X], (float)aArray2[Vista::X] );
	ASSERT_EQ( v3Test[Vista::Y], (float)aArray2[Vista::Y] );
	ASSERT_EQ( v3Test[Vista::Z], (float)aArray2[Vista::Z] );
	ASSERT_EQ( v3Test[Vista::W], 1.0f );
	
	double aArrayCheck2[] = { -1.0, -2.0, -3.0, -4.0 };
	v3Test.GetValues( aArrayCheck2 );
	ASSERT_EQ( (float)aArrayCheck2[Vista::X], (float)aArray2[Vista::X] );
	ASSERT_EQ( (float)aArrayCheck2[Vista::Y], (float)aArray2[Vista::Y] );
	ASSERT_EQ( (float)aArrayCheck2[Vista::Z], (float)aArray2[Vista::Z] );
	ASSERT_EQ( (float)aArrayCheck2[Vista::W], -4.0f );
}

TEST( VistaVector3DTest, SetGetValuesFloat )
{
	VistaVector3D v3Test;

	v3Test.SetValues( 0.0f, 0.75e12f, 1.43e-12f );
	ASSERT_EQ( v3Test[Vista::X], 0.0f );
	ASSERT_EQ( v3Test[Vista::Y], 0.75e12f );
	ASSERT_EQ( v3Test[Vista::Z], 1.43e-12f );
	ASSERT_EQ( v3Test[Vista::W],  1.0f );

	float nCheck1X, nCheck1Y, nCheck1Z;
	v3Test.GetValues( nCheck1X, nCheck1Y, nCheck1Z );
	ASSERT_EQ( nCheck1X, 0.0f );
	ASSERT_EQ( nCheck1Y, 0.75e12f );
	ASSERT_EQ( nCheck1Z, 1.43e-12f );

	v3Test.SetValues( -4.32e21f, 0.00070008f, 1.23e-32f, 0.8f );
	ASSERT_EQ( v3Test[Vista::X], -4.32e21f );
	ASSERT_EQ( v3Test[Vista::Y], 0.00070008f );
	ASSERT_EQ( v3Test[Vista::Z], 1.23e-32f );
	ASSERT_EQ( v3Test[Vista::W], 0.8f );

	float nCheck2X, nCheck2Y, nCheck2Z, nCheck2W;
	v3Test.GetValues( nCheck2X, nCheck2Y, nCheck2Z, nCheck2W );
	ASSERT_EQ( nCheck2X, -4.32e21f );
	ASSERT_EQ( nCheck2Y, 0.00070008f );
	ASSERT_EQ( nCheck2Z, 1.23e-32f );
	ASSERT_EQ( nCheck2W, 0.8f );
}

TEST( VistaVector3DTest, SetGetValuesDoubles )
{
	VistaVector3D v3Test;

	v3Test.SetValues( 0.0, 0.75e12, 1.43e-12 );
	ASSERT_EQ( v3Test[Vista::X], (float)0.0 );
	ASSERT_EQ( v3Test[Vista::Y], (float)0.75e12 );
	ASSERT_EQ( v3Test[Vista::Z], (float)1.43e-12 );
	ASSERT_EQ( v3Test[Vista::W], (float)1.0 );

	double nCheck1X, nCheck1Y, nCheck1Z;
	v3Test.GetValues( nCheck1X, nCheck1Y, nCheck1Z );
	ASSERT_EQ( nCheck1X, (float)0.0 );
	ASSERT_EQ( nCheck1Y, (float)0.75e12 );
	ASSERT_EQ( nCheck1Z, (float)1.43e-12 );

	v3Test.SetValues( -4.32e21, 0.00070008, 1.23e-32, 0.8 );
	ASSERT_EQ( v3Test[Vista::X], (float)-4.32e21 );
	ASSERT_EQ( v3Test[Vista::Y], (float)0.00070008 );
	ASSERT_EQ( v3Test[Vista::Z], (float)1.23e-32 );
	ASSERT_EQ( v3Test[Vista::W], (float)0.8 );

	double nCheck2X, nCheck2Y, nCheck2Z, nCheck2W;
	v3Test.GetValues( nCheck2X, nCheck2Y, nCheck2Z, nCheck2W );
	ASSERT_EQ( nCheck2X, (float)-4.32e21 );
	ASSERT_EQ( nCheck2Y, (float)0.00070008 );
	ASSERT_EQ( nCheck2Z, (float)1.23e-32 );
	ASSERT_EQ( nCheck2W, (float)0.8 );
}

TEST( VistaVector3DTest, GetLength )
{
	VistaVector3D v3Test1( 1.0f, 0.0f, 0.0f, 0.6f );
	ASSERT_FLOAT_EQ( v3Test1.GetLength(), 1.0f );

	VistaVector3D v3Test2( 0.0f, -0.2f, 0.0f, 0.0f );
	ASSERT_FLOAT_EQ( v3Test2.GetLength(), 0.2f );

	VistaVector3D v3Test3( 0.1f, -0.1f, 0.1f, 1.0f );
	ASSERT_FLOAT_EQ( v3Test3.GetLength(), sqrt( 0.03f ) );	
}
TEST( VistaVector3DTest, GetLengthSquared )
{
	VistaVector3D v3Test1( 1.0f, 0.0f, 0.0f, 0.6f );
	ASSERT_FLOAT_EQ( v3Test1.GetLengthSquared(), 1.0f );

	VistaVector3D v3Test2( 0.0f, -0.2f, 0.0f, 0.0f );
	ASSERT_FLOAT_EQ( v3Test2.GetLengthSquared(), 0.04f );

	VistaVector3D v3Test3( 0.1f, -0.1f, 0.1f, 1.0f );
	ASSERT_FLOAT_EQ( v3Test3.GetLengthSquared(), 0.03f );	
}

TEST( VistaVector3DTest, Normalize )
{
	VistaVector3D v3Test1( 0.5f, 0.3f, 0.6f, 0.2f );
	v3Test1.Normalize();
	ASSERT_FLOAT_EQ( v3Test1.GetLength(), 1.0f );

	VistaVector3D v3Test2( -1.0f, 65833e13f, -512e-11f );
	v3Test2.Normalize();
	ASSERT_FLOAT_EQ( v3Test2.GetLength(), 1.0f );

	VistaVector3D v3Test3( 0, 0, 0 );
	v3Test3.Normalize();
	ASSERT_FLOAT_EQ( v3Test3.GetLength(), 0.0f );
}

TEST( VistaVector3DTest, GetNormalized )
{
	VistaVector3D v3Test1( 0.5f, 0.3f, 0.6f, 0.2f );
	VistaVector3D v3TestC1 = v3Test1.GetNormalized();
	ASSERT_FLOAT_EQ( v3TestC1.GetLength(), 1.0f );

	VistaVector3D v3Test2( -1.0f, 65833e13f, -512e-11f );
	VistaVector3D v3TestC2 = v3Test2.GetNormalized();
	ASSERT_FLOAT_EQ( v3TestC2.GetLength(), 1.0f );

	VistaVector3D v3Test3( 0, 0, 0 );
	VistaVector3D v3TestC3 = v3Test3.GetNormalized();
	ASSERT_FLOAT_EQ( v3TestC3.GetLength(), 0.0f );
}

TEST( VistaVector3DTest, Homogenize )
{
	VistaVector3D v3Test1( 0.5f, 0.3f, 0.6f, 1.0f );
	v3Test1.Homogenize();
	ASSERT_EQ( v3Test1[Vista::X], 0.5f );
	ASSERT_EQ( v3Test1[Vista::Y], 0.3f );
	ASSERT_EQ( v3Test1[Vista::Z], 0.6f );
	ASSERT_EQ( v3Test1[Vista::W], 1.0f );

	VistaVector3D v3Test2( -1.0f, 65833e13f, -512e-11f, 0.0f );
	v3Test2.Homogenize();
	ASSERT_EQ( v3Test2[Vista::X], -1.0f );
	ASSERT_EQ( v3Test2[Vista::Y], 65833e13f );
	ASSERT_EQ( v3Test2[Vista::Z], -512e-11f );
	ASSERT_EQ( v3Test2[Vista::W], 0.0f );

	VistaVector3D v3Test3( 0.5f, 0.3f, 0.6f, 0.1f );
	v3Test3.Homogenize();
	ASSERT_EQ( v3Test3[Vista::X], 0.5f / 0.1f );
	ASSERT_EQ( v3Test3[Vista::Y], 0.3f / 0.1f );
	ASSERT_EQ( v3Test3[Vista::Z], 0.6f / 0.1f );
	ASSERT_EQ( v3Test3[Vista::W], 1.0f );

	VistaVector3D v3Test4( -1.0f, 65833e13f, -512e-11f, 234.1f );
	v3Test4.Homogenize();
	ASSERT_EQ( v3Test4[Vista::X], -1.0f / 234.1f );
	ASSERT_EQ( v3Test4[Vista::Y], 65833e13f / 234.1f );
	ASSERT_EQ( v3Test4[Vista::Z], -512e-11f / 234.1f );
	ASSERT_EQ( v3Test4[Vista::W], 1.0f );
}

TEST( VistaVector3DTest, GetHomogenized )
{
	VistaVector3D v3Test1( 0.5f, 0.3f, 0.6f, 1.0f );
	v3Test1 = v3Test1.GetHomogenized();
	ASSERT_EQ( v3Test1[Vista::X], 0.5f );
	ASSERT_EQ( v3Test1[Vista::Y], 0.3f );
	ASSERT_EQ( v3Test1[Vista::Z], 0.6f );
	ASSERT_EQ( v3Test1[Vista::W], 1.0f );

	VistaVector3D v3Test2( -1.0f, 65833e13f, -512e-11f, 0.0f );
	v3Test2 = v3Test2.GetHomogenized();
	ASSERT_EQ( v3Test2[Vista::X], -1.0f );
	ASSERT_EQ( v3Test2[Vista::Y], 65833e13f );
	ASSERT_EQ( v3Test2[Vista::Z], -512e-11f );
	ASSERT_EQ( v3Test2[Vista::W], 0.0f );

	VistaVector3D v3Test3( 0.5f, 0.3f, 0.6f, 0.1f );
	v3Test3 = v3Test3.GetHomogenized();
	ASSERT_EQ( v3Test3[Vista::X], 0.5f / 0.1f );
	ASSERT_EQ( v3Test3[Vista::Y], 0.3f / 0.1f );
	ASSERT_EQ( v3Test3[Vista::Z], 0.6f / 0.1f );
	ASSERT_EQ( v3Test3[Vista::W], 1.0f );

	VistaVector3D v3Test4( -1.0f, 65833e13f, -512e-11f, 234.1f );
	v3Test4 = v3Test4.GetHomogenized();
	ASSERT_EQ( v3Test4[Vista::X], -1.0f / 234.1f );
	ASSERT_EQ( v3Test4[Vista::Y], 65833e13f / 234.1f );
	ASSERT_EQ( v3Test4[Vista::Z], -512e-11f / 234.1f );
	ASSERT_EQ( v3Test4[Vista::W], 1.0f );
}

TEST( VistaVector3DTest, GetAbsolute )
{
	VistaVector3D v3Test1( 0.5f, -0.3e12f, -0.6e-12f );
	v3Test1 = v3Test1.GetAbsolute();
	ASSERT_EQ( v3Test1[Vista::X], 0.5f );
	ASSERT_EQ( v3Test1[Vista::Y], 0.3e12f );
	ASSERT_EQ( v3Test1[Vista::Z], 0.6e-12f );
	ASSERT_EQ( v3Test1[Vista::W], 1.0f );

	VistaVector3D v3Test2( -1, 2, -3, -0.4f );
	v3Test2 = v3Test2.GetAbsolute();
	ASSERT_EQ( v3Test2[Vista::X], 1 );
	ASSERT_EQ( v3Test2[Vista::Y], 2 );
	ASSERT_EQ( v3Test2[Vista::Z], 3 );
	ASSERT_EQ( v3Test2[Vista::W], 0.4f );
}

TEST( VistaVector3DTest, SetToZeroVector )
{
	VistaVector3D v3Test1( 1, 2, 3, 0.4f );
	v3Test1.SetToZeroVector();
	ASSERT_EQ( v3Test1[Vista::X], 0 );
	ASSERT_EQ( v3Test1[Vista::Y], 0 );
	ASSERT_EQ( v3Test1[Vista::Z], 0 );
	ASSERT_EQ( v3Test1[Vista::W], 1 );
}

TEST( VistaVector3DTest, GetIsZeroVector )
{
	VistaVector3D v3Test;
	ASSERT_TRUE( v3Test.GetIsZeroVector() );
	v3Test = VistaVector3D( 0, 0, 0, 0 );
	ASSERT_TRUE( v3Test.GetIsZeroVector() );
	v3Test = VistaVector3D( 1, 0, 0, 1 );
	ASSERT_FALSE( v3Test.GetIsZeroVector() );
	v3Test = VistaVector3D( 0, 1, 0, 1 );
	ASSERT_FALSE( v3Test.GetIsZeroVector() );
	v3Test = VistaVector3D( 0, 0, 1, 0 );
	ASSERT_FALSE( v3Test.GetIsZeroVector() );
	v3Test.SetToZeroVector();
	ASSERT_TRUE( v3Test.GetIsZeroVector() );
}

TEST( VistaVector3DTest, Interpolate )
{
	VistaVector3D v3Point1( 1, 2, 3 );
	VistaVector3D v3Point2( 0, -2, 5 );
	VistaVector3D v3Res = v3Point1.Interpolate( v3Point2, 0.25f );
	ASSERT_EQ( v3Res[Vista::X], 0.75f );
	ASSERT_EQ( v3Res[Vista::Y], 1.0f );
	ASSERT_EQ( v3Res[Vista::Z], 3.5f );
	ASSERT_EQ( v3Res[Vista::W], 1 );
}

TEST( VistaVector3DTest, Dot )
{
	VistaVector3D v3Point1( 1, 2, 3 );
	VistaVector3D v3Point2( 0, -2, 5 );
	ASSERT_FLOAT_EQ( v3Point1.Dot( v3Point2 ), 11.0f );

	v3Point1.SetToZeroVector();
	ASSERT_FLOAT_EQ( v3Point1.Dot( v3Point2 ), 0 );

	v3Point1 = VistaVector3D( 0, 1, 0 );
	ASSERT_FLOAT_EQ( v3Point1.Dot( v3Point2 ), -2 );
}

TEST( VistaVector3DTest, Cross )
{
	VistaVector3D v3Point1( 1, 0, 0 );
	VistaVector3D v3Point2( 0, 1, 0 );
	VistaVector3D v3Res = v3Point1.Cross( v3Point2 );
	ASSERT_FLOAT_EQ( v3Res[Vista::X], 0 );
	ASSERT_FLOAT_EQ( v3Res[Vista::Y], 0 );
	ASSERT_FLOAT_EQ( v3Res[Vista::Z], 1 );
	ASSERT_FLOAT_EQ( v3Res[Vista::W], 1 );

	v3Point1 = VistaVector3D( 0.2f, 0, 0 );
	v3Res = v3Point2.Cross( v3Point1 );
	ASSERT_FLOAT_EQ( v3Res[Vista::X], 0 );
	ASSERT_FLOAT_EQ( v3Res[Vista::Y], 0 );
	ASSERT_FLOAT_EQ( v3Res[Vista::Z], -0.2f );
	ASSERT_FLOAT_EQ( v3Res[Vista::W], 1 );

	v3Point1 = VistaVector3D( 1.43f, -2.87f, 3.5e-4f );
	v3Point2 = VistaVector3D( -15.555f, 6.2f, 0.1f );
	v3Res = v3Point1.Cross( v3Point2 );
	ASSERT_FLOAT_EQ( v3Res[Vista::X], -0.28917f );
	ASSERT_FLOAT_EQ( v3Res[Vista::Y], -0.1484442f );
	ASSERT_FLOAT_EQ( v3Res[Vista::Z], -35.77685f );
	ASSERT_EQ( v3Res[Vista::W], 1 );

	v3Point1 = VistaVector3D( -2.32f, 1.45f, 32.2f );
	v3Point2 = VistaVector3D( -5.32f, 0.01f, 0.97f );
	v3Res = v3Point1.Cross( v3Point2 );
	ASSERT_FLOAT_EQ( v3Res[Vista::X], 1.0845f );
	ASSERT_FLOAT_EQ( v3Res[Vista::Y], -169.0536f );
	ASSERT_FLOAT_EQ( v3Res[Vista::Z], 7.6908f );
	ASSERT_EQ( v3Res[Vista::W], 1 );
}

TEST( VistaVector3DTest, CheckForValidity )
{
	VistaVector3D v3Test( 1, 2, 3 );
	ASSERT_TRUE( v3Test.CheckForValidity() );
	v3Test[Vista::X] /= 0;
	ASSERT_FALSE( v3Test.CheckForValidity() );
	v3Test.SetToZeroVector();
	ASSERT_TRUE( v3Test.CheckForValidity() );
	v3Test[Vista::Z] = std::numeric_limits<float>::infinity();
	ASSERT_FALSE( v3Test.CheckForValidity() );
	v3Test.SetToZeroVector();
	v3Test[Vista::X] = std::numeric_limits<float>::quiet_NaN();
	ASSERT_FALSE( v3Test.CheckForValidity() );
	v3Test.SetToZeroVector();
	v3Test[Vista::W] = std::numeric_limits<float>::signaling_NaN();
	ASSERT_FALSE( v3Test.CheckForValidity() );
}

TEST( VistaVector3DTest, OperatorAssign )
{
	VistaVector3D v3Test1( 1, 2, 3 );
	VistaVector3D v3Copy1 = v3Test1;
	ASSERT_TRUE( VistaTesting::Compare( v3Test1, v3Copy1 ) );

	VistaVector3D v3Test2( -2, 2, -6, -204.7f );
	VistaVector3D v3Copy2 = v3Test2;
	ASSERT_TRUE( VistaTesting::Compare( v3Test2, v3Copy2 ) );
}

TEST( VistaVector3DTest, OperatorAdd )
{
	VistaVector3D v3Add1, v3Add2, v3Res, v3Expected;

	v3Add1 = VistaVector3D( 4, 2, 1 );
	v3Add2 = VistaVector3D( -4, 2, 21 );
	v3Res = v3Add1 + v3Add2;
	v3Expected = VistaVector3D( 0, 4, 22 );
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected, std::numeric_limits<float>::epsilon() ) );
	v3Res = v3Add2 + v3Add1;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected, std::numeric_limits<float>::epsilon() ) );

	v3Add1 = VistaVector3D( 4, 2, 1 );
	v3Add2 = VistaVector3D( 0, 0, 0 );
	v3Res = v3Add1 + v3Add2;
	v3Expected = VistaVector3D( 4, 2, 1 );
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected, std::numeric_limits<float>::epsilon() ) );
}
TEST( VistaVector3DTest, OperatorCompoundAdd )
{
	VistaVector3D v3Add1, v3Add2, v3Expected;

	v3Add1 = VistaVector3D( 4, 2, 1 );
	v3Add2 = VistaVector3D( -4, 2, 21 );
	v3Add1 += v3Add2;
	v3Expected = VistaVector3D( 0, 4, 22 );
	ASSERT_TRUE( VistaTesting::Compare( v3Add1, v3Expected, std::numeric_limits<float>::epsilon() ) );

	v3Add1 = VistaVector3D( 4, 2, 1 );
	v3Add2 = VistaVector3D( -4, 2, 21 );
	v3Add2 += v3Add1;
	ASSERT_TRUE( VistaTesting::Compare( v3Add2, v3Expected, std::numeric_limits<float>::epsilon() ) );

	v3Add1 = VistaVector3D( 4, 2, 1 );
	v3Add2 = VistaVector3D( 0, 0, 0 );
	v3Add1 += v3Add2;
	v3Expected = VistaVector3D( 4, 2, 1 );
	ASSERT_TRUE( VistaTesting::Compare( v3Add1, v3Expected, std::numeric_limits<float>::epsilon() ) );
}

TEST( VistaVector3DTest, OperatorSub )
{
	VistaVector3D v3Sub1, v3Sub2, v3Res, v3Expected;

	v3Sub1 = VistaVector3D( 4, 2, 1 );
	v3Sub2 = VistaVector3D( -4, 2, 21 );
	v3Res = v3Sub1 - v3Sub2;
	v3Expected = VistaVector3D( 8, 0, -20 );
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected, std::numeric_limits<float>::epsilon() ) );
	v3Res = v3Sub2 - v3Sub1;
	v3Expected = VistaVector3D( -8, 0, 20 );
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected, std::numeric_limits<float>::epsilon() ) );

	v3Sub1 = VistaVector3D( 4, 2, 1 );
	v3Sub2 = VistaVector3D( 0, 0, 0 );
	v3Res = v3Sub1 - v3Sub2;
	v3Expected = VistaVector3D( 4, 2, 1 );
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected, std::numeric_limits<float>::epsilon() ) );
}
TEST( VistaVector3DTest, OperatorCompoundSub )
{
	VistaVector3D v3Sub1, v3Sub2, v3Expected;

	v3Sub1 = VistaVector3D( 4, 2, 1 );
	v3Sub2 = VistaVector3D( -4, 2, 21 );
	v3Sub1 -= v3Sub2;
	v3Expected = VistaVector3D( 8, 0, -20 );
	ASSERT_TRUE( VistaTesting::Compare( v3Sub1, v3Expected, std::numeric_limits<float>::epsilon() ) );

	v3Sub1 = VistaVector3D( 4, 2, 1 );
	v3Sub2 = VistaVector3D( -4, 2, 21 );
	v3Sub2 -= v3Sub1;
	v3Expected = VistaVector3D( -8, 0, 20 );
	ASSERT_TRUE( VistaTesting::Compare( v3Sub2, v3Expected, std::numeric_limits<float>::epsilon() ) );

	v3Sub1 = VistaVector3D( 4, 2, 1 );
	v3Sub2 = VistaVector3D( 0, 0, 0 );
	v3Sub1 -= v3Sub2;
	v3Expected = VistaVector3D( 4, 2, 1 );
	ASSERT_TRUE( VistaTesting::Compare( v3Sub1, v3Expected, std::numeric_limits<float>::epsilon() ) );
}

TEST( VistaVector3DTest, OperatorNegate )
{
	VistaVector3D v3Test, v3Expected;
	ASSERT_TRUE( ( -v3Test ).GetIsZeroVector() );

	v3Test = VistaVector3D( 1, -2, 3 );
	v3Expected = VistaVector3D( -1, 2, -3 );
	ASSERT_TRUE( VistaTesting::Compare( -v3Test, v3Expected ) );

	v3Test = VistaVector3D( 0.2f, 1e24f, -5.23e-12f, 0.1f );
	v3Expected = VistaVector3D( -0.2f, -1e24f, 5.23e-12f, 0.1f );
	ASSERT_TRUE( VistaTesting::Compare( -v3Test, v3Expected ) );
}

TEST( VistaVector3DTest, OperatorMultFloat )
{
	VistaVector3D v3Vector, v3Res, v3Expected;

	v3Vector = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Expected = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Res = v3Vector * 1.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );
	v3Res = 1.0f * v3Vector ;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );
	v3Expected = VistaVector3D( -4.25f, 4.23f, -11.65f, -0.2f );
	v3Res = v3Vector * -1.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );
	v3Res = -1.0f * v3Vector ;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );

	v3Vector = VistaVector3D( 1, -2, 3 );
	v3Expected = VistaVector3D( 2.5, -5, 7.5 );
	v3Res = v3Vector * 2.5f;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );
	v3Res = 2.5f * v3Vector ;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );

	v3Vector = VistaVector3D( 5, 2, 3 );
	v3Expected = VistaVector3D( 0, 0, 0 );
	v3Res = v3Vector * 0.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );
	v3Res = 0.0f * v3Vector ;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );
}
TEST( VistaVector3DTest, OperatorCompoundMultFloat )
{
	VistaVector3D v3Vector, v3Expected;

	v3Vector = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Expected = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Vector *= 1.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Vector, v3Expected ) );
	
	v3Vector = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Expected = VistaVector3D( -4.25f, 4.23f, -11.65f, -0.2f );
	v3Vector *= -1.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Vector, v3Expected ) );
	
	v3Vector = VistaVector3D( 1, -2, 3 );
	v3Expected = VistaVector3D( 2.5, -5, 7.5 );
	v3Vector *= 2.5f;
	ASSERT_TRUE( VistaTesting::Compare( v3Vector, v3Expected ) );
	

	v3Vector = VistaVector3D( 5, 2, 3 );
	v3Expected = VistaVector3D( 0, 0, 0 );
	v3Vector *= 0.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Vector, v3Expected ) );
}

TEST( VistaVector3DTest, OperatorMultVec )
{
	VistaVector3D v3Vector1, v3Vector2;
	float nExpected;

	v3Vector1 = VistaVector3D( 1, 2, 3 );
	v3Vector2 = VistaVector3D( 0, 1, 0 );
	nExpected = 2;
	ASSERT_FLOAT_EQ( v3Vector1 * v3Vector2, nExpected );
	ASSERT_FLOAT_EQ( v3Vector2 * v3Vector1, nExpected );

	v3Vector1 = VistaVector3D( -0.5f, 0, 0 );
	v3Vector2 = VistaVector3D( 1.8f, -0.2f, 11.4f );
	nExpected = -0.9f;
	ASSERT_FLOAT_EQ( v3Vector1 * v3Vector2, nExpected );
	ASSERT_FLOAT_EQ( v3Vector2 * v3Vector1, nExpected );

	v3Vector1 = VistaVector3D( 3.25, 0.54f, 2.0f );
	v3Vector2 = VistaVector3D( 0, 1, -23.5 );
	nExpected = -46.46f;
	ASSERT_FLOAT_EQ( v3Vector1 * v3Vector2, nExpected );
	ASSERT_FLOAT_EQ( v3Vector2 * v3Vector1, nExpected );

	v3Vector1 = VistaVector3D( 0.5f, -3.0f, 9.3f );
	v3Vector2 = VistaVector3D( -1.35f, -43.2f, 1.0f );
	nExpected = -0.675f + 129.6f + 9.3f;
	ASSERT_FLOAT_EQ( v3Vector1 * v3Vector2, nExpected );
	ASSERT_FLOAT_EQ( v3Vector2 * v3Vector1, nExpected );
}

TEST( VistaVector3DTest, OperatorDivFloat )
{
	VistaVector3D v3Vector, v3Res, v3Expected;

	v3Vector = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Expected = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Res = v3Vector / 1.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );

	v3Expected = VistaVector3D( -4.25f, 4.23f, -11.65f, -0.2f );
	v3Res = v3Vector / -1.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );
	
	v3Vector = VistaVector3D( 2.5, -5, 7.5 );
	v3Expected = VistaVector3D( 1, -2, 3 );
	v3Res = v3Vector / 2.5f;
	ASSERT_TRUE( VistaTesting::Compare( v3Res, v3Expected ) );	
}
TEST( VistaVector3DTest, OperatorCompoundDivFloat )
{
	VistaVector3D v3Vector, v3Expected;

	v3Vector = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Expected = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Vector /= 1.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Vector, v3Expected ) );
	
	v3Vector = VistaVector3D( 4.25f, -4.23f, 11.65f, -0.2f );
	v3Expected = VistaVector3D( -4.25f, 4.23f, -11.65f, -0.2f );
	v3Vector /= -1.0f;
	ASSERT_TRUE( VistaTesting::Compare( v3Vector, v3Expected ) );
	
	v3Vector = VistaVector3D( 2.5, -5, 7.5 );
	v3Expected = VistaVector3D( 1, -2, 3 );
	v3Vector /= 2.5f;
	ASSERT_TRUE( VistaTesting::Compare( v3Vector, v3Expected ) );	
}

TEST( VistaVector3DTest, OperatorAccess )
{
	VistaVector3D v3Test;

	v3Test[Vista::X] = 1.0f;
	ASSERT_EQ( v3Test[Vista::X], 1.0f );
	v3Test[Vista::Y] = 435.0e12f;
	ASSERT_EQ( v3Test[Vista::Y], 435.0e12f );
	v3Test[Vista::Z] = 2.0f;
	ASSERT_EQ( v3Test[Vista::Z], 2.0f );
	v3Test[Vista::W] = 0.00001f;
	ASSERT_EQ( v3Test[Vista::W], 0.00001f );
	v3Test[Vista::X] = 0.0f;
	ASSERT_EQ( v3Test[Vista::X], 0.0f );
	v3Test[Vista::Y] = 8.2f;
	ASSERT_EQ( v3Test[Vista::Y], 8.2f );
	v3Test[Vista::Z] = 4e-25f;
	ASSERT_EQ( v3Test[Vista::Z], 4e-25f );
	v3Test[Vista::W] = 0.0f;
	ASSERT_EQ( v3Test[Vista::W], 0.0f );
}

TEST( VistaVector3DTest, OperatorEq )
{
	VistaVector3D v3Vec1( 1, 2, 3 );
	VistaVector3D v3Vec2( 1, 2, 3 );
	VistaVector3D v3Vec3( 1, 2, 4 );
	VistaVector3D v3Vec4( -1, 2, 3 );
	VistaVector3D v3Vec5( 1, 0, 3 );

	ASSERT_TRUE( v3Vec1 == v3Vec1 );
	ASSERT_TRUE( v3Vec1 == v3Vec2 );
	ASSERT_TRUE( v3Vec3 == v3Vec3 );
	ASSERT_FALSE( v3Vec1 == v3Vec3 );
	ASSERT_FALSE( v3Vec1 == v3Vec4 );
	ASSERT_FALSE( v3Vec1 == v3Vec5 );
	ASSERT_FALSE( v3Vec3 == v3Vec5 );
	ASSERT_FALSE( v3Vec4 == v3Vec5 );
}



int main( int argc, char** argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
} 
