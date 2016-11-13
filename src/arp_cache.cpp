#include "tack/arp_cache.hpp"

#include <algorithm>
#include <iostream>

using namespace tack;

arp_cache::arp_cache(size_t size):
    cache_size_(size)
{
    cache_.clear();
    cache_.reserve(cache_size_);
}

arp_result<ipv4_address> arp_cache::operator[](const hw_address &addr)
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto entry = std::find_if(cache_.begin(), cache_.end(), [&addr] (const arp_entry& e)
            {
                return std::get<hw_address>(e) == addr;
            });

    if (entry != cache_.end()) {
        return std::get<ipv4_address>(*entry);
    }
    return arp_result<ipv4_address>();
}

arp_result<hw_address> arp_cache::operator[](const ipv4_address &addr)
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto entry = std::find_if(cache_.begin(), cache_.end(), [&addr] (const arp_entry& e)
            {
                return std::get<ipv4_address>(e) == addr;
            });

    if (entry != cache_.end()) {
        return std::get<hw_address>(*entry);
    }

    return arp_result<hw_address>();
}

void arp_cache::update(const hw_address &hwaddr, const ipv4_address &ipaddr)
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto entry = std::find_if(cache_.begin(), cache_.end(), [&hwaddr] (const arp_entry& e)
            {
                return std::get<hw_address>(e) == hwaddr;
            });

    if (entry != cache_.end()) {
        (*entry).second = ipaddr;
        std::cout << "ARP cache entry updated\n";
        return;
    }

    if (cache_.size() == cache_size_) {
        cache_.erase(cache_.begin());
    }
    cache_.push_back({hwaddr, ipaddr});
    std::cout << "Added new arp cache entry\n";
}

