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
#include "vari/forward.h"

#include <cassert>
#include <cstddef>

namespace vari
{

template < typename... Ts >
class _vptr
{
public:
        using types = typelist< Ts... >;

        using reference = _vref< Ts... >;

        constexpr _vptr()                              = default;
        constexpr _vptr( _vptr const& )                = default;
        constexpr _vptr( _vptr&& ) noexcept            = default;
        constexpr _vptr& operator=( _vptr const& )     = default;
        constexpr _vptr& operator=( _vptr&& ) noexcept = default;

        constexpr _vptr( std::nullptr_t ) noexcept
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr explicit _vptr( _vref< Us... > r ) noexcept
          : _core( std::move( r._core ) )
        {
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vptr( _vptr< Us... > p ) noexcept
          : _core( std::move( p._core ) )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, types > )
        constexpr _vptr( U* u ) noexcept
        {
                if ( u )
                        _core.set( *u );
        }

        constexpr auto& operator*() const noexcept
        {
                return *_core.ptr;
        }

        constexpr auto* operator->() const noexcept
        {
                return _core.ptr;
        }

        constexpr auto* get() const noexcept
        {
                return _core.ptr;
        }

        [[nodiscard]] constexpr index_type index() const noexcept
        {
                return _core.get_index();
        }

        constexpr operator bool() const noexcept
        {
                return _core.ptr != nullptr;
        }

        constexpr reference vref() const noexcept
        {
                assert( _core.ptr );

                reference r;
                r._core = _core;
                return r;
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... fs ) const
        {
                typename check_unique_invocability< types >::template with_nullable_pure_ref<
                    Fs... >
                    _{};
                if ( _core.ptr == nullptr )
                        return _dispatch_fun( empty, (Fs&&) fs... );
                return _core.visit_impl( (Fs&&) fs... );
        }


        friend constexpr void swap( _vptr& lh, _vptr& rh ) noexcept
        {
                swap( lh._core, rh._core );
        }

        friend constexpr auto operator<=>( _vptr const& lh, _vptr const& rh ) = default;

private:
        _ptr_core< types > _core;

        template < typename... Us >
        friend class _vref;
        template < typename... Us >
        friend class _vptr;
        template < typename Deleter, typename... Us >
        friend class _uvref;
        template < typename Deleter, typename... Us >
        friend class _uvptr;
};

template < typename... Ts >
using vptr = _define_variadic< _vptr, typelist< Ts... > >;

}  // namespace vari
