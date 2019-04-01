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

#include <VistaTestingUtils/VistaTestingCompare.h>
#include <VistaTestingUtils/VistaTestingDummyStruct.h>

#include <VistaAspects/VistaReflectionable.h>

static const int S_nNumIterations = 100;
static const float S_nRequiredFloatPrecision = 0.0001f;



class ReflTester : public IVistaReflectionable
{
public:
	ReflTester()
	: m_fFloat1( 0 )
	, m_fFloat2( 0 )
	, m_dDouble( 0 )
	, m_nUShort( 0 )
	{
		memset( m_a2fData2, 0, sizeof( m_a2fData2 ) );
		memset( m_i3dData2, 0, sizeof( m_i3dData2 ) );
		memset( m_a4bData2, 0, sizeof( m_a4bData2 ) );
	}	
	virtual ~ReflTester() {};

	// to make getter/setter names unified
	static std::string GetFloat1Name() { return "FLOAT1"; };
	static std::string GetFloat2Name() { return "FLOAT2"; };
	static std::string GetDoubleName() { return "DOUBLE"; };
	static std::string GetUShortName() { return "USHORT"; };
	static std::string GetDummyDataName() { return "DUMMYDATA"; };
	static std::string GetPosition1Name() { return "POSITION1"; };
	static std::string GetPosition2Name() { return "POSITION2"; };
	static std::string GetPosition2ArrayName() { return "POSITION2ARRAY"; };
	static std::string GetOrientationName() { return "ORIENTATION"; };
	static std::string GetMatrixName() { return "MATRIX"; };	
	static std::string GetPropListName() { return "PROPLIST"; };
	static std::string GetArray2Name() { return "ARRAY2"; };
	static std::string GetArray3Name() { return "ARRAY3"; };
	static std::string GetArray4Name() { return "ARRAY4"; };
	static std::string GetData2Name() { return "DATA2"; };
	static std::string GetData3Name() { return "DATA3"; };
	static std::string GetData4Name() { return "DATA4"; };
	static std::string GetPublicStringName() { return "PUBLICSTRING"; };

	DummyStruct GetDummyData() const { return m_oSomeData; }
	bool SetDummyData( const DummyStruct& oValue ) { m_oSomeData = oValue; return true; }

	VistaVector3D GetPosition1() const { return m_v3Position1; }
	bool SetPosition1( const VistaVector3D& oValue ) { m_v3Position1 = oValue; return true; }

	VistaVector3D GetPosition2() const { return m_v3Position2; }
	bool GetPosition2Array( float& fX, float& fY, float& fZ ) const
	{
		fX = m_v3Position2[0];
		fY = m_v3Position2[1];
		fZ = m_v3Position2[2];
		return true;
	}
	bool SetPosition2( const VistaVector3D& oValue ) { m_v3Position2 = oValue; return true; }
	bool SetPosition2Array( float fX, float fY, float fZ )
	{
		m_v3Position2[0] = fX;
		m_v3Position2[1] = fY;
		m_v3Position2[2] = fZ;
		return true;
	}

	VistaQuaternion GetOrientation() const { return m_qOrientation; }
	bool SetOrientation( const VistaQuaternion& oValue ) { m_qOrientation = oValue; return true; }

	VistaTransformMatrix GetMatrix() const { return m_matTransform; }
	bool SetMatrix( const VistaTransformMatrix& oValue ) { m_matTransform = oValue; return true; }

	float GetFloat1() const { return m_fFloat1; }
	bool SetFloat1( float oValue ) { m_fFloat1 = oValue; return true; }

	float GetFloat2() const { return m_fFloat2; }
	bool SetFloat2( const float& oValue ) { m_fFloat2 = oValue; return true; }

	double GetDouble() const { return m_dDouble; }
	bool SetDouble( double oValue ) { m_dDouble = oValue; return true; }

	unsigned short GetUShort() const { return m_nUShort; }
	bool SetUShort( unsigned short oValue ) { m_nUShort = oValue; return true; }

