#ifndef ARP_HPP
#define ARP_HPP

#include <cstdint>

#include "tack/arp_cache.hpp"

namespace tack
{

class arp
{
public:
    arp(arp_cache_ptr arp_cache);
    ~arp() = default;

    void process_packet(const uint8_t *payload);

private:
    arp_cache_ptr arp_cache_;
};

}

#endif // ARP_HPP
