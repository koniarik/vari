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
        using V = vptr< void, int, float, std::string >;

        int i;
        V   p1{ i };

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
            [&]( vptr< void, int > ) {},
            [&]( vptr< void, bool > ) {},
            [&]( vptr< void, float, std::string > ) {
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

        p3 = p2;

        CHECK_EQ( p2, p3 );
        CHECK_EQ( p2.get(), p3.get() );
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
            [&]( vptr< void, float, std::string > ) {} );

        p1 = std::move( p1 ).take(
            [&]( empty_t ) -> V {
                    FAIL( "incorrect overload" );
                    return V{};
            },
            [&]( uvptr< void, int > ) -> V {
                    FAIL( "incorrect overload" );
                    return V{};
            },
            [&]( uvptr< void, float, std::string > p ) -> V {
                    return p;
            } );

        vptr< void, int, float, std::string > p2 = p1.get();
        CHECK_EQ( p2.get(), p1.get().get() );
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
                    return V{ k };
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
                bits::dispatch_index< 0, 128 >( i, [&]< std::size_t j > {
                        CHECK_EQ( i, j );
                } );
}

TEST_CASE( "cmp" )
{
        std::set< vptr< void, int > > s1;
        s1.insert( vptr< void, int >{} );

        std::set< vptr< void, int, std::string > > s2;
        s2.insert( vptr< void, int >{} );
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

}  // namespace vari