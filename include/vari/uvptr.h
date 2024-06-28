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

#include <cassert>
#include <cstddef>
#include <optional>

namespace vari
{

template < typename... Ts >
class _uvref;

template < typename... Ts >
class _uvptr
{
        using TL = typelist< Ts... >;

public:
        static_assert( is_flat_v< TL > );

        using pointer          = _vptr< Ts... >;
        using reference        = _vref< Ts... >;
        using owning_reference = _uvref< Ts... >;

        _uvptr() noexcept = default;

        _uvptr( std::nullptr_t ) noexcept
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        _uvptr( _uvref< Us... >&& p ) noexcept
        {
                _ptr._core   = std::move( p._ref._core );
                p._ref._core = _ptr_core< typelist< Us... > >{};
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        _uvptr( _uvptr< Us... >&& p ) noexcept
          : _ptr( p.release() )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, TL > )
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

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        _uvptr& operator=( _uvref< Us... >&& p )
        {
                using std::swap;
                _uvptr tmp{ std::move( p ) };
                swap( _ptr._core, tmp._ptr._core );
                return *this;
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        _uvptr& operator=( _uvptr< Us... >&& p )
        {
                using std::swap;
                _uvptr tmp{ std::move( p ) };
                swap( _ptr._core, tmp._ptr._core );
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

        template < typename... Us >
                requires( vconvertible_to< TL, typelist< Us... > > )
        operator _vptr< Us... >() & noexcept
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
                using std::swap;
                pointer res;
                swap( res, _ptr );
                return res;
        }

        operator bool() const noexcept
        {
                return _ptr;
        }

        reference vref() const& noexcept
        {
                assert( _ptr );
                return _ptr.vref();
        }

        owning_reference vref() && noexcept
        {
                assert( _ptr );
                auto p = release();
                return owning_reference{ p.vref() };
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... f ) const
        {
                return _ptr.visit( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... f ) const
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

        template < typename... Us >
        friend class _uvptr;
};

template < typename... Ts >
using uvptr = _define_vptr< _uvptr, typelist< Ts... > >;

}  // namespace vari
