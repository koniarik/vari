
/// MIT License
///
/// Copyright (c) 2025 koniarik
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
void check_impl(
    mode                 m,
    std::string_view     var,
    std::string_view     expected,
    std::ostream&        out,
    std::source_location sl = std::source_location::current() )
{
        if ( m != mode::gen )
                return;
        out << "break " << sl.file_name() << ":" << sl.line() << "\n";
        out << "commands" << "\n";
        out << "p " << var << "\n";
        out << "c " << "\n";
        out << "end" << "\n";
}
void check_impl(
    mode                 m,
    std::string_view     var,
    std::string_view     expected,
    std::istream&        in,
    std::source_location sl = std::source_location::current() )
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

#define CHECK( m, var, expected, out ) check_impl( m, #var, expected, out );

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
        CHECK( m, v1, "vari::vptr = {0x0}", st );
        v1 = &i;
        CHECK( m, v1, "vari::vptr = {42}", st );

        vari::vptr< int, std::string > v2;
        CHECK( m, v2, "vari::vptr = {0x0}", st );
        v2 = &s;
        CHECK( m, v2, "vari::vptr = {\"wololo\"}", st );

        vari::vref< int > r1 = i;
        CHECK( m, r1, "vari::vref = {42}", st );

        vari::vref< int, std::string > r2 = s;
        CHECK( m, r2, "vari::vref = {\"wololo\"}", st );

        vari::uvptr< int > uv1;
        CHECK( m, uv1, "vari::uvptr = {0x0}", st );
        uv1 = vari::uwrap( i ).vptr();
        CHECK( m, uv1, "vari::uvptr = {42}", st );

        vari::uvptr< int, std::string > uv2;
        CHECK( m, uv2, "vari::uvptr = {0x0}", st );
        uv2 = vari::uwrap( s ).vptr();
        CHECK( m, uv2, "vari::uvptr = {\"wololo\"}", st );

        vari::uvref< int > ur1 = vari::uwrap( i );
        CHECK( m, ur1, "vari::uvref = {42}", st );

        vari::uvref< int, std::string > ur2 = vari::uwrap( s );
        CHECK( m, ur2, "vari::uvref = {\"wololo\"}", st );

        vari::uvref< expr > chain = vari::uwrap( gen_expr( 100 ) );
        CHECK( m, chain, "vari::uvref = {{e = vari::uvptr = {...}}}", st );
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
                run_tests( mode::gen, out );
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
