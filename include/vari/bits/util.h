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

namespace vari::bits
{

template < template < typename... > typename T, typename U, typename TL >
struct vptr_apply;

template < template < typename... > typename T, typename U, typename... Ts >
struct vptr_apply< T, U, typelist< Ts... > >
{
        using type = T< U, Ts... >;
};

template < template < typename... > typename T, typename U, typename TL >
using define_vptr = typename vptr_apply<
    T,
    U,
    typename unique_typelist<  //
        typelist<>,
        typename flatten_typelist< typelist<>, TL >::type >::type >::type;

template < typename F, typename... Args >
concept invocable = requires( F&& f, Args&&... args ) {
        std::forward< F >( f )( std::forward< Args >( args )... );
};

template < typename... Args >
struct function_picker
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

}  // namespace vari::bits