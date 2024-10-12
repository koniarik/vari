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
#include "vari/vptr.h"

#include <cassert>

namespace vari
{

template < typename... Ts >
class _vref
{
public:
        using types = unique_typelist_t< flatten_t< typelist< Ts... > > >;

        using pointer = _vptr< Ts... >;

        template < typename... Us >
                requires( vconvertible_to< typename _vref< Us... >::types, types > )
        constexpr _vref( _vref< Us... > p ) noexcept
          : _core( p._core )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, types > )
        constexpr _vref( U& u ) noexcept
        {
                _core.set( u );
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

        template < typename U >
                requires( vconvertible_to< types, typelist< U > > )
        constexpr operator U&() const noexcept
        {
                return *_core.ptr;
        }

        constexpr pointer vptr() const& noexcept
        {
                pointer res;
                res._core = _core;
                return res;
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... fs ) const
        {
                typename check_unique_invocability< types >::template with_pure_ref< Fs... > _{};
                assert( _core.ptr );
                return _core.visit_impl( (Fs&&) fs... );
        }

        friend constexpr void swap( _vref& lh, _vref& rh ) noexcept
        {
                swap( lh._core, rh._core );
        }

        friend constexpr auto operator<=>( _vref const& lh, _vref const& rh ) = default;

private:
        constexpr _vref() noexcept = default;

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
using vref = _define_variadic< _vref, typelist< Ts... > >;

}  // namespace vari
