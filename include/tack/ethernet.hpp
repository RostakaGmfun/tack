#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <cstdint>
#include <cstddef>

#include "arp.hpp"

namespace tack
{

enum ethertype: uint16_t {
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
    ethernet(size_t mtu = 1500);
    ~ethernet() = default;

    void process_packet(const uint8_t *payload);

private:
    size_t mtu_;
//    arp arp_;
};

} // namespace tack

#endif // ETHERNET_HPP
