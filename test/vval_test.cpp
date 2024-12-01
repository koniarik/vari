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

#include "./common.h"
#include "test_types.h"
#include "vari/vopt.h"

#include <doctest/doctest.h>
#include <source_location>
#include <vector>

namespace vari
{
using namespace std::string_literals;

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
        CHECK_EQ( c1.index, null_index );

        c1.emplace< float >( 0.1f );
        CHECK_EQ( c1.index, 0 );

        c1.destroy();
        CHECK_EQ( c1.index, null_index );

        c1.emplace< char >( 'w' );
        CHECK_EQ( c1.index, 1 );

        c1.destroy();
}

struct throw_mv_const : move_constructible< nothrow::NO >
{
};

struct throw_cp_const : copy_constructible< nothrow::NO >
{
        throw_cp_const() noexcept                   = default;
        throw_cp_const( throw_cp_const const& )     = default;
        throw_cp_const( throw_cp_const&& ) noexcept = default;
};

struct throw_const : move_constructible< nothrow::NO >, copy_constructible< nothrow::NO >
{
};

/*
    typelist< int >,
    typelist< float, int >,
    typelist< throw_const >,
    typelist< int, throw_const >,
    typelist< throw_mv_const >,
    typelist< int, throw_mv_const >,
    typelist< throw_cp_const >,
    typelist< int, throw_cp_const >,
    big_set )
*/

template < typename T, typename U >
void vval_construct_test(
    U&&                  thing,
    nothrow              nt,
    std::size_t          index,
    std::source_location sl = std::source_location::current() )
{
        INFO( std::string{ sl.file_name() }, ":", sl.line() );
        T v1{ (U&&) thing };
        CHECK_EQ( v1.index(), index );
        CHECK( noexcept( T{ (U&&) thing } ) == ( nt == nothrow::YES ) );

        T v2{ std::in_place_type_t< std::remove_cvref_t< U > >{}, (U&&) thing };
        CHECK_EQ( v2.index(), index );
        CHECK(
            noexcept( T{ std::in_place_type_t< std::remove_cvref_t< U > >{}, (U&&) thing } ) ==
            ( nt == nothrow::YES ) );

        T v3{ std::in_place_type_t< int >{}, 42 };
        v3 = (U&&) thing;
        CHECK_EQ( v1.index(), index );
        CHECK( noexcept( v3 = (U&&) thing ) == ( nt == nothrow::YES ) );

        T v4{ std::in_place_type_t< int >{}, 42 };
        v4.template emplace< std::remove_cvref_t< U > >( (U&&) thing );
        CHECK_EQ( v1.index(), index );
        CHECK(
            noexcept( v4.template emplace< std::remove_cvref_t< U > >( (U&&) thing ) ) ==
            ( nt == nothrow::YES ) );
        v4.template emplace< int >( 42 );

        if constexpr ( T::types::size < 32 ) {

                vref< typename T::types > vrf1{ v1.vref() };
                CHECK_EQ( vrf1.index(), index );

                vptr< typename T::types > vpr1{ v2.vptr() };
                CHECK_EQ( vpr1.index(), index );

                vref< typename T::types const > vrf2{ v1.vref() };
                CHECK_EQ( vrf2.index(), index );

                vptr< typename T::types const > vpr2{ v2.vptr() };
                CHECK_EQ( vpr2.index(), index );

                vref< typename T::types const > vrf3{ std::as_const( v1 ).vref() };
                CHECK_EQ( vrf3.index(), index );

                vptr< typename T::types const > vpr3{ std::as_const( v2 ).vptr() };
                CHECK_EQ( vpr3.index(), index );
        }
}

template < template < typename... X > class T >
void test_construct()
{

        int i = 42;
        vval_construct_test< T< int > >( i, nothrow::YES, 0 );
        vval_construct_test< T< int > >( 42, nothrow::YES, 0 );
        vval_construct_test< T< float, int > >( i, nothrow::YES, 1 );
        vval_construct_test< T< float, int > >( 42, nothrow::YES, 1 );
        vval_construct_test< T< float, int > >( 42.0F, nothrow::YES, 0 );
        throw_const tc;
        vval_construct_test< T< throw_const, int > >( tc, nothrow::NO, 0 );
        vval_construct_test< T< throw_const, int > >( throw_const{}, nothrow::NO, 0 );
        vval_construct_test< T< int, throw_const > >( tc, nothrow::NO, 1 );
        vval_construct_test< T< int, throw_const > >( throw_const{}, nothrow::NO, 1 );
        vval_construct_test< T< int, throw_const > >( i, nothrow::YES, 0 );
        vval_construct_test< T< int, throw_const > >( 42, nothrow::YES, 0 );
        throw_mv_const mtc;
        vval_construct_test< T< throw_mv_const, int > >( mtc, nothrow::YES, 0 );
        vval_construct_test< T< throw_mv_const, int > >( throw_mv_const{}, nothrow::NO, 0 );
        vval_construct_test< T< int, throw_mv_const > >( mtc, nothrow::YES, 1 );
        vval_construct_test< T< int, throw_mv_const > >( throw_mv_const{}, nothrow::NO, 1 );
        throw_cp_const ctc;
        vval_construct_test< T< throw_cp_const, int > >( ctc, nothrow::NO, 0 );
        vval_construct_test< T< throw_cp_const, int > >( throw_cp_const{}, nothrow::YES, 0 );
        vval_construct_test< T< int, throw_cp_const > >( ctc, nothrow::NO, 1 );
        vval_construct_test< T< int, throw_cp_const > >( throw_cp_const{}, nothrow::YES, 1 );
}

