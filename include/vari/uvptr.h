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
#include "vari/vptr.h"

#include <cstddef>

namespace vari
{

template < typename B, typename... Ts >
class _uvref;

template < typename B, typename... Ts >
class _uvptr
{
        using TL = typelist< Ts... >;

public:
        static_assert( is_flat_v< TL > );
        static_assert(
            all_or_none_const< B, TL >,
            "Either all types and base type are const, or none are" );

        using pointer = _vptr< B, Ts... >;

        _uvptr() noexcept = default;

        _uvptr( std::nullptr_t ) noexcept
        {
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< C, typelist< Us... >, B, TL > )
        _uvptr( _uvref< C, Us... >&& p ) noexcept
        {
                _ptr._core   = std::move( p._ref._core );
                p._ref._core = _ptr_core< B, typelist< Us... > >{};
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< C, typelist< Us... >, B, TL > )
        _uvptr( _uvptr< C, Us... >&& p ) noexcept
          : _ptr( p.release() )
        {
        }

        template < typename U >
                requires( vconvertible_to< B, typelist< U >, B, TL > )
        explicit _uvptr( U* u ) noexcept
        {
                if ( u )
                        _ptr = pointer{ *u };
        }

        _uvptr& operator=( std::nullptr_t ) noexcept
        {
                reset( nullptr );
                return *this;
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< C, typelist< Us... >, B, TL > )
        _uvptr& operator=( _uvref< C, Us... >&& p )
        {
                _ptr._core   = std::move( p._ref._core );
                p._ref._core = _ptr_core< B, typelist< Us... > >{};
                return *this;
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< C, typelist< Us... >, B, TL > )
        _uvptr& operator=( _uvptr< C, Us... >&& p )
        {
                if ( this == &p )
                        return *this;
                reset( p.release() );
                return *this;
        }

        auto& operator*() const noexcept
        {
                return *_ptr;
        }

        auto* operator->() const noexcept
        {
                return _ptr.get();
        }

        const pointer& get() const noexcept
        {
                return _ptr;
        }

        template < typename C, typename... Us >
                requires( vconvertible_to< B, TL, C, typelist< Us... > > )
        operator _vptr< C, Us... >() & noexcept
        {
                return _ptr;
        }

        void reset( pointer ptr = pointer() )
        {
                auto tmp = _ptr;
                _ptr     = ptr;
                tmp._core.delete_ptr();
        }

        pointer release() noexcept
        {
                pointer res;
                std::swap( res, _ptr );
                return res;
        }

        operator bool() const noexcept
        {
                return _ptr;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... f )
        {
                return _ptr.visit( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... f )
        {
                return _ptr.match( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) take( Fs&&... fs ) &&
        {
                auto p = release();
                if ( p._core.ptr == nullptr )
                        return _dispatch_fun( empty, std::forward< Fs >( fs )... );
                return p._core.template take_impl< _uvref, _vref >( std::forward< Fs >( fs )... );
        }

        friend void swap( _uvptr& lh, _uvptr& rh ) noexcept
        {
                std::swap( lh._ptr, rh._ptr );
        }

        ~_uvptr()
        {
                _ptr._core.delete_ptr();
        }

        friend auto operator<=>( _uvptr const& lh, _uvptr const& rh ) = default;

private:
        pointer _ptr;

        template < typename C, typename... Us >
        friend class _uvptr;
};

template < typename R, typename... Ts >
using uvptr = _define_vptr< _uvptr, R, typelist< Ts... > >;

}  // namespace vari
