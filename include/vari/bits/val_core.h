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

#include "val_union.h"

#include <memory>

namespace vari
{

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

        constexpr _val_core& operator=( _val_core const& other ) noexcept(
            all_nothrow_copy_constructible_v< TL > && all_nothrow_destructible_v< TL > &&
            all_nothrow_swappable_v< TL > )
        {
                _val_core tmp{ other };
                swap( *this, tmp );
                return *this;
        }

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        constexpr _val_core& operator=( _val_core< UL > const& other ) noexcept(
            all_nothrow_copy_constructible_v< UL > && all_nothrow_destructible_v< TL > &&
            all_nothrow_swappable_v< TL > )
        {
                _val_core tmp{ other };
                swap( *this, tmp );
                return *this;
        }

        constexpr _val_core& operator=( _val_core&& other ) noexcept(
            all_nothrow_move_constructible_v< TL > && all_nothrow_destructible_v< TL > &&
            all_nothrow_swappable_v< TL > )
        {
                _val_core tmp{ std::move( other ) };
                swap( *this, tmp );
                return *this;
        }

        template < typename UL >
                requires( vconvertible_to< UL, TL > )
        constexpr _val_core& operator=( _val_core< UL >&& other ) noexcept(
            all_nothrow_move_constructible_v< UL > && all_nothrow_destructible_v< TL > &&
            all_nothrow_swappable_v< TL > )
        {
                _val_core tmp{ std::move( other ) };
                swap( *this, tmp );
                return *this;
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
