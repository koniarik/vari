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

#include "util.h"

#include <cstddef>
#include <utility>

namespace vari
{

template < std::size_t Off, std::size_t N, typename F >
constexpr decltype( auto ) _dispatch_index( std::size_t const i, F&& f )
{
#define GEN( x )                                     \
        case Off + ( x ):                            \
                if constexpr ( ( Off + ( x ) ) < N ) \
                        return ( (F&&) f ).template operator()< Off + ( x ) >();

        switch ( i ) {
                GEN( 0 )
                GEN( 1 )
                GEN( 2 )
                GEN( 3 )
                GEN( 4 )
                GEN( 5 )
                GEN( 6 )
                GEN( 7 )
                GEN( 8 )
                GEN( 9 )
                GEN( 10 )
                GEN( 11 )
                GEN( 12 )
                GEN( 13 )
                GEN( 14 )
                GEN( 15 )
                GEN( 16 )
                GEN( 17 )
                GEN( 18 )
                GEN( 19 )
                GEN( 20 )
                GEN( 21 )
                GEN( 22 )
                GEN( 23 )
                GEN( 24 )
                GEN( 25 )
                GEN( 26 )
                GEN( 27 )
                GEN( 28 )
                GEN( 29 )
                GEN( 30 )
                GEN( 31 )
        default:
                if constexpr ( N > Off + 32 )
                        return _dispatch_index< Off + 32, N >( i, (F&&) f );
        }


#undef GEN

#if defined( __cpp_lib_unreachable )
        std::unreachable();
#elif defined( _MSC_VER )
        __assume( false );
#else
        __builtin_unreachable();
#endif
}

template < typename T, typename... Fs >
concept invocable_for_one = ( invocable< Fs, T > || ... );

template < typename T, typename... Fs >
constexpr decltype( auto ) _dispatch_fun( T&& item, Fs&&... fs )
{
        auto&& f = _function_picker< T >::pick( (Fs&&) fs... );
        return std::forward< decltype( f ) >( f )( (T&&) item );
}

}  // namespace vari
