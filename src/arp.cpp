#include "tack/arp.hpp"
#include "tack/byte_utils.hpp"
#include "tack/arp_cache.hpp"
#include "tack/ndev_worker.hpp"
#include "tack/ndev_pool.hpp"

#include <iostream>

namespace tack {

arp::arp(ndev_worker *worker):
    worker_(worker)
{}

/**
 *  See RFC 826, section "Packet reception"
 */
void arp::process_packet(sockbuf &skb)
{
    if (!skb.push_header<arp_header>()) {
        return;
    }

    arp_header *hdr = skb.get_header<arp_header>();

    // currently only Ethernet <-> IPv4 addresses are supported
    if (tack::ntohs(hdr->hrd) != hrd_type::ethernet || hdr->hln != 6) {
        return;
    }
    if (tack::ntohs(hdr->pro) != pro_type::IPv4 || hdr->pln != 4) {
        return;
    }

    tack::hw_address sha, tha;
    tack::ipv4_address spa, tpa;
    auto arp_cache = worker_->get_pool().arp_cache();
    const auto &device = worker_->get_pool().get_device();
    parse_payload(skb.payload(), sha, tha, spa, tpa);
    switch(tack::ntohs(hdr->op)) {
        case op_type::arp_request:
            send_reply(sha, spa);
        // fallthrough is expected
        case op_type::arp_reply:
            if (sha != device.get_hwaddr()) {
                arp_cache->update(sha, spa);
            }
        break;
        default:
            return;
    }

    std::cout << "sha: "  << sha << '\n';
    std::cout << "spa: "  << spa << '\n';
    std::cout << "tha: "  << tha << '\n';
    std::cout << "tpa: "  << tpa << '\n';
}

void arp::parse_payload(const uint8_t *payload,
        hw_address &sha, hw_address &tha, ipv4_address &spa, ipv4_address &tpa)
{
    std::copy(payload, payload+sizeof(sha), &sha[0]);
    payload += sizeof(sha);
    std::copy(payload, payload+sizeof(spa), &spa[0]);
    payload += sizeof(spa);
    std::copy(payload, payload+sizeof(sha), &tha[0]);
    payload += sizeof(sha);
    std::copy(payload, payload+sizeof(tpa), &tpa[0]);
}

void arp::send_reply(const hw_address &tha, const ipv4_address &tpa)
{
    static constexpr auto payload_len = (sizeof(hw_address) + sizeof(ipv4_address))*2;
    static constexpr auto header_len = sizeof(ethernet_header) + sizeof(arp_header);
    static sockbuf reply(header_len + payload_len, header_len);

    std::cout << "Sending reply to " << tpa << " @ " << tha << '\n';

    auto arp_cache = worker_->get_pool().arp_cache();
    auto sha = worker_->get_pool().get_device().get_hwaddr();
    auto spa = worker_->get_pool().get_device().get_ipaddr();
    arp_header hdr;

    hdr.hrd = hrd_type::ethernet;
    hdr.pro = pro_type::IPv4;
    hdr.hln = sizeof(hw_address);
    hdr.pln = sizeof(ipv4_address);
    hdr.op = op_type::arp_reply;

    reply.clear_all(header_len);
    reply.add_payload(sha.data(), sizeof(hw_address));
    reply.add_payload(spa.data(), sizeof(ipv4_address));
    reply.add_payload(tha.data(), sizeof(hw_address));
    reply.add_payload(tpa.data(), sizeof(ipv4_address));
    reply.wrap(&hdr);
    worker_->get_layer<tack::ethernet>().transfer_packet(reply, tha);
}

}
