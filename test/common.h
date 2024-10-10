#include "vari/bits/typelist.h"
#include "vari/forward.h"
#include "vari/uvref.h"
#include "vari/vref.h"

#include <doctest/doctest.h>

#pragma once

namespace vari
{

template < typename T >
struct first_type;

template < typename T, typename... Ts >
struct first_type< typelist< T, Ts... > >
{
        using type = T;
        using rest = typelist< Ts... >;
};

template < typename T >
using first_type_t = typename first_type< T >::type;

template < typename T, typename TL >
concept one_of = contains_type_v< T, TL >;

template < typename T >
concept null_visitable = requires( T t ) {
        typename T::types;
        t.visit( []( empty_t ) {}, []< one_of< typename T::types > U >( U& ) {} );
        {
                t
        } -> std::convertible_to< bool >;
} && std::is_default_constructible_v< T >;

template < typename T >
concept visitable = requires( T t ) { t.visit( []< one_of< typename T::types > U >( U& ) {} ); };

template < typename T >
concept owning = !std::is_copy_constructible_v< T > && !std::is_copy_assignable_v< T >;

template < typename T >
concept valid_variadic_base = std::totally_ordered< T > && std::is_nothrow_move_assignable_v< T > &&
                              std::is_nothrow_move_constructible_v< T >;

template < typename T >
concept valid_variadic = valid_variadic_base< T > && visitable< T >;

template < typename T >
concept valid_null_variadic = valid_variadic_base< T > && null_visitable< T >;

template < typename T >
concept valid_owning_variadic = valid_variadic< T > && owning< T >;

template < typename T >
concept valid_null_owning_variadic = valid_null_variadic< T > && owning< T >;


template < typename V, typename T >
decltype( auto ) check_visit_ref_return( V& variadic, T& val )
{
        static_assert( contains_type_v< T, typename V::types > );

        return variadic.visit( [&]( auto&& item ) -> T& {
                return val;
        } );
}

template < typename V >
void check_swap( V& a, V& b, V& c )
{
        CHECK_EQ( a, b );
        CHECK_NE( a, c );

        swap( b, c );

        CHECK_NE( a, b );
        CHECK_EQ( a, c );
}

template < visitable V, typename T >
void check_visit( V& variadic, T& val )
{
        static_assert( contains_type_v< T, typename V::types > );
        using F  = first_type_t< typename V::types >;
        using FS = typename first_type< typename V::types >::rest;

        std::size_t c = 0;
        variadic.visit( [&]< typename U >( U item ) {
                c++;
                if constexpr ( std::same_as< U, T > )
                        CHECK_EQ( item, val );
                else
                        FAIL( "incorrect overload" );
        } );
        CHECK_EQ( c, 1 );
        variadic.visit( [&]( vref< typename V::types > item ) {
                c++;
        } );
        CHECK_EQ( c, 2 );

        auto& ref = check_visit_ref_return( variadic, val );
        CHECK_EQ( &ref, &val );

        if constexpr ( FS::size > 0 ) {
                variadic.visit(
                    [&]( vref< F > ) {
                            c++;
                    },
                    [&]( vref< FS > ) {
                            c++;
                    } );
                CHECK_EQ( c, 3 );
        }
}

template < typename V, typename T >
void check_vref( V& variadic, T& val )
{
        using VT = vref< typename std::decay_t< V >::types >;
        VT v     = variadic.vref();
        check_visit( v, val );
}

template < typename V, typename T >
void check_uvref( V& variadic, T& val )
{
        using VT = uvref< typename std::decay_t< V >::types >;
        VT v     = std::move( variadic ).vref();
        check_visit( v, val );
}

template < null_visitable V, typename T >
void check_nullable_visit( V& variadic, T& val )
{
        static_assert( contains_type_v< T, typename V::types > );
        using F  = first_type_t< typename V::types >;
        using FS = typename first_type< typename V::types >::rest;

        CHECK( variadic );

        std::size_t c = 0;
        variadic.visit(
            [&]( empty_t ) {
                    FAIL( "incorrect overload" );
            },
            [&]< one_of< typename V::types > U >( U& item ) {
                    c++;
                    if constexpr ( std::same_as< U, T > )
                            CHECK_EQ( item, val );
                    else
                            FAIL( "incorrect overload" );
            } );
        CHECK_EQ( c, 1 );
        variadic.visit(
            [&]( empty_t ) {
                    FAIL( "incorrect overload" );
            },
            [&]( vref< typename V::types > item ) {
                    c++;
            } );
        CHECK_EQ( c, 2 );

        V n;
        CHECK_FALSE( n );
        n.visit(
            [&]( empty_t ) {
                    c++;
            },
            [&]( vref< typename V::types > item ) {
                    FAIL( "incorrect overload" );
            } );
        CHECK_EQ( c, 3 );

        auto& ref = check_visit_ref_return( variadic, val );
        CHECK_EQ( &ref, &val );

        if constexpr ( FS::size > 0 ) {
                variadic.visit(
                    [&]( empty_t ) {},
                    [&]( vref< F > ) {
                            c++;
                    },
                    [&]( vref< FS > ) {
                            c++;
                    } );
                CHECK_EQ( c, 4 );
        }
}

}  // namespace vari
