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

template < typename TL, typename UL >
struct _vptr_cnv_map;

template < typename TL, typename... Us >
struct _vptr_cnv_map< TL, typelist< Us... > >
{
        static constexpr std::size_t value[sizeof...( Us ) + 1] = {
            0u,
            1 + index_of_v< Us, TL >... };
};

template < typename T >
constexpr void* to_void_cast( T* p ) noexcept
{
        return static_cast< void* >( const_cast< std::remove_const_t< T >* >( p ) );
}

template < typename TL >
struct _ptr_core
{

        std::size_t index = 0;
        void*       ptr   = nullptr;


        _ptr_core() noexcept = default;

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        _ptr_core( _ptr_core< UL > other ) noexcept
          : index( _vptr_cnv_map< TL, UL >::value[other.get_index()] )
          , ptr( to_void_cast( other.ptr ) )
        {
        }

        template < typename U >
                requires( vconvertible_to< typelist< U >, TL > )
        _ptr_core( U& val ) noexcept
          : index( 1 + index_of_v< U, TL > )
          , ptr( to_void_cast( &val ) )
        {
        }

        [[nodiscard]] constexpr std::size_t get_index() const noexcept
        {
                return index;
        }

        template < typename... Fs >
        decltype( auto ) visit_impl( Fs&&... fs ) const
        {
                return _dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            using U = type_at_t< j, TL >;
                            U* p    = static_cast< U* >( ptr );
                            return _dispatch_fun( *p, std::forward< Fs >( fs )... );
                    } );
        }

        template < template < typename... > typename ArgTempl, typename... Fs >
        decltype( auto ) match_impl( Fs&&... fs ) const
        {
                return _dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            using U       = type_at_t< j, TL >;
                            using ArgType = ArgTempl< U >;
                            U* p          = static_cast< U* >( ptr );
                            return _dispatch_fun( ArgType{ *p }, std::forward< Fs >( fs )... );
                    } );
        }

        template <
            template < typename... >
            typename ArgTempl,
            template < typename... >
            typename ConvTempl,
            typename... Fs >
        decltype( auto ) take_impl( Fs&&... fs ) const
        {
                return _dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            using U        = type_at_t< j, TL >;
                            using ArgType  = ArgTempl< U >;
                            using ConvType = ConvTempl< U >;
                            U* p           = static_cast< U* >( ptr );
                            return _dispatch_fun(
                                ArgType{ ConvType{ *p } }, std::forward< Fs >( fs )... );
                    } );
        }

        void delete_ptr()
        {
                if ( index == 0 )
                        return;
                _dispatch_index< 0, TL::size >( index - 1, [&]< std::size_t j > {
                        using U = type_at_t< j, TL >;
                        delete static_cast< U* >( ptr );
                } );
        }
};

template < typename T >
struct _ptr_core< typelist< T > >
{

        T* ptr = nullptr;

        _ptr_core() noexcept = default;

        _ptr_core( _ptr_core< typelist<> > ) noexcept
          : ptr( nullptr )
        {
        }

        _ptr_core( T& val ) noexcept
          : ptr( &val )
        {
        }

        [[nodiscard]] constexpr std::size_t get_index() const noexcept
        {
                return ptr == nullptr ? 0 : 1;
        }

        template < typename... Fs >
        decltype( auto ) visit_impl( Fs&&... fs ) const
        {
                return _dispatch_fun( *ptr, std::forward< Fs >( fs )... );
        }

        template < template < typename... > typename ArgTempl, typename... Fs >
        decltype( auto ) match_impl( Fs&&... fs ) const
        {
                using ArgType = ArgTempl< T >;
                return _dispatch_fun( ArgType( *ptr ), std::forward< Fs >( fs )... );
        }

        template <
            template < typename... >
            typename ArgTempl,
            template < typename... >
            typename ConvTempl,
            typename... Fs >
        decltype( auto ) take_impl( Fs&&... fs ) const
        {
                using ArgType  = ArgTempl< T >;
                using ConvType = ConvTempl< T >;
                return _dispatch_fun( ArgType( ConvType( *ptr ) ), std::forward< Fs >( fs )... );
        }

        void delete_ptr()
        {
                if ( ptr != nullptr )
                        delete ptr;
        }
};

template < typename T >
constexpr auto operator<=>( _ptr_core< T > const& lh, _ptr_core< T > const& rh )
{
        return std::compare_three_way{}( lh.ptr, rh.ptr );
}

template < typename T >
constexpr bool operator==( _ptr_core< T > const& lh, _ptr_core< T > const& rh )
{
        return lh.ptr == rh.ptr;
}

}  // namespace vari
