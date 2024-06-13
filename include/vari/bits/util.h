#pragma once

#include "vari/bits/typelist.h"

#include <utility>

namespace vari::bits
{

template < template < typename... > typename T, typename U, typename TL >
struct vptr_apply;

template < template < typename... > typename T, typename U, typename... Ts >
struct vptr_apply< T, U, typelist< Ts... > >
{
        using type = T< U, Ts... >;
};

template < template < typename... > typename T, typename U, typename TL >
using define_vptr = typename vptr_apply<
    T,
    U,
    typename unique_typelist<  //
        typelist<>,
        typename flatten_typelist< typelist<>, TL >::type >::type >::type;

template < typename F, typename... Args >
concept invocable = requires( F&& f, Args&&... args ) {
        std::forward< F >( f )( std::forward< Args >( args )... );
};

template < typename... Args >
struct function_picker
{
        template < typename F, typename... Fs >
        static constexpr decltype( auto ) pick( F&& f, Fs&&... fs )
        {
                if constexpr ( invocable< F, Args... > ) {
                        static_assert(
                            ( !invocable< Fs, Args... > && ... ),
                            "Only one of the functors should be invocable with Args..." );
                        return std::forward< F >( f );
                } else {
                        static_assert( sizeof...( fs ) != 0 );
                        return pick( std::forward< Fs >( fs )... );
                }
        }
};

}  // namespace vari::bits