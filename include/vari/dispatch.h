
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

#include "vari/bits/dispatch.h"

namespace vari
{

// Given `i` calls `Cnv` callable with `i` as _template argument_ and passes the result into best
// match of callable out of `fn` set.
//
// `i` has to fit in range 0..N-1, any other value is undefined behavior.
template < std::size_t N, typename Cnv, typename... Fn >
constexpr decltype( auto ) dispatch( index_type i, Cnv&& cnv, Fn&&... fn )
{
        using types = factory_result_types_t< N, Cnv >;

        typename _check_unique_invocability< types >::template with_pure_value< Fn... > _{};

        return _dispatch_index< 0, N >( i, [&]< index_type j >() -> decltype( auto ) {
                auto&& item = cnv.template operator()< j >();

                return _dispatch_fun( (decltype( item )&&) item, (Fn&&) fn... );
        } );
}

}  // namespace vari
