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

#include "vari/bits/typelist.h"
#include "vari/bits/util.h"
#include "vari/bits/val_core.h"
#include "vari/forward.h"
#include "vari/uvref.h"
#include "vari/vptr.h"
#include "vari/vref.h"
#include "vari/vval.h"

#include <type_traits>
#include <utility>

#pragma once

namespace vari
{

// WARNING: experimental
// WARNING: untested
template < typename... Ts >
class _vopt
{
        using core_type = _val_core< typelist< Ts... > >;

public:
        using types           = typelist< Ts... >;
        using pointer         = _vptr< Ts... >;
        using const_pointer   = _vptr< Ts const... >;
        using reference       = _vref< Ts... >;
        using const_reference = _vref< Ts const... >;

        constexpr _vopt() noexcept = default;

        template < typename U >
                requires( vconvertible_type< std::remove_cvref_t< U >, types > )
        constexpr _vopt( U&& v ) noexcept( _forward_nothrow_constructible< U > )
        {
                _core.template emplace< std::remove_cvref_t< U > >( (U&&) v );
        }

        template < typename U, typename... Args >
                requires( vconvertible_type< U, types > )
        constexpr _vopt( std::in_place_type_t< U >, Args&&... args ) noexcept(
            std::is_nothrow_constructible_v< U, Args... > )
        {
                _core.template emplace< U >( (Args&&) args... );
        }

        constexpr _vopt( _vopt&& p ) noexcept( std::is_nothrow_move_constructible_v< core_type > )
          : _core( std::move( p._core ) )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vopt( _vopt< Us... >&& p ) noexcept(
            std::is_nothrow_constructible_v< core_type, typename _vopt< Us... >::core_type&& > )
          : _core( std::move( p._core ) )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vopt( _vval< Us... >&& p ) noexcept(
            std::is_nothrow_constructible_v< core_type, typename _vval< Us... >::core_type&& > )
          : _core( std::move( p._core ) )
        {
        }

        constexpr _vopt( _vopt const& p ) noexcept(
            std::is_nothrow_copy_constructible_v< core_type > )
          : _core( p._core )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vopt( _vopt< Us... > const& p ) noexcept(
            std::
                is_nothrow_constructible_v< core_type, typename _vopt< Us... >::core_type const& > )
          : _core( p._core )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vopt( _vval< Us... > const& p ) noexcept(
            std::
                is_nothrow_constructible_v< core_type, typename _vval< Us... >::core_type const& > )
          : _core( p._core )
        {
        }

        template < typename U >
                requires( vconvertible_type< std::remove_cvref_t< U >, types > )
        constexpr _vopt& operator=( U&& v ) noexcept( _forward_nothrow_constructible< U > )
        {
                if ( _core.index != null_index )
                        _core.destroy();
                _core.template emplace< std::remove_cvref_t< U > >( (U&&) v );
                return *this;
        }

