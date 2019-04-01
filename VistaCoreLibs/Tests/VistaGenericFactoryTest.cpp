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
#include <VistaAspects/VistaGenericFactory.h>
#include <string>

/*============================================================================*/
/* HELPER CLASSES                                                             */
/*============================================================================*/

class Base
{
public:
	virtual ~Base() = default;
protected:
	Base() = default;
};

class DerivedDefCtor : public Base
{ };

class DerivedOneCtorArg : public Base
{
public:
	DerivedOneCtorArg( int iNumber )
	: m_iNumber( iNumber )
	{ }

	int GetNumber() const { return m_iNumber; }

private:
	int m_iNumber;
};

class DerivedTwoCtorArgs : public Base
{
public:
	DerivedTwoCtorArgs( float fNumber, char cChar )
	: m_fNumber( fNumber )
	, m_cChar( cChar )
	{ }

	float GetNumber() const { return m_fNumber; }
	char GetChar() const { return m_cChar; }

private:
	float m_fNumber;
	char m_cChar;
};


/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

// @TODO Add tests for basic factory functionality, i.e., reg/dereg.

TEST( VistaGenericFactoryTest, VistaVanillaCreator )
{
	VistaGenericFactory< Base, std::string > oFactory;
	oFactory.RegisterCreator( "ZeroArgs",
							  new VistaVanillaCreator< DerivedDefCtor, Base > );

	Base* pProduct = oFactory.CreateInstance( "ZeroArgs" );
	ASSERT_NE( pProduct, nullptr );
	ASSERT_NE( dynamic_cast< DerivedDefCtor* >( pProduct ), nullptr );
}

TEST( VistaGenericFactoryTest, VistaGenericCreator )
{
	VistaGenericFactory< Base, std::string > oFactory;
	// For default c'tors, the VistaGenericCreator works just as the
	// VistaVanillaCreator. However, as it requires c'tor arg template deduction,
	// a helper class needs to be used for its instantiation.
	oFactory.RegisterCreator( "ZeroArgs",
							  VistaGenericCreatorGenerator< DerivedDefCtor, Base >::Create() );
	// For types with c'tors that require arguments, those arguments are
	// provided to the Create() function.
	oFactory.RegisterCreator( "OneArg",
							  VistaGenericCreatorGenerator< DerivedOneCtorArg, Base >::Create( 42 ) );
	oFactory.RegisterCreator( "TwoArgs",
						      VistaGenericCreatorGenerator< DerivedTwoCtorArgs, Base >::Create( 3.1415f, 'L' ) );

	Base* pProduct = oFactory.CreateInstance( "ZeroArgs" );
	ASSERT_NE( pProduct, nullptr );
	ASSERT_NE( dynamic_cast< DerivedDefCtor* >( pProduct ), nullptr );
	delete pProduct;

	pProduct = oFactory.CreateInstance( "OneArg" );
	ASSERT_NE( pProduct, nullptr );
	auto pOneArg = dynamic_cast< DerivedOneCtorArg* >( pProduct );
	ASSERT_NE( pOneArg, nullptr );
	ASSERT_EQ( pOneArg->GetNumber(), 42 );
	delete pProduct;

	pProduct = oFactory.CreateInstance( "TwoArgs" );
	ASSERT_NE( pProduct, nullptr );
	auto pTwoArgs = dynamic_cast< DerivedTwoCtorArgs*>( pProduct );
	ASSERT_NE( pTwoArgs, nullptr );
	ASSERT_EQ( pTwoArgs->GetNumber(), 3.1415f );
	ASSERT_EQ( pTwoArgs->GetChar(), 'L' );
	delete pProduct;
}

int main( int argc, char** argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
} 
