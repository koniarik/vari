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

#include "vari/uvptr.h"
#include "vari/uvref.h"
#include "vari/vptr.h"
#include "vari/vref.h"

#include <iostream>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

template < typename... Ts >
using tl = vari::typelist< Ts... >;

template < typename... Ts >
using uvref = vari::uvref< void, Ts... >;
template < typename... Ts >
using vref = vari::vref< void, Ts... >;

template < typename T >
auto uwrap( T item )
{
        return vari::uwrap< void >( std::move( item ) );
}

struct arithm_op;
struct variable;
struct constant;
struct reference;

using value_type = float;
using expr       = tl< arithm_op, variable, constant, reference >;

struct arithm_op
{
        enum op_t
        {
                PLUS,
                MINUS,
                MULT,
                DIV
        };

        uvref< expr > lh;
        op_t          op;
        uvref< expr > rh;
};

struct variable
{
        std::size_t      var_i;
        std::string_view name;
};

struct constant
{
        value_type val;
};

struct reference
{
        vref< expr > ref;
};

std::ostream& operator<<( std::ostream& os, vref< expr > e )
{
        e.visit(
            [&]( arithm_op& aop ) {
                    char c = ' ';
                    switch ( aop.op ) {
                    case arithm_op::PLUS:
                            c = '+';
                            break;
                    case arithm_op::MINUS:
                            c = '-';
                            break;
                    case arithm_op::MULT:
                            c = '*';
                            break;
                    case arithm_op::DIV:
                            c = '/';
                            break;
                    }
                    switch ( aop.op ) {
                    case arithm_op::PLUS:
                    case arithm_op::MINUS:
                            os << '(' << aop.lh.get() << c << aop.rh.get() << ')';
                            break;
                    case arithm_op::MULT:
                    case arithm_op::DIV:
                            os << aop.lh.get() << c << aop.rh.get();
                    }
            },
            [&]( variable& v ) {
                    os << v.name;
            },
            [&]( constant& c ) {
                    os << c.val;
            },
            [&]( reference& r ) {
                    os << r.ref;
            } );

        return os;
}

value_type eval( vref< expr > e, std::vector< value_type > const& inpt )
{
        return e.visit(
            [&]( arithm_op& aop ) -> value_type {
                    auto lh = eval( aop.lh.get(), inpt );
                    auto rh = eval( aop.rh.get(), inpt );

                    switch ( aop.op ) {
                    case arithm_op::PLUS:
                            return lh + rh;
                    case arithm_op::MINUS:
                            return lh - rh;
                    case arithm_op::MULT:
                            return lh * rh;
                    case arithm_op::DIV:
                            return lh / rh;
                    }
                    return 0;
            },
            [&]( variable& v ) {
                    return inpt.at( v.var_i );
            },
            [&]( constant& c ) {
                    return c.val;
            },
            [&]( reference& r ) {
                    return eval( r.ref, inpt );
            } );
}

uvref< expr > simplify( vref< expr > e, std::vector< value_type > const& inpt )
{
        return e.match(
            [&]( vref< arithm_op > aop ) -> uvref< expr > {
                    auto lh = simplify( aop->lh.get(), inpt );
                    auto rh = simplify( aop->rh.get(), inpt );

                    return std::move( lh ).take(
                        [&]( uvref< variable, arithm_op, reference > lhu ) -> uvref< expr > {
                                return uwrap( arithm_op{
                                    .lh = std::move( lhu ),
                                    .op = aop->op,
                                    .rh = std::move( rh ),
                                } );
                        },
                        [&]( uvref< constant > lhc ) -> uvref< expr > {
                                return std::move( rh ).take(
                                    [&]( uvref< variable, arithm_op, reference > rhu )
                                        -> uvref< expr > {
                                            return uwrap( arithm_op{
                                                .lh = std::move( lhc ),
                                                .op = aop->op,
                                                .rh = std::move( rhu ),
                                            } );
                                    },
                                    [&]( uvref< constant > rhc ) -> uvref< expr > {
                                            arithm_op tmp{
                                                .lh = std::move( lhc ),
                                                .op = aop->op,
                                                .rh = std::move( rhc ) };
                                            return uwrap( constant{
                                                .val = eval( vref< expr >( tmp ), {} ),
                                            } );
                                    } );
                        } );
            },
            [&]( vref< variable > v ) -> uvref< expr > {
                    if ( v->var_i < inpt.size() )
                            return uwrap( constant{ inpt[v->var_i] } );
                    v->var_i -= inpt.size();
                    return uwrap( *v );
            },
            [&]( vref< constant > c ) -> uvref< expr > {
                    return uwrap( *c );
            },
            [&]( vref< reference > r ) -> uvref< expr > {
                    return simplify( r->ref, inpt );
            } );
}

uvref< arithm_op > make_op( uvref< expr > a, arithm_op::op_t op, uvref< expr > b )
{
        return uwrap( arithm_op{ std::move( a ), op, std::move( b ) } );
}

uvref< expr > fwd_expr( value_type v )
{
        return uwrap( constant{ v } );
}
uvref< expr > fwd_expr( vref< expr > v )
{
        return uwrap( reference{ v } );
}
uvref< expr > fwd_expr( uvref< expr > v )
{
        return v;
}

template < typename T >
concept expr_or_val =
    std::convertible_to< T, vref< expr > > || std::convertible_to< T, uvref< expr > > ||
    std::convertible_to< T, value_type >;

template < expr_or_val LH, expr_or_val RH >
auto operator+( LH&& lh, RH&& rh )
{
        return make_op(
            fwd_expr( std::forward< LH >( lh ) ),
            arithm_op::PLUS,
            fwd_expr( std::forward< RH >( rh ) ) );
}
template < expr_or_val LH, expr_or_val RH >
auto operator-( LH&& lh, RH&& rh )
{
        return make_op(
            fwd_expr( std::forward< LH >( lh ) ),
            arithm_op::MINUS,
            fwd_expr( std::forward< RH >( rh ) ) );
}
template < expr_or_val LH, expr_or_val RH >
auto operator*( LH&& lh, RH&& rh )
{
        return make_op(
            fwd_expr( std::forward< LH >( lh ) ),
            arithm_op::MULT,
            fwd_expr( std::forward< RH >( rh ) ) );
}
template < expr_or_val LH, expr_or_val RH >
auto operator/( LH&& lh, RH&& rh )
{
        return make_op(
            fwd_expr( std::forward< LH >( lh ) ),
            arithm_op::DIV,
            fwd_expr( std::forward< RH >( rh ) ) );
}

int main( int argc, char* argv[] )
{
        variable X{ .var_i = 0, .name = "X"sv };
        variable Y{ .var_i = 1, .name = "Y"sv };

        auto f = X + Y * 2 / 42 + ( X - 42 );
        std::cout << f.get() << std::endl;
        auto f2 = simplify( f.get(), { 1 } );
        std::cout << f2.get() << std::endl;

        std::cout << eval( f2.get(), { 1 } ) << std::endl;

        return 0;
}
