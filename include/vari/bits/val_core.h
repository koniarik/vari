///
/// Copyright (C) 2020 Jan Veverak Koniarik
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
/// and associated documentation files (the "Software"), to deal in the Software without
/// restriction, including without limitation the rights to use, copy, modify, merge, publish,
/// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all copies or
/// substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
/// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
/// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///

#pragma once

#include "vari/bits/typelist.h"
#include "vari/bits/util.h"

#include <cstdint>
#include <memory>

namespace vari
{

template < typename UL, typename TL >
struct _split_impl;

template < typename... Us, typename T, typename... Ts >
        requires( sizeof...( Us ) < sizeof...( Ts ) )
struct _split_impl< typelist< Us... >, typelist< T, Ts... > >
{
        using sub = _split_impl< typelist< Us..., T >, typelist< Ts... > >;
        using lh  = sub::lh;
        using rh  = sub::rh;
};

template < typename... Us, typename T, typename... Ts >
        requires( sizeof...( Us ) >= sizeof...( Ts ) )
struct _split_impl< typelist< Us... >, typelist< T, Ts... > >
{
        using lh = typelist< Us... >;
        using rh = typelist< T, Ts... >;
};

template <>
struct _split_impl< typelist<>, typelist<> >
{
        using lh = typelist<>;
        using rh = typelist<>;
};

template < typename TL >
using split = _split_impl< typelist<>, TL >;

template < typename TL >
union _val_union;

template < typename... Ts >
        requires( sizeof...( Ts ) > 4 )
union _val_union< typelist< Ts... > >
{
        static constexpr std::size_t size = sizeof...( Ts );

        _val_union() noexcept {};
        ~_val_union(){};

        using s1 = split< typelist< Ts... > >;
        using s2 = s1::lh;
        using s3 = s1::rh;

        using b0 = _val_union< typename s2::lh >;
        using b1 = _val_union< typename s2::rh >;
        using b2 = _val_union< typename s3::lh >;
        using b3 = _val_union< typename s3::rh >;

        b0 box0;
        b1 box1;
        b2 box2;
        b3 box3;

        template < std::size_t i >
        constexpr static auto& get( auto& s )
        {
                if constexpr ( i < b0::size )
                        return b0::template get< i >( s.box0 );
                else if constexpr ( i < b0::size + b1::size )
                        return b1::template get< i - b0::size >( s.box1 );
                else if constexpr ( i < b0::size + b1::size + b2::size )
                        return b2::template get< i - b0::size - b1::size >( s.box2 );
                else if constexpr ( i < b0::size + b1::size + b2::size + b3::size )
                        return b3::template get< i - b0::size - b1::size - b2::size >( s.box3 );
        }
};

template < typename T >
union _val_union< typelist< T > >
{
        static constexpr std::size_t size = 1;

        _val_union() noexcept {};
        ~_val_union(){};

        T item0;

        template < std::size_t >
        constexpr static auto& get( auto& s )
        {
                return s.item0;
        }
};

template < typename T0, typename T1 >
union _val_union< typelist< T0, T1 > >
{
        static constexpr std::size_t size = 2;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;

        template < std::size_t i >
        constexpr static auto& get( auto& s )
        {
                if constexpr ( i == 0 )
                        return s.item0;
                if constexpr ( i == 1 )
                        return s.item1;
        }
};

template < typename T0, typename T1, typename T2 >
union _val_union< typelist< T0, T1, T2 > >
{
        static constexpr std::size_t size = 3;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;

        template < std::size_t i >
        constexpr static auto& get( auto& s )
        {
                if constexpr ( i == 0 )
                        return s.item0;
                if constexpr ( i == 1 )
                        return s.item1;
                if constexpr ( i == 2 )
                        return s.item2;
        }
};

template < typename T0, typename T1, typename T2, typename T3 >
union _val_union< typelist< T0, T1, T2, T3 > >
{
        static constexpr std::size_t size = 4;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;
        T3 item3;

        template < std::size_t i >
        constexpr static auto& get( auto& s )
        {
                if constexpr ( i == 0 )
                        return s.item0;
                if constexpr ( i == 1 )
                        return s.item1;
                if constexpr ( i == 2 )
                        return s.item2;
                if constexpr ( i == 3 )
                        return s.item3;
        }
};

template < typename T0, typename T1, typename T2, typename T3, typename T4 >
union _val_union< typelist< T0, T1, T2, T3, T4 > >
{
        static constexpr std::size_t size = 5;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;
        T3 item3;
        T4 item4;

