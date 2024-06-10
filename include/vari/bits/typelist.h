#pragma once

#include <concepts>
#include <cstdint>

namespace vari::bits
{
template < typename... Ts >
struct typelist
{
        static constexpr std::size_t size = sizeof...( Ts );
};

template < typename T, typename TL >
struct index_of;

template < typename T, typename... Ts >
struct index_of< T, typelist< T, Ts... > >
{
        static constexpr std::size_t value = 0;
};

template < typename T, typename U, typename... Ts >
struct index_of< T, typelist< U, Ts... > >
{
        static constexpr std::size_t value = 1 + index_of< T, typelist< Ts... > >::value;
};

template < std::size_t j, typename TL >
struct type_at;

template < std::size_t j, typename T, typename... Ts >
struct type_at< j, typelist< T, Ts... > >
{
        using type = typename type_at< j - 1, typelist< Ts... > >::type;
};

template < typename T, typename... Ts >
struct type_at< 0, typelist< T, Ts... > >
{
        using type = T;
};

template < typename T, typename TL >
struct contains_type;

template < typename T, typename... Ts >
struct contains_type< T, typelist< Ts... > >
{
        static constexpr bool value = ( std::same_as< T, Ts > || ... || false );
};

template < typename TL1, typename TL2 >
struct unique_typelist;

template < typename... Ts >
struct unique_typelist< typelist< Ts... >, typelist<> >
{
        using type = typelist< Ts... >;
};

template < typename TL1, typename T, typename... Ts >
        requires( contains_type< T, typelist< Ts... > >::value )
struct unique_typelist< TL1, typelist< T, Ts... > >
{
        using type = typename unique_typelist< TL1, typelist< Ts... > >::type;
};

template < typename... Us, typename T, typename... Ts >
        requires( !contains_type< T, typelist< Ts... > >::value )
struct unique_typelist< typelist< Us... >, typelist< T, Ts... > >
{
        using type = typename unique_typelist< typelist< Us..., T >, typelist< Ts... > >::type;
};

template < typename TL, typename... Ts >
struct flatten_typelist;

template < typename TL >
struct flatten_typelist< TL >
{
        using type = TL;
};

template < typename... Us, typename T, typename... Ts >
struct flatten_typelist< typelist< Us... >, T, Ts... >
{
        using type = typename flatten_typelist< typelist< Us..., T >, Ts... >::type;
};

template < typename... Us, typename... Ks, typename... Ts >
struct flatten_typelist< typelist< Us... >, typelist< Ks... >, Ts... >
{
        using type = typename flatten_typelist< typelist< Us... >, Ks..., Ts... >::type;
};

template < typename T >
struct is_flat;

template <>
struct is_flat< typelist<> >
{
        static constexpr bool value = true;
};

template < typename T, typename... Ts >
struct is_flat< typelist< T, Ts... > >
{
        static constexpr bool value = is_flat< typelist< Ts... > >::value;
};

template < typename... Us, typename... Ts >
struct is_flat< typelist< typelist< Us... >, Ts... > >
{
        static constexpr bool value = false;
};

template < typename LH, typename RH >
struct is_subset;

template < typename... Us, typename RH >
struct is_subset< typelist< Us... >, RH >
{
        static constexpr bool value = ( contains_type< Us, RH >::value && ... );
};

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

}  // namespace vari::bits
