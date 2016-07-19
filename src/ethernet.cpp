#include "tack/ethernet.hpp"

#include <iostream>
#include <stdexcept>

#include <arpa/inet.h>

using namespace tack;

ethernet::ethernet(size_t mtu):
    mtu_(mtu)
{
    if (!mtu_) {
        throw std::invalid_argument("MTU can't be zero");
    }
}

void ethernet::process_packet(const uint8_t *payload)
{
    // TODO: are such checks really required?
    if (!payload) {
        return;
    }

    const ethernet_header *hdr = reinterpret_cast<const ethernet_header *>(payload);

    switch (ntohs(hdr->type)) {
        case IPv4:
            // TODO
            std::cout << "IPv4" << std::endl;
        break;
        case IPv6:
            // TODO
            std::cout << "IPv6" << std::endl;
        break;
        case ARP:
            // TODO
            std::cout << "ARP" << std::endl;
        break;
        default:
            std::cerr << std::hex << ntohs(hdr->type) << std::endl;
    }
}
