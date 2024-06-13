#pragma once

#include "vari/bits/ptr_core.h"
#include "vari/vptr.h"

namespace vari::bits
{

template < typename B, typename... Ts >
class uvref;

template < typename B, typename... Ts >
class uvptr
{
        using TL = bits::typelist< Ts... >;

public:
        static_assert( is_flat< TL >::value );

        using pointer = vptr< B, Ts... >;

        uvptr() noexcept = default;

        uvptr( std::nullptr_t ) noexcept
        {
        }

        template < typename... Us >
                requires( is_subset< typelist< Us... >, TL >::value )
        uvptr( uvref< B, Us... >&& p ) noexcept
        {
                _ptr._core   = std::move( p._ref._core );
                p._ref._core = ptr_core< B, typelist< Us... > >{};
        }

        template < typename... Us >
                requires( is_subset< typelist< Us... >, TL >::value )
        uvptr( uvptr< B, Us... >&& p ) noexcept
          : _ptr( p.release() )
        {
        }

        template < typename U >
                requires( contains_type< U, TL >::value )
        explicit uvptr( U* u ) noexcept
        {
                if ( u )
                        _ptr = pointer{ *u };
        }

        template < typename... Us >
                requires( is_subset< typelist< Us... >, TL >::value )
        explicit uvptr( vptr< B, Us... > p ) noexcept
          : _ptr( p )
        {
        }

        uvptr& operator=( std::nullptr_t ) noexcept
        {
                reset( nullptr );
        }

        template < typename... Us >
                requires( is_subset< bits::typelist< Us... >, TL >::value )
        uvptr& operator=( uvptr< B, Us... >&& p )
        {
                if ( this == &p )
                        return *this;
                reset( p.release() );
        }

        auto& operator*()
        {
                return *_ptr;
        }

        B* operator->()
        {
                return _ptr.ptr();
        }

        const pointer& ptr() const
        {
                return _ptr;
        }

        void reset( pointer ptr = pointer() )
        {
                auto tmp = _ptr;
                _ptr     = ptr;
                tmp._core.reset();
        }

        pointer release() noexcept
        {
                pointer res;
                std::swap( res, _ptr );
                return res;
        }

        operator bool() const
        {
                return _ptr;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... f )
        {
                return _ptr.visit( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... f )
        {
                return _ptr.match( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) take( Fs&&... fs ) &&
        {
                auto p = release();
                return p.match(
                    [&]( empty_t ) -> decltype( auto ) {
                            return dispatch_fun( empty, std::forward< Fs >( fs )... );
                    },
                    [&]< typename T >( vptr< B, T > p ) -> decltype( auto ) {
                            return dispatch_fun( uvptr< B, T >{ p }, std::forward< Fs >( fs )... );
                    } );
        }

        ~uvptr()
        {
                _ptr._core.delete_ptr();
        }

private:
        pointer _ptr;

        template < typename C, typename... Us >
        friend class uvptr;
};

}  // namespace vari::bits

namespace vari
{
template < typename R, typename... Ts >
using uvptr = bits::define_vptr< bits::uvptr, R, bits::typelist< Ts... > >;

}  // namespace vari