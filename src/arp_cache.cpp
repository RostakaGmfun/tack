#include "tack/arp_cache.hpp"

#include <algorithm>

using namespace tack;

arp_cache::arp_cache(const uint8_t (&self)[6], size_t cache_size):
    cache_size_(cache_size)
{
    std::copy(self, self+6, &self_[0]);
    cache_.clear();
    cache_.reserve(cache_size_);
}

arp_result<ipv4_address> arp_cache::operator[](const hw_address &addr)
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto entry = std::find_if(cache_.begin(), cache_.end(), [&addr] (const arp_entry& e)
            {
                return e.hw_addr == addr;
            });

    if (entry != cache_.end()) {
        return (*entry).ip_addr;
    }
    return arp_result<ipv4_address>();
}

arp_result<hw_address> arp_cache::operator[](const ipv4_address &addr)
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto entry = std::find_if(cache_.begin(), cache_.end(), [&addr] (const arp_entry& e)
            {
                return e.ip_addr == addr;
            });

    if (entry != cache_.end()) {
        return (*entry).hw_addr;
    }

    return arp_result<hw_address>();
}

void arp_cache::update(const hw_address &hwaddr, const ipv4_address &ipaddr)
{
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto entry = std::find_if(cache_.begin(), cache_.end(), [&hwaddr] (const arp_entry& e)
            {
                return e.hw_addr == hwaddr;
            });

    if (entry != cache_.end()) {
        (*entry).ip_addr = ipaddr;
    }

    if (cache_.size() == cache_size_) {
        cache_.erase(cache_.begin());
    }
    cache_.push_back(arp_entry(hwaddr, ipaddr));
}

