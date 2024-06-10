#pragma once

#include <utility>

namespace vari::bits
{

template < typename... Callables >
struct overloaded : Callables...
{
        overloaded( const overloaded& )     = default;
        overloaded( overloaded&& ) noexcept = default;

        overloaded& operator=( const overloaded& )     = default;
        overloaded& operator=( overloaded&& ) noexcept = default;

        template < typename... Ts >
        explicit overloaded( Ts&&... ts )
          : Callables{ std::forward< Ts >( ts ) }...
        {
        }

        using Callables::operator()...;

        ~overloaded() = default;
};

}  // namespace vari::bits