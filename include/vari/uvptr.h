/// MIT License
///
/// Copyright (c) 2025 koniarik
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#pragma once

#include "vari/bits/assert.h"
#include "vari/bits/ptr_core.h"
#include "vari/bits/typelist.h"
#include "vari/bits/util.h"
#include "vari/deleter.h"
#include "vari/forward.h"
#include "vari/vptr.h"

#include <cstddef>

namespace vari
{

/// A nullable owning pointer to one of the types in Ts...
///
template < typename Deleter, typename... Ts >
class _uvptr : private _deleter_box< Deleter >
{
        template < typename... Us >
        using same_uvref = _uvref< Deleter, Us... >;
        using dbox       = _deleter_box< Deleter >;

public:
        using types = typelist< Ts... >;

        using core_type        = _ptr_core< types >;
        using pointer          = _vptr< Ts... >;
        using reference        = _vref< Ts... >;
        using owning_reference = _uvref< Deleter, Ts... >;

        constexpr _uvptr() noexcept = default;

        constexpr _uvptr( _uvptr const& )            = delete;
        constexpr _uvptr& operator=( _uvptr const& ) = delete;

        /// Construct a pointer in a null state.
        ///
        constexpr _uvptr( std::nullptr_t ) noexcept
        {
        }

        /// Constructs an `uvptr` by transfering ownership from `uvref` with compatible types.
        ///
        template < typename Deleter2, typename... Us >
                requires(
                    vconvertible_to< typelist< Us... >, types > &&
                    convertible_deleter< Deleter2, Deleter > )
        constexpr explicit _uvptr( _uvref< Deleter2, Us... >&& p ) noexcept
        {
                _core = std::move( p._core );
                p._core.reset();
        }

        /// Constructs an `uvptr` by transfering ownership from `uvptr` with compatible types.
        ///
        template < typename Deleter2, typename... Us >
                requires(
                    vconvertible_to< typelist< Us... >, types > &&
                    convertible_deleter< Deleter2, Deleter > )
        constexpr _uvptr( _uvptr< Deleter2, Us... >&& p ) noexcept
          : dbox( std::move( (dbox&) p ) )
        {
                _core = std::move( p._core );
                p._core.reset();
        }


        /// Constructs an `uvptr` which owns a pointer to one of the types that `uvptr` can
        /// reference. If pointer is null, `uvptr` is constructed as if `nullptr` was passed.
        template < typename U >
                requires( vconvertible_type< U, types > )
        constexpr _uvptr( U* u ) noexcept
        {
                if ( u )
                        _core.set( *u );
        }

        /// Constructs an `uvptr` which owns a pointer to one of the types that `uvptr` can
        /// reference. If pointer is null, `uvptr` is constructed as if `nullptr` was passed.
        ///
        /// Internal `Deleter` is copy-constructed from `d`.
        template < typename U >
                requires( vconvertible_type< U, types > && copy_constructible_deleter< Deleter > )
        constexpr _uvptr( U* u, Deleter const& d ) noexcept
          : dbox( d )
        {
                if ( u )
                        _core.set( *u );
        }

        /// Constructs an `uvptr` which owns a pointer to one of the types that `uvptr` can
        /// reference. If pointer is null, `uvptr` is constructed as if `nullptr` was passed.
        ///
        /// Internal `Deleter` is move-constructed from `d`.
        template < typename U >
                requires( vconvertible_type< U, types > && move_constructible_deleter< Deleter > )
        constexpr _uvptr( U* u, Deleter&& d ) noexcept
          : dbox( std::move( d ) )
        {
                if ( u )
                        _core.set( *u );
        }


        /// `nullptr` assignment operator resets this pointer.
        ///
        constexpr _uvptr& operator=( std::nullptr_t ) noexcept
        {
                reset( nullptr );
                return *this;
        }

        /// Move assignment operator transfering ownership from `uvptr` with compatible types.
        /// `Deleter` is move-constructed from the other `uvptr`.
        template < typename Deleter2, typename... Us >
                requires(
                    vconvertible_to< typelist< Us... >, types > &&
                    convertible_deleter< Deleter2, Deleter > )
        constexpr _uvptr& operator=( _uvptr< Deleter2, Us... >&& p ) noexcept
        {
                _uvptr tmp{ std::move( p ) };
                swap( _core, tmp._core );
                swap( (dbox&) ( *this ), (dbox&) tmp );
                return *this;
        }

        /// Dereferences to the pointed-to type. It is `T&` if there is only one type in `Ts...`,
        /// or `void&` otherwise. Undefined behavior on null pointer.
        constexpr auto& operator*() const noexcept
        {
                return *_core.ptr;
        }

        /// Provides member access to the pointed-to type. It is `T*` if there is only one type in
        /// `Ts...`, or `void*` otherwise. Undefined behavior on null pointer.
        constexpr auto* operator->() const noexcept
        {
                return _core.ptr;
        }

