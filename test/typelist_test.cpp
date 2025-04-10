
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
#include "vari/bits/typelist.h"

#include "test_types.h"
#include "vari/concept.h"
#include "vari/uvptr.h"
#include "vari/vref.h"

#include <doctest/doctest.h>
#include <string>
#include <vector>

namespace vari
{

static_assert( typelist<>::size == 0 );
static_assert( typelist< int >::size == 1 );
static_assert( typelist< float, int >::size == 2 );
static_assert( typelist< int, int >::size == 2 );

/// ---

static_assert( typelist_compatible< typelist<> > );
static_assert( typelist_compatible< typelist< int > > );
static_assert( !typelist_compatible< int > );

struct derived_tp : typelist<>
{
};

template <>
struct typelist_traits< derived_tp >
{
        static constexpr bool is_compatible = true;
        using types                         = typelist<>;
};
static_assert( typelist_compatible< derived_tp > );

struct traited_tp
{
};

template <>
struct typelist_traits< traited_tp >
{
        static constexpr bool is_compatible = true;
        using types                         = typelist<>;
};

static_assert( typelist_compatible< traited_tp > );

/// ---

static_assert( index_of_t_or_const_t_v< float, typelist< float > > == 0 );
static_assert( index_of_t_or_const_t_v< float, typelist< int, float > > == 1 );
static_assert( index_of_t_or_const_t_v< float, typelist< float, int > > == 0 );
static_assert( index_of_t_or_const_t_v< float, typelist< int, float, float > > == 1 );

static_assert( index_of_t_or_const_t_v< float, typelist< float const > > == 0 );
static_assert( index_of_t_or_const_t_v< float, typelist< int, float const > > == 1 );
static_assert( index_of_t_or_const_t_v< float, typelist< float const, int > > == 0 );
static_assert( index_of_t_or_const_t_v< float, typelist< int, float const, float > > == 1 );

static_assert( index_of_t_or_const_t_v< float const, typelist< float const > > == 0 );
static_assert( index_of_t_or_const_t_v< float const, typelist< int, float const > > == 1 );
static_assert( index_of_t_or_const_t_v< float const, typelist< float const, int > > == 0 );
static_assert( index_of_t_or_const_t_v< float const, typelist< int, float const, float > > == 1 );

/// ---

static_assert( std::same_as< type_at_t< 0, typelist< int > >, int > );
static_assert( std::same_as< type_at_t< 0, typelist< int, float > >, int > );
static_assert( std::same_as< type_at_t< 0, typelist< float, int > >, float > );
static_assert( std::same_as< type_at_t< 1, typelist< int, float > >, float > );
static_assert( std::same_as< type_at_t< 1, typelist< float, int > >, int > );

/// ---

static_assert( contains_type_v< int, typelist< int, float > > );
static_assert( contains_type_v< float, typelist< int, float > > );
static_assert( !contains_type_v< int, typelist< float > > );
static_assert( !contains_type_v< int const, typelist< int, float > > );
static_assert( contains_type_v< int const, typelist< int const, float > > );
static_assert( contains_type_v< float const, typelist< int, float const > > );
static_assert( !contains_type_v< int, typelist< int const > > );

// ---


using ut1 = unique_typelist_t< typelist< float, float, int, float, int > >;
static_assert( std::same_as< ut1, typelist< float, int > > );

// ---

using ft1  = flatten_t<  //
    typelist< float, int >,
    int,
    typelist< float, int, typelist< std::string > >  //
    >;
using uft1 = unique_typelist_t< ft1 >;
static_assert( std::same_as< uft1, typelist< float, int, std::string > > );

using ft2  = flatten_t<  //
    typelist< int const >,
    int,
    typelist< float, int, typelist< float const > >  //
    >;
using uft2 = unique_typelist_t< ft2 >;
static_assert( std::same_as< uft2, typelist< int const, float, int, float const > > );

using ft3  = flatten_t<  //
    typelist< int > const,
    int,
    typelist< float, int, typelist< float > const >  //
    >;
using uft3 = unique_typelist_t< ft3 >;
static_assert( std::same_as< uft3, typelist< int const, float, int, float const > > );

struct ft4 : flatten_t< ft3 const >
{
};
template <>
struct typelist_traits< ft4 >
{
        static constexpr bool is_compatible = true;
        using types                         = typename ft4::types;
};
using uft4 = unique_typelist_t< ft4 >;
static_assert( std::same_as< uft4, typelist< int const, float const > > );

struct ft5 : typelist< int, float >
{
};
template <>
struct typelist_traits< ft5 const >
{
        static constexpr bool is_compatible = true;
        using types                         = typename ft5::types const;
};
static_assert( std::same_as< flatten_t< ft5 const >, typelist< int const, float const > > );
// ---

static_assert( is_subset_v< typelist<>, typelist<> > );
static_assert( is_subset_v< typelist<>, typelist< int > > );
static_assert( !is_subset_v< typelist< int >, typelist<> > );
static_assert( is_subset_v< typelist< float >, typelist< int, float > > );
static_assert( !is_subset_v< typelist< int const >, typelist< int > > );

// ---

static_assert( !any_is_const_v< typelist<> > );
static_assert( !any_is_const_v< typelist< int > > );
static_assert( !any_is_const_v< typelist< float, int > > );
static_assert( any_is_const_v< typelist< int const > > );
static_assert( any_is_const_v< typelist< float, int const > > );
static_assert( any_is_const_v< typelist< float, int, int const > > );

// ---

static_assert( all_is_const_v< typelist<> > );
static_assert( !all_is_const_v< typelist< int > > );
static_assert( !all_is_const_v< typelist< float, int > > );
static_assert( all_is_const_v< typelist< int const > > );
static_assert( all_is_const_v< typelist< float const, int const > > );
static_assert( all_is_const_v< typelist< float const, int const, int const > > );

// ---

static_assert( none_is_const_v< typelist<> > );
static_assert( none_is_const_v< typelist< int > > );
static_assert( none_is_const_v< typelist< float, int > > );
static_assert( !none_is_const_v< typelist< int const > > );
static_assert( !none_is_const_v< typelist< float const, int > > );
static_assert( !none_is_const_v< typelist< float, int const, int > > );

// ---

struct swp1_yes : swappable< nothrow::YES, swp1_yes >
{
};
struct swp2_yes : swappable< nothrow::YES, swp2_yes >
{
};
struct swp1_no : swappable< nothrow::NO, swp1_no >
{
};
struct swp2_no : swappable< nothrow::NO, swp2_no >
{
};

static_assert( all_nothrow_swappable_v< typelist<> > );
static_assert( all_nothrow_swappable_v< typelist< swp1_yes > > );
static_assert( all_nothrow_swappable_v< typelist< swp1_yes, swp2_yes > > );
static_assert( !all_nothrow_swappable_v< typelist< swp1_no > > );
static_assert( !all_nothrow_swappable_v< typelist< swp1_no, swp1_yes, swp2_no, swp2_yes > > );
static_assert( !all_nothrow_swappable_v< typelist< swp1_no, swp2_no, swp2_yes > > );

// ---

struct mv_const_yes : move_constructible< nothrow::YES >
{
};
struct mv_const_no : move_constructible< nothrow::NO >
{
};

static_assert( all_nothrow_move_constructible_v< typelist<> > );
static_assert( all_nothrow_move_constructible_v< typelist< mv_const_yes > > );
static_assert( !all_nothrow_move_constructible_v< typelist< mv_const_no > > );
static_assert( !all_nothrow_move_constructible_v< typelist< mv_const_yes, mv_const_no > > );
static_assert( !all_nothrow_move_constructible_v< typelist< mv_const_no, mv_const_yes > > );

// ---

struct cp_const_yes : copy_constructible< nothrow::YES >
{
};
struct cp_const_no : copy_constructible< nothrow::NO >
{
};

static_assert( all_nothrow_copy_constructible_v< typelist<> > );
static_assert( all_nothrow_copy_constructible_v< typelist< cp_const_yes > > );
static_assert( !all_nothrow_copy_constructible_v< typelist< cp_const_no > > );
static_assert( !all_nothrow_copy_constructible_v< typelist< cp_const_yes, cp_const_no > > );
static_assert( !all_nothrow_copy_constructible_v< typelist< cp_const_no, cp_const_yes > > );

// ---

struct destr_yes : destructible< nothrow::YES >
{
};
struct destr_no : destructible< nothrow::NO >
{
};

static_assert( all_nothrow_destructible_v< typelist<> > );
static_assert( all_nothrow_destructible_v< typelist< destr_yes > > );
static_assert( !all_nothrow_destructible_v< typelist< destr_no > > );
static_assert( !all_nothrow_destructible_v< typelist< destr_yes, destr_no > > );
static_assert( !all_nothrow_destructible_v< typelist< destr_no, destr_yes > > );

// ---

struct three_way_yes : three_way_comparable< nothrow::YES, three_way_yes >
{
};
struct three_way_no : three_way_comparable< nothrow::NO, three_way_no >
{
};

static_assert( all_nothrow_three_way_comparable_v< typelist<> > );
static_assert( all_nothrow_three_way_comparable_v< typelist< three_way_yes > > );
static_assert( !all_nothrow_three_way_comparable_v< typelist< three_way_no > > );
static_assert( !all_nothrow_three_way_comparable_v< typelist< three_way_yes, three_way_no > > );
static_assert( !all_nothrow_three_way_comparable_v< typelist< three_way_no, three_way_yes > > );

// ---

struct equality_yes : equality_comparable< nothrow::YES, equality_yes >
{
};
struct equality_no : equality_comparable< nothrow::NO, equality_no >
{
};

static_assert( all_nothrow_equality_comparable_v< typelist<> > );
static_assert( all_nothrow_equality_comparable_v< typelist< equality_yes > > );
static_assert( !all_nothrow_equality_comparable_v< typelist< equality_no > > );
static_assert( !all_nothrow_equality_comparable_v< typelist< equality_yes, equality_no > > );
static_assert( !all_nothrow_equality_comparable_v< typelist< equality_no, equality_yes > > );

// ---

static_assert( variadic_with< vref< int, float >, typelist< float > > );
static_assert( variadic_with< uvptr< int, float >, typelist< float > > );
static_assert( !variadic_with< vref< int, float >, typelist< std::string > > );
static_assert( !variadic_with< uvptr< int, float >, typelist< std::string > > );
static_assert( variadic_with< vref< int, float >, typelist<> > );
static_assert( variadic_with< uvptr< int, float >, typelist<> > );
static_assert( variadic_with_type< vref< int, float >, int > );
static_assert( variadic_with_type< uvptr< int, float >, int > );

}  // namespace vari
