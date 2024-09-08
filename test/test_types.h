
#include "vari/bits/typelist.h"

#include <string>
#include <tuple>

namespace vari
{

enum class nothrow
{
        YES,
        NO
};

template < nothrow NT, typename D >
struct swappable
{
        friend void swap( D& lh, D& rh ) noexcept( NT == nothrow::YES )
        {
                std::ignore = lh;
                std::ignore = rh;
        };
};

template < nothrow NT >
struct move_constructible
{
        move_constructible() noexcept                            = default;
        move_constructible( move_constructible const& ) noexcept = default;
        move_constructible( move_constructible&& ) noexcept( NT == nothrow::YES ){};
};

template < nothrow NT >
struct copy_constructible
{
        copy_constructible() noexcept = default;
        copy_constructible( copy_constructible const& ) noexcept( NT == nothrow::YES ){};
};

template < nothrow NT >
struct default_constructible
{
        default_constructible() noexcept( NT == nothrow::YES )
        {
        }
};

template < nothrow NT >
struct destructible
{
        ~destructible() noexcept( NT == nothrow::YES )
        {
        }
};

template < nothrow NT, typename D >
struct three_way_comparable
{
        friend auto operator<=>( D const& lh, D const& rh ) noexcept( NT == nothrow::YES )
        {
                return &lh <=> &rh;
        };
};

template < nothrow NT, typename D >
struct equality_comparable
{
        friend bool operator==( D const& lh, D const& rh ) noexcept( NT == nothrow::YES )
        {
                return &lh == &rh;
        };
};

template < std::size_t i >
struct tag
{
        std::string j = std::to_string( i );
};

template < std::size_t N >
struct tag_set
{
        using type = typelist<
            typename tag_set< N - 1 >::type,
            tag< N * 4 + 0 >,
            tag< N * 4 + 1 >,
            tag< N * 4 + 2 >,
            tag< N * 4 + 3 > >;
};

template <>
struct tag_set< 0 >
{
        using type = typelist< tag< 0 >, tag< 1 >, tag< 2 >, tag< 3 > >;
};

using big_set = typename tag_set< 255 / 4 >::type;

}  // namespace vari
