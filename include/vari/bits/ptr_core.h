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

#include <utility>

namespace vari::bits
{

template < typename TL, typename UL >
struct vptr_cnv_map;

template < typename TL, typename... Us >
struct vptr_cnv_map< TL, typelist< Us... > >
{
        static constexpr std::size_t value[sizeof...( Us ) + 1] = {
            0u,
            1 + index_of< Us, TL >::value... };
};

template < typename B, typename TL >
struct ptr_core
{
        std::size_t index = 0;
        B*          ptr   = nullptr;

        ptr_core() noexcept = default;

        template < typename UL >
                requires( is_subset< UL, TL >::value )
        ptr_core( ptr_core< B, UL > other ) noexcept
          : index( vptr_cnv_map< TL, UL >::value[other.get_index()] )
          , ptr( other.ptr )
        {
        }

        template < typename U >
                requires( contains_type< U, TL >::value )
        ptr_core( U& val ) noexcept
          : index( 1 + index_of< U, TL >::value )
          , ptr( &val )
        {
        }

        constexpr std::size_t get_index() noexcept
        {
                return index;
        }

        template < typename F >
        decltype( auto ) match_impl( F&& f )
        {
                return dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            using U = typename type_at< j, TL >::type;
                            return std::forward< F >( f )( static_cast< U* >( ptr ) );
                    } );
        }

        void delete_ptr()
        {
                dispatch_index< 0, TL::size >( index - 1, [&]< std::size_t j > {
                        using U = typename type_at< j, TL >::type;
                        delete static_cast< U* >( ptr );
                } );
        }
};

template < typename B, typename T >
struct ptr_core< B, typelist< T > >
{
        T* ptr = nullptr;

        ptr_core() noexcept = default;

        ptr_core( ptr_core< B, typelist<> > ) noexcept
          : ptr( nullptr )
        {
        }

        ptr_core( T& val ) noexcept
          : ptr( &val )
        {
        }

        constexpr std::size_t get_index() noexcept
        {
                return ptr == nullptr ? 0 : 1;
        }

        template < typename F >
        decltype( auto ) match_impl( F&& f )
        {
                return std::forward< F >( f )( ptr );
        }

        void delete_ptr()
        {
                delete ptr;
        }
};

template < typename B, typename T >
constexpr auto operator<=>( ptr_core< B, T > const& lh, ptr_core< B, T > const& rh )
{
        return std::compare_three_way{}( lh.ptr, rh.ptr );
}

template < typename B, typename T >
constexpr bool operator==( ptr_core< B, T > const& lh, ptr_core< B, T > const& rh )
{
        return lh.ptr == rh.ptr;
}

}  // namespace vari::bits