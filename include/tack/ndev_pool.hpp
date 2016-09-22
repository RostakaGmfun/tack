#ifndef NDEV_POOL_HPP
#define NDEV_POOL_HPP

#include <cstddef>
#include <vector>
#include <thread>
#include <memory>

#include "tack/network_device.hpp"

namespace tack
{

class arp_cache;
using arp_cache_ptr = std::shared_ptr<arp_cache>;

class ndev_pool
{
public:
    ndev_pool(const network_device &ndev);
    ~ndev_pool();

    bool is_stop() const;

private:
    std::vector<std::thread> threads_;
    bool stop_;
    arp_cache_ptr arp_cache_;
};

} // namespace tack

#endif // NDEV_POOL_HPP
