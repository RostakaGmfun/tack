#include "tack/ipv4.hpp"
#include "tack/byte_utils.hpp"

#include <iostream>

namespace tack
{

ipv4::ipv4(ndev_worker *worker): worker_(worker)
{}

/**
 * See RFC 791
 */
void ipv4::process_packet(sockbuf &skb)
{
    if (!skb.push_header<ipv4_header>()) {
        return;
    }

    const ipv4_header *hdr = skb.get_header<ipv4_header>();

    // Version is 4 for IPv4
    if (((hdr->version_ihl & 0xF0) >> 4) != 4) {
        return;
    }

    // Internet header length should be at least 5 32-bit words
    if ((hdr->version_ihl & 0x0F) < 5) {
        std::cout << "ihl\n";
        return;
    }

    // Packets with TTL equal to 0 should be discarded
    if (hdr->ttl == 0) {
        std::cout << "ttl\n";
        return;
    }

    std::cout << "Received packet from " << hdr->src << " to " << hdr->dst << '\n';
    //TODO
}

}
