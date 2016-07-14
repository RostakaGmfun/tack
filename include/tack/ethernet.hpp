#ifndef ETHERNET_HPP
#define ETHERNET_HPP

#include <cstdint>

namespace tack {

typedef uint8_t mac_address[6];

struct ethernet_header {
    mac_address dest;
    mac_address src;
    uint16_t type;
    uint8_t *payload;
} __attribute__((packed));

class ethernet {
public:
    ethernet() = default;

    void process_packet(const uint8_t *payload,
            size_t payload_size);
};

} // namespace tack

#endif // ETHERNET_HPP
