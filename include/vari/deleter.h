
#pragma once

#include <compare>

namespace vari
{

/// Default library deleter
///
struct def_del
{
        constexpr void operator()( auto* item ) const
        {
                static_assert(
                    sizeof( *item ) != 0,
                    "Beware that all types has to be fully defined for delete to work." );
                delete item;
        }

        friend constexpr auto operator<=>( def_del const&, def_del const& ) = default;
};

}  // namespace vari
