
/// MIT License
///
/// Copyright (c) 2025-2026 koniarik
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#include "vari/uvptr.h"
#include "vari/uvref.h"
#include "vari/vptr.h"
#include "vari/vref.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string_view>

enum class mode
{
        gen,
        run,
        eval
};
// gdb stops here once per check and prints `v`. Each instantiation must stay a separate,
// non-inlined function, so that `v` has its own type and is addressable at the breakpoint in
// optimized builds too. GCC folds identical functions unless they are `noipa`.
#if defined( __clang__ )
#define VARI_GDB_PROBE [[gnu::noinline]]
#else
#define VARI_GDB_PROBE [[gnu::noipa]]
#endif

template < typename T >
VARI_GDB_PROBE void gdb_probe( T const& v )
{
        asm volatile( "" : : "r"( &v ) : "memory" );
}

template < typename T >
void check(
    [[maybe_unused]] mode             m,
    T const&                          var,
    [[maybe_unused]] std::string_view expected,
    [[maybe_unused]] std::ostream&    out )
{
        assert( m == mode::run );
        gdb_probe( var );
}

template < typename T >
void check(
    [[maybe_unused]] mode     m,
    [[maybe_unused]] T const& var,
    std::string_view          expected,
    std::istream&             in,
    std::source_location      sl = std::source_location::current() )
{
        assert( m == mode::eval );
        while ( in && in.get() != '$' ) {
        }

        std::string line;
        std::getline( in, line );

        if ( line.ends_with( expected ) )
                return;
        std::cerr << "Failed match:" << "\n";
        std::cerr << "Expected: " << expected << "\n";
        std::cerr << "     Got: " << line << "\n";
        std::cerr << "  Source: " << sl.file_name() << ":" << sl.line() << "\n";
        std::exit( 2 );
}

struct expr
{
        vari::uvptr< expr > e;
};

expr gen_expr( std::size_t n )
{
        if ( n == 1 )
                return expr{};
        return expr{ .e = vari::uwrap( gen_expr( n - 1 ) ).vptr() };
}

void run_tests( mode m, auto& st )
{
        int         i = 42;
        std::string s = "wololo";

        vari::vptr< int > v1;
        check( m, v1, "vari::vptr = {0x0}", st );
        v1 = &i;
        check( m, v1, "vari::vptr = {42}", st );

        vari::vptr< int, std::string > v2;
        check( m, v2, "vari::vptr = {0x0}", st );
        v2 = &s;
        check( m, v2, "vari::vptr = {\"wololo\"}", st );

        vari::vref< int > r1 = i;
        check( m, r1, "vari::vref = {42}", st );

        vari::vref< int, std::string > r2 = s;
        check( m, r2, "vari::vref = {\"wololo\"}", st );

        vari::uvptr< int > uv1;
        check( m, uv1, "vari::uvptr = {0x0}", st );
        uv1 = vari::uwrap( i ).vptr();
        check( m, uv1, "vari::uvptr = {42}", st );

        vari::uvptr< int, std::string > uv2;
        check( m, uv2, "vari::uvptr = {0x0}", st );
        uv2 = vari::uwrap( s ).vptr();
        check( m, uv2, "vari::uvptr = {\"wololo\"}", st );

        vari::uvref< int > ur1 = vari::uwrap( i );
        check( m, ur1, "vari::uvref = {42}", st );

        vari::uvref< int, std::string > ur2 = vari::uwrap( s );
        check( m, ur2, "vari::uvref = {\"wololo\"}", st );

        vari::uvref< expr > chain = vari::uwrap( gen_expr( 100 ) );
        check( m, chain, "vari::uvref = {{e = vari::uvptr = {...}}}", st );
}

int main( int argc, char* argv[] )
{

        assert( argc >= 2 );
        std::string_view mode = argv[1];
        if ( mode == "gen" ) {
                assert( argc >= 4 );
                std::string_view pprinter = argv[2];
                std::ofstream    out{ argv[3] };
                out << "source " << pprinter << "\n";
                out << "set logging overwrite" << "\n";
                out << "set logging on" << "\n";
                out << "set print max-depth 2" << "\n";
                out << "break gdb_probe" << "\n";
                out << "commands" << "\n";
                out << "p *&v" << "\n";
                out << "c" << "\n";
                out << "end" << "\n";
                out << "run run" << std::endl;
        } else if ( mode == "run" ) {
                std::ostringstream ss;
                run_tests( mode::run, ss );
        } else if ( mode == "eval" ) {
                assert( argc >= 3 );
                std::ifstream inpt{ argv[2] };
                run_tests( mode::eval, inpt );
        } else {
                std::cout << "invalid mode" << std::endl;
                return 1;
        }
}
