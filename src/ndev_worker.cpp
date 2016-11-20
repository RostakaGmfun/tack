#include "tack/ndev_worker.hpp"

#include <unistd.h>
#include <iostream>

namespace tack
{

ndev_worker::ndev_worker(ndev_pool &pool,
        int fd, size_t mtu):
    pool_(pool), fd_(fd), mtu_(mtu),
    layers_(tack::ethernet(this), tack::arp(this), tack::ipv4(this))
{}

void ndev_worker::run()
{
    // TODO: sockbuf size configuration instead of meaningless values
    sockbuf skb_in(mtu_, 256);
    sockbuf skb_out(1, 0);

    ndev_pool::write_callback_t write_cb;
    while (!pool_.is_stop()) {
        if (skb_in.read(*this) > 0) {
            std::get<tack::ethernet>(layers_).process_packet(skb_in);
        }

        if (pool_.pick_task(skb_out, write_cb)) {
            int64_t ret = skb_in.write(*this);
            if (write_cb) {
                write_cb(ret);
            }
        }
    }
}

int64_t ndev_worker::read(uint8_t *dest, size_t max_size)
{
    if (dest == nullptr || max_size < mtu_) {
        return -1;
    }

    return ::read(fd_, dest, mtu_);
}

int64_t ndev_worker::write(const uint8_t *src, size_t size)
{
    // TODO: think about fragmentation for frames bigger than MTU
    return ::write(fd_, src, size);
}

}
