#pragma once

#include "vari/bits/ptr_core.h"
#include "vari/vref.h"

namespace vari::bits
{

template < typename B, typename... Ts >
class uvref
{
        using TL = bits::typelist< Ts... >;

public:
        static_assert( is_flat< TL >::value );

        using reference = vref< B, Ts... >;

        template < typename... Us >
                requires( is_subset< typelist< Us... >, TL >::value )
        uvref( uvref< B, Us... >&& p ) noexcept
          : _ref( p._ref )
        {
                p._ref._core = ptr_core< B, typelist< Us... > >{};
        }

        template < typename... Us >
                requires( is_subset< typelist< Us... >, TL >::value )
        explicit uvref( vref< B, Us... > p ) noexcept
          : _ref( p )
        {
        }

        template < typename U >
                requires( is_subset< typelist< U >, TL >::value )
        explicit uvref( U item ) noexcept
          : _ref( *new U( std::move( item ) ) )
        {
        }

        template < typename... Us >
                requires( is_subset< bits::typelist< Us... >, TL >::value )
        uvref& operator=( uvref< B, Us... >&& p )
        {
                if ( this == &p )
                        return *this;
                using std::swap;
                swap( _ref._core, p._ref._core );
        }

        auto& operator*()
        {
                return *_ref;
        }

        B* operator->()
        {
                return _ref.ptr();
        }

        const reference& ptr() const
        {
                return _ref;
        }

        template < typename... Fs >
        decltype( auto ) visit( Fs&&... f )
        {
                return _ref.visit( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) match( Fs&&... f )
        {
                return _ref.match( (Fs&&) f... );
        }

        template < typename... Fs >
        decltype( auto ) take( Fs&&... f ) &&
        {
                auto tmp   = _ref;
                _ref._core = ptr_core< B, TL >{};
                return tmp.match( [&]< typename T >( vref< B, T > p ) -> decltype( auto ) {
                        return bits::overloaded< std::remove_reference_t< Fs >... >(
                            std::forward< Fs >( f )... )( uvref< B, T >( p ) );
                } );
        }

        ~uvref()
        {
                _ref._core.reset();
        }

private:
        reference _ref;

        template < typename C, typename... Us >
        friend class vptr;

        template < typename C, typename... Us >
        friend class uvptr;

        template < typename C, typename... Us >
        friend class uvref;
};

}  // namespace vari::bits

namespace vari
{
template < typename R, typename... Ts >
using uvref = bits::define_vptr< bits::uvref, R, bits::typelist< Ts... > >;

template < typename R, typename T >
uvref< R, T > uwrap( T item )
{
        return uvref< R, T >( std::move( item ) );
}

}  // namespace vari