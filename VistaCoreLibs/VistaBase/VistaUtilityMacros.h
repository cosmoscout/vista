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


#ifndef _VISTAUTILITYMACROS_H
#define _VISTAUTILITYMACROS_H

#include <VistaBase/VistaExceptionBase.h>

#include <cassert>

/**
 * Helper macros: calling VISTA_MACRO_TOSTRING allows using non-string values (e.g. __FILE__ or __LINE__) as
 * strings in macros
 * VISTA_GCC_PRAGME allows calling gcc pragmas from within macros, without havind to quote the parameter
 */
#define VISTA_MACRO_STRINGIFY( arg ) #arg
#define VISTA_MACRO_TOSTRING( arg ) VISTA_MACRO_STRINGIFY(arg)
#define VISTA_GCC_PRAGMA_MACRO( call ) _Pragma( #call )

/**
 * VISTA_CHECK executes the passed Statement, and checks that the returned
 * value is equal to the expected value. If not, an Exception is thrown
 * The statement is always evaluated
 */
#define VISTA_CHECK_MSG( Statement, ExpectedValue, sMessage ) \
	if( ( Statement ) != ExpectedValue ) \
	{ \
		VISTA_THROW( "VistaCheck failed at " VISTA_MACRO_TOSTRING(__FILE__) "(" VISTA_MACRO_TOSTRING(__LINE__) ") : " sMessage, -1 ); \
	}
#define VISTA_CHECK( Statement, ExpectedValue ) VISTA_CHECK_MSG( Statement, ExpectedValue, #Statement" != "#ExpectedValue )

/**
 * VISTA_VERIFY is similar to VISTA_CHECK, but only checks and throws in debug mode
 * similar to an assert. However, the statement will always be evaluated
 */
#ifdef DEBUG
	#define VISTA_VERIFY_MSG( Statement, ExpectedValue, sMessage ) \
		if( ( Statement ) != ExpectedValue ) \
		{ \
			VISTA_THROW( "VistaVerify failed at " VISTA_MACRO_TOSTRING(__FILE__) "(" VISTA_MACRO_TOSTRING(__LINE__) ") : " sMessage, -1 ); \
		}
	#define VISTA_VERIFY( Statement, ExpectedValue ) VISTA_VERIFY_MSG( Statement, ExpectedValue, #Statement" != "#ExpectedValue )
#else
	#define VISTA_VERIFY( Statement, ExpectedValue ) ( Statement )
	#define VISTA_VERIFY_MSG( Statement, ExpectedValue, sMessage ) ( Statement )
#endif

/**
 * VISTA_ASSERT (like the c-assert) evaluates and checks the file only in debug mode
 * The statement will ONLY be evaluated in release mode
 */
#ifdef DEBUG
	#define VISTA_ASSERT_MSG( Statement, ExpectedValue, sMessage ) \
		if( ( Statement ) != ExpectedValue ) \
		{ \
			VISTA_THROW( "VistaAssert failed at " VISTA_MACRO_TOSTRING(__FILE__) "(" VISTA_MACRO_TOSTRING(__LINE__) ") : " sMessage, -1 ); \
		}
	#define VISTA_ASSERT( Statement, ExpectedValue ) VISTA_ASSERT_MSG( Statement, ExpectedValue, #Statement" != "#ExpectedValue )
#else
	// defining it as (void)0 also does nothing, but gives a compiler error when missing the ; at the end
	#define VISTA_ASSERT( Statement, ExpectedValue ) (void)0
	#define VISTA_ASSERT_MSG( Statement, ExpectedValue, sMessage ) (void)0
#endif

namespace Vista
{
	/**
	 * Vista::checked_cast works like a static_cast in release mode, but
	 * in Debug mode, a dynamic_cast is performed and the cast pointer is
	 * checked to be non-NULL
	 */
	template< typename Target, typename Source >
	Target assert_cast( Source* pPointer )
	{
#ifdef DEBUG
		Target pResult = dynamic_cast<Target>( pPointer );
		assert( pResult );
		return pResult;
#else
		return static_cast<Target>( pPointer );
#endif
	}
}

/**
 * VISTA_DEPRECATED marks a class as deprecated (if the compiler supports it)
 * so that usage of this class emits a compilation warning.
 * Note that some compilers may generate deprecated warning also when the class
 * itself is compiled
 */
#if defined _MSC_VER
#define VISTA_DEPRECATED __declspec( deprecated )
#elif defined __GNUC__
#define VISTA_DEPRECATED __attribute__ ((deprecated))
#else
#define VISTA_DEPRECATED
#endif

/**
 * VISTA_COMPILATION_MESSAGE( sMessage ) prints the passed string during compilation
 * VISTA_COMPILATION_WARNING( sMessage ) prints the passed string during compilation as a warning,
 *                                      prefixing "FILE(LINE) : VISTA_COMPILATION_WARNING"
 */
#if defined _MSC_VER
#define VISTA_COMPILATION_MESSAGE( sMessage ) __pragma( message( sMessage ) )
#elif defined __GNUC__
#define VISTA_COMPILATION_MESSAGE( sMessage ) VISTA_GCC_PRAGMA_MACRO( message sMessage )
#else
#define VISTA_COMPILATION_MESSAGE( sMessage ) 
#endif
#define VISTA_COMPILATION_WARNING( sMessage ) \
	VISTA_COMPILATION_MESSAGE( __FILE__ "(" VISTA_MACRO_TOSTRING( __LINE__ ) ") : warning: " sMessage )

#define VISTA_FUNCTION_NOT_IMPLEMENTED( FunctionName )			\
	VISTA_COMPILATION_WARNING( "Function "#FunctionName" not implemented" );	\
	VISTA_THROW( "Function "#FunctionName" not implemented", -1 );


#endif // _VISTAUTILITYMACROS_H
