#ifndef ARP_CACHE
#define ARP_CACHE

#include <cstddef>
#include <vector>
#include <memory>

namespace tack
{

/**
 * Thread-safe ARP cache implementation.
 */
class arp_cache
{
public:
    arp_cache(size_t cache_size = 1024);
    ~arp_cache();

    typedef uint8_t ipv4_address[4];
    typedef uint8_t hw_address[6];

    void to_hwaddr(const ipv4_address from, hw_address &to);
    void from_hwaddr(const hw_address from, ipv4_address &to);

private:

    struct arp_entry
    {
        hw_address hw_adrr;
        ipv4_address ip_addr;
    };

    std::vector<arp_entry> cache_;
};

using arp_cache_ptr = std::shared_ptr<arp_cache>;

} // namepsace tack

#endif // ARP_CACHE
