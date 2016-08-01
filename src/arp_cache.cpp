#include "tack/arp_cache.hpp"

#include <algorithm>

using namespace tack;

arp_cache::arp_cache(size_t cache_size):
    cache_size_(cache_size)
{
    cache_.clear();
    cache_.reserve(cache_size_);
}

bool arp_cache::to_hwaddr(const ipv4_address &from, hw_address &to)
{
    // TODO: Add locking
    auto entry = std::find_if(cache_.begin(), cache_.end(), [&from] (const arp_entry& e)
            {
                const ipv4_address &ipaddr = e.ip_addr;
                return ipaddr == from;
            });

    if (entry != cache_.end()) {
        to = (*entry).hw_addr;
        return true;
    }

    return false;
}

bool arp_cache::from_hwaddr(const hw_address &from, ipv4_address &to)
{
    // TODO: Add locking
    auto entry = std::find_if(cache_.begin(), cache_.end(), [&from] (const arp_entry& e)
            {
                return e.hw_addr == from;
            });

    if (entry != cache_.end()) {
        to = (*entry).ip_addr;
        return true;
    }

    return false;
}

bool arp_cache::update(const hw_address &hwaddr, const ipv4_address &ipaddr)
{
    // TODO: Add locking
    auto entry = std::find_if(cache_.begin(), cache_.end(), [&hwaddr] (const arp_entry& e)
            {
                return e.hw_addr == hwaddr;
            });

    //TODO
    return false;
}

