#include "tack/device_pool.hpp"
#include "tack/network_device.hpp"

#include <stdexcept>

using namespace tack;

device_pool::device_pool(size_t num_threads)
{
    if (!num_threads) {
        throw std::runtime_error("Wrong number of threads");
    }

    for (int i = 0;i<num_threads;i++) {
        threads_.push_back(
                std::thread([this] {
                    // TODO
                }));
    }
}

device_pool::~device_pool()
{
    // TODO
}
