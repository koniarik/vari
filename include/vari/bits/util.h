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

#include "vari/bits/typelist.h"

#include <utility>

namespace vari
{

template < template < typename... > typename T, typename TL >
struct _vptr_apply;

template < template < typename... > typename T, typename... Ts >
struct _vptr_apply< T, typelist< Ts... > >
{
        using type = T< Ts... >;
};

template < template < typename... > typename T, typename TL >
using _vptr_apply_t = typename _vptr_apply< T, TL >::type;

template < template < typename... > typename T, typename TL >
using _define_vptr = _vptr_apply_t< T, unique_typelist_t< flatten_t< TL > > >;

template < typename F, typename... Args >
concept invocable = requires( F&& f, Args&&... args ) {
        std::forward< F >( f )( std::forward< Args >( args )... );
};

template < typename From_TL, typename To_TL >
concept vconvertible_to = is_subset_v< From_TL, To_TL > || is_subset_v< From_TL const, To_TL >;

template < typename... Args >
struct _function_picker
{
        template < typename F, typename... Fs >
        static constexpr decltype( auto ) pick( F&& f, Fs&&... fs )
        {
                if constexpr ( invocable< F, Args... > ) {
                        static_assert(
                            ( !invocable< Fs, Args... > && ... ),
                            "Only one of the functors should be invocable with Args..." );
                        return std::forward< F >( f );
                } else {
                        static_assert( sizeof...( fs ) != 0 );
                        return pick( std::forward< Fs >( fs )... );
                }
        }
};

}  // namespace vari
