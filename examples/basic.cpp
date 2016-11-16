#include "tack/network_device.hpp"
#include "tack/ndev_pool.hpp"
#include "tack/arp_cache.hpp"

#include <signal.h>
#include <cstdlib>
#include <thread>
#include <iostream>
#include <atomic>

std::atomic_bool is_exit{false};

void sigint_handler(int sig)
{
    static_cast<void>(sig);
    is_exit = true;
}

int main()
{
    signal(SIGINT, sigint_handler);
    unsigned int num_threads = std::thread::hardware_concurrency();
    try {
        tack::network_device ndev("tack0", num_threads > 1 ? num_threads-1 : 1);
        tack::ndev_pool pool(ndev);
        auto arp_cache = pool.arp_cache();
        arp_cache->update(ndev.get_hwaddr(), {20, 0, 0, 1});
        while (!is_exit);
    } catch (std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
