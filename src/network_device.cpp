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
        network_device_type type, size_t mtu):
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
            close(device_fd_);
            throw std::runtime_error("Unknown device type");
    }

    if (!ret) {
        close(device_fd_);
        throw std::runtime_error("Failed to initalize tun/tap device");
    }

    type_ = type;

    sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd_ < 0) {
        close(device_fd_);
        throw std::runtime_error("Failed to open socket: " +
                std::string(strerror(errno)));
    }

    if (!set_device_mtu(mtu)) {
        close(device_fd_);
        close(sock_fd_);
        throw std::runtime_error("Failed to set device MTU");
    }

    if (!init_pkb()) {
        close(device_fd_);
        close(sock_fd_);
        throw std::runtime_error("Failed to initalize packet buffer");
    }
}

network_device::~network_device()
{
    if (device_fd_) {
        close(device_fd_);
    }

    if (sock_fd_) {
        close(sock_fd_);
    }

    delete packet_buffer_;
}

bool network_device::set_device_mtu(size_t mtu)
{
    if (!mtu) {
        return false;
    }

    ifreq ifr{};

    strncpy(ifr.ifr_name, device_name_.c_str(), sizeof(ifr.ifr_name));
    ifr.ifr_addr.sa_family = AF_INET;
    ifr.ifr_mtu = mtu;

    if (ioctl(sock_fd_, SIOCSIFMTU, (void *)&ifr) < 0) {
        std::cerr << strerror(errno) << std::endl;
        return false;
    }

    device_mtu_ = mtu;

    std::cout << "MTU set to " << device_mtu_ << std::endl;
    return true;
}

bool network_device::init_tun()
{
    // TODO: should we implement TUN?
    return false;
}

bool network_device::init_tap()
{
    ifreq ifr{};

    ifr.ifr_flags |= IFF_TAP | IFF_NO_PI;

    strncpy(ifr.ifr_name, device_name_.c_str(), sizeof(ifr.ifr_name));

    if (ioctl(device_fd_, TUNSETIFF, (void *)&ifr) < 0) {
        std::cerr << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool network_device::init_pkb()
{
    packet_buffer_ = new uint8_t[device_mtu_];
    return true;
}

