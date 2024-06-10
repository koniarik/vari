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
        decltype( auto ) take_bits( F&& f )
        {
                dispatch< 0, TL::size >( index - 1, [&]< std::size_t j > {
                        using U = typename type_at< j, TL >::type;
                        return std::forward< F >( f )( static_cast< U* >( ptr ) );
                } );
        }
};

template < typename B, typename T >
struct ptr_core< B, typelist< T > >
{
        T* ptr = nullptr;

        ptr_core() noexcept = default;

        ptr_core( T& val ) noexcept
          : ptr( &val )
        {
        }

        constexpr std::size_t get_index() noexcept
        {
                return ptr == nullptr ? 0 : 1;
        }

        template < typename F >
        decltype( auto ) take_bits( F&& f )
        {
                return std::forward< F >( f )( ptr );
        }
};

}  // namespace vari::bits