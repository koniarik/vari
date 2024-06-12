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

        auto& operator*() noexcept
        {
                return *_core.ptr;
        }

        B* operator->() noexcept
        {
                return _core.ptr;
        }

        B* ptr() const noexcept
        {
                return _core.ptr;
        }

        template < typename F >
        decltype( auto ) visit( F&& f )
        {
                static_assert(
                    ( invocable< F, Ts& > && ... ), "Functor has to be invocable with all types" );
                return match( [&]< typename T >( vref< B, T > p ) -> decltype( auto ) {
                        return std::forward< F >( f )( *p );
                } );
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... f )
        {
                return visit( bits::overloaded< std::remove_reference_t< Fs >... >(
                    std::forward< Fs >( f )... ) );
        }

        template < typename F >
        decltype( auto ) match( F&& f )
        {
                static_assert(
                    ( invocable< F, vref< B, Ts > > && ... ),
                    "Functor has to be invocable with all types" );
                return _core.match_impl( [&]< typename T >( T* p ) -> decltype( auto ) {
                        return std::forward< F >( f )( vref< B, T >{ *p } );
                } );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... f )
        {
                return _core.match_impl( [&]< typename T >( T* p ) -> decltype( auto ) {
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