/// MIT License
///
/// Copyright (c) 2025 koniarik
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#pragma once

#include "vari/bits/typelist.h"

#include <algorithm>
#include <bit>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>

namespace vari
{

struct empty_t
{
};

static constexpr empty_t empty;


template < template < typename... > typename T, typename TL, typename... Us >
struct _vptr_apply;

template < template < typename... > typename T, typename... Ts, typename... Us >
struct _vptr_apply< T, typelist< Ts... >, Us... >
{
        using type = T< Us..., Ts... >;
};

template < template < typename... > typename T, typename TL, typename... Us >
using _vptr_apply_t = typename _vptr_apply< T, TL, Us... >::type;

/// Given a templated type `T` and `typelist` of types `TL`, aliases `T<Us...>` where `Us...` is
/// flattend version of `TL` without any duplicates. If `Extra...` list is provided, it is appended
/// *before* `Us...`: `T<Extra...,Us...>`
template < template < typename... > typename T, typename TL, typename... Extra >
using _define_variadic = _vptr_apply_t< T, unique_typelist_t< flatten_t< TL > >, Extra... >;

template < typename F, typename... Args >
concept invocable = requires( F&& f, Args&&... args ) { ( (F&&) f )( (Args&&) args... ); };

// XXX: test
template < typename U >
concept _forward_nothrow_constructible =
    ( std::is_lvalue_reference_v< U > ?
          std::is_nothrow_copy_constructible_v< std::remove_reference_t< U > > :
          std::is_nothrow_move_constructible_v< U > );

template < typename T, typename... Fs >
concept invocable_for_one = ( (+invocable< Fs, T >) +... ) == 1;

template < typename F, typename... Ts >
concept invocable_with_any = ( invocable< F, Ts > || ... || false );

template < typename Deleter, typename... Ts >
struct _uvref;

template < typename T >
struct _check_unique_invocability;

template < typename... Ts >
struct _check_unique_invocability< typelist< Ts... > >
{
        template < typename... Fs >
        struct with_pure_value
        {
                static_assert(
                    ( invocable_for_one< Ts, Fs... > && ... ),
                    "For each type, there has to be one and only one callable" );
                static_assert(
                    ( invocable_with_any< Fs, Ts... > && ... ),
                    "For each function, there has to be at least one type it is invocable with" );
        };

        template < typename... Fs >
        struct with_pure_ref
        {
                static_assert(
                    ( invocable_for_one< Ts&, Fs... > && ... ),
                    "For each type, there has to be one and only one callable" );
                static_assert(
                    ( invocable_with_any< Fs, Ts&... > && ... ),
                    "For each function, there has to be at least one type it is invocable with" );
        };

        template < typename... Fs >
        struct with_nullable_pure_ref
        {
                static_assert(
                    (invocable_for_one< Ts&, Fs... > && ... && invocable_for_one< empty_t, Fs... >),
                    "For each type, there has to be one and only one callable" );
                static_assert(
                    ( invocable_with_any< Fs, empty_t, Ts&... > && ... ),
                    "For each function, there has to be at least one type it is invocable with" );
        };

        template < typename... Fs >
        struct with_pure_cref
        {
                static_assert(
                    ( invocable_for_one< Ts const&, Fs... > && ... ),
                    "For each type, there has to be one and only one callable" );
                static_assert(
                    ( invocable_with_any< Fs, Ts const&... > && ... ),
                    "For each function, there has to be at least one type it is invocable with" );
        };

        template < typename... Fs >
        struct with_nullable_pure_cref
        {
                static_assert(
                    (invocable_for_one< Ts const&, Fs... > && ... &&
                     invocable_for_one< empty_t, Fs... >),
                    "For each type, there has to be one and only one callable" );
                static_assert(
                    ( invocable_with_any< Fs, empty_t, Ts const&... > && ... ),
                    "For each function, there has to be at least one type it is invocable with" );
        };

        template < typename Deleter >
        struct with_deleter
        {
                template < typename... Fs >
                struct with_uvref
                {
                        static_assert(
                            ( invocable_for_one< _uvref< Deleter, Ts >, Fs... > && ... ),
                            "For each type, there has to be one and only one callable" );
                        static_assert(
                            ( invocable_with_any< Fs, _uvref< Deleter, Ts >... > && ... ),
                            "For each function, there has to be at least one type it is invocable with" );
                };

