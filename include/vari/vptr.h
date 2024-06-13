#pragma once

#include "vari/bits/ptr_core.h"
#include "vari/bits/util.h"

#include <variant>

namespace vari
{
struct empty_t
{
};

static constexpr empty_t empty;
}  // namespace vari

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

        auto* ptr() const noexcept
        {
                return _core.ptr;
        }

        operator bool() const noexcept
        {
                return _core.ptr != nullptr;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... fs )
        {
                if ( _core.ptr == nullptr ) {
                        return dispatch_fun( empty, std::forward< Fs >( fs )... );
                }
                return _core.match_impl( [&]< typename T >( T* p ) -> decltype( auto ) {
                        return dispatch_fun( *p, std::forward< Fs >( fs )... );
                } );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... fs )
        {
                if ( _core.ptr == nullptr ) {
                        return dispatch_fun( empty, std::forward< Fs >( fs )... );
                }
                return _core.match_impl( [&]< typename T >( T* p ) -> decltype( auto ) {
                        return dispatch_fun( vptr< B, T >{ *p }, std::forward< Fs >( fs )... );
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

        template < typename C, typename... Us >
        friend class uvptr;
};

}  // namespace vari::bits

namespace vari
{
template < typename R, typename... Ts >
using vptr = bits::define_vptr< bits::vptr, R, bits::typelist< Ts... > >;
}