	VistaPropertyList GetPropList() const { return m_oPropList; }
	bool SetPropList( const VistaPropertyList& oValue ) { m_oPropList = oValue; return true; }

	bool GetData2( float& oVal1, float& oVal2 ) const
	{
		oVal1 = m_a2fData2[0];
		oVal2 = m_a2fData2[1];
		return true;	
	}
	bool SetData2( float oVal1, float oVal2 )
	{
		m_a2fData2[0] = oVal1;
		m_a2fData2[1] = oVal2;
		return true;
	}

	bool GetData3( int& oVal1, int& oVal2, int& oVal3 ) const
	{
		oVal1 = m_i3dData2[0];
		oVal2 = m_i3dData2[1];
		oVal3 = m_i3dData2[2];
		return true;
	}
	bool SetData3( int oVal1, int oVal2, int oVal3 )
	{
		m_i3dData2[0] = oVal1;
		m_i3dData2[1] = oVal2;
		m_i3dData2[2] = oVal3;
		return true;
	}

	bool GetData4( bool& oVal1, bool& oVal2, bool& oVal3, bool& oVal4 ) const
	{
		oVal1 = m_a4bData2[0];
		oVal2 = m_a4bData2[1];
		oVal3 = m_a4bData2[2];
		oVal4 = m_a4bData2[3];
		return true;
	}
	bool SetData4( bool oVal1, bool oVal2, bool oVal3, bool oVal4 )
	{
		m_a4bData2[0] = oVal1;
		m_a4bData2[1] = oVal2;
		m_a4bData2[2] = oVal3;
		m_a4bData2[3] = oVal4;
		return true;
	}

	void GetArray2( float* bData ) const
	{
		bData[0] = m_a2fData2[0];
		bData[1] = m_a2fData2[1];
	}
	bool SetArray2( const float* bData)
	{
		m_a2fData2[0] = bData[0];
		m_a2fData2[1] = bData[1];
		return true;
	}
	void GetArray3( int* bData ) const
	{
		bData[0] = m_i3dData2[0];
		bData[1] = m_i3dData2[1];
		bData[2] = m_i3dData2[2];
	}
	bool SetArray3( const int* bData)
	{
		m_i3dData2[0] = bData[0];
		m_i3dData2[1] = bData[1];
		m_i3dData2[2] = bData[2];
		return true;
	}
	void GetArray4( bool* bData ) const
	{
		bData[0] = m_a4bData2[0];
		bData[1] = m_a4bData2[1];
		bData[2] = m_a4bData2[2];
		bData[3] = m_a4bData2[3];
	}
	bool SetArray4( const bool* bData)
	{
		m_a4bData2[0] = bData[0];
		m_a4bData2[1] = bData[1];
		m_a4bData2[2] = bData[2];
		m_a4bData2[3] = bData[3];
		return true;
	}

	REFL_DECLARE;
public:
	std::string		m_sSomePublicString;
private:
	DummyStruct		m_oSomeData;	
	VistaVector3D	m_v3Position1;	
	VistaVector3D	m_v3Position2;
	VistaQuaternion	m_qOrientation;
	VistaTransformMatrix m_matTransform;
	float			m_fFloat1;
	float			m_fFloat2;
	double			m_dDouble;
	unsigned short	m_nUShort;
	float			m_a2fData2[2];
	int				m_i3dData2[4];
	bool			m_a4bData2[4];
	VistaPropertyList	m_oPropList;
};


REFL_IMPLEMENT_FULL( ReflTester, IVistaReflectionable );

