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

#include "vari/vval.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

namespace vari
{

template < typename TL, typename LH, typename RH >
concept valid_split =
    std::same_as< typename split< TL >::lh, LH > && std::same_as< typename split< TL >::rh, RH >;

static_assert( valid_split< typelist<>, typelist<>, typelist<> > );
static_assert( valid_split< typelist< int >, typelist<>, typelist< int > > );
static_assert( valid_split< typelist< int, float >, typelist< int >, typelist< float > > );
static_assert(
    valid_split< typelist< int, float, char >, typelist< int >, typelist< float, char > > );

static_assert( valid_split<
               typelist< int, float, char, uint8_t >,
               typelist< int, float >,
               typelist< char, uint8_t > > );

TEST_CASE( "val_core" )
{
        _val_core< typelist< float, char > > c1;
        CHECK_EQ( c1.index, 0 );

        c1.emplace< float >( 0.1f );
        CHECK_EQ( c1.index, 1 );

        c1.destroy();
        CHECK_EQ( c1.index, 0 );

        c1.emplace< char >( 'w' );
        CHECK_EQ( c1.index, 2 );

        c1.destroy();
}

TEST_CASE( "vval" )
{
        vval< float, int, std::string > v1{ 0.1f };
        v1.visit(
            [&]( float& f ) {
                    CHECK_EQ( f, 0.1f );
            },
            [&]( int& ) {
                    FAIL( "" );
            },
            [&]( std::string& ) {
                    FAIL( "" );
            } );

        vval< float, int, std::string > v2{ std::in_place_type_t< float >{}, 0.1f };
        v2.visit(
            [&]( float& f ) {
                    CHECK_EQ( f, 0.1f );
            },
            [&]( int& ) {
                    FAIL( "" );
            },
            [&]( std::string& ) {
                    FAIL( "" );
            } );

        vval< float, int, std::string > v3{ std::string{ "wololo" } };
        v3.visit(
            [&]( float& ) {
                    FAIL( "" );
            },
            [&]( int& ) {
                    FAIL( "" );
            },
            [&]( std::string& s ) {
                    CHECK_EQ( s, "wololo" );
            } );
        v3.emplace< int >( 42 );
        v3.visit(
            [&]( float& ) {
                    FAIL( "" );
            },
            [&]( int& v ) {
                    CHECK_EQ( v, 42 );
            },
            [&]( std::string& ) {
                    FAIL( "" );
            } );
}

}  // namespace vari
