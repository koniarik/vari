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

#include "vari/bits/dispatch.h"
#include "vari/bits/val_core.h"
#include "vari/vref.h"

#pragma once

namespace vari
{

template < typename... Ts >
class _vval
{
        using core_type = _val_core< typelist< Ts... > >;

public:
        using types = typelist< Ts... >;

        template < typename U >
                requires( vconvertible_to< typelist< std::remove_reference_t< U > >, types > )
        constexpr _vval( U&& v ) noexcept( forward_nothrow_constructible< U > )
        {
                _core.template emplace< std::remove_reference_t< U > >( (U&&) v );
        }

        template < typename U, typename... Args >
                requires( vconvertible_to< typelist< U >, types > )
        constexpr _vval( std::in_place_type_t< U >, Args&&... args ) noexcept(
            std::is_nothrow_constructible_v< U, Args... > )
        {
                _core.template emplace< U >( (Args&&) args... );
        }

        // XXX: test the noexcept behavior
        // test move itself
        constexpr _vval( _vval&& p ) noexcept( std::is_nothrow_move_constructible_v< core_type > )
          : _core( std::move( p._core ) )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vval( _vval< Us... >&& p ) noexcept(
            std::is_nothrow_constructible_v< core_type, typename _vval< Us... >::core_type > )
          : _core( std::move( p._core ) )
        {
        }

        constexpr _vval( _vval const& p ) noexcept(
            std::is_nothrow_copy_constructible_v< core_type > )
          : _core( p._core )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vval( _vval< Us... > const& p ) noexcept(
            std::is_nothrow_constructible_v< core_type, typename _vval< Us... >::core_type > )
          : _core( p._core )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< std::remove_reference_t< U > >, types > )
        constexpr _vval& operator=( U&& v ) noexcept( forward_nothrow_constructible< U > )
        {
                if ( _core.index != 0 )
                        _core.destroy();
                _core.template emplace< std::remove_reference_t< U > >( (U&&) v );
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vval& operator=( _vval< Us... >&& p ) noexcept(
            std::is_nothrow_assignable_v< core_type, typename _vval< Us... >::core_type&& > )
        {
                _core = std::move( p._core );
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vval& operator=( _vval< Us... > const& p ) noexcept(
            std::is_nothrow_assignable_v< core_type, typename _vval< Us... >::core_type const& > )
        {
                _core = p._core;
        }

        template < typename T, typename... Args >
                requires( vconvertible_to< typelist< T >, types > )
        constexpr T&
        emplace( Args&&... args ) noexcept( std::is_nothrow_constructible_v< T, Args... > )
        {
                return _core.template emplace< T >( (Args&&) args... );
        }

        constexpr std::size_t get_index() const noexcept
        {
                return _core.index;
        }

        constexpr auto& operator*() const noexcept
        {
                static_assert( types::size == 1 );
                return _core.template get< 0 >();
        }

        constexpr auto* operator->() const noexcept
        {
                static_assert( types::size == 1 );
                return &_core.template get< 0 >();
        }

        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        constexpr operator _vref< Us... >() & noexcept
        {
                return core_type::visit_impl( _core, [&]( auto& item ) {
                        return _vref< Us... >( item );
                } );
        }

        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        constexpr operator _vref< Us... >() const& noexcept
        {
                return core_type::visit_impl( _core, [&]( auto& item ) {
                        return _vref< Us... >( item );
                } );
        }


        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... f ) const
        {
                static_assert(
                    ( invocable_for_one< Ts const&, Fs... > && ... ),
                    "For each type, there has to be one and only one callable" );
                return core_type::visit_impl( _core, (Fs&&) f... );
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... f )
        {
                static_assert(
                    ( invocable_for_one< Ts&, Fs... > && ... ),
                    "For each type, there has to be one and only one callable" );
                return core_type::visit_impl( _core, (Fs&&) f... );
        }

        // XXX:
        // - test swappability of values
        // - test nothrow check
        constexpr friend void
        swap( _vval& lh, _vval& rh ) noexcept( std::is_nothrow_swappable_v< core_type > )
        {
                swap( lh._core, rh._core );
        }

        constexpr ~_vval() noexcept( std::is_nothrow_destructible_v< core_type > )
        {
                _core.destroy();
        }

        friend constexpr auto operator<=>( _vval const& lh, _vval const& rh ) noexcept(
            all_nothrow_three_way_comparable_v< types > )
        {
                return core_type::three_way_compare( lh._core, rh._core );
        };

        friend constexpr auto operator==( _vval const& lh, _vval const& rh ) noexcept(
            all_nothrow_equality_comparable_v< types > )
        {
                return core_type::compare( lh._core, rh._core );
        };

private:
        template < typename... Us >
        friend class _vval;

        core_type _core;
};

template < typename... Ts >
using vval = _define_variadic< _vval, typelist< Ts... > >;

}  // namespace vari
