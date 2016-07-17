#include "tack/network_device.hpp"
#include "tack/ndev_pool.hpp"

#include <signal.h>
#include <cstdlib>

void sigint_handler(int sig)
{
    // This should flush and close all opened fds
    exit(0);
}

int main()
{
    signal(SIGINT, sigint_handler);
    tack::network_device ndev("tack0");
    tack::ndev_pool pool(ndev);
    while(1);
    return 0;
}
