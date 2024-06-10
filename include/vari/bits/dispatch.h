#pragma once

#include <utility>

namespace vari::bits
{

template < std::size_t Off, std::size_t N, typename F >
constexpr decltype( auto ) dispatch( std::size_t i, F&& f )
{

#define GEN( x )                       \
        case x:                        \
                if constexpr ( x < N ) \
                        return std::forward< F >( f ).template operator()< Off + x >();

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
                break;
        }

#undef GEN

        if constexpr ( N > 32 )
                return index_switch< 32, N - 32 >( i - 32, std::forward< F >( f ) );

#if defined( __cpp_lib_unreachable )
        std::unreachable();
#elif defined( _MSC_VER )
        __assume( false );
#else
        __builtin_unreachable();
#endif
}

}  // namespace vari::bits