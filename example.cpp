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

#include <charconv>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <span>
#include <string_view>
#include <vector>

using namespace std::string_view_literals;

using namespace vari;

template < typename... Ts >
using tl = vari::typelist< Ts... >;

struct bin_arithm_op;
struct constant;
struct id;
struct definition;

using value_type = long;
using expr       = tl< bin_arithm_op, constant, id >;

struct bin_arithm_op
{

        uvref< expr > lh;
        char          op;
        uvref< expr > rh;
};

struct constant
{
        value_type val;
};

struct id
{
        std::string_view text;
};

struct definition
{
        std::string_view name;
        uvref< expr >    e;
};

namespace lexer
{
enum class token_type
{
        NUM,
        OP,
        ID,
        LINE,
        END
};

struct lex_tok
{
        token_type       token;
        std::string_view text;
};

std::vector< lex_tok > full_lex( std::string_view inpt );

}  // namespace lexer

namespace parser
{
std::vector< definition > parse_defs( std::span< lexer::lex_tok > toks );
}

std::ostream& operator<<( std::ostream& os, vref< const expr > e )
{
        e.visit(
            [&]( bin_arithm_op const& aop ) {
                    char c = aop.op;
                    switch ( aop.op ) {
                    case '+':
                    case '-':
                            os << '(' << aop.lh.get() << c << aop.rh.get() << ')';
                            break;
                    case '*':
                    case '/':
                            os << aop.lh.get() << c << aop.rh.get();
                    }
            },
            [&]( id const& id ) {
                    os << id.text;
            },
            [&]( constant const& c ) {
                    os << c.val;
            } );

        return os;
}

std::ostream& operator<<( std::ostream& os, definition const& d )
{
        std::cout << d.name << " = " << d.e.get();
        return os;
}

using state = std::map< std::string_view, value_type >;
value_type eval( state& st, vref< const expr > e )
{
        return e.visit(
            [&]( id const& id ) {
                    assert( st.contains( id.text ) );
                    return st[id.text];
            },
            [&]( bin_arithm_op const& aop ) -> value_type {
                    auto lh = eval( st, aop.lh.get() );
                    auto rh = eval( st, aop.rh.get() );

                    switch ( aop.op ) {
                    case '+':
                            return lh + rh;
                    case '-':
                            return lh - rh;
                    case '*':
                            return lh * rh;
                    case '/':
                            return lh / rh;
                    }
                    return 0;
            },
            [&]( constant const& c ) {
                    return c.val;
            } );
}

int main( int argc, char* argv[] )
{
        std::string inpt = R"""(
a = 42 + 42 / 2 - 8*8
b = a * 2
main = b
    )""";

        auto toks = lexer::full_lex( inpt );
        auto defs = parser::parse_defs( toks );

        for ( auto const& d : defs )
                std::cout << d << std::endl;

        state st;
        for ( auto& d : defs )
                st[d.name] = eval( st, d.e );

        std::cout << std::endl;
        std::cout << "main = " << st["main"] << std::endl;

        return 0;
}


// Implementation of lexer and parser

[[noreturn]] void abort( std::string_view msg, auto const&... args )
{
        std::cerr << msg;
        ( ( std::cerr << args ), ... );
        std::cerr << std::endl;
        std::abort();
}

void check( bool cond, std::string_view msg, auto const&... args )
{
        if ( !cond )
                abort( msg, args... );
}

