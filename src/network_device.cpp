#include "tack/network_device.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include <stdexcept>
#include <cstring>
#include <iostream>

using namespace tack;

network_device::network_device(std::string name,
        network_device_type type):
        device_name_(name)
{
    device_fd_ = open("/dev/net/tun", O_RDWR);
    if (device_fd_ < 0) {
        throw std::runtime_error("Failed to open " + name +
                std::string(strerror(errno)));
    }

    bool ret = true;
    switch (type) {
        case network_device_type::tun:
            ret = init_tun();
        break;
        case network_device_type::tap:
            ret = init_tap();
        break;
        default:
            throw std::runtime_error("Unknown device type");
    }

    if (!ret) {
        throw std::runtime_error("Failed to initalize tun/tap device");
    }

    type_ = type;

    if (!init_pkb()) {
        close(device_fd_);
        throw std::runtime_error("Failed to initalize packet buffer");
    }
}

bool network_device::init_tun()
{
    // TODO: should we implement TUN?
    return false;
}

bool network_device::init_tap()
{
    ifreq ifr{};

    ifr.ifr_flags |= IFF_TAP|IFF_NO_PI;
    std::cout << ifr.ifr_mtu << std::endl;

    strncpy(ifr.ifr_name, device_name_.c_str(), IFNAMSIZ);

    if (ioctl(device_fd_, TUNSETIFF, (void *)&ifr) < 0) {
        close(device_fd_);
        std::cerr << strerror(errno) << std::endl;
        return false;
    }

    device_mtu_ = ifr.ifr_mtu;

    if (!device_mtu_) {
        std::cerr << "Invalid MTU=0" << std::endl;
    }

    return true;
}

bool network_device::init_pkb()
{
    packet_buffer_ = new uint8_t[device_mtu_];
    return true;
}

