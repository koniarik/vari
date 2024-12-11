
#include "vari/uvptr.h"
#include "vari/uvref.h"

#include <doctest/doctest.h>
#include <memory_resource>

namespace vari
{

template < std::size_t N >
struct pmr_del
{
        vref< std::pmr::memory_resource > mem_res;

        pmr_del( std::pmr::memory_resource& mem )
          : mem_res( mem )
        {
        }

        template < std::size_t M >
        pmr_del( pmr_del< M > const& other ) noexcept
          : mem_res( other.mem_res )
        {
        }

        template < typename T >
        constexpr void operator()( T* item ) const
        {
                std::destroy_at( item );
                mem_res->deallocate( item, sizeof( T ), alignof( T ) );
        }

        friend constexpr auto operator<=>( pmr_del const&, pmr_del const& ) = default;
};

struct throwing_del
{
        throwing_del( throwing_del const& )            = default;
        throwing_del( throwing_del&& )                 = default;
        throwing_del& operator=( throwing_del const& ) = default;
        throwing_del& operator=( throwing_del&& )      = default;

        constexpr void operator()( auto* item ) const
        {
                delete item;
        }

        friend constexpr auto operator<=>( throwing_del const&, throwing_del const& ) = default;
};

namespace deleter_api_impl
{

        template < typename T, typename D >
        concept get_deleter = requires( T v ) {
                {
                        v.get_deleter()
                } -> std::convertible_to< D& >;
                noexcept( v.get_deleter() );
                {
                        std::as_const( v ).get_deleter()
                } -> std::convertible_to< D const& >;
                noexcept( std::as_const( v ).get_deleter() );
        };

        template < template < typename D, typename... Ts > typename T >
        concept value_del = get_deleter< T< def_del, int >, def_del > && requires(
                                                                             T< def_del, int > v,
                                                                             def_del           d ) {
                requires std::constructible_from< T< def_del, int >, int&, def_del const& >;
                requires std::constructible_from< T< def_del, int >, int&, def_del&& >;
                requires !std::constructible_from< T< def_del, int >, int&, throwing_del const& >;
                requires !std::constructible_from< T< def_del, int >, int&, throwing_del&& >;
        };

