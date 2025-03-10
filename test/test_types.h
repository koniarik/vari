
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

using big_set = typename tag_set< 220 / 4 >::type;

}  // namespace vari
