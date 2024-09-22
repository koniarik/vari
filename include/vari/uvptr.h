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
public:
        using types = typelist< Ts... >;

        using pointer          = _vptr< Ts... >;
        using reference        = _vref< Ts... >;
        using owning_reference = _uvref< Ts... >;

        _uvptr() noexcept = default;

        _uvptr( _uvptr const& )            = delete;
        _uvptr& operator=( _uvptr const& ) = delete;

        _uvptr( std::nullptr_t ) noexcept
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        _uvptr( _uvref< Us... >&& p ) noexcept
        {
                _ptr._core   = std::move( p._ref._core );
                p._ref._core = _ptr_core< typelist< Us... > >{};
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        _uvptr( _uvptr< Us... >&& p ) noexcept
          : _ptr( p.release() )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, types > )
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
                requires( vconvertible_to< typelist< Us... >, types > )
        _uvptr& operator=( _uvref< Us... >&& p ) noexcept
        {
                using std::swap;
                _uvptr tmp{ std::move( p ) };
                swap( _ptr._core, tmp._ptr._core );
                return *this;
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        _uvptr& operator=( _uvptr< Us... >&& p ) noexcept
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

        [[nodiscard]] constexpr index_type index() const noexcept
        {
                return _ptr.index();
        }

        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        operator _vptr< Us... >() const noexcept
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
                static_assert(
                    (invocable_for_one< Ts&, Fs... > && ... && invocable_for_one< empty_t, Fs... >),
                    "For each type, there has to be one and only one callable" );
                return _ptr.visit( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) take( Fs&&... fs ) &&
        {
                static_assert(
                    (invocable_for_one< _uvref< Ts >, Fs... > && ... &&
                     invocable_for_one< empty_t, Fs... >),
                    "For each type, there has to be one and only one callable" );
                auto p = release();
                if ( p._core.ptr == nullptr )
                        return _dispatch_fun( empty, (Fs&&) fs... );
                return p._core.template take_impl< _uvref, _vref >( (Fs&&) fs... );
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
using uvptr = _define_variadic< _uvptr, typelist< Ts... > >;

}  // namespace vari