        /// Returns a `pointer` to the pointed-to type.
        constexpr pointer get() const noexcept
        {
                pointer res;
                res._core = _core;
                return res;
        }

        /// Returns the index representing the type currently being pointed-to.
        /// The index of the first type is 0, with subsequent types sequentially numbered.
        /// `null_index` constant is used in case the pointer is null.
        [[nodiscard]] constexpr index_type index() const noexcept
        {
                return _core.get_index();
        }

        /// Conversion operator from lvalue reference to types-compatible `vptr`
        ///
        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        constexpr operator _vptr< Us... >() & noexcept
        {
                _vptr< Us... > res;
                res._core = _core;
                return res;
        }

        /// Conversion operator from lvalue const reference to types-compatible `vptr`
        ///
        template < typename... Us >
                requires( vconvertible_to< types, typelist< Us... > > )
        constexpr operator _vptr< Us... >() const& noexcept
        {
                _vptr< Us... > res;
                res._core = _core;
                return res;
        }

        /// Conversion operator from rvalue reference to `vptr` is forbidden
        template < typename... Us >
        constexpr operator _vptr< Us... >() && = delete;

        constexpr void reset( pointer ptr = pointer() )
        {
                auto tmp = _core;
                _core    = std::move( ptr._core );
                tmp.delete_ptr( _deleter_box< Deleter >::get() );
        }

        /// Release the ownership of managed object, if any. Returns `pointer` to the managed
        /// object, it is reponsibility of caller to clean it up.
        constexpr pointer release() noexcept
        {
                pointer res;
                swap( res._core, _core );
                return res;
        }

        /// Check if the pointer is not null.
        ///
        constexpr explicit operator bool() const noexcept
        {
                return _core.get_index() != null_index;
        }

        /// Constructs a variadic reference that points to the same target as this pointer.
        /// Undefined behavior if the pointer is null.
        constexpr reference vref() const& noexcept
        {
                VARI_ASSERT( _core.get_index() != null_index );
                reference res;
                res._core = _core;
                return res;
        }

        /// Constructs an owning variadic reference and trasnfers ownership of current target to the
        /// reference. Undefined behavior if the pointer is null.
        constexpr owning_reference vref() && noexcept
        {
                VARI_ASSERT( _core.get_index() != null_index );
                owning_reference res;
                swap( res._core, _core );
                return res;
        }

        /// Calls the appropriate function from the list `fs...`, based on the type of the current
        /// target, or one with `empty_t` in case of null pointer.
        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... f ) const
        {

                typename _check_unique_invocability< types >::template with_nullable_pure_ref<
                    Fs... >
                    _{};
                if ( _core.ptr == nullptr )
                        return _dispatch_fun( empty, (Fs&&) f... );
                return _core.visit_impl( (Fs&&) f... );
        }

        /// Constructs an owning reference to currently pointed-to type and transfers ownership to
        /// it. Moves it to the appropriate function from the list `fs...`. In case the this is null
        /// calls overload with `empty_t`.
        template < typename... Fs >
        constexpr decltype( auto ) take( Fs&&... fs ) &&
        {
                typename _check_unique_invocability< types >::template with_deleter<
                    Deleter >::template with_nullable_uvref< Fs... >
                     _{};
                auto p = release();
                if ( p._core.ptr == nullptr )
                        return _dispatch_fun( empty, (Fs&&) fs... );
                return p._core.template take_impl< same_uvref >( (Fs&&) fs... );
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

        /// Destroys the owned object, if any.
        ///
        constexpr ~_uvptr()
        {
                _core.delete_ptr( _deleter_box< Deleter >::get() );
        }

        /// Swaps `uvptr` with each other.
        ///
        friend constexpr void swap( _uvptr& lh, _uvptr& rh ) noexcept
        {
                swap( lh._core, rh._core );
                swap( (dbox&) lh, (dbox&) rh );
        }

private:
        core_type _core;

        template < typename Deleter2, typename... Us >
        friend class _uvptr;

        template < typename Deleter2, typename... Us >
        friend class _uvref;
};

/// Compares the internal pointers of both pointers.
///
template < typename... Lhs, typename... Rhs >
constexpr auto operator<=>( _uvptr< Lhs... > const& lh, _uvptr< Rhs... > const& rh ) noexcept
{
        return lh.get() <=> rh.get();
}

/// Compares the internal pointers of both pointers.
///
template < typename... Lhs, typename... Rhs >
constexpr bool operator==( _uvptr< Lhs... > const& lh, _uvptr< Rhs... > const& rh ) noexcept
{
        return lh.get() == rh.get();
}

/// A nullable owning pointer to types derived out of `Ts...` list by flattening it and
/// filtering for unique types.
template < typename... Ts >
using uvptr = _define_variadic< _uvptr, typelist< Ts... >, def_del >;

}  // namespace vari

VARI_REC_GET_HASH_SPECIALIZATION( vari::_uvptr );
