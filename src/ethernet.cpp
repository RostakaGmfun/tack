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

void ethernet::process_packet(const std::vector<uint8_t> &payload)
{
    // Sometimes Ethernet frames are prepended with a bunch of 0 bytes.
    // Currently I don't get why this happens,
    // but this li'l hack make things happen the way I like them
    auto it = payload.begin();
    while (!*it) {
        it++;
        if (static_cast<size_t>(std::distance(it, payload.end())) < sizeof(ethernet_header)) {
            // Looks like we've read all zeroes
            return;
        }
    }
    const ethernet_header *hdr = reinterpret_cast<const ethernet_header *>(&*it);

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
