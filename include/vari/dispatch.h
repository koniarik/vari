
#pragma once

#include "vari/bits/dispatch.h"

namespace vari
{

template < std::size_t N, typename Cnv, typename... Fn >
constexpr decltype( auto ) dispatch( index_type i, Cnv&& cnv, Fn&&... fn )
{
        using types = factory_result_types_t< N, Cnv >;

        typename check_unique_invocability< types >::template with_pure_value< Fn... > _{};

        return _dispatch_index< 0, N >( i, [&]< index_type j >() -> decltype( auto ) {
                auto&& item = cnv.template operator()< j >();

                return _dispatch_fun( (decltype( item )&&) item, (Fn&&) fn... );
        } );
}

}  // namespace vari
