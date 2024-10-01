
#include "vari/bits/typelist.h"

#include "test_types.h"

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

static_assert( index_of_t_or_const_t_v< float, typelist< const float > > == 0 );
static_assert( index_of_t_or_const_t_v< float, typelist< int, const float > > == 1 );
static_assert( index_of_t_or_const_t_v< float, typelist< const float, int > > == 0 );
static_assert( index_of_t_or_const_t_v< float, typelist< int, const float, float > > == 1 );

static_assert( index_of_t_or_const_t_v< const float, typelist< const float > > == 0 );
static_assert( index_of_t_or_const_t_v< const float, typelist< int, const float > > == 1 );
static_assert( index_of_t_or_const_t_v< const float, typelist< const float, int > > == 0 );
static_assert( index_of_t_or_const_t_v< const float, typelist< int, const float, float > > == 1 );

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
static_assert( !contains_type_v< const int, typelist< int, float > > );
static_assert( contains_type_v< const int, typelist< const int, float > > );
static_assert( contains_type_v< const float, typelist< int, const float > > );
static_assert( !contains_type_v< int, typelist< const int > > );

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
    typelist< const int >,
    int,
    typelist< float, int, typelist< float const > >  //
    >;
using uft2 = unique_typelist_t< ft2 >;
static_assert( std::same_as< uft2, typelist< const int, float, int, const float > > );

using ft3  = flatten_t<  //
    const typelist< int >,
    int,
    typelist< float, int, const typelist< float > >  //
    >;
using uft3 = unique_typelist_t< ft3 >;
static_assert( std::same_as< uft3, typelist< const int, float, int, const float > > );

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
static_assert( all_is_const_v< typelist< const float, int const > > );
static_assert( all_is_const_v< typelist< float const, int const, int const > > );

// ---

static_assert( none_is_const_v< typelist<> > );
static_assert( none_is_const_v< typelist< int > > );
static_assert( none_is_const_v< typelist< float, int > > );
static_assert( !none_is_const_v< typelist< int const > > );
static_assert( !none_is_const_v< typelist< const float, int > > );
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

}  // namespace vari