namespace
{
	IVistaPropertyGetFunctor *SaGetter[] =
	{
		new TVistaPropertyGet<float, ReflTester, VistaProperty::PROPT_DOUBLE>(
						ReflTester::GetFloat1Name(),
						SsReflectionName,
						&ReflTester::GetFloat1 ),
		new TVistaPropertyGet<float, ReflTester, VistaProperty::PROPT_DOUBLE>(
						ReflTester::GetFloat2Name(),
						SsReflectionName,
						&ReflTester::GetFloat2 ),
		new TVistaPropertyGet<double, ReflTester, VistaProperty::PROPT_DOUBLE>(
						ReflTester::GetDoubleName(),
						SsReflectionName,
						&ReflTester::GetDouble ),
		new TVistaPropertyGet<unsigned short, ReflTester, VistaProperty::PROPT_INT>(
						ReflTester::GetUShortName(),
						SsReflectionName,
						&ReflTester::GetUShort ),
		new TVistaPropertyConvertAndGet<DummyStruct, const DummyStruct&, ReflTester, VistaProperty::PROPT_STRING>(
						ReflTester::GetDummyDataName(),
						SsReflectionName,
						&ReflTester::GetDummyData,
						&DummyStruct::ConvertToString ),
		new TVistaPropertyGet<VistaVector3D, ReflTester, VistaProperty::PROPT_LIST>(
						ReflTester::GetPosition1Name(),
						SsReflectionName,
						&ReflTester::GetPosition1 ),
		new TVistaPropertyGet<VistaVector3D, ReflTester, VistaProperty::PROPT_LIST>(
						ReflTester::GetPosition2Name(),
						SsReflectionName,
						&ReflTester::GetPosition2 ),
		new TVistaProperty3RefGet<float, ReflTester, VistaProperty::PROPT_LIST>( 
						ReflTester::GetPosition2ArrayName(),
						SsReflectionName,
						&ReflTester::GetPosition2Array ),
		new TVistaPropertyGet<VistaQuaternion, ReflTester, VistaProperty::PROPT_LIST>( 
						ReflTester::GetOrientationName(),
						SsReflectionName,
						&ReflTester::GetOrientation ),
		new TVistaPropertyGet<VistaTransformMatrix, ReflTester, VistaProperty::PROPT_LIST>( 
						ReflTester::GetMatrixName(),
						SsReflectionName,
						&ReflTester::GetMatrix ),
		new TVistaPropertyListGet<ReflTester>(
						ReflTester::GetPropListName(),
						SsReflectionName,
						&ReflTester::GetPropList ),
		new TVistaProperty2RefGet<float, ReflTester>(
						ReflTester::GetData2Name(),
						SsReflectionName,
						&ReflTester::GetData2 ),
		new TVistaPropertyArrayGet<ReflTester, float, 2>(
						ReflTester::GetArray2Name(),
						SsReflectionName,
						&ReflTester::GetArray2 ),
		new TVistaProperty3RefGet<int, ReflTester>(
						ReflTester::GetData3Name(),
						SsReflectionName,
						&ReflTester::GetData3 ),
		new TVistaPropertyArrayGet<ReflTester, int, 3>(
						ReflTester::GetArray3Name(),
						SsReflectionName,
						&ReflTester::GetArray3 ),
		new TVistaProperty4RefGet<bool, ReflTester>(
						ReflTester::GetData4Name(),
						SsReflectionName,
						&ReflTester::GetData4 ),
		new TVistaPropertyArrayGet<ReflTester, bool, 4>(
						ReflTester::GetArray4Name(),
						SsReflectionName,
						&ReflTester::GetArray4 ),
		new TVistaPublicStringPropertyGet<ReflTester>(
						ReflTester::GetPublicStringName(),
						SsReflectionName,
						&ReflTester::m_sSomePublicString ),						
		NULL,
	};

