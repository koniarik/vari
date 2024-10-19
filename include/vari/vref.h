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

// Non-nullable pointer to one of types in Ts....
template < typename... Ts >
class _vref
{
public:
        using types     = typelist< Ts... >;
        using pointer   = _vptr< Ts... >;
        using reference = _vref< Ts... >;

        // Copy constructor for any compatible vref
        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, types > )
        constexpr _vref( _vref< Us... > const& p ) noexcept
          : _core( p._core )
        {
        }

        // Construct vref from reference to one of types referenceable by vref
        template < typename U >
                requires( vconvertible_to< typelist< U >, types > )
        constexpr _vref( U& u ) noexcept
        {
                _core.set( u );
        }

        // Dereference to pointed-to type, it is T& in case there is only one type in `Ts...`, void&
        // otherwise
        constexpr auto& operator*() const noexcept
        {
                return *_core.ptr;
        }

        // Member access to pointed-to type, it is T* in case there is only one type in `Ts...`,
        // void* otherwise
        constexpr auto* operator->() const noexcept
        {
                return _core.ptr;
        }

        // Returns pointer to pointed-to type, it is T* in case there is only one type in `Ts...`,
        // void* otherwise
        constexpr auto* get() const noexcept
        {
                return _core.ptr;
        }

        // Returns index representing index of type currently being pointed-to by the reference.
        // Index of first type is 0, rest is sequential from that.
        [[nodiscard]] constexpr index_type index() const noexcept
        {
                return _core.get_index();
        }

        // If reference can point only to one type, allows conversion into reference to such type.
        template < typename U >
                requires( vconvertible_to< types, typelist< U > > )
        constexpr operator U&() const noexcept
        {
                return *_core.ptr;
        }

        // Creates variadic pointer pointing to same thing as current reference.
        constexpr pointer vptr() const& noexcept
        {
                pointer res;
                res._core = _core;
                return res;
        }

        // With the current pointed-to value, calls callable out of `fs...` list matching the value
        // type.
        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... fs ) const
        {
                typename _check_unique_invocability< types >::template with_pure_ref< Fs... > _{};
                assert( _core.ptr );
                return _core.visit_impl( (Fs&&) fs... );
        }

        // Swaps current reference with another one
        friend constexpr void swap( _vref& lh, _vref& rh ) noexcept
        {
                swap( lh._core, rh._core );
        }

        // Compares internal pointers of both references
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
