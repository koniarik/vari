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

#include "vari/bits/dispatch.h"
#include "vari/bits/typelist.h"
#include "vari/bits/util.h"

#include <compare>
#include <cstddef>
#include <utility>

namespace vari
{

template < typename T >
constexpr void* _to_void_cast( T* p ) noexcept
{
        return static_cast< void* >( const_cast< std::remove_const_t< T >* >( p ) );
}

template < typename TL >
struct _ptr_core
{

        index_type index = null_index;
        void*      ptr   = nullptr;


        constexpr _ptr_core() noexcept = default;

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        constexpr _ptr_core( _ptr_core< UL > other ) noexcept
          : index( _vptr_cnv_map< TL, UL >::conv( other.get_index() ) )
          , ptr( _to_void_cast( other.ptr ) )
        {
        }

        friend constexpr void swap( _ptr_core& lh, _ptr_core& rh ) noexcept
        {
                std::swap( lh.ptr, rh.ptr );
                std::swap( lh.index, rh.index );
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, TL > )
        constexpr void set( U& val ) noexcept
        {
                index = index_of_t_or_const_t_v< U, TL >;
                ptr   = _to_void_cast( &val );
        }

        constexpr void reset() noexcept
        {
                *this = _ptr_core{};
        }

        [[nodiscard]] constexpr index_type get_index() const noexcept
        {
                return index;
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit_impl( Fs&&... fs ) const
        {
                return _dispatch_index< 0, TL::size >(
                    index, [&]< index_type j >() -> decltype( auto ) {
                            using U = type_at_t< j, TL >;
                            U* p    = static_cast< U* >( ptr );
                            return _dispatch_fun( *p, (Fs&&) fs... );
                    } );
        }

        template < template < typename... > typename ArgTempl, typename... Fs >
        constexpr decltype( auto ) take_impl( Fs&&... fs ) const
        {
                return _dispatch_index< 0, TL::size >(
                    index, [&]< index_type j >() -> decltype( auto ) {
                            using U       = type_at_t< j, TL >;
                            using ArgType = ArgTempl< U >;
                            U* p          = static_cast< U* >( ptr );
                            return _dispatch_fun( ArgType{ *p }, (Fs&&) fs... );
                    } );
        }

        constexpr void delete_ptr( auto&& del )
        {
                if ( index == null_index )
                        return;
                _dispatch_index< 0, TL::size >( index, [&]< index_type j > {
                        using U = type_at_t< j, TL >;
                        del( static_cast< U* >( ptr ) );
                } );
        }
};

template < typename T >
struct _ptr_core< typelist< T > >
{

        T* ptr = nullptr;

        constexpr _ptr_core() noexcept = default;

        constexpr _ptr_core( _ptr_core< typelist<> > ) noexcept
          : ptr( nullptr )
        {
        }

        template < typename U >
                requires( std::same_as< U, T > || std::same_as< U const, T > )
        constexpr _ptr_core( _ptr_core< typelist< U > > other ) noexcept
          : ptr( other.ptr )
        {
        }

        friend constexpr void swap( _ptr_core& lh, _ptr_core& rh ) noexcept
        {
                std::swap( lh.ptr, rh.ptr );
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, typelist< T > > )
        constexpr void set( U& val ) noexcept
        {
                ptr = &val;
        }

        constexpr void reset() noexcept
        {
                *this = _ptr_core{};
        }


        [[nodiscard]] constexpr index_type get_index() const noexcept
        {
                return ptr == nullptr ? null_index : 0;
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit_impl( Fs&&... fs ) const
        {
                return _dispatch_fun( *ptr, (Fs&&) fs... );
        }

        template < template < typename... > typename ArgTempl, typename... Fs >
        constexpr decltype( auto ) take_impl( Fs&&... fs ) const
        {
                using ArgType = ArgTempl< T >;
                return _dispatch_fun( ArgType( *ptr ), (Fs&&) fs... );
        }

        constexpr void delete_ptr( auto&& del )
        {
                if ( ptr != nullptr )
                        del( ptr );
        }
};

template < typename T >
constexpr auto operator<=>( _ptr_core< T > const& lh, _ptr_core< T > const& rh ) noexcept
{
        return std::compare_three_way{}( lh.ptr, rh.ptr );
}

template < typename T >
constexpr bool operator==( _ptr_core< T > const& lh, _ptr_core< T > const& rh ) noexcept
{
        return lh.ptr == rh.ptr;
}

}  // namespace vari
