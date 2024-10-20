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
#include <optional>
#include <string>
#include <vector>

namespace vari
{

using namespace std::string_literals;

struct tset : typelist< std::string, int, float >
{
};
template <>
struct typelist_traits< tset >
{
        static constexpr bool is_compatible = true;
        using types                         = typename tset::types;
};
template <>
struct typelist_traits< tset const >
{
        static constexpr bool is_compatible = true;
        using types                         = typename tset::types const;
};

TEST_CASE_TEMPLATE(
    "vptr",
    T,
    typelist< int, float, std::string >,
    typelist< std::string, float, int >,
    tset )
{
        using V = vptr< T >;
        static_assert( valid_null_variadic< V > );

        int         i1 = 42;
        std::string s1{ "123456" };
        float       f1 = 0.42;

        SUBCASE( "construction" )
        {
                V p0;
                CHECK( !p0 );
                p0 = &i1;
                CHECK( p0 );
                p0 = V{ nullptr };
                CHECK( !p0 );
                p0 = &s1;
                CHECK( p0 );
                p0 = nullptr;
                CHECK( !p0 );
        }

        SUBCASE( "visit int" )
        {
                V p1{ &i1 };
                check_nullable_visit( p1, i1 );
        }

        SUBCASE( "visit single type" )
        {
                vptr< float > p3 = &f1;
                check_nullable_visit( p3, f1 );
        }

        SUBCASE( "swap int" )
        {
                vptr< int > p1{ &i1 }, p2{ &i1 }, p3;
                check_swap( p1, p2, p3 );
                CHECK( !p2 );
                CHECK( p3 );
        }

        SUBCASE( "swap str" )
        {
                vptr< std::string, int > p1{ &s1 }, p2{ &s1 }, p3;
                check_swap( p1, p2, p3 );
                CHECK( !p2 );
                CHECK( p3 );
        }

        SUBCASE( "complex type" )
        {
                std::vector< int >         vec = { 1, 2, 3, 4 };
                vptr< std::vector< int > > p4( &vec );
                check_nullable_visit( p4, vec );
        }
}

TEST_CASE_TEMPLATE(
    "vref",
    T,
    typelist< int, float, std::string >,
    typelist< std::string, float, int >,
    tset )
{
        using V = vref< T >;
        static_assert( valid_variadic< V > );

        int         i1 = 42;
        std::string s1{ "123456" };

        SUBCASE( "visit" )
        {
                V p1{ s1 };
                check_visit( p1, s1 );
        }

        SUBCASE( "visit single type" )
        {
                vref< std::string > p1{ s1 };
                check_visit( p1, s1 );
        }

        SUBCASE( "swap" )
        {
                V p1{ s1 }, p2{ s1 }, p3{ i1 };

                check_swap( p1, p2, p3 );
        }

        SUBCASE( "get" )
        {
                V p1{ s1 }, p2{ p1 };
                CHECK_EQ( p2.get(), p1.get() );
        }

        SUBCASE( "complex type" )
        {
                std::vector< int >         vec = { 1, 2, 3, 4 };
                vref< std::vector< int > > p1( vec );
                check_visit( p1, vec );
                vref< std::vector< int > > p2( vec );

                std::vector< int >         vec2 = { 42 };
                vref< std::vector< int > > p3( vec2 );
                check_swap( p1, p2, p3 );
        }

        SUBCASE( "reference conversion" )
        {
                vref< std::string > v1{ s1 };

                std::string& s2 = v1;
                CHECK_EQ( s2.c_str(), v1->c_str() );
                std::string const& s3 = v1;
                CHECK_EQ( s3.c_str(), v1->c_str() );
        }

        SUBCASE( "types" )
        {
                // XXX: candidate for generalization over all variadics
                static_assert( std::same_as< typename V::types, typename T::types > );

                vref< std::string const > v1{ s1 };

                std::string                     s2 = "baz";
                vref< typename V::types const > v2 = s2;

                v2 = v1;
                CHECK_EQ( v1, v2 );
        }
}

TEST_CASE_TEMPLATE(
    "uvptr",
    T,
    typelist< int, float, std::string >,
    typelist< std::string, float, int >,
    tset )
{
        using V = uvptr< T >;
        static_assert( valid_null_owning_variadic< V > );

        std::string s1{ "s" };
        int         i1 = 42;

        SUBCASE( "visit" )
        {
                V p1{ uwrap( s1 ) };
                check_nullable_visit( p1, s1 );
        }

        SUBCASE( "swap" )
        {
                V p1, p2, p3{ uwrap( s1 ) };
                check_swap( p1, p2, p3 );
                CHECK( !p3 );
                CHECK( p2 );
        }

        SUBCASE( "simple swap" )
        {
                uvptr< int > p1, p2, p3{ uwrap( 666 ) };
                check_swap( p1, p2, p3 );
                CHECK( !p3 );
                CHECK( p2 );
        }

        SUBCASE( "take" )
        {
                V p1{ uwrap( s1 ) };
                p1 = std::move( p1 ).take(
                    [&]( empty_t ) -> V {
                            FAIL( "incorrect overload" );
                            return V{};
                    },
                    [&]( uvref< int > ) -> V {
                            FAIL( "incorrect overload" );
                            return V{};
                    },
                    [&]( uvref< float, std::string > p ) -> V {
                            return V{ std::move( p ) };
                    } );
                CHECK( p1 );
                check_nullable_visit( p1, s1 );
        }

        SUBCASE( "empty take" )
        {
                V p3 = V();
                std::move( p3 ).take(
                    [&]( empty_t ) {},
                    [&]( uvref< int > ) {
                            FAIL( "incorrect overload" );
                    },
                    [&]( uvref< float, std::string > ) {
                            FAIL( "incorrect overload" );
                    } );
                CHECK( !p3 );
        }

        SUBCASE( "vref" )
        {
                V p1{ uwrap( s1 ) };
                check_vref( p1, s1 );

                uvptr< int > p2{ uwrap( i1 ) };
                check_vref( p2, i1 );
        }

        SUBCASE( "uvref" )
        {
                V p1{ uwrap( s1 ) };
                check_uvref( p1, s1 );

                uvptr< int > p2{ uwrap( i1 ) };
                check_uvref( p2, i1 );
        }

        SUBCASE( "get" )
        {
                V p1{ uwrap( s1 ) };

                vptr< int, float, std::string > p2 = p1.get();
                CHECK_EQ( p2.get(), p1.get().get() );
        }

        SUBCASE( "nullable" )
        {
                V p1{ uwrap( s1 ) };
                CHECK( p1 );
                p1 = nullptr;
                CHECK_FALSE( p1 );
        }

        SUBCASE( "selfmove" )
        {
                V p1{ uwrap( s1 ) };
                CHECK( p1 );
                p1 = std::move( p1 );
                CHECK( p1 );

                V p2;
                CHECK( !p2 );
                p2 = std::move( p2 );
                CHECK( !p2 );
        }

        SUBCASE( "uvref move" )
        {
                V            p1;
                uvref< int > r1 = uwrap( int{ 42 } );

                CHECK( !p1 );
                p1 = std::move( r1 );
                CHECK( p1 );
        }

        SUBCASE( "deref" )
        {
                uvptr< int > p1{ new int{ 33 } };
                CHECK_EQ( *p1, 33 );

                uvptr< int > p2{ (int*) nullptr };
                CHECK( !p2 );
                uvptr< int > p3{ nullptr };
                CHECK( !p3 );

                uvptr< std::string > p4{ new std::string{ "wololo" } };
                CHECK_EQ( p4->front(), 'w' );
        }

        SUBCASE( "vptr" )
        {
                uvptr< std::string > p1{ new std::string{ "wololo" } };
                vptr< std::string >  p2 = p1;
                CHECK_EQ( p2->c_str(), p1->c_str() );

                vptr< std::string > p3;
                p3 = p1;
                CHECK_EQ( p3->c_str(), p1->c_str() );
        }

        static_assert( lvalue_reference_convertible_only< vptr< int >, uvptr< int > > );
}

TEST_CASE_TEMPLATE(
    "uvref",
    T,
    typelist< int, float, std::string >,
    typelist< std::string, float, int >,
    tset )
{
        using V = uvref< T >;

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

        vptr< int, float, std::string > p2{ p1.get() };
        CHECK_EQ( p2.get(), p1.get().get() );

        p1 = uwrap( float{ 42 } );

        using tset = typelist< std::string, int >;

        std::vector< uvref< tset > > vec1, vec2;

        vec2.emplace_back( uwrap( "wololo"s ) );

        vec1 = std::move( vec2 );

        uvref< int > p3 = uwrap( 42 );
        std::move( p3 ).take( [&]( uvref< int > u ) {
                CHECK_EQ( *u, 42 );
        } );

        uvref< int > p4 = uwrap( 666 );
        uvref< int > p5 = uwrap( 42 );
        CHECK_EQ( *p4, 666 );
        swap( p5, p4 );
        CHECK_EQ( *p4, 42 );
        CHECK_EQ( *p5, 666 );

        uvref< int > p6 = uwrap( 666 );
        CHECK_EQ( p6.vptr().index(), 0 );

        uvptr< int > p7 = std::move( p6 ).vptr();
        CHECK_EQ( p7.index(), 0 );

        static_assert( lvalue_reference_convertible_only< vref< int >, uvref< int > > );
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

template < template < typename... > typename T >
void try_null_set()
{
        std::set< T< int, std::string > > s3;
        std::vector< int >                idata{ 127, 0 };
        std::vector< std::string >        sdata{ 127, "wololo" };

        for ( int& i : idata )
                s3.insert( T< int >{ &i } );

        for ( std::string& s : sdata )
                s3.insert( T< std::string >{ &s } );

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

        try_null_set< vptr >();
        try_set< vref >();
        try_upset();
        try_urset();
}

TEST_CASE( "reference to functor" )
{
        struct foo
        {
                foo()                        = default;
                foo( foo const& )            = delete;
                foo( foo&& )                 = delete;
                foo& operator=( foo const& ) = delete;
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
        vptr< int, std::string > p1{ &i };
        p1.visit( f );
        uvref< int, std::string > r2 = uwrap( std::string{ "wololo" } );
        r2.visit( f );
        uvptr< int, std::string > p2{ uwrap( std::string{ "wololo" } ) };
        p2.visit( f );
}

TEST_CASE( "moved functor" )
{
        struct foo
        {
                std::size_t count = 0;

                foo()                        = default;
                foo( foo const& )            = delete;
                foo( foo&& )                 = default;
                foo& operator=( foo const& ) = delete;
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
        vptr< int, std::string > p1{ &i };
        f = p1.visit( std::move( f ) );

        foo                       f3;
        uvref< int, std::string > r2 = uwrap( std::string{ "wololo" } );

        f = r2.visit( std::move( f ) );

        foo                       f4;
        uvptr< int, std::string > p2{ uwrap( std::string{ "wololo" } ) };

        f = p2.visit( std::move( f ) );

        CHECK_EQ( f.count, 4 );
}

template < typename T >
struct rec_tset
{
        uvref< T, rec_tset< T > > item;
};

TEST_CASE_TEMPLATE( "recursive set", T, typelist< int, float >, tset )
{
        using V = vref< T, rec_tset< T > >;

        int i1;
        V   v1{ i1 };
}

TEST_CASE_TEMPLATE(
    "const vptr",
    T,
    typelist< int const, float const, std::string const >,
    typelist< std::string const, float const, int const >,
    tset const )
{
        using V = vptr< T >;

        int         i = -1;
        vptr< int > tmp{ &i };

        SUBCASE( "visit" )
        {
                V p1{ tmp };
                CHECK( p1 );

                check_nullable_visit( p1, std::as_const( i ) );
        }

        SUBCASE( "swap" )
        {
                V p1{ tmp }, p2{ tmp }, p3;
                check_swap( p1, p2, p3 );
                CHECK( !p2 );
                CHECK( p3 );
        }

        SUBCASE( "assign" )
        {
                V                   p1{ tmp };
                vptr< float const > p2;

                p1 = p2;

                CHECK( !p1 );
        }

        SUBCASE( "compile conversions" )
        {
                vptr< float const > p3;
                p3.visit( []( empty_t ) {}, []( float const& ) {} );
                p3.visit( []( empty_t ) {}, []( vref< float const > ) {} );

                auto        f = []( vref< int const > ) {};
                vref< int > r1{ i };
                f( r1 );
        }
}

}  // namespace vari