	IVistaPropertySetFunctor *SaSetter[] =
	{
		new TVistaPropertySet<float, float, ReflTester>(
						ReflTester::GetFloat1Name(),
						SsReflectionName,
						&ReflTester::SetFloat1 ),
		new TVistaPropertySet<const float&, float, ReflTester>(
						ReflTester::GetFloat2Name(),
						SsReflectionName,
						&ReflTester::SetFloat2 ),
		new TVistaPropertySet<double, double, ReflTester>(
						ReflTester::GetDoubleName(),
						SsReflectionName,
						&ReflTester::SetDouble ),
		new TVistaPropertySet<unsigned short, unsigned short, ReflTester>(
						ReflTester::GetUShortName(),
						SsReflectionName,
						&ReflTester::SetUShort ),
		new TVistaPropertyConvertAndSet<const DummyStruct&, DummyStruct, ReflTester>(
						ReflTester::GetDummyDataName(),
						SsReflectionName,
						&ReflTester::SetDummyData,
						&DummyStruct::ConvertFromString ),
		new TVistaPropertySet<const VistaVector3D&, VistaVector3D, ReflTester>(
						ReflTester::GetPosition1Name(),
						SsReflectionName,
						&ReflTester::SetPosition1 ),
		new TVistaPropertySet<const VistaVector3D&, VistaVector3D, ReflTester>(
						ReflTester::GetPosition2Name(),
						SsReflectionName,
						&ReflTester::SetPosition2 ),
		new TVistaProperty3ValSet<float, ReflTester>(
						ReflTester::GetPosition2ArrayName(),
						SsReflectionName,
						&ReflTester::SetPosition2Array ),
		new TVistaPropertySet<const VistaQuaternion&, VistaQuaternion, ReflTester>(
						ReflTester::GetOrientationName(),
						SsReflectionName,
						&ReflTester::SetOrientation ),
		new TVistaPropertySet<const VistaTransformMatrix&, VistaTransformMatrix, ReflTester>(
						ReflTester::GetMatrixName(),
						SsReflectionName,
						&ReflTester::SetMatrix ),
		new TVistaPropertyListSet<ReflTester>(
						ReflTester::GetPropListName(),
						SsReflectionName,
						&ReflTester::SetPropList ),
		new TVistaProperty2ValSet<float, ReflTester>(
						ReflTester::GetData2Name(),
						SsReflectionName,
						&ReflTester::SetData2 ),
		new TVistaPropertyArraySet<ReflTester, float, 2>(
						ReflTester::GetArray2Name(),
						SsReflectionName,
						&ReflTester::SetArray2 ),
		new TVistaProperty3ValSet<int, ReflTester>(
						ReflTester::GetData3Name(),
						SsReflectionName,
						&ReflTester::SetData3 ),
		new TVistaPropertyArraySet<ReflTester, int, 3>(
						ReflTester::GetArray3Name(),
						SsReflectionName,
						&ReflTester::SetArray3 ),
		new TVistaProperty4ValSet<bool, ReflTester>(
						ReflTester::GetData4Name(),
						SsReflectionName,
						&ReflTester::SetData4 ),
		new TVistaPropertyArraySet<ReflTester, bool, 4>(
						ReflTester::GetArray4Name(),
						SsReflectionName,
						&ReflTester::SetArray4 ),
		new TVistaPublicStringPropertySet<ReflTester>(
						ReflTester::GetPublicStringName(),
						SsReflectionName,
						&ReflTester::m_sSomePublicString ),
		NULL,
	};
}






class SetPorpertyByListTest : public ::testing::Test
{

	virtual void SetUp()
	{
		m_oPropList.SetValue( ReflTester::GetPublicStringName(), "StringTest" );
		m_oPropList.SetValue( ReflTester::GetDummyDataName(), "12.12 | false" );
	}

private:
	VistaPropertyList m_oPropList;
};

template<typename T>
inline void TestReflSetGetProp( IVistaReflectionable* pRefl, const std::string& sName )
{
	T oOrig = VistaTesting::GenerateRandom<T>();
	VistaProperty oOrigProp( sName, VistaConversion::ToString( oOrig ) );
	ASSERT_TRUE( pRefl->SetProperty( oOrigProp ) != 0 );
	VistaProperty oReturn = pRefl->GetPropertyByName( sName );
	T oNew;
	ASSERT_NO_THROW( oNew = VistaConversion::FromString<T>( oReturn.GetValueConstRef() ) );
	ASSERT_TRUE( VistaTesting::Compare( oOrig, oNew, S_nRequiredFloatPrecision ) );
}

