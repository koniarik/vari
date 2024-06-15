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

template < bool ConstBase, typename TL, typename UL >
struct _vptr_cnv_map;

template < typename TL, typename... Us >
struct _vptr_cnv_map< false, TL, typelist< Us... > >
{
        static constexpr std::size_t value[sizeof...( Us ) + 1] = {
            0u,
            1 + index_of_v< Us, TL >... };
};

template < typename TL, typename... Us >
struct _vptr_cnv_map< true, TL, typelist< Us... > >
{
        static_assert( all_is_const_v< TL > );

        static constexpr std::size_t value[sizeof...( Us ) + 1] = {
            0u,
            1 + index_of_v< const Us, TL >... };
};

template < typename B, typename TL >
struct _ptr_core
{
        std::size_t index = 0;
        B*          ptr   = nullptr;

        static constexpr bool const_base = std::is_const_v< B >;

        _ptr_core() noexcept = default;

        template < typename C, typename UL >
                requires( vconvertible_to< C, UL, B, TL > )
        _ptr_core( _ptr_core< C, UL > other ) noexcept
          : index( _vptr_cnv_map< const_base, TL, UL >::value[other.get_index()] )
          , ptr( other.ptr )
        {
        }

        template < typename U >
                requires( contains_type_v< U, TL > )
        _ptr_core( U& val ) noexcept
          : index( 1 + index_of_v< U, TL > )
          , ptr( &val )
        {
        }

        constexpr std::size_t get_index() const noexcept
        {
                return index;
        }

        template < typename... Fs >
        decltype( auto ) visit_impl( Fs&&... fs ) const
        {
                return _dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            using U  = type_at_t< j, TL >;
                            auto&& f = _function_picker< U& >::pick( std::forward< Fs >( fs )... );
                            U*     p = static_cast< U* >( ptr );
                            return std::forward< decltype( f ) >( f )( *p );
                    } );
        }

        template < template < typename... > typename ArgTempl, typename... Fs >
        decltype( auto ) match_impl( Fs&&... fs ) const
        {
                return _dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            using U       = type_at_t< j, TL >;
                            using ArgType = ArgTempl< B, U >;
                            auto&& f =
                                _function_picker< ArgType >::pick( std::forward< Fs >( fs )... );
                            U* p = static_cast< U* >( ptr );
                            return std::forward< decltype( f ) >( f )( ArgType{ *p } );
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
                            using ArgType  = ArgTempl< B, U >;
                            using ConvType = ConvTempl< B, U >;
                            auto&& f =
                                _function_picker< ArgType >::pick( std::forward< Fs >( fs )... );
                            U* p = static_cast< U* >( ptr );
                            return std::forward< decltype( f ) >( f )( ArgType( ConvType( *p ) ) );
                    } );
        }

        void delete_ptr()
        {
                _dispatch_index< 0, TL::size >( index - 1, [&]< std::size_t j > {
                        using U = type_at_t< j, TL >;
                        delete static_cast< U* >( ptr );
                } );
        }
};

template < typename B, typename T >
struct _ptr_core< B, typelist< T > >
{
        T* ptr = nullptr;

        _ptr_core() noexcept = default;

        template < typename C >
        _ptr_core( _ptr_core< C, typelist<> > ) noexcept
          : ptr( nullptr )
        {
        }

        _ptr_core( T& val ) noexcept
          : ptr( &val )
        {
        }

        constexpr std::size_t get_index() const noexcept
        {
                return ptr == nullptr ? 0 : 1;
        }

        template < typename... Fs >
        decltype( auto ) visit_impl( Fs&&... fs ) const
        {
                auto&& f = _function_picker< T& >::pick( std::forward< Fs >( fs )... );
                return std::forward< decltype( f ) >( f )( *ptr );
        }

        template < template < typename... > typename ArgTempl, typename... Fs >
        decltype( auto ) match_impl( Fs&&... fs ) const
        {
                using ArgType = ArgTempl< B, T >;
                auto&& f      = _function_picker< ArgType >::pick( std::forward< Fs >( fs )... );
                return std::forward< decltype( f ) >( f )( ArgType( *ptr ) );
        }

        template <
            template < typename... >
            typename ArgTempl,
            template < typename... >
            typename ConvTempl,
            typename... Fs >
        decltype( auto ) take_impl( Fs&&... fs ) const
        {
                using ArgType  = ArgTempl< B, T >;
                auto&& f       = _function_picker< ArgType >::pick( std::forward< Fs >( fs )... );
                using ConvType = ConvTempl< B, T >;
                return std::forward< decltype( f ) >( f )( ArgType( ConvType( *ptr ) ) );
        }

        void delete_ptr()
        {
                delete ptr;
        }
};

template < typename B, typename T >
constexpr auto operator<=>( _ptr_core< B, T > const& lh, _ptr_core< B, T > const& rh )
{
        return std::compare_three_way{}( lh.ptr, rh.ptr );
}

template < typename B, typename T >
constexpr bool operator==( _ptr_core< B, T > const& lh, _ptr_core< B, T > const& rh )
{
        return lh.ptr == rh.ptr;
}

}  // namespace vari