                template < typename... Fs >
                struct with_nullable_uvref
                {
                        static_assert(
                            (invocable_for_one< _uvref< Deleter, Ts >, Fs... > && ... &&
                             invocable_for_one< empty_t, Fs... >),
                            "For each type, there has to be one and only one callable" );
                        static_assert(
                            ( invocable_with_any< Fs, empty_t, _uvref< Deleter, Ts >... > && ... ),
                            "For each function, there has to be at least one type it is invocable with" );
                };
        };
};

template < typename... Args >
struct _function_picker
{
        template < typename F, typename... Fs >
        static constexpr decltype( auto ) pick( F&& f, Fs&&... fs )
        {
                if constexpr ( invocable< F, Args... > ) {
                        return (F&&) f;
                } else {
                        static_assert( sizeof...( fs ) != 0 );
                        return pick( (Fs&&) fs... );
                }
        }
};

using index_type                 = uint32_t;
static constexpr auto null_index = std::numeric_limits< index_type >::max();

template < typename TL, typename UL >
struct _vptr_cnv_map;

template < typename TL, typename... Us >
struct _vptr_cnv_map< TL, typelist< Us... > >
{
        static constexpr index_type conv( std::size_t i )
        {
                return i == null_index ? null_index : value[i];
        }

private:
        static constexpr index_type value[sizeof...( Us )] = {
            index_of_t_or_const_t_v< Us, TL >... };
};

template < typename UL, typename TL >
struct _split_impl;

template < typename... Us, typename T, typename... Ts >
        requires( sizeof...( Us ) < sizeof...( Ts ) )
struct _split_impl< typelist< Us... >, typelist< T, Ts... > >
{
        using sub = _split_impl< typelist< Us..., T >, typelist< Ts... > >;
        using lh  = typename sub::lh;
        using rh  = typename sub::rh;
};

template < typename... Us, typename T, typename... Ts >
        requires( sizeof...( Us ) >= sizeof...( Ts ) )
struct _split_impl< typelist< Us... >, typelist< T, Ts... > >
{
        using lh = typelist< Us... >;
        using rh = typelist< T, Ts... >;
};

template <>
struct _split_impl< typelist<>, typelist<> >
{
        using lh = typelist<>;
        using rh = typelist<>;
};

template < typename TL >
using split = _split_impl< typelist<>, TL >;

template < typename Deleter >
struct _deleter_box;

template < typename Deleter >
        requires( !std::is_lvalue_reference_v< Deleter > )
struct _deleter_box< Deleter > : private Deleter
{
        constexpr _deleter_box() noexcept = default;

        constexpr _deleter_box( Deleter&& d ) noexcept
          : Deleter( std::move( d ) )
        {
                static_assert( std::is_nothrow_move_constructible_v< Deleter > );
        }

        constexpr Deleter& get() noexcept
        {
                return *this;
        }

        friend constexpr void swap( _deleter_box& lh, _deleter_box& rh ) noexcept
        {
                using std::swap;
                swap( (Deleter&) lh, (Deleter&) rh );
        }

        friend constexpr auto operator<=>( _deleter_box const&, _deleter_box const& ) = default;
};

template < typename Deleter >
        requires( std::is_lvalue_reference_v< Deleter& > )
struct _deleter_box< Deleter& >
{
        constexpr _deleter_box() noexcept = delete;

        constexpr _deleter_box( Deleter& d ) noexcept
          : _del_ref( &d )
        {
        }

        constexpr Deleter& get() noexcept
        {
                return *_del_ref;
        }

        friend constexpr void swap( _deleter_box& lh, _deleter_box& rh ) noexcept
        {
                using std::swap;
                swap( lh._del_ref, rh._del_ref );
        }

        friend constexpr auto operator<=>( _deleter_box const&, _deleter_box const& ) = default;

private:
        Deleter* _del_ref;
};

template < std::size_t Align >
constexpr std::intptr_t hash_ptr( void* p )
{
        return (std::intptr_t) ( p ) >> std::bit_width( Align );
}

}  // namespace vari

#define VARI_GET_PTR_HASH_SPECIALIZATION( TYPE )                                              \
        template < typename... Ts >                                                           \
        struct std::hash< TYPE< Ts... > >                                                     \
        {                                                                                     \
                std::size_t operator()( TYPE< Ts... > const& r ) const                        \
                {                                                                             \
                        return vari::hash_ptr< std::min( { alignof( Ts )... } ) >( r.get() ); \
                }                                                                             \
        }

#define VARI_REC_GET_HASH_SPECIALIZATION( TYPE )                       \
        template < typename... Ts >                                    \
        struct std::hash< TYPE< Ts... > >                              \
        {                                                              \
                std::size_t operator()( TYPE< Ts... > const& r ) const \
                {                                                      \
                        auto x = r.get();                              \
                        return std::hash< decltype( x ) >()( x );      \
                }                                                      \
        }