TEST_CASE( "vval_construct" )
{
        test_construct< _vval >();
        for ( std::size_t i = 0; i < big_set::size; i++ )
                _dispatch_index< 0, big_set::size >( i, [&]< std::size_t j >() -> decltype( auto ) {
                        tag< j > tg{};
                        vval_construct_test< vval< big_set, int > >( tg, nothrow::NO, j );
                        vval_construct_test< vval< big_set, int > >( tag< j >{}, nothrow::YES, j );
                } );

        test_construct< _vopt >();
        for ( std::size_t i = 0; i < big_set::size; i++ )
                _dispatch_index< 0, big_set::size >( i, [&]< std::size_t j >() -> decltype( auto ) {
                        tag< j > tg{};
                        vval_construct_test< vopt< big_set, int > >( tg, nothrow::NO, j );
                        vval_construct_test< vopt< big_set, int > >( tag< j >{}, nothrow::YES, j );
                } );
}

template < typename To, typename From >
void vval_copy_test(
    From&&               val,
    nothrow              nt,
    std::size_t          index,
    std::source_location sl = std::source_location::current() )
{
        INFO( std::string{ sl.file_name() }, ":", sl.line() );
        To v1{ val };
        CHECK_EQ( v1.index(), index );
        CHECK( noexcept( To{ val } ) == ( nt == nothrow::YES ) );

        To v2{ std::in_place_type_t< int >{}, 42 };
        v2 = val;
        CHECK_EQ( v2.index(), index );
        CHECK( noexcept( v2 = val ) == ( nt == nothrow::YES ) );
}

TEST_CASE( "vval_copy" )
{
        vval_copy_test< vval< int > >( vval< int >{ 42 }, nothrow::YES, 0 );
        vval_copy_test< vval< int, float > >( vval< int >{ 42 }, nothrow::YES, 0 );
        vval_copy_test< vval< std::string, int > >( vval< int >{ 42 }, nothrow::YES, 1 );
        vval_copy_test< vval< std::string, int > >(
            vval< std::string >{ "wololo"s }, nothrow::NO, 0 );

        for ( std::size_t i = 0; i < big_set::size; i++ )
                _dispatch_index< 0, big_set::size >( i, [&]< std::size_t j >() -> decltype( auto ) {
                        vval< tag< 0 >, tag< 4 >, tag< j >, tag< 42 > > tg{ tag< j >{} };
                        vval_copy_test< vval< big_set, int > >( tg, nothrow::NO, j );
                } );
}

template < typename To, typename From >
void vval_move_test(
    From&&               val,
    nothrow              nt,
    std::size_t          index,
    std::source_location sl = std::source_location::current() )
{
        INFO( std::string{ sl.file_name() }, ":", sl.line() );
        To v1{ std::move( val ) };
        CHECK_EQ( v1.index(), index );
        CHECK( noexcept( To{ std::move( val ) } ) == ( nt == nothrow::YES ) );
}

struct vval_move_constr : move_constructible< nothrow::NO >
{
};

TEST_CASE( "vval_move" )
{
        vval_move_test< vval< int > >( vval< int >{ 42 }, nothrow::YES, 0 );
        vval_move_test< vval< int, float > >( vval< int >{ 42 }, nothrow::YES, 0 );
        vval_move_test< vval< vval_move_constr, int > >( vval< int >{ 42 }, nothrow::YES, 1 );
        vval_move_test< vval< vval_move_constr, int > >(
            vval< vval_move_constr >{ vval_move_constr{} }, nothrow::NO, 0 );

        for ( std::size_t i = 0; i < big_set::size; i++ )
                _dispatch_index< 0, big_set::size >( i, [&]< std::size_t j >() -> decltype( auto ) {
                        vval< tag< 0 >, tag< 4 >, tag< j >, tag< 42 > > tg{ tag< j >{} };
                        vval_move_test< vval< big_set > >( std::move( tg ), nothrow::YES, j );
                } );
}

TEST_CASE( "vval_deref" )
{
        vval< std::string > v1{ "wololo"s };
        std::string         vv{ "wololo"s };
        CHECK_EQ( *v1, vv );
        CHECK_EQ( v1->size(), vv.size() );
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

        vval< std::vector< float >, int >        v6{ v5 };
        vval< std::vector< float >, int, float > v7{ std::move( v6 ) };
}

TEST_CASE( "vopt_visit" )
{
        // static_assert( valid_null_variadic< vopt< float, int > > );
        // static_assert( valid_null_variadic< vopt< float > > );

        float                           fv = 0.1f;
        vopt< float, int, std::string > v1{ fv };
        vopt< float, int, std::string > v1_5{ 0.1f };
        check_nullable_visit( v1, fv );
}

TEST_CASE( "vval_swap" )
{
        vval< int, float > v1{ 42 }, v2{ 42 }, v3{ 3.141592f };

        check_swap( v1, v2, v3 );
}


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

TEST_CASE( "to_uvref" )
{
        vval< int, float > v1{ 42 };

        uvref< int, float > r = to_uvref( std::move( v1 ) );
        CHECK_EQ( r.index(), 0 );
        r.visit(
            [&]( int& i ) {
                    CHECK_EQ( i, 42 );
            },
            [&]( float& ) {
                    FAIL( "bad branch" );
            } );
}

}  // namespace vari
