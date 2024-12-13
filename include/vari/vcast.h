#pragma once

#include "uvref.h"
#include "vref.h"

namespace vari
{

/// Applies static cast to R to item currently referenced by `vref`
///
template < typename R, typename... Ts >
        requires( std::convertible_to< Ts&, R > && ... )
R vcast( vari::_vref< Ts... > const& r )
{
        return r.visit( [&]( auto& item ) -> R {
                return static_cast< R >( item );
        } );
}

/// Applies static cast to R to item currently owned by `uvref`
///
template < typename R, typename Del, typename... Ts >
        requires( std::convertible_to< Ts&, R > && ... )
R vcast( vari::_uvref< Del, Ts... > const& r )
{
        return r.visit( [&]( auto& item ) -> R {
                return static_cast< R >( item );
        } );
}

}  // namespace vari
