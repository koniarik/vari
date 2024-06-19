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
#include "vari/uvref.h"
#include "vari/vref.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

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

static_assert( vconvertible_to< const void, ivst_tl1, const void, ivst_tl1 > );
static_assert( vconvertible_to< void, ivst_tl2, const void, ivst_tl1 > );
static_assert( vconvertible_to< void, ivst_tl2, void, ivst_tl2 > );

void check_cmp_operators( auto& lh, auto& rh )
{
        std::ignore = lh == rh;
        std::ignore = lh < rh;
        std::ignore = lh <= rh;
        std::ignore = lh >= rh;
        std::ignore = lh > rh;
        std::ignore = lh != rh;
}

TEST_CASE( "vptr" )
{
        using V = vptr< void, int, float, std::string >;

        V p0{};
        CHECK_FALSE( p0 );

        p0 = V{ nullptr };
        CHECK_FALSE( p0 );

        int i = 42;
        p0    = V{ i };

        V p1{ i };
        CHECK( p1 );
        p1.visit(
            [&]( empty_t ) {
                    FAIL( "incorrect overload" );
            },
            [&]( int& ) {},
            [&]( float& ) {
                    FAIL( "incorrect overload" );
            },
            [&]( std::string& ) {
                    FAIL( "incorrect overload" );
            } );

        p1.match(
            [&]( empty_t ) {
                    FAIL( "incorrect overload" );
            },
            [&]( vref< void, int > ) {},
            [&]( vptr< void, bool > ) {},
            [&]( vref< void, float, std::string > ) {
                    FAIL( "incorrect overload" );
            } );

        int& ii = p1.visit(
            [&]( empty_t ) -> int& {
                    return i;
            },
            [&]( int& ) -> int& {
                    return i;
            },
            [&]( float& ) -> int& {
                    return i;
            },
            [&]( std::string& ) -> int& {
                    return i;
            } );
        CHECK_EQ( &ii, &i );

        vptr< void, float > p2;

        CHECK_FALSE( p2 );

        p1 = p2;

        CHECK_FALSE( p1 );

        vptr< void, float > p3;
        p3.visit( []( empty_t ) {}, []( float& ) {} );
        p3.match( []( empty_t ) {}, []( vptr< void, float > ) {} );

        p3 = p2;

        CHECK_EQ( p2, p3 );
        CHECK_EQ( p2.get(), p3.get() );

        swap( p3, p2 );
        CHECK_EQ( p2.get(), p3.get() );

        check_cmp_operators( p3, p2 );

        vptr< void, int > p4{ i };
        CHECK( p4 );
        p4 = nullptr;
        CHECK_FALSE( p4 );
}
struct base_t
{
        int v;
};

template < std::size_t i >
struct derived_t : base_t
{
};

TEST_CASE( "vptr_nonvoid" )
{
        using V = vptr< base_t, derived_t< 0 >, derived_t< 1 > >;

        derived_t< 0 > d;
        d.v = 42;

        V p0{ d };

        CHECK_EQ( ( *p0 ).v, 42 );
        CHECK_EQ( p0->v, 42 );

        V p1;
        swap( p0, p1 );

        CHECK_EQ( ( *p1 ).v, 42 );
        CHECK_EQ( p1->v, 42 );
        CHECK_FALSE( p0 );

        check_cmp_operators( p0, p1 );
}

static_assert( std::same_as< type_at_t< 0, typelist< int, float, std::string > >, int > );
static_assert( std::same_as< type_at_t< 1, typelist< int, float, std::string > >, float > );
static_assert( std::same_as< type_at_t< 2, typelist< int, float, std::string > >, std::string > );

TEST_CASE( "vref" )
{
        using V = vref< void, int, float, std::string >;

        std::string i{ "123456" };
        V           p1{ i };

        p1.visit(
            [&]( int& ) {
                    FAIL( "incorrect overload" );
            },
            [&]( float& ) {
                    FAIL( "incorrect overload" );
            },
            [&]( std::string& ) {} );

        std::string& ii = p1.visit( [&]( auto& ) -> std::string& {
                return i;
        } );
        CHECK_EQ( i.data(), ii.data() );

        p1.match(
            [&]( vref< void, int > ) {
                    FAIL( "incorrect overload" );
            },
            [&]( vref< void, float, std::string > ) {} );

        vptr< void, int, float, std::string > p2 = p1;
        vptr< void, int >                     p3;

        std::vector< int >               vec = { 1, 2, 3, 4 };
        vptr< void, std::vector< int > > p4( vec );
        CHECK_EQ( p2.get(), p1.get() );

        check_cmp_operators( p2, p1 );
}

