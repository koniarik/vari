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

#include "util.h"

#include <cstddef>
#include <utility>

namespace vari
{

template < index_type Off, index_type N, typename F >
constexpr decltype( auto ) _dispatch_index( index_type const i, F&& f )
{
#define VARI_GEN_CASE( x )                           \
        case Off + ( x ):                            \
                if constexpr ( ( Off + ( x ) ) < N ) \
                        return ( (F&&) f ).template operator()< Off + ( x ) >();

        switch ( i ) {
                VARI_GEN_CASE( 0 )
                VARI_GEN_CASE( 1 )
                VARI_GEN_CASE( 2 )
                VARI_GEN_CASE( 3 )
                VARI_GEN_CASE( 4 )
                VARI_GEN_CASE( 5 )
                VARI_GEN_CASE( 6 )
                VARI_GEN_CASE( 7 )
                VARI_GEN_CASE( 8 )
                VARI_GEN_CASE( 9 )
                VARI_GEN_CASE( 10 )
                VARI_GEN_CASE( 11 )
                VARI_GEN_CASE( 12 )
                VARI_GEN_CASE( 13 )
                VARI_GEN_CASE( 14 )
                VARI_GEN_CASE( 15 )
                VARI_GEN_CASE( 16 )
                VARI_GEN_CASE( 17 )
                VARI_GEN_CASE( 18 )
                VARI_GEN_CASE( 19 )
                VARI_GEN_CASE( 20 )
                VARI_GEN_CASE( 21 )
                VARI_GEN_CASE( 22 )
                VARI_GEN_CASE( 23 )
                VARI_GEN_CASE( 24 )
                VARI_GEN_CASE( 25 )
                VARI_GEN_CASE( 26 )
                VARI_GEN_CASE( 27 )
                VARI_GEN_CASE( 28 )
                VARI_GEN_CASE( 29 )
                VARI_GEN_CASE( 30 )
                VARI_GEN_CASE( 31 )
        default:
                if constexpr ( N > Off + 32 )
                        return _dispatch_index< Off + 32, N >( i, (F&&) f );
        }


#undef VARI_GEN_CASE

#if defined( __cpp_lib_unreachable )
        std::unreachable();
#elif defined( _MSC_VER )
        __assume( false );
#else
        __builtin_unreachable();
#endif
}

template < typename T, typename... Fs >
constexpr decltype( auto ) _dispatch_fun( T&& item, Fs&&... fs )
{
        auto&& f = _function_picker< T >::pick( (Fs&&) fs... );
        return std::forward< decltype( f ) >( f )( (T&&) item );
}

}  // namespace vari
