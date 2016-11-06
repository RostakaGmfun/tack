#include <catch.hpp>
#include <tack/sockbuf.hpp>

#include <array>

// mock protocol headers
//
template <size_t N>
struct mock_proto {
    std::array<uint8_t, N> data;
};

using proto_0 = mock_proto<20>;
using proto_1 = mock_proto<40>;
using payload_t = mock_proto<100>;

static proto_0 p0;
static proto_1 p1;
static payload_t payload;

struct mock_reader
{
    int64_t read(uint8_t *dest, size_t size)
    {
        uint8_t *start = dest;
        if (size < sizeof(p0) + sizeof(p1) + sizeof(payload)) {
            return -1;
        }

        std::copy(&p0, &p0+1, reinterpret_cast<proto_0*>(dest));
        dest += sizeof(p0);
        std::copy(&p1, &p1+1, reinterpret_cast<proto_1*>(dest));
        dest += sizeof(p1);

        std::copy(&payload, &payload+1, reinterpret_cast<payload_t*>(dest));
        dest += sizeof(payload);
        return dest - start;
    }
};

TEST_CASE( "sockbuf_test", "[sockbuf]")
{
    SECTION("Constructor test")
    {
        tack::sockbuf skb(1500, sizeof(p0) + sizeof(p1));
        REQUIRE(skb.raw_size() == 0);
        REQUIRE(skb.payload_size() == 0);
        REQUIRE(skb.num_headers() == 0);
        REQUIRE(skb.get_header<proto_0>() == nullptr);

        REQUIRE_THROWS(tack::sockbuf skb2(100, 200));
    }

    SECTION("Read test")
    {
        tack::sockbuf skb(1500, sizeof(p0) + sizeof(p1));
        mock_reader reader;
        size_t real_size = sizeof(p0) + sizeof(p1) + sizeof(payload);
        REQUIRE(skb.read(reader) == real_size);
        REQUIRE(skb.raw_size() == real_size);
        REQUIRE(skb.payload_size() == real_size);
        REQUIRE(skb.num_headers() == 0);
        REQUIRE(skb.payload() == skb.raw());
    }

    SECTION("Parsing test")
    {
        tack::sockbuf skb(1500, sizeof(p0) + sizeof(p1));
        mock_reader reader;
        size_t real_size = sizeof(p0) + sizeof(p1) + sizeof(payload);
        REQUIRE(skb.read(reader) == real_size);
        skb.push_header<proto_0>();
        REQUIRE(skb.payload() == skb.raw() + sizeof(proto_0));
        REQUIRE(reinterpret_cast<uint8_t*>(skb.get_header<proto_0>()) == skb.raw());
        REQUIRE(skb.num_headers() == 1);
    }
}
