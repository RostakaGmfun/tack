#ifndef ARP_HPP
#define ARP_HPP

#include <cstdint>
#include <vector>
#include <memory>

#include "tack/ethernet.hpp"
#include "tack/sockbuf.hpp"

namespace tack {

class arp_cache;
using arp_cache_ptr = std::shared_ptr<arp_cache>;

enum class hrd_type: uint16_t
{
    ethernet = 1,
};

using pro_type = ethertype;

enum class op_type: uint16_t
{
    arp_request = 1,
    arp_reply = 2
};

struct arp_header
{
    hrd_type hrd;
    pro_type pro;
    uint8_t hln;
    uint8_t pln;
    op_type op;
} __attribute__((packed));

class arp
{
public:
    arp(arp_cache_ptr cache);
    ~arp() = default;

    void process_packet(sockbuf &skb);

private:
    arp_cache_ptr arp_cache_;
};

}

#endif // ARP_HPP
