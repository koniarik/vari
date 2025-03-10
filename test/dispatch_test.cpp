
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
#include "vari/dispatch.h"

#include <doctest/doctest.h>

namespace vari
{

template < std::size_t I >
struct test_tag
{
        static constexpr auto value = I;
};

TEST_CASE( "dispatch tag" )
{
        static constexpr std::size_t N = 42;

        for ( std::size_t i = 0; i < N; i++ ) {
                std::size_t k = dispatch< N >(
                    i,
                    [&]< std::size_t j >() {
                            return test_tag< j >{};
                    },
                    [&]( auto tag ) {
                            return tag.value;
                    } );
                CHECK_EQ( k, i );

                k = dispatch< N >(
                    i,
                    [&]< std::size_t j >() {
                            return test_tag< j * j >{};
                    },
                    [&]( auto tag ) {
                            return tag.value;
                    } );
                CHECK_EQ( k, i * i );
        }
}
TEST_CASE( "dispatch multiple fun" )
{
        static constexpr std::size_t N = 3;

        for ( std::size_t i = 0; i < N; i++ ) {
                std::size_t k = dispatch< N >(
                    i,
                    [&]< std::size_t j >() {
                            return test_tag< j >{};
                    },
                    [&]( test_tag< 0 > ) {
                            return 0;
                    },
                    [&]( test_tag< 1 > ) {
                            return 1;
                    },
                    [&]( test_tag< 2 > ) {
                            return 2;
                    } );
                CHECK_EQ( k, i );
        }
}

}  // namespace vari
