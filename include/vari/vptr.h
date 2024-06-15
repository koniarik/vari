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

#include <cstddef>

namespace vari
{
struct empty_t
{
};

static constexpr empty_t empty;

template < typename B, typename... Ts >
class _vref;

template < typename B, typename... Ts >
class _vptr
{
        using TL = typelist< Ts... >;

public:
        static constexpr bool const_base = std::is_const_v< B >;

        static_assert( is_flat_v< TL >, "The provided typelist has to be flat" );
        static_assert(
            all_or_none_const< B, TL >,
            "Either all types and base type are const, or none are" );

        _vptr()                              = default;
        _vptr( const _vptr& )                = default;
        _vptr( _vptr&& ) noexcept            = default;
        _vptr& operator=( const _vptr& )     = default;
        _vptr& operator=( _vptr&& ) noexcept = default;

        _vptr( std::nullptr_t ) noexcept
        {
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< C, typelist< Us... >, B, TL > )
        _vptr( _vptr< C, Us... > p ) noexcept
          : _core( std::move( p._core ) )
        {
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< C, typelist< Us... >, B, TL > )
        _vptr( _vref< C, Us... > r ) noexcept
          : _core( std::move( r._core ) )
        {
        }

        template < typename U >
                requires( vconvertible_to< B, typelist< U >, B, TL > )
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

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... fs )
        {
                if ( _core.ptr == nullptr )
                        return _dispatch_fun( empty, std::forward< Fs >( fs )... );
                return _core.template visit_impl( std::forward< Fs >( fs )... );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... fs )
        {
                if ( _core.ptr == nullptr )
                        return _dispatch_fun( empty, std::forward< Fs >( fs )... );
                return _core.template match_impl< _vref >( std::forward< Fs >( fs )... );
        }

        friend void swap( _vptr& lh, _vptr& rh ) noexcept
        {
                std::swap( lh._core, rh._core );
        }

        friend auto operator<=>( _vptr const& lh, _vptr const& rh ) = default;

private:
        _ptr_core< B, TL > _core;

        template < typename C, typename... Us >
        friend class _vptr;

        template < typename C, typename... Us >
        friend class _uvptr;
};

template < typename R, typename... Ts >
using vptr = _define_vptr< _vptr, R, typelist< Ts... > >;

}  // namespace vari