template<typename T>
inline void TestReflSetGetAlt( IVistaReflectionable* pRefl, const std::string& sName )
{
	T oOrig = VistaTesting::GenerateRandom<T>();
	ASSERT_TRUE( pRefl->SetPropertyByName( sName, VistaConversion::ToString( oOrig ) ) != 0 );
	VistaProperty oReturn = pRefl->GetPropertyByName( sName );
	T oNew;
	ASSERT_NO_THROW( oNew = VistaConversion::FromString<T>( oReturn.GetValueConstRef() ) );
	ASSERT_TRUE( VistaTesting::Compare( oOrig, oNew, S_nRequiredFloatPrecision ) );
}

template<>
inline void TestReflSetGetProp<VistaPropertyList>( IVistaReflectionable* pRefl, const std::string& sName )
{
	VistaPropertyList oOrig = VistaTesting::GenerateRandom<VistaPropertyList>();
	VistaProperty oOrigProp( sName );
	oOrigProp.SetPropertyListValue( oOrig );
	ASSERT_TRUE( pRefl->SetProperty( oOrigProp ) != 0 );
	VistaProperty oReturn = pRefl->GetPropertyByName( sName );
	VistaPropertyList oNew;
	ASSERT_NO_THROW( oNew = oReturn.GetPropertyListConstRef() );
	ASSERT_TRUE( VistaTesting::Compare( oNew, oReturn.GetPropertyListConstRef(), S_nRequiredFloatPrecision ) );
}


template<typename T, int N>
inline void TestReflSetGetPropArray( IVistaReflectionable* pRefl, const std::string& sName )
{
	T oOrig[N];
	for( int i = 0; i < N; ++i )
		oOrig[i] = VistaTesting::GenerateRandom<T>();

	VistaProperty oOrigProp( sName, VistaConversion::ArrayToString<N>( oOrig ) );
	ASSERT_TRUE( pRefl->SetProperty( oOrigProp ) != 0 );
	VistaProperty oReturn = pRefl->GetPropertyByName( sName );
	T oNew[N];
	ASSERT_NO_THROW( VistaConversion::ArrayFromString<N>( oReturn.GetValueConstRef(), oNew ) );
	for( int i = 0; i < N; ++i )
	{
		ASSERT_TRUE( VistaTesting::Compare( oOrig[i], oNew[i], S_nRequiredFloatPrecision ) );
	}
}


/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/


ReflTester S_oReflectionable;

TEST( VistaReflectionableTest, SetGetFloat1 )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<float>( &S_oReflectionable, ReflTester::GetFloat1Name() );
		TestReflSetGetAlt<float>( &S_oReflectionable, ReflTester::GetFloat1Name() );
	}
}

TEST( VistaReflectionableTest, SetGetFloat2 )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<float>( &S_oReflectionable, ReflTester::GetFloat2Name() );
		TestReflSetGetAlt<float>( &S_oReflectionable, ReflTester::GetFloat2Name() );
	}
}

TEST( VistaReflectionableTest, SetGetDouble )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<float>( &S_oReflectionable, ReflTester::GetDoubleName() );
		TestReflSetGetAlt<float>( &S_oReflectionable, ReflTester::GetDoubleName() );
	}
}

TEST( VistaReflectionableTest, SetGetUShort )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<unsigned short>( &S_oReflectionable, ReflTester::GetUShortName() );
		TestReflSetGetAlt<unsigned short>( &S_oReflectionable, ReflTester::GetUShortName() );
	}
}

