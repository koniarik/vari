///
/// Copyright (C) 2020 Jan Veverak Koniarik
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
/// and associated documentation files (the "Software"), to deal in the Software without
/// restriction, including without limitation the rights to use, copy, modify, merge, publish,
/// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies or
/// substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
/// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
/// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///

#include "vari/vptr.h"

#include "vari/uvptr.h"
#include "vari/vref.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include <string>
#include <vector>

namespace vari
{
namespace bits
{
        static_assert( contains_type< int, typelist< int, float > >::value );
        static_assert( !contains_type< std::string, typelist< int, float > >::value );

        using ut1 = unique_typelist< typelist<>, typelist< float, float, int, float, int > >;
        static_assert( std::same_as< typename ut1::type, typelist< float, int > > );

        using ft1 = flatten_typelist<
            typelist<>,
            typelist<  //
                typelist< float, int >,
                int,
                typelist< float, int, typelist< std::string > >  //
                > >;
        using uft1 = unique_typelist< typelist<>, typename ft1::type >;
        static_assert( std::same_as< typename uft1::type, typelist< float, int, std::string > > );
}  // namespace bits

TEST_CASE( "vptr" )
{
        using V = vptr< int, float, std::string >;

        int i;
        V   p1{ i };

        CHECK( p1 );

        p1.take(
            [&]( vptr<> ) {
                    FAIL( "incorrect overload" );
            },
            [&]( vptr< int > ) {},
            [&]( vptr< float, std::string > ) {
                    FAIL( "incorrect overload" );
            } );

        vptr< float > p2;

        CHECK_FALSE( p2 );

        p1 = p2;

        CHECK_FALSE( p1 );

        vptr< float > p3;

        p3 = p2;

        CHECK_EQ( p2, p3 );
        CHECK_EQ( p2.raw(), p3.raw() );
}

static_assert( std::same_as<
               typename bits::type_at< 0, bits::typelist< int, float, std::string > >::type,
               int > );
static_assert( std::same_as<
               typename bits::type_at< 1, bits::typelist< int, float, std::string > >::type,
               float > );
static_assert( std::same_as<
               typename bits::type_at< 2, bits::typelist< int, float, std::string > >::type,
               std::string > );

TEST_CASE( "vref" )
{
        using V = vref< int, float, std::string >;

        std::string i{ "123456" };
        V           p1{ i };

        p1.take(
            [&]( vref< int > ) {
                    FAIL( "incorrect overload" );
            },
            [&]( vref< float, std::string > ) {} );

        vptr< int, float, std::string > p2 = p1;
        vptr< int >                     p3;

        std::vector< int >         vec = { 1, 2, 3, 4 };
        vptr< std::vector< int > > p4( vec );
        CHECK_EQ( p2.raw(), p1.raw() );
}

TEST_CASE( "uvptr" )
{
        using V = uvptr< int, float, std::string >;

        V p1 = uvptr< std::string >( new std::string{ "s" } );

        std::move( p1 ).take(
            [&]( uvptr<> ) {
                    FAIL( "incorrect overload" );
            },
            [&]( uvptr< int > ) {
                    FAIL( "incorrect overload" );
            },
            [&]( uvptr< float, std::string > ) {} );

        vptr< int, float, std::string > p2 = p1.ptr();
        CHECK_EQ( p2.raw(), p1.ptr().raw() );
}

}  // namespace vari