        template < template < typename D, typename... Ts > typename T >
        concept ref_del = get_deleter< T< def_del&, int >, def_del > &&
                          requires( T< def_del&, int > v, def_del d ) {
                                  std::constructible_from< T< def_del&, int >, int*, def_del& >;
                                  !std::constructible_from< T< def_del&, int >, int*, def_del&& >;
                          };

}  // namespace deleter_api_impl

template < template < typename D, typename... Ts > typename T >
concept deleter_api = deleter_api_impl::value_del< T > && deleter_api_impl::ref_del< T >;

static_assert( deleter_api< _uvref > );
// XXX: fix
// static_assert( deleter_api< _uvptr > );

TEST_CASE( "basic allocator construction" )
{
        std::pmr::polymorphic_allocator< std::string > pa;

        auto test_visit_f = [&]( auto& v ) {
                v.visit( [&]< typename T >( T&& ) {
                        if constexpr ( !std::same_as< T, std::string& > )
                                FAIL( "bad branch" );
                } );
        };

        SUBCASE( "value uvref" )
        {
                using V = _uvref< pmr_del< 0 >, std::string, int, float >;
                V v{ *pa.new_object< std::string >(), pmr_del< 0 >{ *pa.resource() } };
                test_visit_f( v );
        }
        SUBCASE( "lvalue uvref" )
        {
                using V = _uvref< pmr_del< 0 >&, std::string, int, float >;
                pmr_del< 0 > d{ *pa.resource() };
                V            v{ *pa.new_object< std::string >(), d };
                test_visit_f( v );
        }
        SUBCASE( "value uvptr" )
        {
                using V = _uvptr< pmr_del< 0 >, std::string, int, float >;
                V v{ pa.new_object< std::string >(), pmr_del< 0 >{ *pa.resource() } };
                test_visit_f( v );
        }
        SUBCASE( "lvalue uvptr" )
        {
                using V = _uvptr< pmr_del< 0 >&, std::string, int, float >;
                pmr_del< 0 > d{ *pa.resource() };
                V            v{ pa.new_object< std::string >(), d };
                test_visit_f( v );
        }
}

TEST_CASE( "basic allocator swap" )
{
        std::pmr::unsynchronized_pool_resource         upr1, upr2;
        std::pmr::polymorphic_allocator< std::string > pa1{ &upr1 }, pa2{ &upr2 };

        auto test_swap_f = [&]( auto& v1, auto& v2 ) {
                CHECK_EQ( v1.get_deleter().mem_res.get(), pa1.resource() );
                CHECK_EQ( v2.get_deleter().mem_res.get(), pa2.resource() );
                swap( v1, v2 );
                CHECK_EQ( v1.get_deleter().mem_res.get(), pa2.resource() );
                CHECK_EQ( v2.get_deleter().mem_res.get(), pa1.resource() );
                CHECK_NE( pa1.resource(), pa2.resource() );
        };

        SUBCASE( "value uvref" )
        {
                using V = _uvref< pmr_del< 0 >, std::string, int, float >;
                V v1{ *pa1.new_object< std::string >(), pmr_del< 0 >{ *pa1.resource() } };
                V v2{ *pa2.new_object< std::string >(), pmr_del< 0 >{ *pa2.resource() } };

                test_swap_f( v1, v2 );
        }
        SUBCASE( "lvalue uvref" )
        {
                using V = _uvref< pmr_del< 0 >&, std::string, int, float >;
                pmr_del< 0 > d1{ *pa1.resource() }, d2{ *pa2.resource() };

                V v1{ *pa1.new_object< std::string >(), d1 };
                V v2{ *pa2.new_object< std::string >(), d2 };

                test_swap_f( v1, v2 );
        }
        SUBCASE( "value uvptr" )
        {
                using V = _uvptr< pmr_del< 0 >, std::string, int, float >;
                V v1{ pa1.new_object< std::string >(), pmr_del< 0 >{ *pa1.resource() } };
                V v2{ pa2.new_object< std::string >(), pmr_del< 0 >{ *pa2.resource() } };

                test_swap_f( v1, v2 );
        }
        SUBCASE( "lvalue uvptr" )
        {
                using V = _uvptr< pmr_del< 0 >&, std::string, int, float >;
                pmr_del< 0 > d1{ *pa1.resource() }, d2{ *pa2.resource() };

                V v1{ pa1.new_object< std::string >(), d1 };
                V v2{ pa2.new_object< std::string >(), d2 };

                test_swap_f( v1, v2 );
        }
}

TEST_CASE( "basic allocator assign" )
{
        std::pmr::unsynchronized_pool_resource         upr1, upr2;
        std::pmr::polymorphic_allocator< std::string > pa1{ &upr1 }, pa2{ &upr2 };

        auto test_assign_f = [&]( auto& v1, auto& v2 ) {
                CHECK_EQ( v1.get_deleter().mem_res.get(), pa1.resource() );
                CHECK_EQ( v2.get_deleter().mem_res.get(), pa2.resource() );
                v1 = std::move( v2 );
                CHECK_EQ( v1.get_deleter().mem_res.get(), pa2.resource() );
        };

        SUBCASE( "value uvref" )
        {
                using V = _uvref< pmr_del< 0 >, std::string, int, float >;
                V v1{ *pa1.new_object< std::string >(), pmr_del< 0 >{ *pa1.resource() } };
                V v2{ *pa2.new_object< std::string >(), pmr_del< 0 >{ *pa2.resource() } };

                test_assign_f( v1, v2 );
        }
        SUBCASE( "value uvref heterogenous" )
        {
                using V1 = _uvref< pmr_del< 0 >, std::string, int, float >;
                using V2 = _uvref< pmr_del< 1 >, std::string, int, float >;
                V1 v1{ *pa1.new_object< std::string >(), pmr_del< 0 >{ *pa1.resource() } };
                V2 v2{ *pa2.new_object< std::string >(), pmr_del< 1 >{ *pa2.resource() } };

                test_assign_f( v1, v2 );
        }
        SUBCASE( "lvalue uvref" )
        {
                using V = _uvref< pmr_del< 0 >&, std::string, int, float >;
                pmr_del< 0 > d1{ *pa1.resource() }, d2{ *pa2.resource() };

                V v1{ *pa1.new_object< std::string >(), d1 };
                V v2{ *pa2.new_object< std::string >(), d2 };

                test_assign_f( v1, v2 );
        }
        SUBCASE( "value uvptr" )
        {
                using V = _uvptr< pmr_del< 0 >, std::string, int, float >;
                V v1{ pa1.new_object< std::string >(), pmr_del< 0 >{ *pa1.resource() } };
                V v2{ pa2.new_object< std::string >(), pmr_del< 0 >{ *pa2.resource() } };

                test_assign_f( v1, v2 );
        }
        SUBCASE( "value uvptr heterogenous" )
        {
                using V1 = _uvptr< pmr_del< 0 >, std::string, int, float >;
                using V2 = _uvptr< pmr_del< 1 >, std::string, int, float >;
                V1 v1{ pa1.new_object< std::string >(), pmr_del< 0 >{ *pa1.resource() } };
                V2 v2{ pa2.new_object< std::string >(), pmr_del< 1 >{ *pa2.resource() } };

                test_assign_f( v1, v2 );
        }
        SUBCASE( "lvalue uvptr" )
        {
                using V = _uvptr< pmr_del< 0 >&, std::string, int, float >;
                pmr_del< 0 > d1{ *pa1.resource() }, d2{ *pa2.resource() };

                V v1{ pa1.new_object< std::string >(), d1 };
                V v2{ pa2.new_object< std::string >(), d2 };

                test_assign_f( v1, v2 );
        }
}

}  // namespace vari
