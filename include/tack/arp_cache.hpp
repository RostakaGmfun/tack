#ifndef ARP_CACHE
#define ARP_CACHE

#include <cstddef>
#include <vector>
#include <memory>
#include <mutex>
#include <array>

namespace tack
{

template <typename T>
struct arp_result
{
    arp_result(T r): value(r), is_null(false)
    {}
    arp_result(): is_null(true)
    {}

    T value;
    bool is_null;
};

using hw_address = std::array<std::uint8_t, 6>;
using ipv4_address = std::array<std::uint8_t, 4>;

/**
 * Thread-safe ARP cache implementation.
 */
class arp_cache
{
public:
    arp_cache(size_t size = 1024);
    ~arp_cache() = default;

    /**
     * Retrieve IPv4 address by a hardware (MAC) address.
     */
    arp_result<ipv4_address> operator[](const hw_address &from);
    /**
     * Retrieve hardware (MAC) address by IPV4 address.
     */
    arp_result<hw_address> operator[](const ipv4_address &from);
    /**
     * Update the cache with address pair.
     */
    void update(const hw_address &hwaddr, const ipv4_address &ipaddr);

private:
    struct arp_entry
    {
        arp_entry(hw_address hw, ipv4_address ip):
            hw_addr(hw), ip_addr(ip)
        {}
        hw_address hw_addr;
        ipv4_address ip_addr;
    };

    std::vector<arp_entry> cache_;
    std::mutex cache_mutex_;
    size_t cache_size_;
};

using arp_cache_ptr = std::shared_ptr<arp_cache>;

} // namepsace tack

#endif // ARP_CACHE