        constexpr _vopt& operator=( _vopt&& p ) noexcept(
            core_type::template is_nothrow_assignable< typename _vopt::core_type&& > )
        {
                // XXX not null friendly
                _core.assign( std::move( p._core ) );
                return *this;
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vopt& operator=( _vopt< Us... >&& p ) noexcept(
            core_type::template is_nothrow_assignable< typename _vopt< Us... >::core_type&& > )
        {
                // XXX not null friendly
                _core.assign( std::move( p._core ) );
                return *this;
        }

        constexpr _vopt& operator=( _vopt const& p ) noexcept(
            core_type::template is_nothrow_assignable< typename _vopt::core_type const& > )
        {
                // XXX not null friendly
                _core.assign( p._core );
                return *this;
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vopt& operator=( _vopt< Us... > const& p ) noexcept(
            core_type::template is_nothrow_assignable< typename _vopt< Us... >::core_type const& > )
        {
                // XXX not null friendly
                _core.assign( p._core );
                return *this;
        }

        template < typename T, typename... Args >
                requires( vconvertible_type< T, types > )
        constexpr T&
        emplace( Args&&... args ) noexcept( std::is_nothrow_constructible_v< T, Args... > )
        {
                if ( _core.index != null_index )
                        _core.destroy();
                return _core.template emplace< T >( (Args&&) args... );
        }

        [[nodiscard]] constexpr index_type index() const noexcept
        {
                return _core.index;
        }

        constexpr auto& operator*() const noexcept
                requires( types::size == 1 )
        {
                return core_type::ST::template get< 0 >( _core.storage );
        }

        constexpr auto* operator->() const noexcept
                requires( types::size == 1 )
        {
                return &**this;
        }

        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        constexpr operator _vptr< Us... >() & noexcept
        {
                if ( _core.index == null_index )
                        return _vptr< Us... >{};
                return core_type::visit_impl( _core, [&]( auto& item ) {
                        return _vptr< Us... >( &item );
                } );
        }

        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        constexpr operator _vptr< Us... >() const& noexcept
        {
                static_assert( all_is_const_v< typelist< Us... > > );
                if ( _core.index == null_index )
                        return _vptr< Us... >{};
                return core_type::visit_impl( _core, [&]( auto& item ) {
                        return _vptr< Us... >( &item );
                } );
        }

        constexpr explicit operator bool() const noexcept
        {
                return _core.index != null_index;
        }

        constexpr reference vref() & noexcept
        {
                VARI_ASSERT( *this );
                return vptr().vref();
        }

        constexpr const_reference vref() const& noexcept
        {
                VARI_ASSERT( *this );
                return vptr().vref();
        }

        constexpr pointer vptr() & noexcept
        {
                return pointer{ *this };
        }

        constexpr const_pointer vptr() const& noexcept
        {
                return const_pointer{ *this };
        }

        // XXX: noexcept if possible
        // XXX: test
        // XXX: test visit?
        constexpr _vval< Ts... > vval() &&
        {
                VARI_ASSERT( !!*this );
                _vval< Ts... > res;
                swap( res._core, _core );
                return res;
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... f ) const
        {
                typename _check_unique_invocability< types >::template with_nullable_pure_cref<
                    Fs... >
                    _{};
                if ( _core.index == null_index )
                        return _dispatch_fun( empty, (Fs&&) f... );
                return core_type::visit_impl( _core, (Fs&&) f... );
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... f )
        {
                typename _check_unique_invocability< types >::template with_nullable_pure_ref<
                    Fs... >
                    _{};
                if ( _core.index == null_index )
                        return _dispatch_fun( empty, (Fs&&) f... );
                return core_type::visit_impl( _core, (Fs&&) f... );
        }

        constexpr friend void
        swap( _vopt& lh, _vopt& rh ) noexcept( std::is_nothrow_swappable_v< core_type > )
        {
                // XXX: not null friendly
                swap( lh._core, rh._core );
        }

        constexpr ~_vopt() noexcept( std::is_nothrow_destructible_v< core_type > )
        {
                if ( _core.index != null_index )
                        _core.destroy();
        }

        friend constexpr std::partial_ordering operator<=>(
            _vopt const& lh,
            _vopt const& rh ) noexcept( all_nothrow_three_way_comparable_v< types > )
        {
                if ( lh._core.index == null_index || rh._core.index == null_index )
                        return lh._core.index <=> rh._core.index;
                return core_type::three_way_compare( lh._core, rh._core );
        };

        friend constexpr auto operator==( _vopt const& lh, _vopt const& rh ) noexcept(
            all_nothrow_equality_comparable_v< types > )
        {
                if ( lh._core.index == null_index || rh._core.index == null_index )
                        return lh._core.index == rh._core.index;
                return core_type::compare( lh._core, rh._core );
        };

private:
        template < typename... Us >
        friend class _vopt;

        core_type _core;
};

template < typename... Ts >
using vopt = _define_variadic< _vopt, typelist< Ts... > >;

}  // namespace vari
