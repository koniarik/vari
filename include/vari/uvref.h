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
#include "vari/vref.h"

#include <cassert>

namespace vari
{

template < typename Deleter, typename... Ts >
class _uvref : private deleter_box< Deleter >
{
        template < typename... Us >
        using same_uvref = _uvref< Deleter, Us... >;

public:
        using types = typelist< Ts... >;

        using core_type      = _ptr_core< types >;
        using reference      = _vref< Ts... >;
        using pointer        = _vptr< Ts... >;
        using owning_pointer = _uvptr< Deleter, Ts... >;

        constexpr _uvref( _uvref const& )            = delete;
        constexpr _uvref& operator=( _uvref const& ) = delete;

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _uvref( _uvref< Deleter, Us... >&& p ) noexcept
        {
                _core = std::move( p._core );
                p._core.reset();
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, types > )
        constexpr explicit _uvref( U& u ) noexcept
        {
                _core.set( u );
        }

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _uvref& operator=( _uvref< Deleter, Us... >&& p ) noexcept
        {
                _uvref tmp{ std::move( p ) };
                swap( _core, tmp._core );
                return *this;
        }

        constexpr auto& operator*() const noexcept
        {
                return *_core.ptr;
        }

        constexpr auto* operator->() const noexcept
        {
                return _core.ptr;
        }

        constexpr reference get() const noexcept
        {
                reference res;
                res._core = _core;
                return res;
        }

        [[nodiscard]] constexpr index_type index() const noexcept
        {
                return _core.index;
        }

        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        constexpr operator _vref< Us... >() const noexcept
        {
                return vptr().vref();
        }

        constexpr pointer vptr() const& noexcept
        {
                pointer res;
                res._core = _core;
                return res;
        }

        constexpr owning_pointer vptr() && noexcept
        {
                owning_pointer res;
                swap( res._core, _core );
                return res;
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... f ) const
        {
                typename check_unique_invocability< types >::template with_pure_ref< Fs... > _{};
                assert( _core.ptr );
                return _core.visit_impl( (Fs&&) f... );
        }

        template < typename... Fs >
        constexpr decltype( auto ) take( Fs&&... fs ) &&
        {
                typename check_unique_invocability< types >::template with_deleter<
                    Deleter >::template with_uvref< Fs... >
                    _{};
                assert( _core.ptr );
                auto tmp = _core;
                _core.reset();
                return tmp.template take_impl< same_uvref >( (Fs&&) fs... );
        }

        friend constexpr void swap( _uvref& lh, _uvref& rh ) noexcept
        {
                swap( lh._core, rh._core );
        }

        constexpr ~_uvref()
        {
                _core.delete_ptr( deleter_box< Deleter >::get() );
        }

        friend constexpr auto operator<=>( _uvref const& lh, _uvref const& rh ) = default;

private:
        constexpr _uvref() noexcept = default;

        core_type _core;

        template < typename... Us >
        friend class _vptr;

        template < typename Deleter2, typename... Us >
        friend class _uvptr;

        template < typename Deleter2, typename... Us >
        friend class _uvref;
};

template < typename... Ts >
using uvref = _define_variadic< _uvref, typelist< Ts... >, def_del >;

template < typename T >
constexpr uvref< T > uwrap( T item )
{
        return uvref< T >( *new T( std::move( item ) ) );
}

}  // namespace vari
