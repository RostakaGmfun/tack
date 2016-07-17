#include "tack/ndev_pool.hpp"
#include "tack/network_device.hpp"

#include <unistd.h>

#include <stdexcept>
#include <cstddef>

namespace tack
{

class ndev_worker
{
public:
    ndev_worker(const ndev_pool *pool,
            int fd, size_t mtu):
        pool_(pool), fd_(fd), mtu_(mtu)
    { }

    void run()
    {
        while (true) {
            // TODO

            if (pool_->is_stop()) {
                return;
            }
        }
    }

private:
    const ndev_pool *pool_;
    int fd_;
    size_t mtu_;
};

ndev_pool::ndev_pool(const network_device &ndev): stop_(false)
{
    for (int i = 0;i<ndev.get_num_devices();i++) {
        threads_.push_back(
                std::thread([this, &ndev, i] {
                    int fd = ndev.get_device_fd(i);
                    size_t mtu = ndev.get_mtu();

                    ndev_worker worker(this, fd, mtu);
                    worker.run();
                }));
    }
}

bool ndev_pool::is_stop() const
{
    return stop_;
}

ndev_pool::~ndev_pool()
{
    stop_ = true;
    for (auto &thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

} // namespace tack
