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

#include "vari/vptr.h"

#include "./common.h"
#include "vari/uvptr.h"
#include "vari/uvref.h"
#include "vari/vref.h"

#include <doctest/doctest.h>
#include <string>
#include <vector>

namespace vari
{
static_assert( contains_type_v< int, typelist< int, float > > );
static_assert( !contains_type_v< std::string, typelist< int, float > > );

using ut1 = unique_typelist_t< typelist< float, float, int, float, int > >;
static_assert( std::same_as< ut1, typelist< float, int > > );

using ft1  = flatten_t<  //
    typelist< float, int >,
    int,
    typelist< float, int, typelist< std::string > >  //
    >;
using uft1 = unique_typelist_t< ft1 >;
static_assert( std::same_as< uft1, typelist< float, int, std::string > > );

static_assert( contains_type_v< int, typelist< int, const int > > );
static_assert( contains_type_v< const int, typelist< int, const int > > );

using ft2  = flatten_t<  //
    typelist< const int >,
    int,
    typelist< float, int, typelist< float const > >  //
    >;
using uft2 = unique_typelist_t< ft2 >;
static_assert( std::same_as< uft2, typelist< const int, float, int, const float > > );

using ft3  = flatten_t<  //
    const typelist< int >,
    int,
    typelist< float, int, const typelist< float > >  //
    >;
using uft3 = unique_typelist_t< ft3 >;
static_assert( std::same_as< uft3, typelist< const int, float, int, const float > > );
using ivst_tl1 = typelist< const int, const float >;
using ivst_tl2 = typelist< int, float >;

static_assert( vconvertible_to< ivst_tl1, ivst_tl1 > );
static_assert( vconvertible_to< ivst_tl2, ivst_tl1 > );
static_assert( vconvertible_to< ivst_tl2, ivst_tl2 > );

TEST_CASE( "vptr" )
{
        using V = vptr< int, float, std::string >;
        static_assert( valid_null_variadic< V > );

        V p0;

        p0 = V{ nullptr };
        CHECK_FALSE( p0 );

        int i = 42;

        V p1{ i };
        check_nullable_visit( p1, i );

        vptr< float > p2;
        CHECK_FALSE( p2 );

        p1 = p2;
        CHECK_FALSE( p1 );

        float         f1 = 0.42;
        vptr< float > p3 = f1;
        check_nullable_visit( p3, f1 );

        vptr< int > p4{ i };
        p4 = nullptr;
        CHECK_FALSE( p4 );
}

static_assert( std::same_as< type_at_t< 0, typelist< int, float, std::string > >, int > );
static_assert( std::same_as< type_at_t< 1, typelist< int, float, std::string > >, float > );
static_assert( std::same_as< type_at_t< 2, typelist< int, float, std::string > >, std::string > );

TEST_CASE( "vref" )
{
        using V = vref< int, float, std::string >;
        static_assert( valid_variadic< V > );

        std::string i{ "123456" };
        V           p1{ i };

        check_visit( p1, i );
        check_swap( p1 );

        vptr< int, float, std::string > p2 = p1;
        CHECK_EQ( p2.get(), p1.get() );

        std::vector< int >         vec = { 1, 2, 3, 4 };
        vptr< std::vector< int > > p4( vec );
        check_nullable_visit( p4, vec );
        check_nullable_swap( p4 );
}

TEST_CASE( "uvptr" )
{
        using V = uvptr< int, float, std::string >;
        static_assert( valid_null_owning_variadic< V > );

        std::string inpt{ "s" };

        V p1 = uwrap( inpt );
        check_nullable_visit( p1, inpt );
        // check_nullable_swap( p1 );

        p1 = std::move( p1 ).take(
            [&]( empty_t ) -> V {
                    FAIL( "incorrect overload" );
                    return V{};
            },
            [&]( uvref< int > ) -> V {
                    FAIL( "incorrect overload" );
                    return V{};
            },
            [&]( uvptr< float, std::string > p ) -> V {
                    return p;
            } );

        std::optional< vref< int, float, std::string > > opt_ref = p1.vref();
        CHECK( opt_ref );
        opt_ref->visit( [&]( int& ) {}, [&]( float& ) {}, [&]( std::string& ) {} );

        vptr< int, float, std::string > p2 = p1.get();
        CHECK_EQ( p2.get(), p1.get().get() );

        CHECK( p1 );
        p1 = nullptr;
        CHECK_FALSE( p1 );

        V p3 = V();

        std::move( p3 ).take(
            [&]( empty_t ) {},
            [&]( uvref< int > ) {
                    FAIL( "incorrect overload" );
            },
            [&]( uvptr< float, std::string > ) {
                    FAIL( "incorrect overload" );
            } );

        p3 = std::move( p3 );

        uvref< int > r1 = uwrap( int{ 42 } );

        CHECK_FALSE( p3 );
        p3 = std::move( r1 );
        CHECK( p3 );

        uvptr< int > p4{ new int{ 33 } };
        CHECK_EQ( *p4, 33 );

        uvptr< int > p5{ (int*) nullptr };
        CHECK_FALSE( p5 );

        uvptr< std::string > p6{ new std::string{ "wololo" } };
        CHECK_EQ( p6->front(), 'w' );

        vptr< std::string > p7 = p6;
        CHECK_EQ( p7->c_str(), p6->c_str() );

        uvref< std::string > r8 = std::move( p6 ).vref();
        r8.visit( [&]( auto& ) {} );

        p1 = uwrap( int{ 42 } );
        p1 = std::move( p6 );
}

TEST_CASE( "uvref" )
{
        using V = uvref< int, float, std::string >;

        std::string inpt{ "s" };
        V           p1 = uwrap( inpt );

        check_visit( p1, inpt );
        // check_swap(p1);

        int k;
        p1 = std::move( p1 ).take(
            [&]( uvref< int > ) -> V {
                    FAIL( "incorrect overload" );
                    return uwrap( k );
            },
            [&]( uvref< float, std::string > r ) -> V {
                    return r;
            } );

        vptr< int, float, std::string > p2 = p1.get();
        CHECK_EQ( p2.get(), p1.get().get() );

        p1 = uwrap( float{ 42 } );
}

TEST_CASE( "dispatch" )
{
        for ( std::size_t i = 0; i < 128; i++ )
                _dispatch_index< 0, 128 >( i, [&]< std::size_t j > {
                        CHECK_EQ( i, j );
                } );
}

template < template < typename... > typename T >
void try_set()
{
        std::set< T< int, std::string > > s3;
        std::vector< int >                idata{ 127, 0 };
        std::vector< std::string >        sdata{ 127, "wololo" };

        for ( int& i : idata )
                s3.insert( T< int >{ i } );

        for ( std::string& s : sdata )
                s3.insert( T< std::string >{ s } );

        CHECK_EQ( idata.size() + sdata.size(), s3.size() );
}

void try_upset()
{
        std::set< uvptr< int, std::string > > s3;
        static constexpr std::size_t          n = 128;
        for ( std::size_t i = 0; i < n; i++ )
                s3.insert( uvptr< int >{ new int{ 42 } } );
        for ( std::size_t i = 0; i < n; i++ )
                s3.insert( uvptr< std::string >{ new std::string{ "wololo" } } );

        CHECK_EQ( 2 * n, s3.size() );
}

void try_urset()
{
        std::set< uvref< int, std::string > > s3;
        static constexpr std::size_t          n = 128;
        for ( std::size_t i = 0; i < n; i++ )
                s3.insert( uvref< int >{ *new int{ 42 } } );
        for ( std::size_t i = 0; i < n; i++ )
                s3.insert( uvref< std::string >{ *new std::string{ "wololo" } } );

        CHECK_EQ( 2 * n, s3.size() );
}

TEST_CASE( "cmp" )
{
        std::set< vptr< int > > s1;
        s1.insert( vptr< int >{} );

        std::set< vptr< int, std::string > > s2;
        s2.insert( vptr< int >{} );

        try_set< vptr >();
        try_set< vref >();
        try_upset();
        try_urset();
}

TEST_CASE( "reference to functor" )
{
        struct foo
        {
                foo()                        = default;
                foo( const foo& )            = delete;
                foo( foo&& )                 = delete;
                foo& operator=( const foo& ) = delete;
                foo& operator=( foo&& )      = delete;

                void operator()( empty_t )
                {
                }
                void operator()( int& )
                {
                }
                void operator()( std::string& )
                {
                }
        };

        foo f;
        int i;

        vref< int, std::string > r1{ i };
        r1.visit( f );
        vptr< int, std::string > p1{ i };
        p1.visit( f );
        uvref< int, std::string > r2 = uwrap( std::string{ "wololo" } );
        r2.visit( f );
        uvptr< int, std::string > p2 = uwrap( std::string{ "wololo" } );
        p2.visit( f );
}

TEST_CASE( "moved functor" )
{
        struct foo
        {
                std::size_t count = 0;

                foo()                        = default;
                foo( const foo& )            = delete;
                foo( foo&& )                 = default;
                foo& operator=( const foo& ) = delete;
                foo& operator=( foo&& )      = default;

                auto&& operator()( empty_t ) &&
                {
                        count += 1;
                        return std::move( *this );
                }
                auto&& operator()( int& ) &&
                {
                        count += 1;
                        return std::move( *this );
                }
                auto&& operator()( std::string& ) &&
                {
                        count += 1;
                        return std::move( *this );
                }
        };

        foo f;
        int i;

        vref< int, std::string > r1{ i };
        f = r1.visit( std::move( f ) );

        foo                      f2;
        vptr< int, std::string > p1{ i };
        f = p1.visit( std::move( f ) );

        foo                       f3;
        uvref< int, std::string > r2 = uwrap( std::string{ "wololo" } );

        f = r2.visit( std::move( f ) );

        foo                       f4;
        uvptr< int, std::string > p2 = uwrap( std::string{ "wololo" } );

        f = p2.visit( std::move( f ) );

        CHECK_EQ( f.count, 4 );
}

TEST_CASE( "const vptr" )
{
        using V = vptr< const int, const float, const std::string >;

        int         i = 0;
        vptr< int > tmp{ i };
        V           p1{ tmp };

        CHECK( p1 );

        check_nullable_visit( p1, std::as_const( i ) );
        check_nullable_swap( p1 );

        vptr< float const > p2;

        p1 = p2;

        CHECK_FALSE( p1 );

        vptr< float const > p3;
        p3.visit( []( empty_t ) {}, []( float const& ) {} );
        p3.visit( []( empty_t ) {}, []( vptr< float const > ) {} );

        p3 = p2;

        CHECK_EQ( p2, p3 );
        CHECK_EQ( p2.get(), p3.get() );
}

}  // namespace vari
