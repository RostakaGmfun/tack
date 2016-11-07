#include <unistd.h>
#include <stdexcept>
#include <cstddef>
#include <iostream>
#include <vector>

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
    ndev_worker(const ndev_pool &pool,
            int fd, size_t mtu):
        pool_(pool), fd_(fd), mtu_(mtu),
        ethernet_(mtu)
    {}

    void run()
    {
        // TODO: sockbuf size configuration instead of meaningless values
        sockbuf skb(mtu_, 256);
        while (!pool_.is_stop()) {
            if (skb.read(*this) > 0) {
                ethernet_.process_packet(skb);
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
        for(uint8_t *p = dest; p != dest+ r; p++) {
            if (*p !=0) {
                return std::copy(p, dest + r, dest) - dest;
            }
        }
        return -1;
    }


private:
    const ndev_pool &pool_;
    int fd_;
    size_t mtu_;
    ethernet ethernet_;
};

ndev_pool::ndev_pool(const network_device &ndev):
    stop_(false),
    arp_cache_(std::make_shared<arp_cache>(ndev.get_hwaddr()))
{
    for (size_t i = 0;i<ndev.get_num_devices();i++) {
        threads_.push_back(
                std::thread([this, &ndev, i] {
                    int fd = ndev.get_device_fd(i);
                    size_t mtu = ndev.get_mtu();

                    ndev_worker worker(*this, fd, mtu);
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
