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

#pragma once

#include "vari/bits/ptr_core.h"
#include "vari/vref.h"

namespace vari
{

template < typename B, typename... Ts >
class _uvref
{
        using TL = typelist< Ts... >;

public:
        static_assert( is_flat_v< TL > );
        static_assert(
            all_or_none_const< B, TL >,
            "Either all types and base type are const, or none are" );

        using reference = _vref< B, Ts... >;

        template < typename C, typename... Us >
                requires( vconvertible_to< C, typelist< Us... >, B, TL > )
        _uvref( _uvref< C, Us... >&& p ) noexcept
          : _ref( p._ref )
        {
                p._ref._core = _ptr_core< B, typelist< Us... > >{};
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< C, typelist< Us... >, B, TL > )
        explicit _uvref( _vref< C, Us... > p ) noexcept
          : _ref( p )
        {
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< C, typelist< Us... >, B, TL > )
        _uvref& operator=( _uvref< C, Us... >&& p ) noexcept
        {
                if ( this == &p )
                        return *this;
                using std::swap;
                swap( _ref._core, p._ref._core );
                return *this;
        }

        auto& operator*() const noexcept
        {
                return *_ref;
        }

        auto* operator->() const noexcept
        {
                return _ref.get();
        }

        const reference& get() const noexcept
        {
                return _ref;
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< B, TL, C, typelist< Us... > > )
        operator _vref< C, Us... >() & noexcept
        {
                return _ref;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... f )
        {
                return _ref.visit( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... f )
        {
                return _ref.match( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) take( Fs&&... fs ) &&
        {
                auto tmp   = _ref;
                _ref._core = _ptr_core< B, TL >{};
                return tmp._core.template take_impl< _uvref, _vref >( std::forward< Fs >( fs )... );
        }

        ~_uvref()
        {
                _ref._core.delete_ptr();
        }

        friend auto operator<=>( _uvref const& lh, _uvref const& rh ) = default;

private:
        reference _ref;

        template < typename C, typename... Us >
        friend class _vptr;

        template < typename C, typename... Us >
        friend class _uvptr;

        template < typename C, typename... Us >
        friend class _uvref;
};

template < typename R, typename... Ts >
using uvref = _define_vptr< _uvref, R, typelist< Ts... > >;

template < typename R, typename T >
uvref< R, T > uwrap( T item )
{
        return uvref< R, T >( vref< R, T >( *new T( std::move( item ) ) ) );
}

}  // namespace vari