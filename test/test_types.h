
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
        move_constructible( move_constructible&& ) noexcept( NT == nothrow::YES ){};
};

template < nothrow NT >
struct copy_constructible
{
        copy_constructible( copy_constructible const& ) noexcept( NT == nothrow::YES ){};
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
        friend void operator<=>( D const& lh, D const& rh ) noexcept( NT == nothrow::YES )
        {
                std::ignore = lh;
                std::ignore = rh;
        };
};

template < nothrow NT, typename D >
struct equality_comparable
{
        friend bool operator==( D const& lh, D const& rh ) noexcept( NT == nothrow::YES )
        {
                std::ignore = lh;
                std::ignore = rh;
        };
};

}  // namespace vari
