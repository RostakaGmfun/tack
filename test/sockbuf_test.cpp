#include <catch.hpp>
#include <tack/sockbuf.hpp>

#include <array>

// mock protocol headers
using proto_0 = std::array<uint8_t, 20>;
using proto_1 = std::array<uint8_t, 40>;
using payload_t = std::array<uint8_t, 100>;

static proto_0 p0;
static proto_1 p1;
static payload_t payload;

struct mock_reader
{
    int64_t read(uint8_t *dest, size_t size)
    {
        uint8_t *start = dest;
        if (size < p0.size() + p1.size() + payload.size()) {
            return -1;
        }

        std::copy(p0.begin(), p0.end(), dest);
        dest += p0.size();
        std::copy(p1.begin(), p1.end(), dest);
        dest += p1.size();

        std::copy(payload.begin(), payload.end(), dest);
        dest += payload.size();
        return dest - start;
    }
};

TEST_CASE( "sockbuf_test", "[sockbuf]") {
    SECTION("Constructor test") {
        tack::sockbuf skb(1500, p0.size() + p1.size());
        REQUIRE(skb.raw_size() == 0);
        REQUIRE(skb.payload_size() == 0);
        REQUIRE(skb.num_headers() == 0);
        REQUIRE(skb.get_header<proto_0>() == nullptr);

        REQUIRE_THROWS(tack::sockbuf skb2(100, 200));
    }

    SECTION("Read test") {
        tack::sockbuf skb(1500, p0.size() + p1.size());
        mock_reader reader;
        size_t real_size = p0.size() + p1.size() + payload.size();
        REQUIRE(skb.read(reader) == real_size);
        REQUIRE(skb.raw_size() == real_size);
        REQUIRE(skb.payload_size() == real_size);
        REQUIRE(skb.num_headers() == 0);
        REQUIRE(skb.payload() == skb.raw());
    }
}
