#include "tack/arp.hpp"
#include "tack/arp_cache.hpp"
#include "tack/byte_utils.hpp"

#include <iostream>

namespace tack {

arp::arp(arp_cache_ptr cache):
    arp_cache_(cache)
{}

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

    switch(tack::ntohs(hdr->op)) {
        case op_type::arp_request:
            process_request(skb.payload());
        case op_type::arp_reply:
            process_reply(skb.payload());
        break;
    }

    /*std::cout << "sha: ";
    for (auto b : sha) {
        std::cout << std::hex << (int)b << ' ';
    }

    std::cout << "\nspa: ";
    for (auto b : spa) {
        std::cout << std::dec << (int)b << ' ';
    }

    std::cout << "\ntha: ";
    for (auto b : tha) {
        std::cout << std::hex << (int)b << ' ';
    }

    std::cout << "\ntpa: ";
    for (auto b : tpa) {
        std::cout << std::dec << (int)b << ' ';
    }

    std::cout << '\n';*/
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


void arp::process_request(const uint8_t *payload)
{
    tack::hw_address sha, tha;
    tack::ipv4_address spa, tpa;
    parse_payload(payload, sha, tha, spa, tpa);
}

void arp::process_reply(const uint8_t *payload)
{
    tack::hw_address sha, tha;
    tack::ipv4_address spa, tpa;
    parse_payload(payload, sha, tha, spa, tpa);
}

}

