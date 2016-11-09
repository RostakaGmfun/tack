#include "tack/network_device.hpp"
#include "tack/ndev_pool.hpp"
#include "tack/arp_cache.hpp"

#include <signal.h>
#include <cstdlib>
#include <thread>
#include <iostream>

void sigint_handler(int sig)
{
    static_cast<void>(sig);
    // This should flush and close all opened fds
    exit(0);
}

int main()
{
    signal(SIGINT, sigint_handler);
    unsigned int num_threads = std::thread::hardware_concurrency();
    try {
        tack::network_device ndev("tack0", num_threads > 1 ? num_threads-1 : 1);
        tack::ndev_pool pool(ndev);
        auto arp_cache = pool.arp_cache();
        arp_cache->update(arp_cache->get_self(), {10,0,0,12});
        while (true) {
            // TODO
        }
        return 0;
    } catch (std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
