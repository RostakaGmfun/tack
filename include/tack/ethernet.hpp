#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <cstdint>
#include <cstddef>
#include <memory>

#include  "tack/sockbuf.hpp"

namespace tack
{

class arp;
using arp_ptr = std::shared_ptr<arp>;

class arp_cache;
using arp_cache_ptr = std::shared_ptr<arp_cache>;

enum class ethertype: uint16_t {
    IPv4 = 0x800,
    IPv6 = 0x86DD,
    ARP = 0x806
};

struct ethernet_header {
    uint8_t dest[6];
    uint8_t src[6];
    ethertype type;
} __attribute__((packed));

class ethernet {
public:
    ethernet(size_t mtu, const arp_cache_ptr &arp_cache);
    ~ethernet() = default;

    void process_packet(sockbuf &sockbuf);

private:
    size_t mtu_;
    arp_ptr arp_;
};

} // namespace tack

#endif // ETHERNET_HPP
