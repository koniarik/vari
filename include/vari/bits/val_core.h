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
#include <limits>
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
union _val_union< typelist< Ts... > >
{
        static constexpr index_type size = sizeof...( Ts );

        _val_union() noexcept {};
        ~_val_union(){};

        using s1 = split< typelist< Ts... > >;
        using s2 = split< typename s1::lh >;
        using s3 = split< typename s1::rh >;

        using b0 = _val_union< typename s2::lh >;
        using b1 = _val_union< typename s2::rh >;
        using b2 = _val_union< typename s3::lh >;
        using b3 = _val_union< typename s3::rh >;

        b0 box0;
        b1 box1;
        b2 box2;
        b3 box3;

        template < index_type i >
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
        static constexpr index_type size = 1;

        _val_union() noexcept {};
        ~_val_union(){};

        T item0;

        template < index_type >
        constexpr static auto& get( auto& s )
        {
                return s.item0;
        }
};

template < typename T0, typename T1 >
union _val_union< typelist< T0, T1 > >
{
        static constexpr index_type size = 2;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;

        template < index_type i >
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
        static constexpr index_type size = 3;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;

        template < index_type i >
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
        static constexpr index_type size = 4;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;
        T3 item3;

        template < index_type i >
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
        static constexpr index_type size = 5;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;
        T3 item3;
        T4 item4;

        template < index_type i >
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
        static constexpr index_type size = 6;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;
        T3 item3;
        T4 item4;
        T5 item5;

        template < index_type i >
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
        static constexpr index_type size = 7;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;
        T1 item1;
        T2 item2;
        T3 item3;
        T4 item4;
        T5 item5;
        T6 item6;

        template < index_type i >
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
        static constexpr index_type size = 8;

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

        template < index_type i >
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

        index_type index = null_index;
        ST         storage;

        constexpr _val_core() noexcept = default;

        constexpr _val_core( _val_core const& other ) noexcept(
            all_nothrow_copy_constructible_v< TL > )
        {
                _copy_or_move_construct< false, TL >( *this, other );
        }

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        constexpr _val_core( _val_core< UL > const& other ) noexcept(
            all_nothrow_copy_constructible_v< UL > )
        {
                _copy_or_move_construct< false, UL >( *this, other );
        }

        constexpr _val_core( _val_core&& other ) noexcept( all_nothrow_move_constructible_v< TL > )
        {
                _copy_or_move_construct< true, TL >( *this, other );
        }

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        constexpr _val_core( _val_core< UL >&& other ) noexcept(
            all_nothrow_move_constructible_v< UL > )
        {
                _copy_or_move_construct< true, UL >( *this, other );
        }

