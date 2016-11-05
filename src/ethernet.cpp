#include "tack/ethernet.hpp"
#include "tack/arp.hpp"
#include "tack/arp_cache.hpp"
#include "tack/byte_utils.hpp"

#include <iostream>
#include <stdexcept>

using namespace tack;

ethernet::ethernet(size_t mtu):
    mtu_(mtu)
{
    if (!mtu_) {
        throw std::invalid_argument("MTU can't be zero");
    }
}

void ethernet::process_packet(sockbuf &skb)
{
    if (!skb.push_header<ethernet_header>()) {
        return;
    }
    const ethernet_header *hdr = skb.get_header<ethernet_header>();

    std::cout << "src: ";
    for (auto b : hdr->src) {
        std::cerr  << std::hex << (int)b << ' ';
    }
    std::cout << "\ndst: ";
    for (auto b : hdr->dest) {
        std::cerr << std::hex << (int)b << ' ';
    }

    std::cout << '\n';

    switch (tack::ntohs(hdr->type)) {
        case ethertype::IPv4:
            // TODO
            std::cout << "IPv4" << std::endl;
        break;
        case ethertype::IPv6:
            // TODO
            std::cout << "IPv6" << std::endl;
        break;
        case ethertype::ARP:
            // TODO
            std::cout << "ARP" << std::endl;
        break;
        default:
            std::cerr << std::hex << static_cast<uint16_t>(tack::ntohs((hdr->type))) << std::endl;
    }
}
