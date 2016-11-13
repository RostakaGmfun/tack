#ifndef NDEV_WORKER_HPP
#define NDEV_WORKER_HPP

#include <tuple>

#include "tack/ndev_pool.hpp"
#include "tack/ethernet.hpp"
#include "tack/arp.hpp"

namespace tack
{

class ndev_worker
{
public:
    ndev_worker(ndev_pool &pool,int fd, size_t mtu);

    void run();

    int64_t read(uint8_t *dest, size_t max_size);

    int64_t write(const uint8_t *src, size_t size);

    template <class Layer>
    Layer &get_layer()
    {
        return std::get<Layer>(layers_);
    }

    ndev_pool &get_pool()
    {
        return pool_;
    }

private:
    ndev_pool &pool_;
    int fd_;
    size_t mtu_;
    std::tuple<ethernet, arp> layers_;
};


}

#endif // NDEV_WORKER_HPP
