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
    ~arp_cache() = default;

    struct ipv4_address
    {
        bool operator==(const ipv4_address &addr) const
        {
            for (int i = 0;i<4;i++) {
                if (addr.address[i] != address[i]) {
                    return false;
                }
            }

            return true;
        }

        uint8_t address[4];
    };

    struct hw_address
    {
        bool operator==(const hw_address &addr) const
        {
            for (int i = 0;i<4;i++) {
                if (addr.address[i] != address[i]) {
                    return false;
                }
            }

            return true;
        }

        uint8_t address[6];
    };

    bool to_hwaddr(const ipv4_address &from, hw_address &to);
    bool from_hwaddr(const hw_address &from, ipv4_address &to);

    bool update(const hw_address &hwaddr, const ipv4_address &ipaddr);

private:
    struct arp_entry
    {
        hw_address hw_addr;
        ipv4_address ip_addr;
    };

    std::vector<arp_entry> cache_;
    size_t cache_size_;
};

using arp_cache_ptr = std::shared_ptr<arp_cache>;

} // namepsace tack

#endif // ARP_CACHE
