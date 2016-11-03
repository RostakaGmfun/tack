#include "tack/arp.hpp"
#include "tack/arp_cache.hpp"

namespace tack {

arp::arp(arp_cache_ptr cache): arp_cache_(cache)
{}

void process_pack(const std::vector<uint8_t> & payload)
{
    static_cast<void>(payload);
}

}

