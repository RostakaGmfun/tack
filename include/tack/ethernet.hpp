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
    ipv4 = 0x800,
    ipv6 = 0x86DD,
    arp = 0x806
};

struct ethernet_header {
    hw_address dest;
    hw_address src;
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
