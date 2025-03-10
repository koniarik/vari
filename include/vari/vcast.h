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
