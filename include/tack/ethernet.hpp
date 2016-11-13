#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <cstdint>
#include <cstddef>
#include <memory>

#include "tack/sockbuf.hpp"
#include "tack/addresses.hpp"

namespace tack
{

class arp;
using arp_ptr = std::shared_ptr<arp>;

class arp_cache;
using arp_cache_ptr = std::shared_ptr<arp_cache>;

class ndev_worker;

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
    ethernet(ndev_worker *worker);
    ~ethernet() = default;

    void process_packet(sockbuf &sockbuf);
    void transfer_packet(sockbuf &sockbuf, const hw_address &dest);

private:
    ndev_worker *worker_;
};

} // namespace tack

#endif // ETHERNET_HPP
