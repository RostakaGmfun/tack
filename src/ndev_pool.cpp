#include <stdexcept>
#include <cstddef>
#include <vector>

#include "tack/ndev_pool.hpp"
#include "tack/ndev_worker.hpp"
#include "tack/network_device.hpp"
#include "tack/arp_cache.hpp"
#include "tack/sockbuf.hpp"

namespace tack
{

ndev_pool::ndev_pool(const network_device &ndev):
    stop_(false),
    arp_cache_(new tack::arp_cache),
    device_(ndev)
{
    for (size_t i = 0;i<ndev.get_num_devices();i++) {
        threads_.emplace_back(
            std::thread([this, &ndev, i] {
                int fd = ndev.get_device_fd(i);
                size_t mtu = ndev.get_mtu();

                ndev_worker worker(*this, fd, mtu);
                worker.run();
            }));
    }
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

bool ndev_pool::is_stop() const
{
    return stop_;
}

void ndev_pool::write(tack::sockbuf &&skb, write_callback_t callback)
{
    std::lock_guard<std::mutex> lock(write_queue_mutex_);
    write_queue_.push(std::make_pair(std::move(skb), std::move(callback)));
}

bool ndev_pool::pick_task(sockbuf &skb, write_callback_t &cb)
{
    std::lock_guard<std::mutex> lock(write_queue_mutex_);
    if (write_queue_.empty()) {
        return false;
    }

    skb = std::move(write_queue_.front().first);
    cb = std::move(write_queue_.front().second);
    write_queue_.pop();
    return true;
}

} // namespace tack