TEST_CASE( "vref_nonvoid" )
{
        using V = vref< base_t, derived_t< 0 >, derived_t< 1 > >;

        derived_t< 0 > d0;
        d0.v = 42;

        V p0{ d0 };

        CHECK_EQ( ( *p0 ).v, 42 );
        CHECK_EQ( p0->v, 42 );

        derived_t< 0 > d1;
        d1.v = 777;
        V p1{ d1 };

        swap( p0, p1 );

        CHECK_EQ( ( *p1 ).v, 42 );
        CHECK_EQ( p1->v, 42 );

        CHECK_EQ( ( *p0 ).v, 777 );
        CHECK_EQ( p0->v, 777 );
        d1.v = 666;

        CHECK_EQ( ( *p0 ).v, 666 );
        CHECK_EQ( p0->v, 666 );

        check_cmp_operators( p0, p1 );
}

TEST_CASE( "uvptr" )
{
        using V = uvptr< void, int, float, std::string >;

        V p1 = uwrap< void >( std::string{ "s" } );

        p1.visit(
            [&]( empty_t ) {
                    FAIL( "incorrect overload" );
            },
            [&]( int& ) {
                    FAIL( "incorrect overload" );
            },
            [&]( float& ) {
                    FAIL( "incorrect overload" );
            },
            [&]( std::string& ) {} );

        p1.match(
            [&]( empty_t ) {
                    FAIL( "incorrect overload" );
            },
            [&]( vptr< void, int > ) {
                    FAIL( "incorrect overload" );
            },
            [&]( vref< void, float, std::string > ) {} );

        p1 = std::move( p1 ).take(
            [&]( empty_t ) -> V {
                    FAIL( "incorrect overload" );
                    return V{};
            },
            [&]( uvref< void, int > ) -> V {
                    FAIL( "incorrect overload" );
                    return V{};
            },
            [&]( uvptr< void, float, std::string > p ) -> V {
                    return p;
            } );

        std::optional< vref< void, int, float, std::string > > opt_ref = p1.ref();
        CHECK( opt_ref );
        opt_ref->visit( [&]( int& ) {}, [&]( float& ) {}, [&]( std::string& ) {} );

        vptr< void, int, float, std::string > p2 = p1.get();
        CHECK_EQ( p2.get(), p1.get().get() );

        CHECK( p1 );
        p1 = nullptr;
        CHECK_FALSE( p1 );

        V p3 = V();

        std::move( p3 ).take(
            [&]( empty_t ) {},
            [&]( uvref< void, int > ) {
                    FAIL( "incorrect overload" );
            },
            [&]( uvptr< void, float, std::string > ) {
                    FAIL( "incorrect overload" );
            } );

        p3 = std::move( p3 );

        uvref< void, int > r1 = uwrap< void >( int{ 42 } );

        CHECK_FALSE( p3 );
        p3 = std::move( r1 );
        CHECK( p3 );

        uvptr< void, int > p4{ new int{ 33 } };
        CHECK_EQ( *p4, 33 );

        uvptr< void, int > p5{ (int*) nullptr };
        CHECK_FALSE( p5 );

        uvptr< void, std::string > p6{ new std::string{ "wololo" } };
        CHECK_EQ( p6->front(), 'w' );

        vptr< void, std::string > p7 = p6;
        CHECK_EQ( p7->c_str(), p6->c_str() );

        std::optional< uvref< void, std::string > > r8 = std::move( p6 ).ref();
        CHECK( r8 );
        r8 = std::move( p6 ).ref();
        CHECK_FALSE( r8 );
}

