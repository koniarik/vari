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

#include <concepts>
#include <cstddef>

namespace vari
{

/// ---

template < typename... Ts >
struct typelist
{
        static constexpr std::size_t size                 = sizeof...( Ts );
        using types                                       = typelist< Ts... >;
        static constexpr bool is_vari_compatible_typelist = true;
};

template < typename T >
concept typelist_compatible = T::is_vari_compatible_typelist;

/// ---

template < typename T, typename TL >
struct index_of_t_or_const_t;

template < typename T, typelist_compatible TL >
struct index_of_t_or_const_t< T, TL > : index_of_t_or_const_t< typename T::types, TL >
{
};

template < typename T >
struct index_of_t_or_const_t< T, typelist<> >
{
        static_assert( !std::same_as< T, T >, "Type T is not present in the type list" );
};

template < typename T, typename... Ts >
struct index_of_t_or_const_t< T, typelist< T, Ts... > >
{
        static constexpr std::size_t value = 0;
};

template < typename T, typename... Ts >
struct index_of_t_or_const_t< T, typelist< const T, Ts... > >
{
        static constexpr std::size_t value = 0;
};

template < typename T, typename U, typename... Ts >
struct index_of_t_or_const_t< T, typelist< U, Ts... > >
{
        static constexpr std::size_t value =
            1 + index_of_t_or_const_t< T, typelist< Ts... > >::value;
};

template < typename T, typename TL >
static constexpr std::size_t index_of_t_or_const_t_v = index_of_t_or_const_t< T, TL >::value;

/// ---

template < std::size_t j, typename TL >
struct type_at;

template < std::size_t j, typelist_compatible TL >
struct type_at< j, TL > : type_at< j, typename TL::types >
{
};

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

template < std::size_t j, typename TL >
using type_at_t = typename type_at< j, TL >::type;

/// ---

template < typename T, typename TL >
struct contains_type;

template < typename T, typelist_compatible TL >
struct contains_type< T, TL > : contains_type< T, typename TL::types >
{
};

template < typename T, typename... Ts >
struct contains_type< T, typelist< Ts... > >
{
        static constexpr bool value = ( std::same_as< T, Ts > || ... || false );
};

template < typename T, typename TL >
static constexpr bool contains_type_v = contains_type< T, TL >::value;

/// ---

template < typename TL1, typename TL2 >
struct unique_tl_impl;

template < typename TL1, typelist_compatible TL2 >
struct unique_tl_impl< TL1, TL2 > : unique_tl_impl< TL1, typename TL2::types >
{
};

template < typename... Ts >
struct unique_tl_impl< typelist< Ts... >, typelist<> >
{
        using type = typelist< Ts... >;
};

template < typename TL1, typename T, typename... Ts >
        requires( contains_type_v< T, typelist< Ts... > > )
struct unique_tl_impl< TL1, typelist< T, Ts... > > : unique_tl_impl< TL1, typelist< Ts... > >
{
};

template < typename... Us, typename T, typename... Ts >
        requires( !contains_type_v< T, typelist< Ts... > > )
struct unique_tl_impl< typelist< Us... >, typelist< T, Ts... > >
  : unique_tl_impl< typelist< Us..., T >, typelist< Ts... > >
{
};

template < typename TL >
using unique_typelist_t = typename unique_tl_impl< typelist<>, TL >::type;

/// ---

template < typename TL, typename... Ts >
struct flatten_impl;

template < typename TL >
struct flatten_impl< TL >
{
        using type = TL;
};

template < typename... Us, typelist_compatible T, typename... Ts >
struct flatten_impl< typelist< Us... >, T, Ts... >
  : flatten_impl< typelist< Us... >, typename T::types, Ts... >
{
};

template < typename... Us, typename T, typename... Ts >
struct flatten_impl< typelist< Us... >, T, Ts... > : flatten_impl< typelist< Us..., T >, Ts... >
{
};

template < typename... Us, typename... Ks, typename... Ts >
struct flatten_impl< typelist< Us... >, typelist< Ks... >, Ts... >
  : flatten_impl< typelist< Us... >, Ks..., Ts... >
{
};

template < typename... Us, typename... Ks, typename... Ts >
struct flatten_impl< typelist< Us... >, typelist< Ks... > const, Ts... >
  : flatten_impl< typelist< Us... >, Ks const..., Ts... >
{
};

template < typename... Ts >
using flatten_t = typename flatten_impl< typelist<>, Ts... >::type;

/// ---

template < typename LH, typename RH >
struct is_subset;

template < typename... Us, typename RH >
struct is_subset< typelist< Us... >, RH >
{
        static constexpr bool value = ( contains_type_v< Us, RH > && ... );
};

template < typename... Us, typename RH >
struct is_subset< typelist< Us... > const, RH >
{
        static constexpr bool value = ( contains_type_v< Us const, RH > && ... );
};

template < typename LH, typename RH >
static constexpr bool is_subset_v = is_subset< LH, RH >::value;

/// ---

template < typename TL >
struct any_is_const;

template < typelist_compatible TL >
struct any_is_const< TL > : any_is_const< typename TL::types >
{
};

template < typename... Us >
struct any_is_const< typelist< Us... > >
{
        static constexpr bool value = ( std::is_const_v< Us > || ... );
};

template < typename TL >
static constexpr bool any_is_const_v = any_is_const< TL >::value;

/// ---

template < typename TL >
struct all_is_const;

template < typelist_compatible TL >
struct all_is_const< TL > : all_is_const< typename TL::types >
{
};

template < typename... Us >
struct all_is_const< typelist< Us... > >
{
        static constexpr bool value = ( std::is_const_v< Us > && ... );
};

template < typename TL >
static constexpr bool all_is_const_v = all_is_const< TL >::value;

/// ---

template < typename TL >
struct none_is_const;

template < typelist_compatible TL >
struct none_is_const< TL > : none_is_const< typename TL::types >
{
};

template < typename... Us >
struct none_is_const< typelist< Us... > >
{
        static constexpr bool value = !( std::is_const_v< Us > || ... );
};

template < typename TL >
static constexpr bool none_is_const_v = none_is_const< TL >::value;

/// ---

template < typename TL >
struct all_nothrow_swappable;

template < typelist_compatible TL >
struct all_nothrow_swappable< TL > : all_nothrow_swappable< typename TL::types >
{
};

template < typename... Us >
struct all_nothrow_swappable< typelist< Us... > >
{
        static constexpr bool value = ( std::is_nothrow_swappable_v< Us > && ... && true );
};

template < typename TL >
static constexpr bool all_nothrow_swappable_v = all_nothrow_swappable< TL >::value;

/// ---

template < typename TL >
struct all_nothrow_move_constructible;

template < typelist_compatible TL >
struct all_nothrow_move_constructible< TL > : all_nothrow_move_constructible< typename TL::types >
{
};

template < typename... Us >
struct all_nothrow_move_constructible< typelist< Us... > >
{
        static constexpr bool value = ( std::is_nothrow_move_constructible_v< Us > && ... && true );
};

template < typename TL >
static constexpr bool all_nothrow_move_constructible_v =
    all_nothrow_move_constructible< TL >::value;

/// ---

template < typename TL >
struct all_nothrow_copy_constructible;

template < typelist_compatible TL >
struct all_nothrow_copy_constructible< TL > : all_nothrow_copy_constructible< typename TL::types >
{
};

template < typename... Us >
struct all_nothrow_copy_constructible< typelist< Us... > >
{
        static constexpr bool value = ( std::is_nothrow_copy_constructible_v< Us > && ... && true );
};

template < typename TL >
static constexpr bool all_nothrow_copy_constructible_v =
    all_nothrow_copy_constructible< TL >::value;

/// ---

template < typename TL >
struct all_nothrow_destructible;

template < typelist_compatible TL >
struct all_nothrow_destructible< TL > : all_nothrow_destructible< typename TL::types >
{
};


template < typename... Us >
struct all_nothrow_destructible< typelist< Us... > >
{
        static constexpr bool value = ( std::is_nothrow_destructible_v< Us > && ... && true );
};

template < typename TL >
static constexpr bool all_nothrow_destructible_v = all_nothrow_destructible< TL >::value;


/// ---

template < typename U, typename T >
concept nothrow_three_way_comparable = noexcept( std::declval< U >() <=> std::declval< T >() );

template < typename TL >
struct all_nothrow_three_way_comparable;

template < typelist_compatible TL >
struct all_nothrow_three_way_comparable< TL >
  : all_nothrow_three_way_comparable< typename TL::types >
{
};


template < typename... Us >
struct all_nothrow_three_way_comparable< typelist< Us... > >
{
        static constexpr bool value = ( nothrow_three_way_comparable< Us, Us > && ... && true );
};

template < typename TL >
static constexpr bool all_nothrow_three_way_comparable_v =
    all_nothrow_three_way_comparable< TL >::value;

/// ---

template < typename U, typename T >
concept nothrow_equality_comparable = noexcept( std::declval< U >() == std::declval< T >() );

template < typename TL >
struct all_nothrow_equality_comparable;

template < typelist_compatible TL >
struct all_nothrow_equality_comparable< TL > : all_nothrow_copy_constructible< typename TL::types >
{
};

template < typename... Us >
struct all_nothrow_equality_comparable< typelist< Us... > >
{
        static constexpr bool value = ( nothrow_equality_comparable< Us, Us > && ... && true );
};

template < typename TL >
static constexpr bool all_nothrow_equality_comparable_v =
    all_nothrow_equality_comparable< TL >::value;

}  // namespace vari
