#pragma once

#include "vari/bits/typelist.h"

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

template < typename F, typename... Ts >
concept invokes_something = ( invocable< F, Ts > || ... || false );

}  // namespace vari::bits