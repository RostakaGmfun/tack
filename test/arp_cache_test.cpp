#include <catch.hpp>
#include <tack/arp_cache.hpp>

TEST_CASE( "arp_cache test", "[arp_cache]") {
    static constexpr tack::hw_address test_hw = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa };
    static constexpr tack::ipv4_address test_ip = { 0x12, 0x34, 0x56, 0x67 };

    SECTION("Basic test") {
        tack::arp_cache cache({0});
        cache.update(test_hw, test_ip);
        auto hw = cache[test_ip];
        REQUIRE(hw.is_null == false);
        REQUIRE(hw.value == test_hw);
        auto ip = cache[test_hw];
        REQUIRE(ip.is_null == false);
        REQUIRE(ip.value == test_ip);
    }

    SECTION("Cache rotation test") {
        const size_t cache_size = 32;
        tack::arp_cache cache({0}, cache_size);
        cache.update(test_hw, test_ip);

        for (size_t i = 0;i<cache_size;i++) {
            uint8_t v = i;
            cache.update({ v, v, v, v, v, v}, {v, v, v, v});
        }
        REQUIRE(cache[test_ip].is_null == true);
        REQUIRE(cache[test_hw].is_null == true);
    }
}
