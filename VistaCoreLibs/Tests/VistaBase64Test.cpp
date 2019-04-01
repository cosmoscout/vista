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

#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaTools/VistaBase64.h>
#include <string>


/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/

/**
 * @TODO Maybe add some test to check correct placement of padding char '='.
 * @TODO Add more test cases for encoding and decoding.
 */


/**
 * Only got a 'success' test for encode as _every_ byte sequence is
 * representable as base64 per definition.
 */
TEST( VistaBase64Test, EncodeSuccess )
{
	// Encode base64 from pointer
	std::string strInput = "ViSTA is a virtual reality toolkit.";
	VistaType::byte* pInput = reinterpret_cast< VistaType::byte* >( &strInput[ 0 ] );
	const std::size_t iInputLength = strInput.length();

	std::string strResult = VistaBase64::Encode( pInput, iInputLength );
	ASSERT_EQ( strResult, "VmlTVEEgaXMgYSB2aXJ0dWFsIHJlYWxpdHkgdG9vbGtpdC4=" );

	// Encode one-fill from byte vector
	// Intentionally chose 12*8=96 bits, as it leads to 16 full sextets
	std::vector< VistaType::byte > vecByteBuffer( 12 );
	for( std::size_t i=0; i<12; ++i )
		vecByteBuffer[ i ] = 0xFF;

	strResult = VistaBase64::Encode( vecByteBuffer );
	ASSERT_EQ( strResult, "////////////////" );

	// Encode zero-fill from byte vector
	// Intentionally chose 12*8=96 bits, as it leads to 16 full sextets
	for( std::size_t i=0; i<12; ++i )
		vecByteBuffer[ i ] = 0x00;

	strResult = VistaBase64::Encode( vecByteBuffer );
	ASSERT_EQ( strResult, "AAAAAAAAAAAAAAAA" );
}

TEST( VistaBase64Test, DecodeSuccess )
{
	// Decode base64 to pointer
	std::string strInput = "VmlTVEEgaXMgYSB2aXJ0dWFsIHJlYWxpdHkgdG9vbGtpdC4=";
	VistaType::byte* pResult = NULL;
	std::size_t nNumBytes = 0;

	ASSERT_TRUE( VistaBase64::Decode( strInput, pResult, nNumBytes ) );
		
	std::string strResult = "";
	strResult.resize( nNumBytes );
	strResult.assign( reinterpret_cast< char* >( pResult ),
					  reinterpret_cast< char* >( pResult ) + nNumBytes );
	ASSERT_EQ( strResult, "ViSTA is a virtual reality toolkit." );
	delete [] pResult;

	// Decode one-fill to vector
	std::vector< VistaType::byte > vecOutput;
	ASSERT_TRUE( VistaBase64::Decode( "////////////////", vecOutput ) );
	ASSERT_EQ( vecOutput.size(), 12 ); // 16 sextets decode to 12 bytes
	bool bEqual = true;
	for( std::size_t i=0; i<vecOutput.size(); ++i )
	{
		if( vecOutput[ i ] != 0xFF )
		{
			bEqual = false;
			break;
		}
	}
	ASSERT_TRUE( bEqual );

	// Decode zero-fill to vector
	vecOutput.clear();
	ASSERT_TRUE( VistaBase64::Decode( "AAAAAAAAAAAAAAAA", vecOutput ) );
	ASSERT_EQ( vecOutput.size(), 12 ); // 16 sextets decode to 12 bytes
	bEqual = true;
	for( std::size_t i=0; i<vecOutput.size(); ++i )
	{
		if( vecOutput[ i ] != 0x00 )
		{
			bEqual = false;
			break;
		}
	}
	ASSERT_TRUE( bEqual );

	// Check if vector is cleared by call to function
	ASSERT_TRUE( VistaBase64::Decode( "", vecOutput ) );
	ASSERT_EQ( vecOutput.size(), 0 );
}

TEST( VistaBase64Test, DecodeFailure )
{
	// Invalid length
	std::vector< VistaType::byte > vecOutput;
	ASSERT_FALSE( VistaBase64::Decode( "///", vecOutput ) );

	// Invalid character, should crash
	ASSERT_THROW( VistaBase64::Decode( "///?", vecOutput ), VistaExceptionBase );

	// Padding character at wrong position
	ASSERT_THROW( VistaBase64::Decode( "/=//", vecOutput ), VistaExceptionBase );

	// Too many padding characters
	ASSERT_THROW( VistaBase64::Decode( "/===", vecOutput ), VistaExceptionBase );
}


int main( int argc, char** argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
} 