TEST_CASE( "uvref" )
{
        using V = uvref< void, int, float, std::string >;

        V p1 = uwrap< void >( std::string{ "s" } );

        p1.visit(
            [&]( int& ) {
                    FAIL( "incorrect overload" );
            },
            [&]( float& ) {
                    FAIL( "incorrect overload" );
            },
            [&]( std::string& ) {} );

        p1.match(
            [&]( vref< void, int > ) {
                    FAIL( "incorrect overload" );
            },
            [&]( vref< void, float, std::string > ) {} );

        int k;
        p1 = std::move( p1 ).take(
            [&]( uvref< void, int > ) -> V {
                    FAIL( "incorrect overload" );
                    return uwrap< void >( k );
            },
            [&]( uvref< void, float, std::string > r ) -> V {
                    return r;
            } );

        vptr< void, int, float, std::string > p2 = p1.get();
        CHECK_EQ( p2.get(), p1.get().get() );
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
        std::set< T< void, int, std::string > > s3;
        std::vector< int >                      idata{ 127, 0 };
        std::vector< std::string >              sdata{ 127, "wololo" };

        for ( int& i : idata )
                s3.insert( T< void, int >{ i } );

        for ( std::string& s : sdata )
                s3.insert( T< void, std::string >{ s } );

        CHECK_EQ( idata.size() + sdata.size(), s3.size() );
}

void try_upset()
{
        std::set< uvptr< void, int, std::string > > s3;
        static constexpr std::size_t                n = 128;
        for ( std::size_t i = 0; i < n; i++ )
                s3.insert( uvptr< void, int >{ new int{ 42 } } );
        for ( std::size_t i = 0; i < n; i++ )
                s3.insert( uvptr< void, std::string >{ new std::string{ "wololo" } } );

        CHECK_EQ( 2 * n, s3.size() );
}

void try_urset()
{
        std::set< uvref< void, int, std::string > > s3;
        static constexpr std::size_t                n = 128;
        for ( std::size_t i = 0; i < n; i++ )
                s3.insert( uvref< void, int >{ *new int{ 42 } } );
        for ( std::size_t i = 0; i < n; i++ )
                s3.insert( uvref< void, std::string >{ *new std::string{ "wololo" } } );

        CHECK_EQ( 2 * n, s3.size() );
}

TEST_CASE( "cmp" )
{
        std::set< vptr< void, int > > s1;
        s1.insert( vptr< void, int >{} );

        std::set< vptr< void, int, std::string > > s2;
        s2.insert( vptr< void, int >{} );

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

        vref< void, int, std::string > r1{ i };
        r1.visit( f );
        vptr< void, int, std::string > p1{ i };
        p1.visit( f );
        uvref< void, int, std::string > r2 = uwrap< void >( std::string{ "wololo" } );
        r2.visit( f );
        uvptr< void, int, std::string > p2 = uwrap< void >( std::string{ "wololo" } );
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

        vref< void, int, std::string > r1{ i };
        f = r1.visit( std::move( f ) );

        foo                            f2;
        vptr< void, int, std::string > p1{ i };
        f = p1.visit( std::move( f ) );

        foo                             f3;
        uvref< void, int, std::string > r2 = uwrap< void >( std::string{ "wololo" } );

        f = r2.visit( std::move( f ) );

        foo                             f4;
        uvptr< void, int, std::string > p2 = uwrap< void >( std::string{ "wololo" } );

        f = p2.visit( std::move( f ) );

        CHECK_EQ( f.count, 4 );
}

TEST_CASE( "const vptr" )
{
        using V = vptr< const void, const int, const float, const std::string >;

        int               i = 0;
        vptr< void, int > tmp{ i };
        V                 p1{ tmp };

        CHECK( p1 );

        p1.visit(
            [&]( empty_t ) {
                    FAIL( "incorrect overload" );
            },
            [&]( int const& ) {},

            [&]( const std::string& ) {
                    FAIL( "incorrect overload" );
            } );

        p1.match(
            [&]( empty_t ) {
                    FAIL( "incorrect overload" );
            },
            [&]( vptr< const void, int const > ) {},
            [&]( vptr< const void, float const, std::string const > ) {
                    FAIL( "incorrect overload" );
            } );

        int const& ii = p1.visit(
            [&]( empty_t ) -> int const& {
                    return i;
            },
            [&]( int const& ) -> int const& {
                    return i;
            },
            [&]( std::string const& ) -> int const& {
                    return i;
            } );
        CHECK_EQ( &ii, &i );

        vptr< void const, float const > p2;

        p1 = p2;

        CHECK_FALSE( p1 );

        vptr< void const, float const > p3;
        p3.visit( []( empty_t ) {}, []( float const& ) {} );
        p3.match( []( empty_t ) {}, []( vptr< void const, float const > ) {} );

        p3 = p2;

        CHECK_EQ( p2, p3 );
        CHECK_EQ( p2.get(), p3.get() );
}

}  // namespace vari
