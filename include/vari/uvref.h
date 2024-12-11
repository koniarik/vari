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
#include "vari/deleter.h"
#include "vari/forward.h"
#include "vari/vptr.h"
#include "vari/vref.h"

#include <cassert>

namespace vari
{

/// A non-nullable owning pointer to one of the types in Ts...
///
template < typename Deleter, typename... Ts >
class _uvref : private _deleter_box< Deleter >
{
        template < typename... Us >
        using same_uvref = _uvref< Deleter, Us... >;
        using dbox       = _deleter_box< Deleter >;

public:
        using types = typelist< Ts... >;

        using core_type      = _ptr_core< types >;
        using reference      = _vref< Ts... >;
        using pointer        = _vptr< Ts... >;
        using owning_pointer = _uvptr< Deleter, Ts... >;

        constexpr _uvref( _uvref const& )            = delete;
        constexpr _uvref& operator=( _uvref const& ) = delete;

        /// Constructs an `uvref` by transfering ownership from `uvref` with compatible types.
        ///
        template < typename Deleter2, typename... Us >
                requires(
                    vconvertible_to< typelist< Us... >, types > &&
                    convertible_deleter< Deleter2, Deleter > )
        constexpr _uvref( _uvref< Deleter2, Us... >&& p ) noexcept
          : dbox( std::move( (dbox&) p ) )
        {
                _core = std::move( p._core );
                p._core.reset();
        }

        /// Constructs an `uvref` which owns a reference to one of the types that `uvref` can
        /// reference.
        template < typename U >
                requires( vconvertible_type< U, types > )
        constexpr explicit _uvref( U& u ) noexcept
        {
                _core.set( u );
        }

        /// Constructs an `uvref` which owns a reference to one of the types that `uvref` can
        /// reference.
        ///
        /// Internal `Deleter` is copy-constructed from `d`.
        template < typename U >
                requires( vconvertible_type< U, types > && copy_constructible_deleter< Deleter > )
        constexpr explicit _uvref( U& u, Deleter const& d ) noexcept
          : dbox( d )
        {
                _core.set( u );
        }

        /// Constructs an `uvref` which owns a reference to one of the types that `uvref` can
        /// reference.
        ///
        /// Internal `Deleter` is move-constructed from `d`.
        template < typename U >
                requires( vconvertible_type< U, types > && move_constructible_deleter< Deleter > )
        constexpr explicit _uvref( U& u, Deleter&& d ) noexcept
          : dbox( std::move( d ) )
        {
                _core.set( u );
        }

        /// Move assignment operator transfering ownership from `uvref` with compatible types.
        /// `Deleter` is move-constructed from the other `uvref`.
        template < typename Deleter2, typename... Us >
                requires(
                    vconvertible_to< typelist< Us... >, types > &&
                    convertible_deleter< Deleter2, Deleter > )
        constexpr _uvref& operator=( _uvref< Deleter2, Us... >&& p ) noexcept
        {
                _uvref tmp{ std::move( p ) };
                swap( _core, tmp._core );
                swap( (dbox&) ( *this ), (dbox&) tmp );
                return *this;
        }

        /// Dereferences to the pointed-to type. It is `T&` if there is only one type in `Ts...`,
        /// or `void&` otherwise.
        constexpr auto& operator*() const noexcept
        {
                return *_core.ptr;
        }

        /// Provides member access to the pointed-to type. It is `T*` if there is only one type in
        /// `Ts...`, or `void*` otherwise.
        constexpr auto* operator->() const noexcept
        {
                return _core.ptr;
        }

        /// Returns a `reference` to the pointed-to type.
        constexpr reference get() const noexcept
        {
                assert( _core.ptr );
                reference res;
                res._core = _core;
                return res;
        }

        /// Returns the index representing the type currently being referenced.
        /// The index of the first type is 0, with subsequent types sequentially numbered.
        [[nodiscard]] constexpr index_type index() const noexcept
        {
                assert( _core.ptr );
                return _core.index;
        }

        /// Conversion operator from lvalue reference to types-compatible `vref`
        ///
        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        constexpr operator _vref< Us... >() & noexcept
        {
                assert( _core.ptr );
                return vptr().vref();
        }
        /// Conversion operator from lvalue const reference to types-compatible `vref`
        ///
        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        constexpr operator _vref< Us... >() const& noexcept
        {
                assert( _core.ptr );
                return vptr().vref();
        }

        /// Conversion operator from rvalue reference to `vref` is forbidden
        template < typename... Us >
        constexpr operator _vref< Us... >() && = delete;

        /// Constructs a variadic pointer that points to the same target as the current reference.
        ///
        constexpr pointer vptr() const& noexcept
        {
                assert( _core.ptr );
                pointer res;
                res._core = _core;
                return res;
        }

        /// Constructs an owning variadic pointer and transfers ownership of current target to the
        /// pointer.
        constexpr owning_pointer vptr() && noexcept
        {
                assert( _core.ptr );
                owning_pointer res;
                swap( res._core, _core );
                return res;
        }

        /// Calls the appropriate function from the list `fs...`, based on the type of the current
        /// target.
        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... f ) const
        {
                typename _check_unique_invocability< types >::template with_pure_ref< Fs... > _{};
                assert( _core.ptr );
                return _core.visit_impl( (Fs&&) f... );
        }

        /// Constructs an owning reference to currently pointed-to type and transfers ownership to
        /// it. Moves it to the appropriate function from the list `fs...`.
        template < typename... Fs >
        constexpr decltype( auto ) take( Fs&&... fs ) &&
        {
                typename _check_unique_invocability< types >::template with_deleter<
                    Deleter >::template with_uvref< Fs... >
                    _{};
                assert( _core.ptr );
                auto tmp = _core;
                _core.reset();
                return tmp.template take_impl< same_uvref >( (Fs&&) fs... );
        }

        /// Getter to the internal deleter
        ///
        Deleter& get_deleter() noexcept
        {
                return dbox::get();
        }

        /// Getter to the internal deleter
        ///
        Deleter const& get_deleter() const noexcept
        {
                return dbox::get();
        }


        /// Destroys the owned object.
        ///
        constexpr ~_uvref()
        {
                _core.delete_ptr( dbox::get() );
        }

        /// Swaps `uvref` with each other.
        ///
        friend constexpr void swap( _uvref& lh, _uvref& rh ) noexcept
        {
                swap( lh._core, rh._core );
                swap( (dbox&) lh, (dbox&) rh );
        }

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

/// Compares the internal pointers of both references.
///
template < typename... Lhs, typename... Rhs >
constexpr auto operator<=>( _uvref< Lhs... > const& lh, _uvref< Rhs... > const& rh ) noexcept
{
        return lh.get() <=> rh.get();
}

/// Compares the internal pointers of both references.
///
template < typename... Lhs, typename... Rhs >
constexpr bool operator==( _uvref< Lhs... > const& lh, _uvref< Rhs... > const& rh ) noexcept
{
        return lh.get() == rh.get();
}

/// A non-nullable owning pointer to types derived out of `Ts...` list by flattening it and
/// filtering for unique types.
template < typename... Ts >
using uvref = _define_variadic< _uvref, typelist< Ts... >, def_del >;

/// Wraps object `item` into `uvref` of its type.
template < typename T >
constexpr uvref< T > uwrap( T item )
{
        return uvref< T >( *new T( std::move( item ) ) );
}

}  // namespace vari

VARI_REC_GET_HASH_SPECIALIZATION( vari::_uvref );
