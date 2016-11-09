#ifndef ADDRESSES_HPP
#define ADDRESSES_HPP

#include <array>
#include <ostream>

namespace tack {

using hw_address = std::array<std::uint8_t, 6>;
using ipv4_address = std::array<std::uint8_t, 4>;

inline std::ostream &operator<<(std::ostream &o, const hw_address &hw)
{
    for (int i = 0; i < 6; i++) {
        o << std::hex << (int)hw[i] << (i < 5 ? ":" : "");
    }
    return o;
}

inline std::ostream &operator<<(std::ostream &o, const ipv4_address &ip)
{
    for (int i = 0; i < 4; i++) {
        o << std::dec << (int)ip[i] << (i < 3 ? "." : "");
    }
    return o;
}

}

#endif // ADDRESSES_HPP
