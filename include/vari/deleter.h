
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
                delete item;
        }

        friend constexpr auto operator<=>( def_del const&, def_del const& ) = default;
};

}  // namespace vari
