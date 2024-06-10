#pragma once

#include "vari/bits/overloaded.h"
#include "vari/bits/ptr_core.h"

#include <variant>

namespace vari::bits
{

template < typename B, typename... Ts >
class vref;

template < typename B, typename... Ts >
class vptr
{
        using TL = bits::typelist< Ts... >;

public:
        static_assert( is_flat< TL >::value );

        vptr()                             = default;
        vptr( const vptr& )                = default;
        vptr( vptr&& ) noexcept            = default;
        vptr& operator=( const vptr& )     = default;
        vptr& operator=( vptr&& ) noexcept = default;

        vptr( std::nullptr_t ) noexcept
        {
        }

        template < typename... Us >
                requires( is_subset< bits::typelist< Us... >, TL >::value )
        vptr( vptr< B, Us... > p ) noexcept
          : _core( std::move( p._core ) )
        {
        }

        template < typename... Us >
                requires( is_subset< bits::typelist< Us... >, TL >::value )
        vptr( vref< B, Us... > r ) noexcept
          : _core( std::move( r._core ) )
        {
        }

        template < typename U >
                requires( contains_type< U, TL >::value )
        vptr( U& u ) noexcept
          : _core( u )
        {
        }

        auto& operator*() noexcept
        {
                return *_core.ptr;
        }

        auto* operator->() noexcept
        {
                return _core.ptr;
        }

        auto* raw() const noexcept
        {
                return _core.ptr;
        }

        operator bool() const noexcept
        {
                return _core.ptr != nullptr;
        }

        template < typename F >
        decltype( auto ) take( F&& f )
        {
                if ( _core.ptr == nullptr )
                        return std::forward< F >( f )( vptr< B >{} );
                return _core.take_bits( [&]< typename T >( T* p ) {
                        return std::forward< F >( f )( vptr< B, T >{ *p } );
                } );
        }

        template < typename... Fs >
        decltype( auto ) take( Fs&&... f )
        {
                if ( _core.ptr == nullptr )
                        return bits::overloaded< std::remove_reference_t< Fs >... >(
                            std::forward< Fs >( f )... )( vptr< B >{} );
                return _core.take_bits( [&]< typename T >( T* p ) {
                        return bits::overloaded< std::remove_reference_t< Fs >... >(
                            std::forward< Fs >( f )... )( vptr< B, T >{ *p } );
                } );
        }

        friend void swap( vptr& lh, vptr& rh ) noexcept
        {
                std::swap( lh._core, rh._core );
        }

private:
        ptr_core< B, TL > _core;

        template < typename C, typename... Us >
        friend class vptr;
};

}  // namespace vari::bits

namespace vari
{
template < typename... Ts >
using vptr = bits::define_vptr< bits::vptr, void, bits::typelist< Ts... > >;
}