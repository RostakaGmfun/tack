#include <unistd.h>
#include <stdexcept>
#include <cstddef>
#include <iostream>
#include <vector>
#include <memory>

#include "tack/ndev_pool.hpp"
#include "tack/network_device.hpp"
#include "tack/ethernet.hpp"
#include "tack/arp_cache.hpp"
#include "tack/sockbuf.hpp"

namespace tack
{

class ndev_worker
{
public:
    ndev_worker(ndev_pool &pool,
            int fd, size_t mtu):
        pool_(pool), fd_(fd), mtu_(mtu),
        ethernet_(mtu, pool.arp_cache())
    {}

    void run()
    {
        // TODO: sockbuf size configuration instead of meaningless values
        sockbuf skb_in(mtu_, 256);
        sockbuf skb_out(1, 0);

        ndev_pool::write_callback_t write_cb;
        while (!pool_.is_stop()) {
            if (skb_in.read(*this) > 0) {
                ethernet_.process_packet(skb_in);
            }

            if (pool_.pick_task(skb_out, write_cb)) {
                int64_t ret = skb_in.write(*this);
                if (write_cb) {
                    write_cb(ret);
                }
            }
        }
    }

    int64_t read(uint8_t *dest, size_t max_size)
    {
        if (dest == nullptr || max_size < mtu_) {
            return -1;
        }

        // Sometimes Ethernet frames are prepended with bunch bytes equal to zero.
        // Currently I don't get why this happens,
        // but this li'l hack make things happen the way I like them
        ssize_t r = ::read(fd_, dest, mtu_);
        if (r < 0) {
            return r;
        }
        for(uint8_t *p = dest; p != dest + r; p++) {
            if (*p !=0) {
                return std::copy(p, dest + r, dest) - dest;
            }
        }
        return -1;
    }

    int64_t write(const uint8_t *src, size_t size)
    {
        // TODO: think about fragmentation for frames bigger than MTU
        return ::write(fd_, src, size);
    }


private:
    ndev_pool &pool_;
    int fd_;
    size_t mtu_;
    ethernet ethernet_;
};

ndev_pool::ndev_pool(const network_device &ndev):
    stop_(false),
    arp_cache_(new tack::arp_cache(ndev.get_hwaddr()))
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
