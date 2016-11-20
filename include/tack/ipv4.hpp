#ifndef IPV4_HPP
#define IPV4_HPP

#include <cstdint>

#include "tack/addresses.hpp"
#include "tack/sockbuf.hpp"

namespace tack
{

class ndev_worker;

enum class ipv4_protocol: uint8_t
{
    icmp = 0x01,
    tcp = 0x06,
    udp = 0x11
};

struct ipv4_header
{
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t frag_flags_offset;
    uint8_t ttl;
    ipv4_protocol protocol;
    uint16_t checksum;
    ipv4_address src;
    ipv4_address dst;
} __attribute__((packed));

class ipv4
{
public:
    ipv4(ndev_worker *worker);
    ~ipv4() = default;

    void process_packet(sockbuf &skb);
    void transfer_packet(sockbuf &&skb);

private:
    ndev_worker *worker_;
};

}

#endif // IPV4_HPP
