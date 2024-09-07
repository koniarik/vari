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
union _val_union< typelist< Ts... > >
{
        static constexpr std::size_t size = sizeof...( Ts );

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
                if ( other.index == 0 )
                        return;
                _dispatch_index< 0, UL::size >(
                    other.index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            static constexpr std::size_t i = _vptr_cnv_map< TL, UL >::value[j + 1];
                            using OST                      = typename _val_core< UL >::ST;

                            self.index = i;
                            if ( IS_MOVE )
                                    std::construct_at(
                                        &ST::template get< i - 1 >( self.storage ),
                                        std::move( OST::template get< j >( other.storage ) ) );
                            else
                                    std::construct_at(
                                        &ST::template get< i - 1 >( self.storage ),
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
                            lh.index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                                    auto& l = ST::template get< j >( lh.storage );
                                    auto& r = ST::template get< j >( rh.storage );
                                    using namespace std;
                                    swap( l, r );
                            } );

                _val_core tmp{ std::move( lh ) };
                if ( lh.index != 0 )
                        lh.destroy();

                if ( rh.index != 0 )
                        move_from_to( rh, lh );

                if ( tmp.index != 0 )
                        move_from_to( tmp, rh );
        }

        friend constexpr void move_from_to( _val_core& lh, _val_core& rh ) noexcept(
            all_nothrow_move_constructible_v< TL > && all_nothrow_destructible_v< TL > )
        {
                _dispatch_index< 0, TL::size >(
                    lh.index - 1, [&]< std::size_t j >() -> decltype( auto ) {
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
                    self.index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            auto& p = ST::template get< j >( self.storage );
                            return _dispatch_fun( p, (Fs&&) fs... );
                    } );
        }

        template < typename F >
        static constexpr decltype( auto ) visit_impl( auto& self, F&& f )
        {
                return _dispatch_index< 0, TL::size >(
                    self.index - 1, [&]< std::size_t j >() -> decltype( auto ) {
                            auto& p = ST::template get< j >( self.storage );

                            return ( (F&&) f )( p );
                    } );
        }

        template < typename T, typename... Args >
        constexpr T&
        emplace( Args&&... args ) noexcept( std::is_nothrow_constructible_v< T, Args... > )
        {
                constexpr std::size_t i = index_of_t_or_const_t_v< T, TL >;

                index = i + 1;
                return *std::construct_at( &ST::template get< i >( storage ), (Args&&) args... );
        }

        constexpr void destroy() noexcept( all_nothrow_destructible_v< TL > )
        {
                _dispatch_index< 0, TL::size >( index - 1, [&]< std::size_t j > {
                        std::destroy_at( &ST::template get< j >( storage ) );
                } );
                index = 0;
        }

        // XXX: this needs serious tests
        // XXX: derive the ordering!
        static constexpr std::partial_ordering three_way_compare(
            _val_core const& lh,
            _val_core const& rh ) noexcept( all_nothrow_move_constructible_v< TL > )
        {
                // XXX: the partial ordering thing might be improved

                std::size_t lh_i = lh.index - 1;
                std::size_t rh_i = rh.index - 1;
                if ( lh_i != rh_i )
                        return lh_i <=> rh_i;
                return _dispatch_index< 0, TL::size >(
                    lh_i, [&]< std::size_t j >() -> std::partial_ordering {
                            return ST::template get< j >( lh.storage ) <=>
                                   ST::template get< j >( rh.storage );
                    } );
        }

        static constexpr decltype( auto ) compare(
            _val_core const& lh,
            _val_core const& rh ) noexcept( all_nothrow_equality_comparable_v< TL > )
        {
                std::size_t lh_i = lh.index - 1;
                std::size_t rh_i = rh.index - 1;
                if ( lh_i != rh_i )
                        return lh_i == rh_i;
                return _dispatch_index< 0, TL::size >( lh_i, [&]< std::size_t j > {
                        return ST::template get< j >( lh.storage ) ==
                               ST::template get< j >( rh.storage );
                } );
        }
};

// XXX:
// - vref/vptr should be constructible from this

}  // namespace vari
