#include "tack/ethernet.hpp"
#include "tack/arp.hpp"
#include "tack/arp_cache.hpp"
#include "tack/byte_utils.hpp"
#include "tack/ndev_worker.hpp"

#include <iostream>
#include <stdexcept>

using namespace tack;

ethernet::ethernet(ndev_worker *worker): worker_(worker)
{
    if (!worker_) {
        throw std::invalid_argument("Worker is nullptr");
    }
}

void ethernet::process_packet(sockbuf &skb)
{
    if (!skb.push_header<ethernet_header>()) {
        return;
    }
    const ethernet_header *hdr = skb.get_header<ethernet_header>();

    switch (tack::ntohs(hdr->type)) {
        case ethertype::IPv4:
            // TODO
        break;
        case ethertype::IPv6:
            // TODO
        break;
        case ethertype::ARP:
            worker_->get_layer<arp>().process_packet(skb);
        break;
        default:
            std::cerr << std::hex << static_cast<uint16_t>(tack::ntohs((hdr->type))) << std::endl;
    }
}

void ethernet::transfer_packet(sockbuf &sockbuf, const hw_address &dest)
{
    static_cast<void>(sockbuf);
    static_cast<void>(dest);
}