        template < bool IS_MOVE, typename UL >
        static constexpr void _copy_or_move_construct( auto& self, auto& other ) noexcept(
            IS_MOVE ? all_nothrow_move_constructible_v< UL > :
                      all_nothrow_copy_constructible_v< UL > )
        {
                if ( other.index == null_index )
                        return;
                _dispatch_index< 0, UL::size >(
                    other.index, [&]< index_type j >() -> decltype( auto ) {
                            static constexpr index_type i = _vptr_cnv_map< TL, UL >::conv( j );
                            using OST                     = typename _val_core< UL >::ST;

                            self.index = i;
                            if ( IS_MOVE )
                                    std::construct_at(
                                        &ST::template get< i >( self.storage ),
                                        std::move( OST::template get< j >( other.storage ) ) );
                            else
                                    std::construct_at(
                                        &ST::template get< i >( self.storage ),
                                        OST::template get< j >( other.storage ) );
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
        constexpr _val_core& operator=( _val_core< UL >&& other ) noexcept(
            all_nothrow_move_constructible_v< UL > && all_nothrow_destructible_v< TL > &&
            all_nothrow_swappable_v< TL > )
        {
                _val_core tmp{ std::move( other ) };
                swap( *this, tmp );
        }

        friend constexpr void swap( _val_core& lh, _val_core& rh ) noexcept(
            all_nothrow_swappable_v< TL > && all_nothrow_move_constructible_v< TL > &&
            all_nothrow_destructible_v< TL > )
        {
                if ( lh.index == rh.index )
                        return _dispatch_index< 0, TL::size >(
                            lh.index, [&]< index_type j >() -> decltype( auto ) {
                                    auto& l = ST::template get< j >( lh.storage );
                                    auto& r = ST::template get< j >( rh.storage );
                                    using namespace std;
                                    swap( l, r );
                            } );

                _val_core tmp{ std::move( lh ) };
                if ( lh.index != null_index )
                        lh.destroy();

                if ( rh.index != null_index )
                        move_from_to( rh, lh );

                if ( tmp.index != null_index )
                        move_from_to( tmp, rh );
        }

        friend constexpr void move_from_to( _val_core& lh, _val_core& rh ) noexcept(
            all_nothrow_move_constructible_v< TL > && all_nothrow_destructible_v< TL > )
        {
                _dispatch_index< 0, TL::size >(
                    lh.index, [&]< index_type j >() -> decltype( auto ) {
                            auto& l = ST::template get< j >( lh.storage );
                            auto& r = ST::template get< j >( rh.storage );
                            std::construct_at( &r, std::move( l ) );
                            rh.index = lh.index;
                            std::destroy_at( &l );
                    } );
        }


        template < typename... Fs >
        static constexpr decltype( auto ) visit_impl( auto& self, Fs&&... fs )
        {
                return _dispatch_index< 0, TL::size >(
                    self.index, [&]< index_type j >() -> decltype( auto ) {
                            auto& p = ST::template get< j >( self.storage );
                            return _dispatch_fun( p, (Fs&&) fs... );
                    } );
        }

        template < typename F >
        static constexpr decltype( auto ) visit_impl( auto& self, F&& f )
        {
                return _dispatch_index< 0, TL::size >(
                    self.index, [&]< index_type j >() -> decltype( auto ) {
                            auto& p = ST::template get< j >( self.storage );

                            return ( (F&&) f )( p );
                    } );
        }

        template < typename T, typename... Args >
        constexpr T&
        emplace( Args&&... args ) noexcept( std::is_nothrow_constructible_v< T, Args... > )
        {
                constexpr index_type i = index_of_t_or_const_t_v< T, TL >;

                index = i;
                return *std::construct_at( &ST::template get< i >( storage ), (Args&&) args... );
        }

        constexpr void destroy() noexcept( all_nothrow_destructible_v< TL > )
        {
                _dispatch_index< 0, TL::size >( index, [&]< index_type j > {
                        std::destroy_at( &ST::template get< j >( storage ) );
                } );
                index = null_index;
        }

        // XXX: this needs serious tests
        // XXX: derive the ordering!
        static constexpr std::partial_ordering three_way_compare(
            _val_core const& lh,
            _val_core const& rh ) noexcept( all_nothrow_move_constructible_v< TL > )
        {
                // XXX: the partial ordering thing might be improved

                index_type lh_i = lh.index;
                index_type rh_i = rh.index;
                if ( lh_i != rh_i )
                        return lh_i <=> rh_i;
                return _dispatch_index< 0, TL::size >(
                    lh_i, [&]< index_type j >() -> std::partial_ordering {
                            return ST::template get< j >( lh.storage ) <=>
                                   ST::template get< j >( rh.storage );
                    } );
        }

        static constexpr decltype( auto ) compare(
            _val_core const& lh,
            _val_core const& rh ) noexcept( all_nothrow_equality_comparable_v< TL > )
        {
                index_type lh_i = lh.index;
                index_type rh_i = rh.index;
                if ( lh_i != rh_i )
                        return lh_i == rh_i;
                return _dispatch_index< 0, TL::size >( lh_i, [&]< index_type j > {
                        return ST::template get< j >( lh.storage ) ==
                               ST::template get< j >( rh.storage );
                } );
        }
};

// XXX:
// - vref/vptr should be constructible from this

}  // namespace vari
