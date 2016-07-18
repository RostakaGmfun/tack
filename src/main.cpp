#include "tack/network_device.hpp"
#include "tack/ndev_pool.hpp"

#include <signal.h>
#include <cstdlib>
#include <thread>

void sigint_handler(int sig)
{
    // This should flush and close all opened fds
    exit(0);
}

int main()
{
    signal(SIGINT, sigint_handler);
    unsigned int num_threads = std::thread::hardware_concurrency();
    tack::network_device ndev("tack0", num_threads > 1 ? num_threads-1 : 1);
    tack::ndev_pool pool(ndev);
    while (true) {
        // TODO
    }
    return 0;
}
