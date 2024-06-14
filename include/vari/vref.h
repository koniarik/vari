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
        vref( vref< B, Us... > p ) noexcept
          : _core( p._core )
        {
        }

        template < typename U >
                requires( contains_type< U, TL >::value )
        vref( U& u ) noexcept
          : _core( u )
        {
        }

        auto& operator*() const noexcept
        {
                return *_core.ptr;
        }

        auto* operator->() const noexcept
        {
                return _core.ptr;
        }

        auto* get() const noexcept
        {
                return _core.ptr;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... fs )
        {
                return _core.match_impl( [&]< typename T >( T* p ) -> decltype( auto ) {
                        return dispatch_fun( *p, std::forward< Fs >( fs )... );
                } );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... fs )
        {
                return _core.match_impl( [&]< typename T >( T* p ) -> decltype( auto ) {
                        return dispatch_fun( vref< B, T >{ *p }, std::forward< Fs >( fs )... );
                } );
        }

        friend auto operator<=>( vref const& lh, vref const& rh ) = default;

private:
        ptr_core< B, TL > _core;

        template < typename C, typename... Us >
        friend class vref;
        template < typename C, typename... Us >
        friend class vptr;
        template < typename C, typename... Us >
        friend class uvref;
        template < typename C, typename... Us >
        friend class uvptr;
};

}  // namespace vari::bits

namespace vari
{
template < typename R, typename... Ts >
using vref = bits::define_vptr< bits::vref, R, bits::typelist< Ts... > >;

}  // namespace vari