TEST( VistaReflectionableTest, SetGetDummyData )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<DummyStruct>( &S_oReflectionable, ReflTester::GetDummyDataName() );
		TestReflSetGetAlt<DummyStruct>( &S_oReflectionable, ReflTester::GetDummyDataName() );
	}
}

TEST( VistaReflectionableTest, SetGetPosition1 )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<VistaVector3D>( &S_oReflectionable, ReflTester::GetPosition1Name() );
		TestReflSetGetAlt<VistaVector3D>( &S_oReflectionable, ReflTester::GetPosition1Name() );
	}
}

TEST( VistaReflectionableTest, SetGetPosition2 )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<VistaVector3D>( &S_oReflectionable, ReflTester::GetPosition2Name() );
		TestReflSetGetAlt<VistaVector3D>( &S_oReflectionable, ReflTester::GetPosition2Name() );
	}
}

TEST( VistaReflectionableTest, SetGetPosition2Array )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<VistaVector3D>( &S_oReflectionable, ReflTester::GetPosition2ArrayName() );
		TestReflSetGetAlt<VistaVector3D>( &S_oReflectionable, ReflTester::GetPosition2ArrayName() );
	}
}

TEST( VistaReflectionableTest, SetGetOrientation )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<VistaQuaternion>( &S_oReflectionable, ReflTester::GetOrientationName() );
		TestReflSetGetAlt<VistaQuaternion>( &S_oReflectionable, ReflTester::GetOrientationName() );
	}
}

TEST( VistaReflectionableTest, SetGetmatrix )
{
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<VistaTransformMatrix>( &S_oReflectionable, ReflTester::GetMatrixName() );
		TestReflSetGetAlt<VistaTransformMatrix>( &S_oReflectionable, ReflTester::GetMatrixName() );
	}
}

TEST( VistaReflectionableTest, SetGetProplist )
{
	// slow, we reduce num iterations
	int nIters = S_nNumIterations % 10;
	if( nIters == 0 )
		nIters = 1;
	for( int i = 0; i < nIters; ++i )
	{
		TestReflSetGetProp<VistaPropertyList>( &S_oReflectionable, ReflTester::GetPropListName() );
		// no alt optionfor proplists - cant set by string
	}
}

TEST( VistaReflectionableTest, SetGetDataFloat2 )
{	
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetPropArray<float,2>( &S_oReflectionable, ReflTester::GetData2Name() );
	}
}
TEST( VistaReflectionableTest, SetGetArrayFloat2 )
{	
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetPropArray<float,2>( &S_oReflectionable, ReflTester::GetArray2Name() );
	}
}

TEST( VistaReflectionableTest, SetGetDataInt3 )
{	
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetPropArray<int,3>( &S_oReflectionable, ReflTester::GetData3Name() );
	}
}
TEST( VistaReflectionableTest, SetGetArrayInt3 )
{	
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetPropArray<int,3>( &S_oReflectionable, ReflTester::GetArray3Name() );
	}
}

TEST( VistaReflectionableTest, SetGetDataBool4 )
{	
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetPropArray<bool,4>( &S_oReflectionable, ReflTester::GetData4Name() );
	}
}
TEST( VistaReflectionableTest, SetGetArrayBool4 )
{	
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetPropArray<bool,4>( &S_oReflectionable, ReflTester::GetArray4Name() );
	}
}

TEST( VistaReflectionableTest, SetGetPublicString )
{	
	for( int i = 0; i < S_nNumIterations; ++i )
	{
		TestReflSetGetProp<std::string>( &S_oReflectionable, ReflTester::GetPublicStringName() );
		TestReflSetGetAlt<std::string>( &S_oReflectionable, ReflTester::GetPublicStringName() );
	}
}


int main( int argc, char** argv )
{
	::testing::InitGoogleTest(&argc, argv);
	unsigned int nSeed = VistaTesting::InitializeRandomSeed();
	SCOPED_TRACE( "RandomSeed: " + VistaConversion::ToString( nSeed ) );
	return RUN_ALL_TESTS();
} 
