
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
