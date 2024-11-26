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


/// A nullable pointer to one of the types in Ts...
///
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

        /// Construct a pointer in a null state.
        ///
        constexpr _vptr( std::nullptr_t ) noexcept
        {
        }

        /// Copy constructor for any compatible `vref`.
        ///
        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr explicit _vptr( _vref< Us... > const& r ) noexcept
          : _core( r._core )
        {
        }

        /// Copy constructor for any compatible `vptr`.
        ///
        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vptr( _vptr< Us... > const& p ) noexcept
          : _core( p._core )
        {
        }

        /// Constructs a `vptr` from a pointer to one of the types that `vptr` can reference.
        /// If pointer is null, `vptr` is constructed as if `nullptr` was passed.
        ///
        template < typename U >
                requires( vconvertible_to< typelist< U >, types > )
        constexpr _vptr( U* u ) noexcept
        {
                if ( u )
                        _core.set( *u );
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

        /// Returns a pointer to the pointed-to type. It is `T*` if there is only one type in
        /// `Ts...`, or `void*` otherwise. Can be null.
        constexpr auto* get() const noexcept
        {
                return _core.ptr;
        }

        /// Returns the index representing the type currently being pointed-to.
        /// The index of the first type is 0, with subsequent types sequentially numbered.
        /// `null_index` constant is used in case the pointer is null.
        [[nodiscard]] constexpr index_type index() const noexcept
        {
                return _core.get_index();
        }

        /// Check if the pointer is not null.
        ///
        constexpr operator bool() const noexcept
        {
                return _core.ptr != nullptr;
        }

        /// Constructs a variadic reference that points to the same target as this pointer.
        /// Undefined behavior if the pointer is null.
        constexpr reference vref() const noexcept
        {
                assert( _core.ptr );

                reference r;
                r._core = _core;
                return r;
        }

        /// Calls the appropriate function from the list `fs...`, based on the type of the current
        /// target, or one with `empty_t` in case of null pointer.
        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... fs ) const
        {
                typename _check_unique_invocability< types >::template with_nullable_pure_ref<
                    Fs... >
                    _{};
                if ( _core.ptr == nullptr )
                        return _dispatch_fun( empty, (Fs&&) fs... );
                return _core.visit_impl( (Fs&&) fs... );
        }


        /// Swaps `vptr` with each other.
        ///
        friend constexpr void swap( _vptr& lh, _vptr& rh ) noexcept
        {
                swap( lh._core, rh._core );
        }

        /// Compares the internal pointers of both pointers.
        ///
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

/// A nullable pointer to types derived out of `Ts...` list by flattening it and filtering for
/// unique types.
template < typename... Ts >
using vptr = _define_variadic< _vptr, typelist< Ts... > >;

}  // namespace vari

VARI_GET_PTR_HASH_SPECIALIZATION( vari::_vptr );
