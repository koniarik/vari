
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
