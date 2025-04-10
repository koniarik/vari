/// MIT License
///
/// Copyright (c) 2025 koniarik
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#pragma once

#include <concepts>
#include <cstddef>

namespace vari
{

template < typename T >
struct _dependent_false : std::false_type
{
};

// Most of the templates for typelist specify operation over typelist, which does not have default
// behavior for non-typelist-types. Any such template is guarded by this template to produce
// compiler error in case they are given something they are not compatible with. Always consult the
// documentation of the template to see what is expected.
template < typename T >
struct _default_template_guard
{
        static_assert(
            _dependent_false< T >::value,
            "Default template matched, this implies unexpected usage of the template. Read the documentation of template." );
};

// ---

// compile time set of types Ts...
template < typename... Ts >
struct typelist
{
        using types                       = typelist< Ts... >;
        static constexpr std::size_t size = sizeof...( Ts );
};

template < typename T >
struct typelist_traits
{
        static constexpr bool is_compatible = false;
};

template < typename... Ts >
struct typelist_traits< typelist< Ts... > >
{
        static constexpr bool is_compatible = true;

        using types = typelist< Ts... >;
};

template < typename T >
using typelist_traits_types = typename typelist_traits< T >::types;

template < typename T >
concept typelist_compatible = typelist_traits< T >::is_compatible;

// ---

// Index of type `T` or `T const` in typelist `TL`, exported as ::value. `TL` has to be `typelist`
// or typelist compatible type.
template < typename T, typename TL >
struct index_of_t_or_const_t : _default_template_guard< T >
{
};

template < typename T, typelist_compatible TL >
struct index_of_t_or_const_t< T, TL > : index_of_t_or_const_t< typelist_traits_types< T >, TL >
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
struct index_of_t_or_const_t< T, typelist< T const, Ts... > >
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

// ---

// Exports type at index `j` in typelist `TL` as ::type. `TL` has to be `typelist` or typelist
// compatible type.
template < std::size_t j, typename TL >
struct type_at : _default_template_guard< TL >
{
};

template < std::size_t j, typelist_compatible TL >
struct type_at< j, TL > : type_at< j, typelist_traits_types< TL > >
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

// ---

// ::value is true if type `T` is present in typelist `TL`, false otherwise. `TL` has to be
// `typelist` or typelist compatible type.
template < typename T, typename TL >
struct contains_type : _default_template_guard< T >
{
};

template < typename T, typelist_compatible TL >
struct contains_type< T, TL > : contains_type< T, typelist_traits_types< TL > >
{
};

template < typename T, typename... Ts >
struct contains_type< T, typelist< Ts... > >
{
        static constexpr bool value = ( std::same_as< T, Ts > || ... || false );
};

template < typename T, typename TL >
static constexpr bool contains_type_v = contains_type< T, TL >::value;

// ---

// Filters out duplicates from typelist `TL2`, `TL1` is used as output argument collecting the
// result typelist as it is filtered. ::type will contain the final filtered typelist. `TL1` and
// `TL2` has to be `typelist` or typelist compatible type.
template < typename TL1, typename TL2 >
struct _unique_tl_impl : _default_template_guard< TL1 >
{
};

template < typename TL1, typelist_compatible TL2 >
struct _unique_tl_impl< TL1, TL2 > : _unique_tl_impl< TL1, typelist_traits_types< TL2 > >
{
};

template < typename... Ts >
struct _unique_tl_impl< typelist< Ts... >, typelist<> >
{
        using type = typelist< Ts... >;
};

template < typename TL1, typename T, typename... Ts >
        requires( contains_type_v< T, typelist< Ts... > > )
struct _unique_tl_impl< TL1, typelist< T, Ts... > > : _unique_tl_impl< TL1, typelist< Ts... > >
{
};

template < typename... Us, typename T, typename... Ts >
        requires( !contains_type_v< T, typelist< Ts... > > )
struct _unique_tl_impl< typelist< Us... >, typelist< T, Ts... > >
  : _unique_tl_impl< typelist< Us..., T >, typelist< Ts... > >
{
};

template < typename TL >
using unique_typelist_t = typename _unique_tl_impl< typelist<>, TL >::type;

// ---

// Flattens any `typelist` or typelist compatible type within the `Ts...` set, producing a single
// flat set. `TL` is used as output argument collecting the result typelist. ::type will contain the
// final flattened typelist. `TL` has to be `typelist`.
template < typename TL, typename... Ts >
struct _flatten_impl : _default_template_guard< TL >
{
};

template < typename TL >
struct _flatten_impl< TL >
{
        using type = TL;
};

template < typename... Us, typelist_compatible T, typename... Ts >
struct _flatten_impl< typelist< Us... >, T, Ts... >
  : _flatten_impl< typelist< Us... >, typelist_traits_types< T >, Ts... >
{
};

template < typename... Us, typename T, typename... Ts >
struct _flatten_impl< typelist< Us... >, T, Ts... > : _flatten_impl< typelist< Us..., T >, Ts... >
{
};

template < typename... Us, typename... Ks, typename... Ts >
struct _flatten_impl< typelist< Us... >, typelist< Ks... >, Ts... >
  : _flatten_impl< typelist< Us... >, Ks..., Ts... >
{
};

template < typename... Us, typename... Ks, typename... Ts >
struct _flatten_impl< typelist< Us... >, typelist< Ks... > const, Ts... >
  : _flatten_impl< typelist< Us... >, Ks const..., Ts... >
{
};

template < typename... Ts >
using flatten_t = typename _flatten_impl< typelist<>, Ts... >::type;

// ---

// ::value is true if all types in `LH` are present in `RH`, false otherwise. `LH` and `RH` has
// to be `typelist` or typelist compatible type.
template < typename LH, typename RH >
struct is_subset : _default_template_guard< LH >
{
};

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

// ---

// ::value is true if any type in typelist `TL` is const qualified, false otherwise. `TL` has to be
// `typelist` or typelist compatible type.
template < typename TL >
struct any_is_const : _default_template_guard< TL >
{
};

template < typelist_compatible TL >
struct any_is_const< TL > : any_is_const< typelist_traits_types< TL > >
{
};

template < typename... Us >
struct any_is_const< typelist< Us... > >
{
        static constexpr bool value = ( std::is_const_v< Us > || ... );
};

template < typename TL >
static constexpr bool any_is_const_v = any_is_const< TL >::value;

// ---

// ::value is true if all types in typelist `TL` are const qualified, false otherwise. `TL` has to
// be `typelist` or typelist compatible type.
template < typename TL >
struct all_is_const : _default_template_guard< TL >
{
};

template < typelist_compatible TL >
struct all_is_const< TL > : all_is_const< typelist_traits_types< TL > >
{
};

template < typename... Us >
struct all_is_const< typelist< Us... > >
{
        static constexpr bool value = ( std::is_const_v< Us > && ... );
};

template < typename TL >
static constexpr bool all_is_const_v = all_is_const< TL >::value;

// ---

// ::value is true if none of the types in typelist `TL` are const qualified, false otherwise. `TL`
// has to be `typelist` or typelist compatible type.
template < typename TL >
struct none_is_const : _default_template_guard< TL >
{
};

template < typelist_compatible TL >
struct none_is_const< TL > : none_is_const< typelist_traits_types< TL > >
{
};

template < typename... Us >
struct none_is_const< typelist< Us... > >
{
        static constexpr bool value = !( std::is_const_v< Us > || ... );
};

template < typename TL >
static constexpr bool none_is_const_v = none_is_const< TL >::value;

// ---

// ::value is true if all types in typelist `TL` are nothrow swappable, false otherwise. `TL` has to
// be `typelist` or typelist compatible type.
template < typename TL >
struct all_nothrow_swappable : _default_template_guard< TL >
{
};

template < typelist_compatible TL >
struct all_nothrow_swappable< TL > : all_nothrow_swappable< typelist_traits_types< TL > >
{
};

template < typename... Us >
struct all_nothrow_swappable< typelist< Us... > >
{
        static constexpr bool value = ( std::is_nothrow_swappable_v< Us > && ... && true );
};

template < typename TL >
static constexpr bool all_nothrow_swappable_v = all_nothrow_swappable< TL >::value;

// ---

// ::value is true if all types in typelist `TL` are nothrow move constructible, false otherwise.
// `TL` has to be `typelist` or typelist compatible type.
template < typename TL >
struct all_nothrow_move_constructible : _default_template_guard< TL >
{
};

template < typelist_compatible TL >
struct all_nothrow_move_constructible< TL >
  : all_nothrow_move_constructible< typelist_traits_types< TL > >
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

// ---

// ::value is true if all types in typelist `TL` are nothrow copy constructible, false otherwise.
// `TL` has to be `typelist` or typelist compatible type.
template < typename TL >
struct all_nothrow_copy_constructible : _default_template_guard< TL >
{
};

template < typelist_compatible TL >
struct all_nothrow_copy_constructible< TL >
  : all_nothrow_copy_constructible< typelist_traits_types< TL > >
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

// ---

// ::value is true if all types in typelist `TL` are nothrow destructible, false otherwise.
// `TL` has to be `typelist` or typelist compatible type.
template < typename TL >
struct all_nothrow_destructible : _default_template_guard< TL >
{
};

template < typelist_compatible TL >
struct all_nothrow_destructible< TL > : all_nothrow_destructible< typelist_traits_types< TL > >
{
};

template < typename... Us >
struct all_nothrow_destructible< typelist< Us... > >
{
        static constexpr bool value = ( std::is_nothrow_destructible_v< Us > && ... && true );
};

template < typename TL >
static constexpr bool all_nothrow_destructible_v = all_nothrow_destructible< TL >::value;


// ---

template < typename U, typename T >
concept nothrow_three_way_comparable = noexcept( std::declval< U >() <=> std::declval< T >() );

// ::value is true if all types in typelist `TL` are nothrow three way comparable, false otherwise.
// `TL` has to be `typelist` or typelist compatible type.
template < typename TL >
struct all_nothrow_three_way_comparable : _default_template_guard< TL >
{
};

template < typelist_compatible TL >
struct all_nothrow_three_way_comparable< TL >
  : all_nothrow_three_way_comparable< typelist_traits_types< TL > >
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

// ---

template < typename U, typename T >
concept nothrow_equality_comparable = noexcept( std::declval< U >() == std::declval< T >() );

// ::value is true if all types in typelist `TL` are nothrow equality comparable, false otherwise.
// `TL` has to be `typelist` or typelist compatible type.
template < typename TL >
struct all_nothrow_equality_comparable : _default_template_guard< TL >
{
};

template < typelist_compatible TL >
struct all_nothrow_equality_comparable< TL >
  : all_nothrow_copy_constructible< typelist_traits_types< TL > >
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

// ---

// ::type is a typelist containing all return types of `Fac::operator()< N >()` for `N` going from 0
// to `N`, exclusive. `TL` has to be `typelist` or typelist compatible type. `Fac` has to be a type
// with `operator()< N >()` defined for `N` from 0 to `N`, exclusive.
template < typename TL, std::size_t N, typename Fac >
struct _factory_result_types_impl : _default_template_guard< TL >
{
};

template < typename... Ts, typename Fac >
struct _factory_result_types_impl< typelist< Ts... >, 0, Fac >
{
        using type = typelist< Ts... >;
};

template < typename... Ts, std::size_t N, typename Fac >
struct _factory_result_types_impl< typelist< Ts... >, N, Fac >
{
        using n_type = decltype( std::declval< Fac >().template operator()< N - 1 >() );
        using type =
            typename _factory_result_types_impl< typelist< Ts..., n_type >, N - 1, Fac >::type;
};

template < std::size_t N, typename Fac >
using factory_result_types_t = typename _factory_result_types_impl< typelist<>, N, Fac >::type;

}  // namespace vari
