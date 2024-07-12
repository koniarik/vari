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
#include "vari/bits/typelist.h"
#include "vari/bits/util.h"
#include "vari/vref.h"

#include <cassert>

namespace vari
{

template < typename... Ts >
class _uvref
{
        using TL = typelist< Ts... >;

public:
        static_assert( is_flat_v< TL > );

        using reference = _vref< Ts... >;

        _uvref( _uvref const& )            = delete;
        _uvref& operator=( _uvref const& ) = delete;

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        _uvref( _uvref< Us... >&& p ) noexcept
          : _ref( p._ref )
        {
                p._ref._core = _ptr_core< typelist< Us... > >{};
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        explicit _uvref( _vref< Us... > p ) noexcept
          : _ref( p )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, TL > )
        explicit _uvref( U& u ) noexcept
          : _ref( u )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        _uvref& operator=( _uvref< Us... >&& p ) noexcept
        {
                using std::swap;
                _uvref tmp{ std::move( p ) };
                swap( _ref._core, tmp._ref._core );
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

        template < typename... Us >
                requires( vconvertible_to< TL, typelist< Us... > > )
        operator _vref< Us... >() & noexcept
        {
                return _ref;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... f ) const
        {
                return _ref.visit( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) take( Fs&&... fs ) &&
        {
                assert( _ref._core.ptr );
                auto tmp   = _ref;
                _ref._core = _ptr_core< TL >{};
                return tmp._core.template take_impl< _uvref, _vref >( std::forward< Fs >( fs )... );
        }

        friend void swap( _uvref& lh, _uvref& rh ) noexcept
        {
                std::swap( lh._ref, rh._ref );
        }

        ~_uvref()
        {
                _ref._core.delete_ptr();
        }

        friend auto operator<=>( _uvref const& lh, _uvref const& rh ) = default;

private:
        reference _ref;

        template < typename... Us >
        friend class _vptr;

        template < typename... Us >
        friend class _uvptr;

        template < typename... Us >
        friend class _uvref;
};

template < typename... Ts >
using uvref = _define_vptr< _uvref, typelist< Ts... > >;

template < typename T >
uvref< T > uwrap( T item )
{
        return uvref< T >( vref< T >( *new T( std::move( item ) ) ) );
}

}  // namespace vari