namespace lexer
{

lex_tok lex( std::string_view& inpt )
{
        if ( inpt.empty() )
                return { .token = token_type::END };

        char c = inpt[0];
        if ( std::isdigit( c ) ) {
                std::size_t i = 1;
                for ( ; i < inpt.size(); i++ )
                        if ( !std::isdigit( inpt[i] ) )
                                break;
                std::string_view text = inpt.substr( 0, i );
                inpt                  = inpt.substr( i );
                return { .token = token_type::NUM, .text = text };
        }
        if ( std::isalpha( c ) ) {
                std::size_t i = 1;
                for ( ; i < inpt.size(); i++ )
                        if ( !std::isalpha( inpt[i] ) )
                                break;
                std::string_view text = inpt.substr( 0, i );
                inpt                  = inpt.substr( i );
                return { .token = token_type::ID, .text = text };
        }
        switch ( c ) {
        case ' ':
        case '\t':
                inpt = inpt.substr( 1 );
                return lex( inpt );
        case '\n': {
                std::string_view text = inpt.substr( 0, 1 );
                inpt                  = inpt.substr( 1 );
                return { .token = token_type::LINE, .text = text };
        }
        case '+':
        case '-':
        case '/':
        case '*':
        case '=': {
                std::string_view text = inpt.substr( 0, 1 );
                inpt                  = inpt.substr( 1 );
                return { .token = token_type::OP, .text = text };
        }
        }
        abort( "invalid char: ", c );
}

std::vector< lex_tok > full_lex( std::string_view inpt )
{
        std::vector< lex_tok > res;
        for ( ;; ) {

                lex_tok tok = lex( inpt );
                res.push_back( tok );
                if ( tok.token == token_type::END )
                        break;
        }
        return res;
}
}  // namespace lexer

namespace parser
{

using lexer::token_type;

const std::vector< std::set< char > > bin_prio = {
    { '+', '-' },
    { '*', '/' },
};

std::optional< std::string_view > eat( std::span< lexer::lex_tok >& toks, token_type tt )
{
        if ( toks.empty() )
                return {};
        auto& tok = toks[0];
        if ( tok.token != tt )
                return {};
        toks = toks.subspan( 1 );
        return tok.text;
}

uvref< expr > parse_simple_expr( std::span< lexer::lex_tok >& toks )
{
        if ( auto st = eat( toks, token_type::ID ) )
                return uwrap( id{ .text = *st } );
        else if ( auto st = eat( toks, token_type::NUM ) ) {
                value_type val;
                std::from_chars( st->data(), st->data() + st->size(), val );
                return uwrap( constant{ .val = val } );
        }

        if ( !toks.empty() )
                abort( "failed to parse simple expr: ", toks[0].text );
        else
                abort( "missing expression" );
}

uvref< expr > parse_expr( std::span< lexer::lex_tok >& toks, std::size_t prio = 0 )
{
        uvref< expr > lh = parse_simple_expr( toks );

        while ( !toks.empty() && toks[0].token == token_type::OP ) {
                char        c = ( toks[0].text )[0];
                std::size_t i = prio;
                for ( ; i < bin_prio.size(); i++ )
                        if ( bin_prio[i].contains( c ) )
                                break;
                if ( i == bin_prio.size() )
                        return lh;
                eat( toks, token_type::OP );
                lh = uwrap( bin_arithm_op{
                    .lh = std::move( lh ),
                    .op = c,
                    .rh = parse_expr( toks, i + 1 ),
                } );
        }
        return lh;
}

std::optional< definition > parse_def( std::span< lexer::lex_tok >& toks )
{
        if ( eat( toks, token_type::LINE ) || eat( toks, token_type::END ) )
                return {};

        std::string_view id;
        if ( auto iid = eat( toks, token_type::ID ) )
                id = *iid;
        else
                abort( "ID expected, got: ", toks[0].text );

        if ( auto op = eat( toks, token_type::OP ) )
                check( *op == "=", "= operator expected, got: ", *op );
        else
                abort( "= operator expected" );

        uvref< expr > e = parse_expr( toks );

        if ( !eat( toks, token_type::LINE ) && !eat( toks, token_type::END ) )
                abort( "expected line end, got: ", toks[0].text );

        return definition{
            .name = id,
            .e    = std::move( e ),
        };
}

std::vector< definition > parse_defs( std::span< lexer::lex_tok > toks )
{
        std::vector< definition > res;
        while ( !toks.empty() )
                if ( auto d = parse_def( toks ) )
                        res.emplace_back( std::move( *d ) );
        return res;
}

}  // namespace parser
