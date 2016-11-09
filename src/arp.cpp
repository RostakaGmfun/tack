#include "tack/arp.hpp"
#include "tack/byte_utils.hpp"
#include "tack/arp_cache.hpp"

#include <iostream>

namespace tack {

arp::arp(const arp_cache_ptr &cache):
    arp_cache_(cache)
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
    parse_payload(skb.payload(), sha, tha, spa, tpa);
    switch(tack::ntohs(hdr->op)) {
        case op_type::arp_request:
            send_reply(sha, spa);
        // fallthrough is expected
        case op_type::arp_reply:
            if (sha != arp_cache_->get_self()) {
                arp_cache_->update(sha, spa);
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

void arp::send_reply(const hw_address &sha, const ipv4_address &spa)
{
    // TODO
    std::cout << "Sending reply to " << sha << ' ' << spa << '\n';
}

}
