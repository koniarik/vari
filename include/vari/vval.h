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
        using TL = typelist< Ts... >;

public:
        template < typename U >
                requires( vconvertible_to< typelist< std::remove_reference_t< U > >, TL > )
        constexpr _vval( U&& v )
        {
                _core.template emplace< std::remove_reference_t< U > >( (U&&) v );
        }

        template < typename U, typename... Args >
                requires( vconvertible_to< typelist< U >, TL > )
        constexpr _vval( std::in_place_type_t< U >, Args&&... args )
        {
                _core.template emplace< U >( (Args&&) args... );
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        constexpr _vval( _vval< Us... >&& p ) noexcept
          : _core( std::move( p._core ) )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        constexpr _vval( _vval< Us... > const& p )
          : _core( p._core )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< std::remove_reference_t< U > >, TL > )
        constexpr _vval& operator=( U&& v )
        {
                if ( _core.index != 0 )
                        _core.destroy();
                _core.template emplace< std::remove_reference_t< U > >( (U&&) v );
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        constexpr _vval& operator=( _vval< Us... >&& p ) noexcept
        {
                _core = std::move( p._core );
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        constexpr _vval& operator=( _vval< Us... > const& p )
        {
                _core = p._core;
        }

        template < typename T, typename... Args >
                requires( vconvertible_to< typelist< T >, TL > )
        constexpr T& emplace( Args&&... args )
        {
                return _core.template emplace< T >( (Args&&) args... );
        }

        constexpr auto& operator*() const noexcept
        {
                static_assert( TL::size == 1 );
                return _core.template get< 0 >();
        }

        constexpr auto* operator->() const noexcept
        {
                static_assert( TL::size == 1 );
                return &_core.template get< 0 >();
        }

        template < typename... Us >
                requires( vconvertible_to< TL, typelist< Us... > > )
        constexpr operator _vref< Us... >() & noexcept
        {

                return _core.visit_impl( [&]( auto& item ) {
                        return _vref< Us... >( item );
                } );
        }

        constexpr friend void swap( _vval& lh, _vval& rh ) noexcept
        {
                swap( lh._core, rh._core );
        }


        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... f ) const
        {
                static_assert(
                    ( invocable_for_one< Ts const&, Fs... > && ... ),
                    "For each type, there has to be one and only one callable" );
                return _core.visit_impl( (Fs&&) f... );
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... f )
        {
                static_assert(
                    ( invocable_for_one< Ts&, Fs... > && ... ),
                    "For each type, there has to be one and only one callable" );
                return _core.visit_impl( (Fs&&) f... );
        }

        constexpr ~_vval()
        {
                _core.destroy();
        }

        friend constexpr auto operator<=>( _vval const& lh, _vval const& rh ) = default;

private:
        _val_core< typelist< Ts... > > _core;
};

template < typename... Ts >
using vval = _define_variadic< _vval, typelist< Ts... > >;

}  // namespace vari
