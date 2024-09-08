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

namespace vari
{

template < typename TL >
union _val_union
{
        static_assert(
            !std::same_as< TL, TL >,
            "val_union<T> created with unacceptable type - only typelist is allowed" );
};

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

// VARI VAL UNION GEN START


template < typename T0 >
union _val_union< typelist< T0 > >
{
        static constexpr index_type size = 1;

        _val_union() noexcept {};
        ~_val_union(){};

        T0 item0;


        template < index_type i >
        constexpr static auto& get( auto& s )
        {
                if constexpr ( i == 0 )
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


template <
    typename T0,
    typename T1,
    typename T2,
    typename T3,
    typename T4,
    typename T5,
    typename T6,
    typename T7,
    typename T8 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6, T7, T8 > >
{
        static constexpr index_type size = 9;

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
        T8 item8;


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
                if constexpr ( i == 8 )
                        return s.item8;
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
    typename T7,
    typename T8,
    typename T9 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6, T7, T8, T9 > >
{
        static constexpr index_type size = 10;

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
        T8 item8;
        T9 item9;


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
                if constexpr ( i == 8 )
                        return s.item8;
                if constexpr ( i == 9 )
                        return s.item9;
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
    typename T7,
    typename T8,
    typename T9,
    typename T10 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 > >
{
        static constexpr index_type size = 11;

        _val_union() noexcept {};
        ~_val_union(){};

        T0  item0;
        T1  item1;
        T2  item2;
        T3  item3;
        T4  item4;
        T5  item5;
        T6  item6;
        T7  item7;
        T8  item8;
        T9  item9;
        T10 item10;


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
                if constexpr ( i == 8 )
                        return s.item8;
                if constexpr ( i == 9 )
                        return s.item9;
                if constexpr ( i == 10 )
                        return s.item10;
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
    typename T7,
    typename T8,
    typename T9,
    typename T10,
    typename T11 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11 > >
{
        static constexpr index_type size = 12;

        _val_union() noexcept {};
        ~_val_union(){};

        T0  item0;
        T1  item1;
        T2  item2;
        T3  item3;
        T4  item4;
        T5  item5;
        T6  item6;
        T7  item7;
        T8  item8;
        T9  item9;
        T10 item10;
        T11 item11;


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
                if constexpr ( i == 8 )
                        return s.item8;
                if constexpr ( i == 9 )
                        return s.item9;
                if constexpr ( i == 10 )
                        return s.item10;
                if constexpr ( i == 11 )
                        return s.item11;
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
    typename T7,
    typename T8,
    typename T9,
    typename T10,
    typename T11,
    typename T12 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12 > >
{
        static constexpr index_type size = 13;

        _val_union() noexcept {};
        ~_val_union(){};

        T0  item0;
        T1  item1;
        T2  item2;
        T3  item3;
        T4  item4;
        T5  item5;
        T6  item6;
        T7  item7;
        T8  item8;
        T9  item9;
        T10 item10;
        T11 item11;
        T12 item12;


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
                if constexpr ( i == 8 )
                        return s.item8;
                if constexpr ( i == 9 )
                        return s.item9;
                if constexpr ( i == 10 )
                        return s.item10;
                if constexpr ( i == 11 )
                        return s.item11;
                if constexpr ( i == 12 )
                        return s.item12;
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
    typename T7,
    typename T8,
    typename T9,
    typename T10,
    typename T11,
    typename T12,
    typename T13 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13 > >
{
        static constexpr index_type size = 14;

        _val_union() noexcept {};
        ~_val_union(){};

        T0  item0;
        T1  item1;
        T2  item2;
        T3  item3;
        T4  item4;
        T5  item5;
        T6  item6;
        T7  item7;
        T8  item8;
        T9  item9;
        T10 item10;
        T11 item11;
        T12 item12;
        T13 item13;


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
                if constexpr ( i == 8 )
                        return s.item8;
                if constexpr ( i == 9 )
                        return s.item9;
                if constexpr ( i == 10 )
                        return s.item10;
                if constexpr ( i == 11 )
                        return s.item11;
                if constexpr ( i == 12 )
                        return s.item12;
                if constexpr ( i == 13 )
                        return s.item13;
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
    typename T7,
    typename T8,
    typename T9,
    typename T10,
    typename T11,
    typename T12,
    typename T13,
    typename T14 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14 > >
{
        static constexpr index_type size = 15;

        _val_union() noexcept {};
        ~_val_union(){};

        T0  item0;
        T1  item1;
        T2  item2;
        T3  item3;
        T4  item4;
        T5  item5;
        T6  item6;
        T7  item7;
        T8  item8;
        T9  item9;
        T10 item10;
        T11 item11;
        T12 item12;
        T13 item13;
        T14 item14;


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
                if constexpr ( i == 8 )
                        return s.item8;
                if constexpr ( i == 9 )
                        return s.item9;
                if constexpr ( i == 10 )
                        return s.item10;
                if constexpr ( i == 11 )
                        return s.item11;
                if constexpr ( i == 12 )
                        return s.item12;
                if constexpr ( i == 13 )
                        return s.item13;
                if constexpr ( i == 14 )
                        return s.item14;
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
    typename T7,
    typename T8,
    typename T9,
    typename T10,
    typename T11,
    typename T12,
    typename T13,
    typename T14,
    typename T15 >
union _val_union< typelist< T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15 > >
{
        static constexpr index_type size = 16;

        _val_union() noexcept {};
        ~_val_union(){};

        T0  item0;
        T1  item1;
        T2  item2;
        T3  item3;
        T4  item4;
        T5  item5;
        T6  item6;
        T7  item7;
        T8  item8;
        T9  item9;
        T10 item10;
        T11 item11;
        T12 item12;
        T13 item13;
        T14 item14;
        T15 item15;


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
                if constexpr ( i == 8 )
                        return s.item8;
                if constexpr ( i == 9 )
                        return s.item9;
                if constexpr ( i == 10 )
                        return s.item10;
                if constexpr ( i == 11 )
                        return s.item11;
                if constexpr ( i == 12 )
                        return s.item12;
                if constexpr ( i == 13 )
                        return s.item13;
                if constexpr ( i == 14 )
                        return s.item14;
                if constexpr ( i == 15 )
                        return s.item15;
        }
};

// VARI VAL UNION GEN END


}  // namespace vari
