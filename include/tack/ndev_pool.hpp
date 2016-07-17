#ifndef NDEV_POOL_HPP
#define NDEV_POOL_HPP

#include <cstddef>
#include <vector>
#include <thread>

#include "tack/network_device.hpp"

namespace tack
{

class ndev_pool
{
public:
    ndev_pool(const network_device &ndev);
    ~ndev_pool();

    bool is_stop() const;

private:
    std::vector<std::thread> threads_;
    bool stop_;
};

} // namespace tack

#endif // NDEV_POOL_HPP
