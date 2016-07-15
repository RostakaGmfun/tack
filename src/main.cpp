#include "tack/network_device.hpp"
#include "tack/device_pool.hpp"

int main()
{
    tack::network_device ndev("tack0", tack::network_device_type::tap);
    while(1);
    return 0;
}
