#pragma once

#include "vari/bits/ptr_core.h"
#include "vari/vptr.h"

namespace vari::bits
{

template < typename B, typename... Ts >
class vref
{
        using TL = bits::typelist< Ts... >;

public:
        template < typename... Us >
                requires( is_subset< typelist< Us... >, TL >::value )
        vref( vref< B, Us... > p )
          : _core( p._core )
        {
        }

        template < typename U >
                requires( contains_type< U, TL >::value )
        vref( U& u )
          : _core( u )
        {
        }

        auto operator*()
        {
                return *_core.ptr;
        }

        B* operator->()
        {
                return _core.ptr;
        }

        B* raw() const
        {
                return _core.ptr;
        }

        template < typename F >
        decltype( auto ) take( F&& f )
        {
                return _core.take_bits( [&]< typename T >( T* p ) {
                        return std::forward< F >( f )( vref< B, T >{ *p } );
                } );
        }

        template < typename... Fs >
        decltype( auto ) take( Fs&&... f )
        {
                return _core.take_bits( [&]< typename T >( T* p ) {
                        return bits::overloaded< std::remove_reference_t< Fs >... >(
                            std::forward< Fs >( f )... )( vref< B, T >{ *p } );
                } );
        }

private:
        ptr_core< B, TL > _core;

        template < typename C, typename... Us >
        friend class vref;
        template < typename C, typename... Us >
        friend class vptr;
};

}  // namespace vari::bits

namespace vari
{
template < typename... Ts >
using vref = bits::define_vptr< bits::vref, void, bits::typelist< Ts... > >;
}  // namespace vari