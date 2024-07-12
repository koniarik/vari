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

#include <cassert>
#include <cstddef>
#include <optional>

namespace vari
{
struct empty_t
{
};

static constexpr empty_t empty;

template < typename... Ts >
class _vref;

template < typename... Ts >
class _vptr
{
        using TL = typelist< Ts... >;

public:
        static_assert( is_flat_v< TL >, "The provided typelist has to be flat" );

        using reference = _vref< Ts... >;

        _vptr()                              = default;
        _vptr( const _vptr& )                = default;
        _vptr( _vptr&& ) noexcept            = default;
        _vptr& operator=( const _vptr& )     = default;
        _vptr& operator=( _vptr&& ) noexcept = default;

        _vptr( std::nullptr_t ) noexcept
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        _vptr( _vptr< Us... > p ) noexcept
          : _core( std::move( p._core ) )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        _vptr( _vref< Us... > r ) noexcept
          : _core( std::move( r._core ) )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, TL > )
        _vptr( U& u ) noexcept
          : _core( u )
        {
        }

        auto& operator*() const noexcept
        {
                return *_core.ptr;
        }

        auto* operator->() const noexcept
        {
                return _core.ptr;
        }

        auto* get() const noexcept
        {
                return _core.ptr;
        }

        operator bool() const noexcept
        {
                return _core.ptr != nullptr;
        }

        reference vref() const noexcept
        {
                assert( _core.ptr );

                reference r;
                r._core = _core;
                return r;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... fs ) const
        {
                if ( _core.ptr == nullptr )
                        return _dispatch_fun( empty, std::forward< Fs >( fs )... );
                return _core.visit_impl( std::forward< Fs >( fs )... );
        }


        friend void swap( _vptr& lh, _vptr& rh ) noexcept
        {
                std::swap( lh._core, rh._core );
        }

        friend auto operator<=>( _vptr const& lh, _vptr const& rh ) = default;

private:
        _ptr_core< TL > _core;

        template < typename... Us >
        friend class _vptr;

        template < typename... Us >
        friend class _uvptr;
};

template < typename... Ts >
using vptr = _define_vptr< _vptr, typelist< Ts... > >;

}  // namespace vari
