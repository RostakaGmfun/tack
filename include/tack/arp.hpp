#ifndef ARP_HPP
#define ARP_HPP

#include <cstdint>
#include <vector>
#include <memory>

#include "tack/ethernet.hpp"

namespace tack {

class arp_cache;
using arp_cache_ptr = std::shared_ptr<arp_cache>;

enum class hrd_type: uint16_t
{
    ETHERNET = 1,
};

enum class op_type: uint16_t
{
    arp_request = 1,
    arp_reply = 2,
    rarp_request = 3,
    rarp_reply = 4
};

struct arp_header
{
    hrd_type hrd;
    ethertype pro;
    uint8_t hln;
    uint8_t pln;
    op_type op;
} __attribute__((packed));

class arp
{
public:
    arp(arp_cache_ptr cache);
    ~arp() = default;

    void process_packet(const std::vector<uint8_t> &payload);

private:
    arp_cache_ptr arp_cache_;
};

}

#endif // ARP_HPP
