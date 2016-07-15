#ifndef DEVICE_POOL_HPP
#define DEVICE_POOL_HPP

#include <cstddef>
#include <vector>
#include <thread>

namespace tack
{

class device_pool
{
public:
    device_pool(size_t num_devices);
    ~device_pool();

private:
    std::vector<std::thread> threads_;
};

} // namespace tack

#endif // DEVICE_POOL_HPP