        template < std::size_t i >
        constexpr static auto& get( auto& s )
        {
                if constexpr ( i == 0 )
                        return s.item0;
                if constexpr ( i == 1 )
                        return s.item1;
                if constexpr ( i == 2 )
                        return s.item2;
                if constexpr ( i == 3 )
                        return s.item3;
                if constexpr ( i == 4 )
                        return s.item4;
        }
};

template < typename T0, typename T1, typename T2, typename T3, typename T4, typename T5 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5 > >
{
        static constexpr std::size_t size = 6;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;
        T3 item3;
        T4 item4;
        T5 item5;

        template < std::size_t i >
        constexpr static auto& get( auto& s )
        {
                if constexpr ( i == 0 )
                        return s.item0;
                if constexpr ( i == 1 )
                        return s.item1;
                if constexpr ( i == 2 )
                        return s.item2;
                if constexpr ( i == 3 )
                        return s.item3;
                if constexpr ( i == 4 )
                        return s.item4;
                if constexpr ( i == 5 )
                        return s.item5;
        }
};

template <
    typename T0,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename T5,
    typename T6 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6 > >
{
        static constexpr std::size_t size = 7;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;
        T3 item3;
        T4 item4;
        T5 item5;
        T6 item6;

        template < std::size_t i >
        constexpr static auto& get( auto& s )
        {
                if constexpr ( i == 0 )
                        return s.item0;
                if constexpr ( i == 1 )
                        return s.item1;
                if constexpr ( i == 2 )
                        return s.item2;
                if constexpr ( i == 3 )
                        return s.item3;
                if constexpr ( i == 4 )
                        return s.item4;
                if constexpr ( i == 5 )
                        return s.item5;
                if constexpr ( i == 6 )
                        return s.item6;
        }
};

template <
    typename T0,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename T5,
    typename T6,
    typename T7 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6, T7 > >
{
        static constexpr std::size_t size = 8;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;
        T3 item3;
        T4 item4;
        T5 item5;
        T6 item6;
        T7 item7;

        template < std::size_t i >
        constexpr static auto& get( auto& s )
        {
                if constexpr ( i == 0 )
                        return s.item0;
                if constexpr ( i == 1 )
                        return s.item1;
                if constexpr ( i == 2 )
                        return s.item2;
                if constexpr ( i == 3 )
                        return s.item3;
                if constexpr ( i == 4 )
                        return s.item4;
                if constexpr ( i == 5 )
                        return s.item5;
                if constexpr ( i == 6 )
                        return s.item6;
                if constexpr ( i == 7 )
                        return s.item7;
        }
};

template < typename TL >
struct _val_core
{
        using ST = _val_union< TL >;

        std::size_t index = 0;
        ST          storage;

        constexpr _val_core() noexcept = default;

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        constexpr _val_core( _val_core< UL > const& other )
        {
                if ( other.index == 0 )
                        return;
                _dispatch_index< 0, TL::size >(
                    other.index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            static constexpr std::size_t i = _vptr_cnv_map< TL, UL >::value[j];

                            index = i;
                            std::construct_at(
                                &storage.template get< i >(), other.storage.template get< j >() );
                    } );
        }

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        constexpr _val_core( _val_core< UL >&& other ) noexcept
        {
                if ( other.index == 0 )
                        return;
                _dispatch_index< 0, TL::size >(
                    other.index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            static constexpr std::size_t i = _vptr_cnv_map< TL, UL >::value[j];

                            index = i;
                            std::construct_at(
                                &storage.template get< i >(),
                                std::move( other.storage.template get< j >() ) );
                    } );
        }

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        constexpr _val_core& operator=( _val_core< UL > const& other )
        {
                _val_core tmp{ other };
                swap( *this, tmp );
        }

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        constexpr _val_core& operator=( _val_core< UL >&& other ) noexcept
        {
                _val_core tmp{ std::move( other ) };
                swap( *this, tmp );
        }

        friend constexpr void swap( _val_core& lh, _val_core& rh )
        {
                if ( lh.index == rh.index )
                        return _dispatch_index< 0, TL::size >(
                            lh.index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                                    auto& l = lh.storage.template get< j >();
                                    auto& r = rh.storage.template get< j >();
                                    using namespace std;
                                    swap( l, r );
                            } );

                _val_core tmp{ std::move( lh ) };
                if ( lh.index != 0 ) {
                        lh.destroy();
                        tmp.index = std::exchange( lh.index, 0 );
                }

                if ( rh.index != 0 )
                        move_from_to( rh, lh );

                if ( tmp.index != 0 )
                        move_from_to( tmp, rh );
        }

        friend constexpr void move_from_to( _val_core& lh, _val_core& rh )
        {
                _dispatch_index< 0, TL::size >(
                    lh.index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            auto& l = lh.storage.template get< j >();
                            auto& r = rh.storage.template get< j >();
                            std::construct_at( &r, std::move( l ) );
                            rh.index = lh.index;
                            std::destroy_at( &l );
                    } );
        }


        // XXX: code dup. much
        template < typename... Fs >
        constexpr decltype( auto ) visit_impl( Fs&&... fs ) const
        {
                return _dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            auto& p = ST::template get< j >( storage );
                            return _dispatch_fun( p, (Fs&&) fs... );
                    } );
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit_impl( Fs&&... fs )
        {
                return _dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            auto& p = ST::template get< j >( storage );
                            return _dispatch_fun( p, (Fs&&) fs... );
                    } );
        }

        template < typename F >
        constexpr decltype( auto ) visit_impl( F&& f ) const
        {
                return _dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            auto& p = ST::template get< j >( storage );

                            return ( (F&&) f )( p );
                    } );
        }

        template < typename F >
        constexpr decltype( auto ) visit_impl( F&& f )
        {
                return _dispatch_index< 0, TL::size >(
                    index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            auto& p = ST::template get< j >( storage );

                            return ( (F&&) f )( p );
                    } );
        }

        template < typename T, typename... Args >
        constexpr T& emplace( Args&&... args )
        {
                constexpr std::size_t i = index_of_v< T, TL >;

                index = i + 1;
                return *std::construct_at( &ST::template get< i >( storage ), (Args&&) args... );
        }

        constexpr void destroy()
        {
                index = 0;
                _dispatch_index< 0, TL::size >( index - 1, [&]< std::size_t j > {
                        std::destroy_at( &ST::template get< j >( storage ) );
                } );
        }
};


}  // namespace vari
