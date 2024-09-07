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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "vari/vval.h"

#include "./common.h"

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

TEST_CASE( "vval_visit" )
{
        static_assert( valid_variadic< vval< float, int > > );
        static_assert( valid_variadic< vval< float > > );

        float                           fv = 0.1f;
        vval< float, int, std::string > v1{ fv };
        vval< float, int, std::string > v1_5{ 0.1f };
        check_visit( v1, fv );

        vval< float, int, std::string > v2{ std::in_place_type_t< float >{}, fv };
        check_visit( v2, fv );

        std::string                     sv = "wololo";
        vval< float, int, std::string > v3{ sv };
        check_visit( v3, sv );

        int iv = 42;
        v3.emplace< int >( iv );
        check_visit( v3, iv );

        vval< float > v4{ fv };
        check_visit( v4, fv );

        std::vector< float >         vv = { 0.1f, 0.2f };
        vval< std::vector< float > > v5{ vv };
        check_visit( v5, vv );

        check_swap( v5 );

        vval< std::vector< float >, int >        v6{ v5 };
        vval< std::vector< float >, int, float > v7{ std::move( v6 ) };
}


template < std::size_t i >
struct tag
{
        std::string j = std::to_string( i );
};
using big_set = typelist<
    tag< 0 >,
    tag< 1 >,
    tag< 2 >,
    tag< 3 >,
    tag< 4 >,
    tag< 5 >,
    tag< 6 >,
    tag< 7 >,
    tag< 8 >,
    tag< 9 >,
    tag< 10 >,
    tag< 11 >,
    tag< 12 >,
    tag< 13 >,
    tag< 14 >,
    tag< 15 >,
    tag< 16 >,
    tag< 17 >,
    tag< 18 >,
    tag< 19 > >;
TEST_CASE( "vval_big" )
{
        vval< big_set > v{ tag< 8 >{} };

        for ( std::size_t i = 0; i < big_set::size; i++ )
                _dispatch_index< 0, big_set::size >( i, [&]< std::size_t j >() -> decltype( auto ) {
                        v.emplace< tag< j > >();
                        v.visit( [&]( auto& item ) {
                                CHECK_EQ( item.j, std::to_string( j ) );
                        } );
                } );
}

TEST_CASE( "cmp" )
{
        vval< int, float > v1{ 42 };
        vval< int, float > v2{ 42 };

        CHECK_EQ( v1, v2 );

        vval< int, float > v3{ 42.F };

        CHECK_NE( v3, v1 );

        swap( v1, v3 );

        CHECK_EQ( v3, v2 );
        CHECK_NE( v1, v2 );

        vval< int, float > v4{ 666 };

        CHECK_LT( v2, v4 );
        CHECK_GT( v4, v2 );
}

}  // namespace vari
