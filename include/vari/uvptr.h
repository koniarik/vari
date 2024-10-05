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

template < typename Deleter, typename... Ts >
class _uvref;

template < typename Deleter, typename... Ts >
class _uvptr
{
public:
        using types = typelist< Ts... >;

        using core_type        = _ptr_core< Deleter, types >;
        using pointer          = _vptr< Ts... >;
        using reference        = _vref< Ts... >;
        using owning_reference = _uvref< Deleter, Ts... >;

        _uvptr() noexcept = default;

        _uvptr( _uvptr const& )            = delete;
        _uvptr& operator=( _uvptr const& ) = delete;

        _uvptr( std::nullptr_t ) noexcept
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        explicit _uvptr( _uvref< Deleter, Us... >&& p ) noexcept
        {
                _core = std::move( p._core );
                p._core.reset();
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        _uvptr( _uvptr< Deleter, Us... >&& p ) noexcept
        {
                _core = std::move( p._core );
                p._core.reset();
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, types > )
        _uvptr( U* u ) noexcept
        {
                if ( u )
                        _core.set( *u );
        }

        _uvptr& operator=( std::nullptr_t ) noexcept
        {
                reset( nullptr );
                return *this;
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        _uvptr& operator=( _uvref< Deleter, Us... >&& p ) noexcept
        {
                using std::swap;
                _uvptr tmp{ std::move( p ) };
                swap( _core, tmp._core );
                return *this;
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        _uvptr& operator=( _uvptr< Deleter, Us... >&& p ) noexcept
        {
                using std::swap;
                _uvptr tmp{ std::move( p ) };
                swap( _core, tmp._core );
                return *this;
        }

        auto& operator*() const noexcept
        {
                return *_core.ptr;
        }

        auto* operator->() const noexcept
        {
                return _core.ptr;
        }

        pointer get() const noexcept
        {
                pointer res;
                res._core = _core;
                return res;
        }

        [[nodiscard]] constexpr index_type index() const noexcept
        {
                return _core.index;
        }

        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        operator _vptr< Us... >() const noexcept
        {
                return _core.ptr;
        }

        void reset( pointer ptr = pointer() )
        {
                auto tmp = _core;
                _core    = std::move( ptr._core );
                tmp.delete_ptr();
        }

        pointer release() noexcept
        {
                using std::swap;
                pointer res;
                swap( res._core, _core );
                return res;
        }

        operator bool() const noexcept
        {
                return _core.get_index() != null_index;
        }

        reference vref() const& noexcept
        {
                assert( _core.get_index() != null_index );
                reference res;
                res._core = _core;
                return res;
        }

        owning_reference vref() && noexcept
        {
                assert( _core.get_index() != null_index );
                using std::swap;
                owning_reference res;
                swap( res._core, _core );
                return res;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... f ) const
        {

                typename check_unique_invocability< types >::template with_nullable_pure_ref<
                    Fs... >
                    _{};
                if ( _core.ptr == nullptr )
                        return _dispatch_fun( empty, (Fs&&) f... );
                return _core.visit_impl( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) take( Fs&&... fs ) &&
        {
                typename check_unique_invocability< types >::template with_deleter<
                    Deleter >::template with_nullable_uvref< Fs... >
                     _{};
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
                _core.delete_ptr();
        }

        friend auto operator<=>( _uvptr const& lh, _uvptr const& rh ) = default;

private:
        core_type _core;

        template < typename Deleter2, typename... Us >
        friend class _uvptr;
};

template < typename... Ts >
using uvptr = _define_variadic< _uvptr, typelist< Ts... >, def_del >;

}  // namespace vari
