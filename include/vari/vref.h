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


namespace vari
{

template < typename... Ts >
class _vref
{
        using TL = typelist< Ts... >;

public:
        static_assert( is_flat_v< TL > );

        template < typename... Us >
                requires( vconvertible_to< typelist< Us... >, TL > )
        _vref( _vref< Us... > p ) noexcept
          : _core( p._core )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, TL > )
        _vref( U& u ) noexcept
          : _core( u )
        {
        }

        auto& operator*() const noexcept
        {
                return *_core.ptr;
        }

        auto* operator->() const noexcept
        {
                return _core.ptr;
        }

        auto* get() const noexcept
        {
                return _core.ptr;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... fs ) const
        {
                return _core.template visit_impl( std::forward< Fs >( fs )... );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... fs ) const
        {
                return _core.template match_impl< _vref >( std::forward< Fs >( fs )... );
        }

        friend void swap( _vref& lh, _vref& rh ) noexcept
        {
                std::swap( lh._core, rh._core );
        }

        friend auto operator<=>( _vref const& lh, _vref const& rh ) = default;

private:
        _vref() = default;

        _ptr_core< TL > _core;

        template < typename... Us >
        friend class _vref;
        template < typename... Us >
        friend class _vptr;
        template < typename... Us >
        friend class _uvref;
        template < typename... Us >
        friend class _uvptr;
};

template < typename... Ts >
using vref = _define_vptr< _vref, typelist< Ts... > >;

}  // namespace vari
