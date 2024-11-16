
#pragma once

#include "vari/bits/typelist.h"

namespace vari
{

template < typename FromTL, typename ToTL >
concept vconvertible_to = is_subset_v< FromTL, ToTL > || is_subset_v< FromTL const, ToTL >;

template < typename U, typename TL >
concept vconvertible_type = contains_type_v< U, TL > || contains_type_v< U const, TL >;

template < typename Deleter >
concept copy_constructible_deleter =
    std::is_lvalue_reference_v< Deleter > ||
    ( !std::is_reference_v< Deleter > && std::is_nothrow_copy_constructible_v< Deleter > );

template < typename Deleter >
concept move_constructible_deleter =
    !std::is_lvalue_reference_v< Deleter > && std::is_nothrow_move_constructible_v< Deleter >;

template < typename FromDeleter, typename ToDeleter >
concept convertible_deleter = std::is_nothrow_constructible_v< ToDeleter, FromDeleter >;

template < typename V, typename TL >
concept variadic_with = is_subset_v< TL, typename V::types >;

template < typename V, typename T >
concept variadic_with_type = contains_type_v< T, typename V::types >;

}  // namespace vari
