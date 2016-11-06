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

struct mock_rw
{
    int64_t read(uint8_t *dest, size_t size)
    {
        uint8_t *start = dest;
        if (size < sizeof(p0) + sizeof(p1) + sizeof(payload) || dest == nullptr) {
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

    int64_t write(uint8_t *src, size_t size)
    {
        if (src == nullptr || size == 0) {
            return -1;
        }
        return size;
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
        mock_rw reader;
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
        mock_rw reader;
        size_t real_size = sizeof(p0) + sizeof(p1) + sizeof(payload);
        p0.data[0] = 42;
        REQUIRE(skb.read(reader) == real_size);
        REQUIRE(skb.raw() == skb.payload());

        REQUIRE(skb.push_header<proto_0>() == true);
        REQUIRE(skb.get_header<proto_0>()->data[0] == 42);
        REQUIRE(skb.push_header<proto_0>() == false);
        REQUIRE(skb.payload() == skb.raw() + sizeof(proto_0));
        REQUIRE(reinterpret_cast<uint8_t*>(skb.get_header<proto_0>()) == skb.raw());
        REQUIRE(skb.num_headers() == 1);

        REQUIRE(skb.push_header<proto_1>() == true);
        REQUIRE(skb.payload() == skb.raw() + sizeof(proto_0) + sizeof(proto_1));
        REQUIRE(reinterpret_cast<uint8_t*>(skb.get_header<proto_1>()) == skb.raw() + sizeof(proto_0));
        REQUIRE(skb.num_headers() == 2);
    }

    SECTION("Serialization test")
    {
        tack::sockbuf skb(1500, sizeof(p0) + sizeof(p1));
        REQUIRE(skb.add_payload(reinterpret_cast<uint8_t*>(&payload),
                    sizeof(payload)) == true);
        REQUIRE(skb.raw_size() == sizeof(payload));
        REQUIRE(skb.payload_size() == sizeof(payload));
        REQUIRE(skb.payload() == skb.raw());

        p0.data[0] = 42;
        REQUIRE(skb.wrap(&p0) == true);
        REQUIRE(reinterpret_cast<uint8_t*>(skb.get_header<proto_0>()) == skb.raw());
        REQUIRE(skb.num_headers() == 1);
        REQUIRE(skb.get_header<proto_0>()->data == p0.data);

        p1.data[0] = 13;
        REQUIRE(skb.wrap(&p1) == true);
        REQUIRE(reinterpret_cast<uint8_t*>(skb.get_header<proto_1>()) == skb.raw());
        REQUIRE(reinterpret_cast<uint8_t*>(skb.get_header<proto_0>()) == skb.raw() + sizeof(proto_1));
        REQUIRE(skb.num_headers() == 2);
        REQUIRE(skb.get_header<proto_1>()->data == p1.data);

        REQUIRE(skb.raw_size() == sizeof(proto_0) + sizeof(proto_1) + sizeof(payload));
    }
}
